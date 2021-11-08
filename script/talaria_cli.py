#!/usr/bin/env python3
import os
import sys
import time
import socket
import threading
import shlex
import hostio
import readline
from enum import Enum
from threading import Event
import ping as rping
from wcm import WCMError, WCM, ScanUtils
from ipaddress import IPv4Address, IPv6Address
from ipaddress import ip_address
from api import base, sock
from api import debug
from bits.hwaddress import HWAddress
from argparse import ArgumentParser, RawTextHelpFormatter

class IfState(Enum):
    NONE      = 0
    CREATED   = 1
    CONNECTED = 2

class IPv4Type(Enum):
    NONE      = 0
    DYNAMIC   = 1
    STATIC    = 2

if sys.version_info.major != 3 or sys.version_info.minor < 3:
    print('Python version 3.3 or higher required (you have {0.major}.{0.minor}.{0.micro})'.format(sys.version_info))
    sys.exit(1)
    pass

class HIOTCPSocket:
    def __init__(self, dev, w, host=None, port=80, is_server=True, family=4):
        self._dev           = dev
        self._w             = w
        self.sock_family      = family
        self.hio_sock         = None
        self.hio_host_ip      = host
        self.hio_host_port    = port
        self.is_tcp           = True
        self.is_server        = is_server
        self.hio_thread       = None
        self.hio_rwlock       = threading.RLock()
        self.hio_host_running = False
        self.hio_dataqueue    = {}
        self.hio_acceptqueue  = {}
        self.hio_srv_seqno    = 0
        self.hio_dataReady    = False

    def accept(self, msg):
        if not self.is_server:
            return
        self.hio_acceptqueue[msg.socket] = msg.socket
        print('accept: {}-->{}'.format(msg.socket, self.hio_acceptqueue))
        self._dev.set_ind_handler(sock.notification, self.data_ready)

    def close(self, msg):
        self.hio_rwlock.acquire()
        fd = msg.socket
        if fd in self.hio_acceptqueue:
            del self.hio_acceptqueue[fd]
            print('HIO server connected sock {} closed'.format(fd))
        if fd in self.hio_dataqueue:
            del self.hio_dataqueue[fd]
        if fd == self.hio_sock:
            self.hio_sock = None
            print('HIO {} sock closed'.format('listen' if self.is_server else 'client'))
        self.hio_rwlock.release()

    def data_ready(self, msg):
        self.hio_rwlock.acquire()
        fd = msg.socket
        if fd in self.hio_acceptqueue:
            self.hio_dataqueue[fd] = fd
            print('HIO server data:', fd)
        if fd == self.hio_sock:
            #self.hio_dataqueue[fd] = fd
            self.hio_dataReady = True
            print('HIO client data:', fd)
        self.hio_rwlock.release()

    def sockets_close_all(self, sockets):
        for fd in sockets.values():
            self._dev.call(sock.close, fd)

    def get_ipaddr_info(self):
        if self.is_server:
            if self.sock_family in [4, 46]:
                ifaceinfo = self._w.get_ifconfig4()
                self.hio_host_ip = ifaceinfo.get('addr4')
            else:
                ifaceinfo = self._w.get_ifconfig6()
                self.hio_host_ip = ifaceinfo.get('addr6')[0]
            return True
        else:
            if self.hio_host_ip == None:
                print('invalid remote server addr: {}'.format(self.hio_host_ip))
                return False
            return True

    def set_cli_rhost(self, rhost):
        self.rhostname=rhost

    def setup(self):
        if self.is_server:
            self._dev.set_ind_handler(sock.connection, self.accept)
            self._dev.set_ind_handler(sock.notification, self.data_ready)
        self._dev.set_ind_handler(sock.close, self.close)
        if self.hio_host_ip == None:
            if not self.get_ipaddr_info():
                return False
        else:
            try:
                self.sock_family = ip_address(self.hio_host_ip).version
            except Exception as ex:
                print('invalid remote ip address: {}'.format(self.hio_host_ip, ex))
                return False
        uri ='tcp://{}:{}'.format(self.hio_host_ip, self.hio_host_port) \
             if self.sock_family == 4 else \
             'tcp6://[{}]:{}'.format(self.hio_host_ip, self.hio_host_port)
        uri = bytes(uri, 'ascii')
        print(uri)
        try:
            rsp = self._dev.call(sock.server if self.is_server else sock.client, uri)
            print('hio socket::{}'.format(rsp))
            assert (rsp.status == 0)
            self.hio_sock = rsp.socket
            rsp = self._dev.call(sock.notification, rsp.socket, 0, 0)
            if self.is_server:
                server_at='http://{}:{}'.format(self.hio_host_ip, self.hio_host_port) \
                         if self.sock_family == 4 else \
                          'http://[{}]:{}'.format(self.hio_host_ip, self.hio_host_port)
                print('Server at: {}'.format(server_at))
        except Exception as ex:
            print('hio {} socket failed#: {}'.format('server' if self.is_server else 'client', ex))
            return False
        return True

    def http_response(self, fd, to_addr, to_port, to_addrlen):
        msg = 'HTTP/1.1 200 OK\r\n\
                    Content-type: text/html\r\n\r\n\
                    <html>\r\n\
                    <head>\r\n<title>\r\nTalaria 2 HIOSocket Server</title>\r\n</head>\r\n\
                    <body><h1>Talaria 2 HIO-Socket Server @ {}</h1></body>\r\n\
                    </html>\r\n'.format(self.hio_host_ip)
        msg = bytes(msg, 'ascii')
        rsp = self._dev.call(sock.sendto, fd, len(msg), 0, to_addr, to_addrlen, to_port, msg)

    def client(self):
        print('=====hio http client=====')
        try:
            to_port  = self.hio_host_port
            self.dataReady_clb = Event()
            self._dev.set_ind_handler(sock.notification, lambda msg:self.dataReady_clb.set())
            if self.sock_family == 4:
                to_addrlen = 4
                to_addr    = bytes(IPv4Address(self.hio_host_ip).packed + bytes(12))
            else:
                to_addrlen = 16
                to_addr    = bytes(IPv6Address(self.hio_host_ip).packed)
            if self.hio_host_running:
                #dataReady_clb.clear()
                time.sleep(0.2)
                if not self._w.connected:
                    dataReady_clb.clear()
                    print('not connected')
                    if self.hio_sock:
                        self._dev.call(sock.close, self.hio_sock)
                    return
                if self.hio_sock is not None:
                    msg = bytes("GET / HTTP/1.1\r\nAccept: */*\r\nHost: {}\r\nConnection: Keep-Alive\r\n\r\n\r\n".format(self.rhostname), 'ascii')
                    rsp = self._dev.call(sock.sendto, self.hio_sock, len(msg), 0, to_addr, to_addrlen, to_port, msg)
                    print(rsp)
                '''sleep 1 second and increment counter'''
                print('request sent to {} port {}'.format(self.hio_host_ip, self.hio_host_port))
                if not self.dataReady_clb.wait(15):
                    print('Read TimeOut')
                else:
                    print('reading response....')
                    while True and self.hio_sock:
                        rsp = self._dev.call(sock.recvfrom, self.hio_sock, 1024, 0)
                        if rsp.status != 0:
                            break
                        print('HTTP HIO client recvfrom:\n ::{}'.format(rsp))
        except Exception as ex:
            print('hio http client exception:{}'.format(ex))
        if self.hio_sock:
            self._dev.call(sock.close, self.hio_sock)

    def process_incoming(self, fd):
        try:
            print('=====hio http server=====')
            is_http = False
            while True:
                addr    = None
                rport   = None
                rdata   = None
                rsp = self._dev.call(sock.recvfrom, fd, 1024, 0)
                if rsp.status != 0:
                    if is_http:
                        print('close...HTTP socket')
                        self._dev.call(sock.close, fd)
                    break
                '''decode packet'''
                try:
                    rport = rsp.port
                    rdata = rsp.data
                    addr  = IPv4Address(bytes(rsp.addr[:4])) if rsp.addrlen == 4 else \
                                IPv6Address(bytes(rsp.addr[0:]))
                except Exception as ae:
                    print('addr_port_data_except:{}'.format(str(ae)))
                    print('rsp:{}'.format(rsp))
                    return
                print('HIO Server recvfrom:\n {} port:{} data:{}'.format(addr, rport, rdata))
                if self.is_server:
                    is_http = True if str(rdata).find('HTTP') != -1 else False
                    if not is_http:
                        self.hio_srv_seqno += 1
                        msg = bytes('HIO Server rsp: {}'.format(self.hio_srv_seqno), 'ascii')
                        rsp = self._dev.call(sock.sendto, fd, len(msg), 0, rsp.addr, rsp.addrlen, rport, msg)
                    else:
                        self.http_response(fd, rsp.addr, rport, rsp.addrlen)
        except Exception as ex:
            print('hio {} process incoming: {}'.format('server' if self.is_server else 'client', str(ex)))

    def server(self):
        self.hio_srv_seqno = 0
        print('=====hio http server=====')
        try:
            while self.hio_host_running:
                if len(self.hio_dataqueue) == 0 or not self._w.connected:
                    time.sleep(0.1)
                    continue
                """DO real stuff"""
                idx, fd = self.hio_dataqueue.popitem()
                if fd is None:
                    continue
                self.process_incoming(fd)
                '''signal Task Done'''
        except Exception as ex:
            print('hio server: {}'.format(str(ex)))
        if len(self.hio_acceptqueue):
            self.sockets_close_all(self.hio_acceptqueue)
        if self.hio_sock:
            self._dev.call(sock.close, self.hio_sock)
        time.sleep(2)
        print('server closed')

    def shutdown(self):
        if not self.hio_host_running:
            return
        self.hio_host_running = False
        if self.hio_thread:
            self.hio_thread.join()
        time.sleep(1)
        print('http {} shutdown'.format('server' if self.is_server else 'client'))

    def sock_thread(self):
        try:
            if self.hio_host_running == False:
                self.hio_host_running = self.setup()
                assert (self.hio_host_running)
                if self.is_server:
                    self.server()
                else:
                    self.client()
        except Exception as ex:
            print('hio {} socket thread failed: {}'.format('server' if self.is_server else 'client', ex))

    def do_run(self):
        try:
            assert (self.hio_host_running == False)
            assert (self.hio_thread == None)
            if self.is_server:
                self.hio_thread = threading.Thread(target=self.sock_thread)
                self.hio_thread.start()
            else:
                self.sock_thread()
        except Exception as ex:
            print('thread start failed: {}'.format(ex))


class StwCli():
    def __init__(self, path):
        dev = hostio.open(path, registry=base.registry)
        if dev is None:
                print("couldn't connect to: {}".format(path))
                sys.exit(1)
        print("connected to {}".format(str(dev)))
        self._dev           = dev
        self._w             = None
        self._ssid          = None
        self._passph        = None
        self._bssid         = None
        self._hwaddr        = None
        self._handle        = None
        self._if_state      = IfState.NONE
        """FIXME: future use"""
        self.networks       = dict(ssid=None, passphrase=None)
        self.set_handle()
        self.set_ipv4_type()

        """For HIO TCP Socket HTTP server/client"""
        self.hio_sock_appls = []

    def link_status_cb(self, link_up, msgtype):
        if link_up:
            if msgtype == 202:
                self._if_state = IfState.CONNECTED
                '''Set IPv4 Type'''
                if self.get_ipv4_type() == IPv4Type.NONE:
                    self.set_ipv4_type(IPv4Type.DYNAMIC)
        else:
            self._if_state = IfState.CREATED

    def set_if_state(self):
        if self._handle != None:
            if self._w.connected:
                self._if_state = IfState.CONNECTED
            else:
                self._if_state = IfState.CREATED
        else:
            self._if_state = IfState.NONE

    def get_if_state(self):
        return  self._if_state

    def set_ipv4_type(self, ipv4_type=IPv4Type.NONE):
        self._ipv4_type = ipv4_type

    def get_ipv4_type(self):
        return self._ipv4_type

    def set_handle(self, handle=None):
        self._handle = handle

    def check_handle(self):
        if self._handle == None:
            #self.do_help()
            return False
        else:
            return True

    def create(self, hwaddr=None):
        if self.check_handle():
            print('{:#x} handle already exists'.format(self._handle))
            return
        mac_addr = HWAddress.from_string(hwaddr if hwaddr else '000000000000')
        # Create interface
        self._w = WCM(self._dev, self.link_status_cb)
        if hwaddr is not None:
            self._w.set_hwaddr(mac_addr)
        self._handle = property(self._w.handle)
        if self._handle != None:
            self.set_handle(self._w._iface_handle)
            self.set_if_state()
            self._hwaddr = self._w.hwaddr
            print('create status OK')
            print("handle {:#x} {}".format(self._handle, self.get_if_state()))
            print("hwaddr {}".format(self._hwaddr))

    def destroy(self):
        if not self.check_handle():
            print('{}'.format(self._if_state))
            return
        elif self._w.close():
            self.set_handle(None)
            self.set_if_state()
            self.set_ipv4_type(IPv4Type.NONE)
            self._ssid   = None
            self._passph = None
            self._hwaddr = None
            print("destroy status: {} {}".format("OK", self._if_state))

    def scan(self, scan_ssid="", show_default=False, **kwargs):
        def scan_range_check(value, min_value, max_value, msg_str=None):
            try:
                value = int(value)
            except ValueError:
                print(f'scan error: invalid value {value}')
                return False
            if min_value <= value <= max_value:
                return True
            else:
                print(f'scan error: invalid <{msg_str}> (values in [{min_value}-{max_value}])')
                return False

        if not self.check_handle():
            print('  {}'.format(self._if_state))
            return
        else:
            if show_default:
                print('Scan params:', kwargs)
                return
            if kwargs['max_listen_time'] < kwargs['min_listen_time']:
                print("scan error: invalid <max_listen_time> (smaller than <min_listen_time>)")
                return
            if not scan_range_check(kwargs['num_probes'],
                                    min_value=1, max_value=2, msg_str='num_probes'):
                return
            if not scan_range_check(kwargs['idleslots'],
                                    min_value=1, max_value=8, msg_str='idleslots'):
                return
            if not scan_range_check(kwargs['min_listen_time'],
                                     min_value=4, max_value=24, msg_str='min_listen_time'):
                return
            if not scan_range_check(kwargs['max_listen_time'],
                                    min_value=24, max_value=1000, msg_str='max_listen_time'):
                return
            if not scan_range_check(kwargs['probe_tx_timeout'],
                                    min_value=4, max_value=8, msg_str='probe_tx_timeout'):
                return
            if not scan_range_check(kwargs['wait_time'],
                                    min_value=0, max_value=50, msg_str='wait_time'):
                return
            if not scan_range_check(kwargs['max_responses'],
                                    min_value=0, max_value=1000, msg_str='max_responses'):
                return

            ScanUtils.scan(self._w,
                            scan_ssid,
                            **kwargs)
            return

    def add_network(self, **kwargs):
        print(f"ssid={kwargs['ssid']} pass={kwargs['password']} bssid={kwargs['bssid']}")
        if not self.check_handle():
            print('{}'.format(self._if_state))
            return
        rsp = self._w.add_network(ssid=kwargs['ssid'],
                                      password=kwargs['password'],
                                      bssid=kwargs['bssid'])
        if rsp:
            self.set_if_state()
            self._ssid   = kwargs['ssid']
            self._passph = kwargs['password']
            self._bssid  = kwargs['bssid']
            print("add_network completed")
        else:
            print("add_network failed")

    def remove_network(self, ssid):
        if not self.check_handle():
            print('{}'.format(self._if_state))
            return
        if self._w.remove_network(ssid):
            self.set_if_state()
            if self._ssid == ssid:
                self._ssid   = None
                self._passph = None
            print("remove_network completed")
        else:
            print("remove_network failed")

    def auto_connect(self, enabled=True):
        if not self.check_handle():
            print('{}'.format(self._if_state))
            return
        print(" {}: {} {}".format('connecting to' \
                                   if enabled == True else \
                                    'disconnecting from', self._ssid, self._passph))
        status = self._w.auto_connect(enabled)
        print("auto_connect status: ", status)

    def quit(self):
        print('stopping...')
        try:
            for app in self.hio_sock_appls:
                app.shutdown()
            if self.check_handle():
                self.destroy()
        except Exception as e:
            pass
        print('Bye......')
        sys.exit(1)

    def get_iface_info(self, args=[]):
        try:
            if not self.check_handle():
                print('{}'.format(self._if_state))
                return
            ifaceinfo4 = self._w.get_ifconfig4()
            ifaceinfo6 = self._w.get_ifconfig6()
            addr  = ifaceinfo4.get('addr4')
            mask  = ifaceinfo4.get('mask')
            gw    = ifaceinfo4.get('gw')
            dns   = ifaceinfo4.get('dns')
            addr6 = ifaceinfo6.get('addr6')
            ap_ch = ScanUtils.get_channel(self._ssid)
            print('wlan_iface info')
            print('----------------------------------')
            print('|AP      | {}{}'.format( self._ssid,'(Ch'+str(ap_ch)+')' \
                                          if ap_ch !=None else ''))
            print('|status  | {}'.format( self.get_if_state() ))
            print('|--------|------------------------')
            print('|hwaddr  | {}'.format(self._hwaddr))
            print('|inet    | {}'.format(addr))
            for i, ip6 in enumerate(addr6):
                if not i:
                    print('|inet6   | {} - link'.format(ip6))
                else:
                    print('|inet6   | {}'.format(ip6))
            print('|netmask | {}'.format(mask))
            print('|gateway | {}'.format(gw))
            print('|dns serv| {}'.format(dns))
            print('|addrtype| {}'.format(self.get_ipv4_type() ))
            print('----------------------------------')
            print('get_iface_info status: {}'.format("OK"))
        except Exception as e:
            print('get_iface_info: {}'.format(str(e)))
            return

    def set_iface_info4(self, ip4addrs):
        try:
            if not self.check_handle():
                print('{}'.format(self._if_state))
                return
            if self.get_if_state() == IfState.NONE:
                print('  no interface exists')
                return
            if self.get_ipv4_type() == IPv4Type.DYNAMIC:
                print("  {} ==> DHCP already running!".format(self.get_ipv4_type()))
                return
            if self.get_ipv4_type() == IPv4Type.NONE or self.get_ipv4_type() == IPv4Type.STATIC:
                if self._w.set_ifconfig4(*ip4addrs):
                    self.set_ipv4_type(IPv4Type.STATIC)
                    print('set_iface_info4 status : {}'.format("OK"))
                    self.get_iface_info(args=[])
        except Exception as e:
            print('set_iface_info exception:{}'.format(str(e)))
            return

    def set_iface_info6(self, addr6, addr6_idx=1):
        try:
            if not self.check_handle():
                print('{}'.format(self._if_state))
                return
            if self.get_if_state() == IfState.NONE:
                print('  no interface exists')
                return
            if self._w.set_ifconfig6(addr6, addr6_idx):
                    print('set_iface_info6 status : {}'.format("OK"))
                    self.get_iface_info(args=[])
        except Exception as ex:
            print('set_iface_info6 failed: {}'.format(ex))

    def dns_resolve(self, host, ai_family=4):
        try:
            if not self.check_handle():
                print('{}'.format(self._if_state))
                return
            if self.get_if_state() == IfState.CONNECTED:
                r = self._dev.call(sock.resolve, ai_family, bytes(host,'ascii'))
                print('dns_resolve status: {}'.format('OK' if r.status== 0 else 'FAIL'))
                if r.status == 0:
                    addr = IPv4Address(bytes(r.ipaddr[:4])) if r.addrlen == 4 else \
                           IPv6Address(bytes(r.ipaddr[0:]))
                    print('{} @ {}'.format(host, str(addr)))
            else:
                print('not connected: {}'.format(self.get_if_state()))
        except Exception as e:
            print('dns_resolve exception:{}'.format(str(e)))
            return

    def txpower(self, tx_power=20):
        try:
            if not self.check_handle():
                print('  {}'.format(self._if_state))
                return
            if tx_power < -10 or tx_power > 20:
                print('invalid TX power in dBm! value in [-10...20(max)]')
                return
            self._w.set_tx_power(tx_power)
            return
        except Exception as e:
            print('txpower exception:{}'.format(str(e)))
            return

    def regdomain(self, domain="FCC"):
        try:
            if not self.check_handle():
                print('  {}'.format(self._if_state))
                return
            if domain.encode('utf-8') not in {b'FCC', b'ETSI', b'TELEC', b'KCC', b'SRCC'}:
                print('invalid domain! Use one of FCC, ETSI, TELEC, KCC, SRCC')
                return
            self._w.set_reg_domain(domain)
            return
        except Exception as e:
            print('regdomain exception:{}'.format(str(e)))
            return

    def pmconfig(self, listen_int, traffictmo, ps_poll, dyn_listen):
        try:
            if listen_int < 0:
                print('invalid listen interval [min= 0, max=65535, default=0]')
                return
            if traffictmo < 0:
                print('invalid traffic timeout interval [min=0, max=undef, default=12]')
                return
            if ps_poll not in [0, 1] or dyn_listen not in [0, 1]:
                print('invalid ps_poll or dyn_listen [must be 0 or 1]')
                return
            if not self.check_handle():
                print('{}'.format(self._if_state))
                return
            if self.get_if_state() == IfState.CONNECTED:
                self._w.set_pm_config(listen_int, traffictmo, ps_poll, dyn_listen)
            else:
                print('not connected: {}'.format(self._if_state))
        except Exception as e:
            print('pmconfig exception:{}'.format(str(e)))
            return

    def enable_suspend(self, on_off):
        try:
            self._dev.enable_suspend(on_off)
        except Exception as e:
            print('enable_suspend exception:{}'.format(str(e)))
            return

    def counters(self):
        try:
            if not self.check_handle():
                print('{}'.format(self._if_state))
                return
            if self.get_if_state() == IfState.CONNECTED:
                r = self._w.get_counters()
                for f in r._fields:
                    print("{:>10} {}".format(f.name, eval("r.{}".format(f.name))))
            else:
                print('not connected: {}'.format(self._if_state))
        except Exception as e:
            print('counters exception:{}'.format(str(e)))
            return

    def rssi(self):
        try:
            if not self.check_handle():
                print('{}'.format(self._if_state))
                return
            if self.get_if_state() == IfState.CONNECTED:
                print("rssi:{}".format(self._w.get_rssi()))
                return
            else:
                print('{} not connected..'.format(self._if_state))
            return
        except Exception as e:
            print('rssi exception:{}'.format(str(e)))
            return

    '''
    Socket API
    '''
    def is_valid_ip(self, ip):
        try:
            ip_address(ip)
            return True
        except Exception as ipe:
            pass
            return False

    def ping(self, host, count=3, interval=1.0, size=32):
        if not self.check_handle():
            print('{}'.format(self._if_state))
            return
        if self.get_if_state() != IfState.CONNECTED:
            print('not connected: {}'.format(self.get_if_state()))
            return
        try:
            if interval < 0:
                print("ping: invalid interval {} [interval >=0]".format(interval))
                return
            if count < 1:
                print('ping: invalid count {} [count >=1]'.format(count))
                return
            if size < 0:
                print("ping: invalid size {} [size >=0]".format(size))
                return
            if size > self._dev.hiomaxsize:
                print('ping: message too large ({} > hio-maxsize({}) )'.format(size, self._dev.hiomaxsize))
                print('[INFO]: reboot the device using BootArg hio.maxsize=X (X - new Max HIO packet size)')
                return
            if self.is_valid_ip(host):
                qry = rping.Ping(self._dev)
                qry.ping(host, count, interval, size)
                return
            else:
                r = self._dev.call(sock.resolve, 46, bytes(host, 'ascii'))
                if r.status == 0:
                    rhost  = str(IPv4Address(bytes(r.ipaddr[:4]))) if r.addrlen == 4 else \
                             str(IPv6Address(bytes(r.ipaddr[0:])))
                    qry = rping.Ping(self._dev)
                    qry.ping(rhost, count, interval, size)
                else:
                    print('ping failed: cannot resolve remote host:{}'.format(host))
        except Exception as ex:
            print('ping failed: {}'.format(str(ex)))
            return

    def http(self, opt):
        try:
            if not self.check_handle():
                print('not interface {}'.format(self._if_state))
                return
            if self.get_if_state() != IfState.CONNECTED:
                print('not connected: {}'.format(self.get_if_state()))
                return
            if opt.exit:
                while self.hio_sock_appls:
                    app = self.hio_sock_appls.pop()
                    app.shutdown()
                    time.sleep(0.5)
                return
            if opt.appl in ['s', 'srv', 'server']:
                try:
                    for app in self.hio_sock_appls:
                        if app.is_server:
                            print('a server is already running @ {} port {}'.format(app.hio_host_ip, app.hio_host_port))
                            return
                    app = HIOTCPSocket(self._dev, self._w, host=None,
                                    port=opt.port, is_server=True, family=opt.family)
                    '''Start Server'''
                    app.do_run()
                    self.hio_sock_appls.append(app)
                except Exception as ex:
                    print('http server exception: {}'.format(str(ex)))
                return
            if opt.appl in ['c', 'cli', 'client']:
                try:
                    print('...resolving {} @ {}'.format(opt.host, opt.port))
                    remotehost = opt.host
                    rsp = self._dev.call(sock.resolve, opt.family, bytes(opt.host, 'ascii'))
                    if rsp.status == 0:
                        rhost_ip = str(IPv4Address(bytes(rsp.ipaddr[:4]))) if rsp.addrlen == 4 else \
                                        str(IPv6Address(bytes(rsp.ipaddr[0:])))
                        print('OK... connecting to:{}'.format(rhost_ip))
                        app = HIOTCPSocket(self._dev, self._w, host=rhost_ip,
                                        port=opt.port, is_server=False, family=opt.family)
                        '''Start Client'''
                        app.set_cli_rhost(remotehost) #MUST BE SET for HTTP REQUEST
                        app.do_run()
                        app.shutdown()
                    else:
                        print('resolve to {} failed'.format(remotehost))

                except Exception as ex:
                    print('http client exception: {}'.format(str(ex)))
                    return
        except Exception as e:
            print('http exception: {}'.format(str(e)))
            return
''''''
class RangeChecker:
    @staticmethod
    def check(min_value, max_value):
        from argparse import ArgumentTypeError
        def range_checker(arg):
            try:
                value = int(arg)
            except ValueError:
                raise ArgumentTypeError(f'invalid value {arg}')
            if min_value <= value <= max_value:
                return value
            else:
                raise ArgumentTypeError(f'value {arg} not in range [{min_value}-{max_value}]')
        return range_checker
    @staticmethod
    def check_rate():
        class hexint(int):
            def __repr__(self):
                return hex(self)
        rates_11b = tuple(hexint(r) for r in list(range(0, 3+1)))
        rates_11g = tuple(hexint(0x100+r) for r in list(range(0, 7+1)))
        rates_11n = tuple(hexint(0x200+r) for r in list(range(0, 7+1)))
        return rates_11b+rates_11g+rates_11n

'''Simple Completer'''
cmd_options=[]
class SimpleCompleter(object):
    '''Simple Completer'''
    def __init__(self, options):
        self.options = sorted(options)
        return
    def complete(self, text, state):
        response = None
        if state == 0:
            # This is the first time for this text, so build a match list.
            if text:
                self.matches = [s
                                for s in self.options
                                if s and s.startswith(text)]
            else:
                self.matches = self.options[:]
        # Return the state'th item from the match list, if we have that many.
        try:
            response = self.matches[state]
        except IndexError:
            response = None
        return response+' ' if response else response

'''Subparsers'''
class ArgparseExitError(Exception):
    pass

class CustomArgumentParser(ArgumentParser):
    def exit(self, status=0, message=None):
        if message:
            self._print_message(message, sys.stderr)
        raise ArgparseExitError(status, message)
    pass

class BaseCommand():
    @classmethod
    def add_arguments(cls, parser):
        subparser = parser.add_subparsers(dest='cmd')
        work = cls.__subclasses__()
        while work:
            c = work.pop()
            work.extend(c.__subclasses__())
            parser = subparser.add_parser(c.__name__.lower(), help=c.__doc__)
            parser.formatter_class=RawTextHelpFormatter
            parser.set_defaults(func=c.execute)
            c.add_arguments(parser)
            '''Store names of commands here'''
            cmd_options.append(c.__name__.lower())
            pass
        pass
    pass

class Create(BaseCommand):
    '''create a wifi interface'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='create'
        parser.add_argument('hwaddr',
                            help='hardware address for interface',
                            nargs='?')
        parser.description='Creates a wifi interface with an optional <hwaddr>'
        parser.epilog='\n'.join(['eg:',' create', ' create 02:03:04:05:06:fb'])
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.create(opt.hwaddr)
        return
    pass

class Destroy(BaseCommand):
    '''Destroys an interface'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='destroy'
        parser.epilog='\n'.join(['eg:',' destroy'])
        parser.description='Destroys a wifi interface'
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.destroy()
        return
    pass

class Scan(BaseCommand):
    '''Scan for Access Points'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='scan'
        parser.add_argument('ssid', help='SSID of AP to scan', nargs='?')
        parser.add_argument('-n', '--num_probes', type=RangeChecker.check(1, 2), default=2,
                            metavar='[1-2]', help='number of probes in active scan, default=%(default)d')
        parser.add_argument('-i', '--idleslots', type=RangeChecker.check(1, 8), default=3,
                            metavar='[1-8]', help='number of idle slots to decide if we keep listening, default=%(default)d')
        parser.add_argument('-b', '--min_listen_time', type=RangeChecker.check(4, 24), default=8,
                            metavar='[4-24]', help='min listen time, default=%(default)d')
        parser.add_argument('-e', '--max_listen_time', type=RangeChecker.check(24, 1000), default=24,
                            metavar='[24-1000]', help='max listen time, default=%(default)d')
        parser.add_argument('-t', '--probe_tx_timeout', type=RangeChecker.check(4, 8), default=4,
                            metavar='[4-8]', help='probe request tx timeout, default=%(default)d')
        parser.add_argument('-w', '--wait_time', type=RangeChecker.check(0, 50), default=0,
                            metavar='[0-50]',help='wait time, default=%(default)d')
        parser.add_argument('-m', '--max_responses', type=RangeChecker.check(0, 1000), default=0,
                            metavar='[0-1000]', help='max number of responses, default=%(default)d')
        parser.add_argument('-r', '--rate', type=lambda x :int(x, 0), choices=RangeChecker.check_rate(), default=256,
                            metavar='[11b:0x0-0x3, 11g:0x100-0x107, 11n:0x200-0x207]', help='rate, default=%(default)d')
        parser.add_argument('-s', '--show', action='store_true', default=False,
                            help='show default scan_param')
        parser.add_argument('-p', '--passive', action='store_true', default=False,
                            help='Passive scan mode')
        parser.description='Scans for Access Points'
        parser.epilog='\n'.join(['eg:',
                                 ' scan',
                                 ' scan --min_listen_time 10 --max_listen_time 30',
                                 ' scan -b10 -e30     : same as above',
                                 ' scan --idleslots 4 #or scan -i4',
                                 ' scan -b10 -e30 -i3 : use idleslots=3',
                                 ' scan -b10 -e30 -w50: with wait_time=50ms between channels',
                                 ' scan --max_responses 10 #or scan -m10',
                                 ' scan innophi_AP',
                                 ' scan --rate 0x100  : scan -r 0x100 # scan with specific rate',
                                 ' scan -n1           : send 1 probe request(active scan)',
                                 ' scan -p            : passive scanning (scan --passive)',
                                 ' scan -p -e250      : passive scanning with max_listen_time=250',
                                 ' scan -p -e250 -w100: passive scanning max_listen_time=250, wait_time=100',
                                 ' scan --show        : displays default scan params'])
        return
    @classmethod
    def execute(cls, opt, cli):
        scan_param=dict(num_probes=opt.num_probes,
                        idleslots=opt.idleslots,
                        min_listen_time=opt.min_listen_time,
                        max_listen_time=opt.max_listen_time,
                        probe_tx_timeout=opt.probe_tx_timeout,
                        wait_time=opt.wait_time,
                        max_responses=opt.max_responses,
                        rate=0xffff if opt.passive else opt.rate)
        if opt.passive:
            scan_param.update(max_listen_time=opt.max_listen_time if opt.max_listen_time != 24 else 125)
        cli.scan(opt.ssid if opt.ssid else "", opt.show, **scan_param)
        return
    pass

class Add_Network(BaseCommand):
    '''Connect asynchronously to a network'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='add_network'
        parser.add_argument('ssid', help='AP ssid')
        parser.add_argument('-p', '--password', help='Optional passphrase/password')
        parser.add_argument('--bssid', default=None, help='BSSID')
        parser.description='Add a network asynchronously to AP with <ssid> and an optional \
                            <passphrase/password> for WEP/TKIP/WPA-PSK/WPA2-PSK and WPA3-SAE'
        parser.epilog='\n'.join(['eg:',
                                 ' add_network "<ssid>"',
                                 ' add_network "<ssid>" -p<passphrase or password>',
                                 ' add_network "<ssid>" --password <passphrase or password>',
                                 ' add_network "<ssid>" -p<passphrase or password> --bssid <BSSID>'])
        return
    @classmethod
    def execute(cls, opt, cli):
        addnet_param=dict(ssid=opt.ssid, password=opt.password, bssid=opt.bssid)
        cli.add_network(**addnet_param)
        return
    pass

class Remove_Network(BaseCommand):
    '''Connect asynchronously to a network'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='remove_network'
        parser.add_argument('ssid', help='AP ssid')
        parser.description='Disconnect from AP with <ssid>'
        parser.epilog='\n'.join(['eg:', ' remove_network "<ssid>"'])
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.remove_network(opt.ssid)
        return
    pass

class Clear(BaseCommand):
    '''Clear Console'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='clear'
        parser.description='Clears Console'
        parser.epilog='\n'.join(['eg:', ' clear'])
    @classmethod
    def execute(cls, opt, cli=None):
        import subprocess as sp
        sp.call('clear',shell=True)
        return
    pass

class Auto_connect(BaseCommand):
    '''Connects and Disconnects device from AP'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='auto_connect'
        parser.add_argument('-e', '--enabled', type=int, default=1, choices=[0,1],
                            help='connects to a network, default=%(default)d')
        parser.description='Connects and disconnects a device to a network (AP)'
        parser.epilog='\n'.join(['eg:',
                                 ' auto_connect : connects to a network',
                                 ' auto_connect -e1 : same..',
                                 ' auto_connect -e0 : disconnects',
                                 ' auto_connect --enable 1 : connects',
                                 ' auto_connect --enable 0 : disconnects'])
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.auto_connect(True if opt.enabled == 1 else False)
        return
    pass

class Quit(BaseCommand):
    '''Quits application'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='quit'
        parser.description='Quit CLI application'
        parser.epilog='\n'.join(['eg: quit'])
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.quit()
        return
    pass

class Get_Iface_Info(BaseCommand):
    '''Returts the interface information'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='get_iface_info'
        parser.description='Returts wifi interface information'
        parser.epilog='\n'.join(['eg:',' get_iface_info'])
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.get_iface_info()
        return
    pass

class Set_Iface_Info4(BaseCommand):
    '''Sets interface information'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='set_iface_info4'
        parser.add_argument('ip4addrs', nargs=4,
                            help='IPv4 address, Netmask, Gateway and DNS Server')
        parser.description='set wifi iface info  with <IPv4> <Netmask> <Gateway> and <DNS>'
        parser.epilog='\n'.join(['eg:',
                                 ' set_iface_info4 172.28.49.173 255.255.255.0 172.28.49.1 172.28.49.1'])
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.set_iface_info4(opt.ip4addrs)
        return
    pass

class Set_Iface_Info6(BaseCommand):
    '''Sets interface information'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='set_iface_info6'
        parser.add_argument('ip6addr', help='IPv6 address')
        parser.add_argument('-i', '--index', default=1, type=int, choices=[0, 1, 2],
                            help='IPv6 addr number [0 is link-local, default=%(default)d]')
        parser.description='set wifi iface info  with <IPv6> address'
        parser.epilog='\n'.join(['eg:',
                                 ' set_iface_info6 fe80::56ee:75ff:febc:c5b8 -i1 : set IPv6 addr number 1'])
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.set_iface_info6(opt.ip6addr, addr6_idx=opt.index)
        return
    pass

class DNS_Resolve(BaseCommand):
    '''Returns the IPv4 address of <hostname> using options'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='dns_resolve'
        parser.description='Returns the IPv4/IPv6 address of <host>'
        parser.add_argument('host', help='the hostname to query')
        parser.add_argument('-v', '--family', default=4,
                            type=int, choices=[4,6,46,64], help='IP Address type [default=%(default)d]')
        parser.epilog='\n'.join(['eg:',
                                 ' dns_resolve www.innophaseinc.com',
                                 ' dns_resolve -v6 <host>  : returns IPv6 address of <host>',
                                 ' dns_resolve -v64 <host> : returns IPv6 address of <host>, if none returns IPv4',
                                 ' dns_resolve -v46 <host> : returns IPv4 address of <host>, if none returns IPv6'])
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.dns_resolve(opt.host, ai_family=opt.family)
        return
    pass

class TXPower(BaseCommand):
    '''Set Tx power'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='txpower'
        parser.add_argument('tx_power',
                            type=int, default=20,
                            help='TX power in dBm [-10..20(max))  (default: %(default)d]')
        parser.description='Set TX power'
        parser.epilog='\n'.join(['eg:',
                                 ' txpower <tx_power>'])
        pass
    @classmethod
    def execute(cls, opt, cli):
        cli.txpower(opt.tx_power)
        return
    pass

class RegDomain(BaseCommand):
    '''Set regulatory domain'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='domain'
        parser.add_argument('domain',
                            default='FCC',
                            type=str, choices=["FCC", "ETSI", "TELEC", "KCC", "SRCC"],
                            help='FCC, ETSI, TELEC, KCC, SRCC (default: %(default)s]')
        parser.description='Set regulatory domain'
        parser.epilog='\n'.join(['eg:',
                                 ' regdomain <domain>'])
        pass
    @classmethod
    def execute(cls, opt, cli):
        cli.regdomain(opt.domain)
        return
    pass

class Suspend(BaseCommand):
    '''Enable or disable device suspend'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='suspend'
        parser.description='Enable/disable device suspend'
        parser.add_argument('on_off', type=int, help='1=enable 0=disable')
        parser.epilog='\n'.join(['eg: suspend <0/1>'])
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.enable_suspend(opt.on_off)
        return
    pass

class PMConfig(BaseCommand):
    '''Sets the <listen> and <traffic timeout> intervals and enables <ps_poll> and <dyn_listen>'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='pmconfig'
        parser.description='Sets the <listen> and <traffic timeout> intervals and enables <ps_poll> and <dyn_listen>'
        parser.add_argument('listen',
                            type=int, default=0, help='Listen interval [0..65535(max), default=%(default)d]')
        parser.add_argument('traffictmo',
                            type=int, default=12, help='Traffic timeout interval [0...inf, default=%(default)d]')
        parser.add_argument('ps_poll', type=int, help='Set 1 to use ps poll when beacon was missed')
        parser.add_argument('dyn_listen', type=int, help='Set 1 to listen to all beacons if there was traffic recently')
        parser.epilog='\n'.join(['eg: pmconfig <listen> <traffic_timeout> <ps_poll> <dyn_listen>'])
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.pmconfig(opt.listen, opt.traffictmo, opt.ps_poll, opt.dyn_listen)
        return
    pass

class Counters(BaseCommand):
    '''Returns wifi statistics'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='counters'
        parser.description='Returns wifi statistics'
        parser.epilog='\n'.join(['eg: counters'])
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.counters()
        return
    pass

class RSSI(BaseCommand):
    '''Returns the RSSI - Received Signal Strength Indicator'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='rssi'
        parser.description='Returns the RSSI'
        parser.epilog='\n'.join(['eg: rssi'])
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.rssi()
        return
    pass

class HTTP(BaseCommand):
    '''Creates an HTTP [Client OR Server] depending on the options provided.'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='http'
        parser.description='Creates an HTTP [Client OR Server] depending on the options.'
        parser.add_argument('appl',  default=None,
                            choices=['c','s','cli','srv','client','server'],
                            help='run as a server/client')
        parser.add_argument('-r', '--host', default=None, help='remote http server')
        parser.add_argument('-p', '--port', default=80, type=int, help='port [default=%(default)d]')
        parser.add_argument('-v', '--family', default=4, type=int, choices=[4,6,46,64], help='IP addr type [default=%(default)d]')
        parser.add_argument('-e', '--exit', action='store_true', help='stops the http server')
        parser.epilog='\n'.join(['eg: ',
                                'http s  - creates an IPv4 HTTP server on port 80',
                                'http s -p8080 - creates an IPv4 HTTP server on port 8080',
                                'http s -v6 - creates an IPv6 HTTP server port 80'
                                'http s -e  - stops the HTTP server',
                                'http c -r www.xpto.com - connects to xpto.com on port 80',
                                'http c -r www.xpto.com -p 8080 - connects to xpto.com on port 8080',
                                'hhtp c -rfe80::56ee:75ff:febc:c54e -v6 - connect to an IPv6 HTTP server port 80'])
        return
    @classmethod
    def execute(cls, opt, cli):
        if opt.appl in ['s', 'srv', 'server']:
            cli.http(opt)
            return
        if opt.appl in ['c', 'cli', 'client']:
            if opt.host:
                cli.http(opt)
            else:
                print('remote host required!')
    pass

class Ping(BaseCommand):
    '''Ping a remote <host>'''
    @classmethod
    def add_arguments(cls, parser):
        parser.prog='ping'
        parser.description='ping a remote host with <IPv4> addr and optional <size> <count> and <interval>'
        parser.add_argument('host', help='Remote IP address')
        parser.add_argument('-s', '--size', type=int, default=32, help='payload size [default=%(default)d]')
        parser.add_argument('-c', '--count', type=int, default=3, help='number of pings [default=%(default)d]')
        parser.add_argument('-i', '--interval',
                            type=float, default=1.0, help='interval in seconds between pings [default=%(default)d]')
        parser.epilog='\n'.join(['eg:',
                                 ' ping 172.28.49.133',
                                 ' ping 172.28.49.133 -i0.2',
                                 ' ping www.google.com -s64 -c15',
                                 ' ping 172.28.49.133 -i5 -s64 -c10'])
        return
    @classmethod
    def execute(cls, opt, cli):
        cli.ping(opt.host, count=opt.count, interval=opt.interval, size=opt.size)
        return
    pass


class Panic(BaseCommand):
    '''Force assert(0)'''
    @classmethod
    def execute(cls, opt, cli):
        cli._dev.call_async(debug.panic)
        print('target should now be in "ASSERTION FAILED:" state')


class Help(BaseCommand):
    '''Display Help Menu'''
    @classmethod
    def add_argument(cls, parser):
        pass
        return
    @classmethod
    def execute(cls, opt=None, cli=None):
        print('|==========================================================|')
        print('| ******* Talaria(TM) TWO - Command Line Interface ******* |')
        print('|==========================================================|')
        print('|help                              : Display Help Menu')
        print('|create <hwaddr>                   : Create a wifi interface with an optional <hwaddr>')
        print('|                                      eg: create 02:03:04:05:06:07')
        print('|destroy                           : Destroys an interface')
        print('|scan <ssid(o)>                    : Scan for Access Points with an optional <ssid>')
        print('|auto_connect -e<1/0>              : Connects to an AP')
        print('|add_network "<ssid>" <passphrase> : Adds asynchronously an AP with <ssid> and an optional')
        print('|                                  : <passphrase> for WEP/TKIP/WPA-PSK/WPA2-PSK')
        print('|remove_network "<ssid>"           : Disconnects from AP and removes network')
        print('|txpower <tx_power>                : Set tx power (attenuation in dB, 0 = max tx power)')
        print('|regdomain <domain>                : Set regulatory domain')
        print('|rssi                              : Returns the RSSI - Received Signal Strength Indicator')
        print('|pmconfig <listen> <traffic_tmo> <ps_poll> <dyn_listen> : Sets the <listen> and <traffic timeout> intervals and enables <ps_poll> and <dyn_listen>')
        print('|suspend <on_off>                  : Enable/disable device suspend <1=on, 0=off>')
        print('|counters                          : Returns wifi statistics')
        print('|get_iface_info                    : Returts the interface information')
        print('|set_iface_info4  <IPv4> <Netmask> <Gateway> <DNS>')
        print('|                                  : Sets interface information')
        print('|                                  :   eg: set_iface_info4 172.28.49.173 255.255.255.0 172.28.49.1 172.28.49.1')
        print('|set_iface_info6 <IPv6> <index>    : eg: set_iface_info6 fe80::56ee:75ff:febc:c5b8 -i1')
        print('|dns_resolve <hostname>            : Returns the IPv4/IPv6 address of <hostname> using options')
        print('|http                              : Creates an HTTP [Client OR Server] using the options provided.')
        print('|ping   <IPv4/IPv6>                : Ping a remote <host>')
        print('|clear                             : Clear Console')
        print('|quit                              : Quits application')
        print('|==========================================================|')
        print('|  ******* Innophase(TM) Inc. All Rights Reserved *******  |')
        print('|==========================================================|')
        return


def main():
    if len(sys.argv) < 2:
        prog = os.path.basename(sys.argv[0])
        print("usage: {} <tty> | <host>:<port> | <unix domain socket>".format(prog))
        sys.exit(1)

    path = sys.argv[1]
    ap = CustomArgumentParser()
    BaseCommand.add_arguments(ap)
    # Register our completer function
    readline.set_completer(SimpleCompleter(cmd_options).complete)
    # Use the tab key for completion
    readline.parse_and_bind('tab: complete')
    cli = StwCli(path)
    Help.execute(None,None)
    while True:
        try:
            line = input('[talaria-2]$ ')
            if line is None:
                break
            args = shlex.split(line)
            if not args:
                continue
            opt = ap.parse_args(args)
            opt.func(opt, cli)
        except ArgparseExitError:
            continue
        except KeyboardInterrupt:
            cli.quit()
            break
        pass


if __name__ == '__main__':
    main()
