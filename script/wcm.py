#!/usr/bin/env python3
import sys
import socket
import threading
import time
import hostio
from ipaddress import IPv4Address, IPv6Address
from api import base, hio, wcm
from bits.hwaddress import HWAddress
import urllib.parse

class WCMError(Exception):
    def __init__(self, *args, **kw):
        super().__init__(*args, kw)
    pass

class WCM():
    rpcs = ( wcm.if_create,
             wcm.if_destroy,
             wcm.if_connect,
             wcm.if_disconnect,
             wcm.if_scan,
             wcm.if_get_addr4,
             wcm.if_get_addr6)

    def __init__(self, dev, link_status_cb = None):
        rsp = dev.call(hio.query)
        if rsp is None or rsp.status != 0:
            raise WCMError('hio.query failed')
        if rsp.version != 0:
            raise WCMError('hio version mismatch')

        avail = { n//256: n % 256 for n in rsp.nmsg }
        req = {}

        for m in self.rpcs:
            if m.group not in avail or m.group >= avail[m.group]:
                raise WCMError(m.__module__ + ' is not available')
            pass

        self.dev = dev
        self.hwaddr = self._convert_hwaddr(None)
        self.connected = False
        self._iface_handle = None
        self.ssid = None
        self.passphrase = None
        self.link_status_cb = link_status_cb
        pass

    @staticmethod
    def _convert_hwaddr(bssid):
        if isinstance(bssid, HWAddress):
            return bssid
        if bssid is None:
            bssid = '00-00-00-00-00-00'
        if isinstance(bssid, str):
            return HWAddress.from_string(bssid)
        return HWAddress(bssid)


    def set_hwaddr(self, hwaddr):
        self.hwaddr = self._convert_hwaddr(hwaddr)

    @property
    def handle(self):
        if self._iface_handle is None:
            rsp = self.dev.call(wcm.if_create, self.hwaddr)
            if rsp.handle != 0:
                self._iface_handle = rsp.handle
                self.hwaddr = HWAddress(rsp.hwaddr)
                self.dev.set_ind_handler(wcm.if_notify, self.notify_received)
        return self._iface_handle

    def notify_received(self, ind):
        if ind.type == 200:
            if self.link_status_cb is not None:
                self.link_status_cb(True, ind.type)
            print('LINK UP')
        elif ind.type == 201:
            self.connected = False
            if self.link_status_cb is not None:
                self.link_status_cb(False, ind.type)
            print('LINK DOWN')
        elif ind.type == 202:
            self.connected = True
            if self.link_status_cb is not None:
                self.link_status_cb(self.connected, ind.type)
            print('CONNECTED')

    def close(self):
        self.auto_connect(False)

        rsp = self.dev.call(wcm.if_destroy, self.handle)
        if rsp.status != 0:
            return False
        self._iface_handle = None
        return True

    def scan(self, **kwargs):
        param = dict(ssid=b'',
                     num_probes=2,
                     idleslots=3,
                     min_listen_time=8,
                     max_listen_time=24,
                     probe_tx_timeout=4,
                     wait_time=75,
                     rate=0x100,
                     max_responses=0)
        param.update(kwargs)
        self.scan_result = set()
        self.dev.set_ind_handler(wcm.if_scan, self.scan_notify)
        r = self.dev.call(wcm.if_scan,
                          self.handle,
                          param['ssid'],
                          param['max_responses'],
                          param['num_probes'],
                          param['idleslots'],
                          param['min_listen_time'],
                          param['max_listen_time'],
                          param['probe_tx_timeout'],
                          param['wait_time'],
                          param['rate'])
        self.dev.set_ind_handler(wcm.if_scan, None)
        return self.scan_result

    def scan_notify(self, ind):
        self.scan_result.add(ind)

    @staticmethod
    def wifi_uri_encode(wifi_uri):
        s = urllib.parse.urlencode(wifi_uri, quote_via=urllib.parse.quote)
        s = s.replace('=', ':')
        s = s.replace('&', ';')
        return s

    def add_network(self, ssid, password=None, bssid=None):
        wifi_uri = {}
        if password is None or password == '':
            wifi_uri['S'] = ssid
        else:
            wifi_uri['T'] = 'WPA'
            wifi_uri['S'] = ssid
            wifi_uri['P'] = password

        if bssid is not None and bssid != '':
            wifi_uri['x-bssid'] = bssid

        wifi_uri = WCM.wifi_uri_encode(wifi_uri)
        wifi_uri = 'WIFI:' + wifi_uri + '\x00'
        wifi_uri = wifi_uri.encode('utf-8')

        rsp = self.dev.call(wcm.if_add_network_uri, self.handle, wifi_uri)
        return rsp.status == 0

    def auto_connect(self, enabled=True):
        rsp = self.dev.call(wcm.if_auto_connect, self.handle, {True: 1, False: 0}[enabled])
        return rsp.status == 0

    def remove_network(self, ssid, bssid=None):
        ssid = ssid.encode('utf-8')
        bssid = self._convert_hwaddr(bssid)
        rsp = self.dev.call(wcm.if_remove_network, self.handle, ssid, bssid)
        return rsp.status == 0

    def connect(self, ssid, passphrase=''):
        assert False, 'deprecated, please use add_network/auto_connect(True)'
        return False

    def disconnect(self):
        assert False, 'deprecated, please use auto_connect(False)/remove_network'
        return False

    def get_ifconfig4(self):
        rsp = self.dev.call(wcm.if_get_addr4, self.handle)
        if rsp.status == 0:
            addr = str(rsp.ipaddr4)
            mask = str(rsp.netmask)
            gw   = str(rsp.gw)
            dns  = str(rsp.dns)
        else:
            addr = mask = gw = addr = str(0)
        return dict(addr4=addr, mask=mask, gw=gw, dns=dns)

    def get_ifconfig6(self):
        rsp = self.dev.call(wcm.if_get_addr6, self.handle)
        if rsp.status == 0:
            addr6= []
            for i in range(rsp.numaddr6):
                ip6 = str(IPv6Address(bytes(rsp.ipaddr6[16*i : 16*(i+1)])))
                if ip6 != '::':
                    addr6.append(ip6)
        else:
            addr6=[str(IPv6Address(bytes([0]*16)))]
        return dict(addr6=addr6, numaddr6=rsp.numaddr6)

    def set_ifconfig4(self, addr4, mask4, gw4, dns4):
        argv = [socket.htonl(int(IPv4Address(addr))) for addr in (addr4, mask4, gw4, dns4)]
        rsp = self.dev.call(wcm.if_set_addr4, self.handle, *argv)
        return rsp.status == 0

    def set_ifconfig6(self, addr6=None, addr6_idx=1):
        if addr6 and addr6_idx in [0, 1, 2]:
            addr6 = bytes(IPv6Address(addr6).packed)
            '''add addr6 and addr6 number'''
            argv = []
            argv.append(addr6_idx)
            argv.append(addr6)
            rsp = self.dev.call(wcm.if_set_addr6, self.handle, *argv)
            return rsp.status == 0
        else:
            return False

    def set_pm_config(self, listen_interval, traffic_tmo, ps_poll, dyn_listen_int):
        rsp = self.dev.call(wcm.if_set_pmconfig, self.handle, listen_interval, traffic_tmo, ps_poll, dyn_listen_int)
        return rsp.status == 0

    def set_tx_power(self, tx_power):
        rsp = self.dev.call(wcm.if_txpower, self.handle, tx_power)
        return rsp.status == 0

    def set_reg_domain(self, domain):
        rsp = self.dev.call(wcm.set_regulatory_domain, self.handle, domain.encode('utf-8'))
        return rsp.status == 0

    def get_counters(self):
        return self.dev.call(wcm.if_get_counters, self.handle)

    def get_rssi(self):
        rsp = self.dev.call(wcm.if_get_rssi, self.handle)
        return rsp.rssi

    def get_bssid(self):
        rsp = self.dev.call(wcm.if_get_bssid, self.handle)
        return self._convert_hwaddr(rsp.bssid)

class ScanUtils:
    '''Decode a <<Scan Indication>> message'''
    channels_db = {}
    param_scan = dict(num_probes=2,
                      idleslots=3,
                      min_listen_time=8,
                      max_listen_time=24,
                      probe_tx_timeout=4,
                      wait_time=75,
                      rate=0x100,
                      max_responses=0)
    hrule = '+'+'-'.join('-' for i in range(48))+'+'

    IE_TAG_WPA1        = 0xdd
    IE_TAG_WPA2        = 0x30
    IE_NOT_SUPPORTED   = 0x0000
    IE_WPA_PERSONAL    = 0x0001
    IE_WPA2_PERSONAL   = 0x0002
    IE_WPA3_PERSONAL   = 0x0004
    IE_WPA_ENTERPRISE  = 0x0008
    IE_WPA2_ENTERPRISE = 0x0010
    IE_WPA3_ENTERPRISE = 0x0020
    IE_WPA2_MGMT_FR_PR = 0x0040
    IE_PROTECT_BIT_CAP = 0x0080

    def __init__(self):
        pass

    @staticmethod
    def print_scan(scan=None):
        if scan == None:
            header = "|{:<18}{:<28} {:<5} {:<9} {:<20}{:>13}".format('BSSID',
                                                            'SSID',
                                                            'CHAN',
                                                            'RSSI',
                                                            'SECURITY','|')
            print(ScanUtils.hrule)
            print(header)
            print(ScanUtils.hrule)
        else:
            print('{:19}{:<25} {:5d}  {:5d}\t{}'.format(scan['bssid'],
                                                scan['ssid'],
                                                scan['chan'],
                                                scan['rssi'],
                                                scan['authmode']))

    @classmethod
    def channels_flush(cls):
        cls.channels_db ={}

    @classmethod
    def channels_update(cls, msg, scan):
        msg.chan = scan['chan']
        msg.ssid = scan['ssid']
        cls.channels_db[scan['bssid']] = msg

    @classmethod
    def lookup(cls, ssid):
        for bssid, msg in cls.channels_db.items():
            if msg.ssid == ssid:
                return msg
        return None

    @classmethod
    def get_channel(cls, ssid):
        if ssid == None:
            return None
        msg = cls.lookup(ssid)
        return msg.chan if msg != None else None

    @staticmethod
    def ie_locate(ie, ietag_id):
        pos = 0
        while pos < len(ie):
            ietag = ie[pos]
            ielen = ie[pos+1]
            if ielen + pos > len(ie):
                print("Malformed IE @ {} (tag={} len={})".format(pos, ietag, ielen))
                return None
            if ietag == ietag_id:
                return ie[pos+2:pos+2+ielen]
            pos += ielen + 2
        return None

    @staticmethod
    def ie_rates(msg):
        rate = ScanUtils.ie_locate(msg.ielist, 1)
        return b'' if rate is None else rate

    @staticmethod
    def ie_channel(msg):
        ds = ScanUtils.ie_locate(msg.ielist, 3)
        return 0 if ds is None else ds[0]

    @staticmethod
    def ie_ssid(msg):
        ssid = ScanUtils.ie_locate(msg.ielist, 0)
        return b'' if ssid is None else ssid

    @staticmethod
    def ie_bssid(msg):
        bssid = ':'.join(["{:02x}".format(x) for x in msg.bssid])
        return bssid

    @staticmethod
    def ie_authmode_tostr(authmask=0x0000):
        auth_names = [
        "WPA-PSK",
        "WPA2-PSK",
        "WPA3-SAE",
        "WPA-Enterprise",
        "WPA2-Enterprise",
        "WPA3-Enterprise"]
        modename, comma = ('', '')
        if authmask == 0:
            return "OPEN"
        if authmask == ScanUtils.IE_PROTECT_BIT_CAP:
            return "WEP?"

        mfp = authmask & ScanUtils.IE_WPA2_MGMT_FR_PR
        authmask &= ~ScanUtils.IE_WPA2_MGMT_FR_PR
        mixedmode = False
        for i in range(len(auth_names)):
            mfpstr = ''
            authbit = authmask & (1 << i);
            if mfp:
                if authbit == ScanUtils.IE_WPA2_PERSONAL \
                    or authbit == ScanUtils.IE_WPA2_ENTERPRISE:
                    mfpstr ='+PMF'
            if authbit and i < len(auth_names):
                mixedmode = True if comma == '/' else False
                modename += comma + auth_names[i] + mfpstr
                comma = '/'
        return modename + str(' Mixed Mode' if mixedmode else '')

    @staticmethod
    def ie_authmode_decode(buf, buflen, authmask):
        '''
        Derived from Linux iwlist
        FIXME: compress code.
        '''
        wpa_type = 0
        ielen  = buf[1] + 2
        offset = 2
        wpa_oui = ""
        wpa1_oui, wpa2_oui = ([0x00, 0x50, 0xf2], [0x00, 0x0f, 0xac])

        if ielen > buflen:
            ielen = buflen
        #WPA2
        if buf[0] == ScanUtils.IE_TAG_WPA2:
            if ielen < 4:
                return authmask
            wpa_oui = wpa2_oui
        elif buf[0] == ScanUtils.IE_TAG_WPA1:
            wpa_oui = wpa1_oui
            '''
            Not all IEs that start with 0xdd are WPA.
            So check that the OUI is valid. Note : offset=2
            '''
            if ielen < 8 or wpa_oui != list(buf[offset:offset+3]) or buf[offset+3] != 0x01:
                return authmask
            '''Skip the OUI type'''
            offset +=4
        else:
            return authmask

        '''Skip version number'''
        offset +=2
        if buf[0] == ScanUtils.IE_TAG_WPA1:
            wpa_type = 1
        if buf[0] == ScanUtils.IE_TAG_WPA2:
            wpa_type = 2
        '''Check if we are done'''
        if ielen < (offset + 4):
            return authmask

        '''Skip wpa_mcipher'''
        offset += 4
        '''Check if we are done'''
        if ielen < (offset + 2):
            return authmask
        '''extract number of pairwise ciphers.'''
        wpa_uciphercnt = buf[offset] | (buf[offset + 1] << 8)
        offset +=2

        if ielen < (offset + 4*wpa_uciphercnt):
            return authmask

        '''Skip number of pairwise ciphers'''
        offset += 4*wpa_uciphercnt
        '''Check if we are done'''
        if ielen < (offset + 2):
            return authmask

        '''get number of authentication suites'''
        wpa_authselcnt = buf[offset] | (buf[offset+1] << 8)
        offset +=2

        if ielen < (offset + 4*wpa_authselcnt):
            return authmask

        for i in range(wpa_authselcnt):
            if wpa_oui == list(buf[offset:offset+3]):
                akm_type = buf[offset + 3]
                if wpa_type == 1 and  akm_type in [1, 2]:
                    if akm_type == 1:
                        authmask |= ScanUtils.IE_WPA_ENTERPRISE
                    else:
                        authmask |= ScanUtils.IE_WPA_PERSONAL
                if wpa_type == 2 and akm_type in [1, 2]:
                    if akm_type == 1:
                        authmask |= ScanUtils.IE_WPA2_ENTERPRISE
                    else:
                        authmask |= ScanUtils.IE_WPA2_PERSONAL
                #FIXME:
                if wpa_type == 2 and akm_type in [4, 5, 6, 8]:
                    if akm_type == 8:
                        authmask |= ScanUtils.IE_WPA3_PERSONAL
            #increment offset
            offset += 4
        '''Check if we are done'''
        if ielen < (offset + 2):
            return authmask

        #print("WPA_CAPS: ", bin(buf[offset]), '-', bin(buf[offset+1]))
        if wpa_type == 2 and buf[offset] & 0x8:
            authmask |= ScanUtils.IE_WPA2_MGMT_FR_PR
        if buf[offset] & 0x4:
            authmask |= ScanUtils.IE_PROTECT_BIT_CAP

        return authmask

    @staticmethod
    def ie_authmode(msg):
        pos = 0
        ie = msg.ielist
        authmask = 0x0000
        buflen = len(msg.ielist)
        ietag_ids = (ScanUtils.IE_TAG_WPA2,  ScanUtils.IE_TAG_WPA1)
        while pos < (buflen - 2):
            ietag = ie[pos]
            ielen = ie[pos+1]
            if ietag in ietag_ids:
                iebuf     = ie[pos:]
                ie_buflen = len(iebuf)
                authmask  = ScanUtils.ie_authmode_decode(iebuf, ie_buflen, authmask)
            pos += ielen + 2
        return authmask

    @staticmethod
    def decode(msg):
        bssid    = ScanUtils.ie_bssid(msg)
        ssid     = ScanUtils.ie_ssid(msg).decode('utf-8')
        chan     = ScanUtils.ie_channel(msg)
        rssi     = msg.rssi
        authmask = ScanUtils.ie_authmode(msg)
        authmode = ScanUtils.ie_authmode_tostr(authmask)
        return dict(bssid=bssid,
                    ssid =ssid,
                    chan =chan,
                    rssi =rssi,
                    authmode=authmode)

    @staticmethod
    def scan_call_wcm(w, scan_ssid):
        try:
            r = w.scan(ssid=bytes(scan_ssid, 'ascii'), **ScanUtils.param_scan)
            return r
        except Exception as ex:
            print('...scan wcm:', str(ex))
        return None

    @classmethod
    def scan(cls, w, scan_ssid="", **kwargs):
        cls.param_scan.update(kwargs)
        print('{} scanning '.format('Passive' \
            if cls.param_scan['rate'] == 0xffff\
            else 'Active'), kwargs)

        resp = cls.scan_call_wcm(w, scan_ssid)
        cls.print_scan()
        if resp == None:
            print(cls.hrule)
            return

        cls.channels_flush()
        while len(resp) > 0:
            try:
                msg  = resp.pop()
                scan = cls.decode(msg)
                cls.channels_update(msg, scan)
                cls.print_scan(scan=scan)
            except Exception as ex:
                print("scan....", str(ex))
        print(cls.hrule)


def main():
    from argparse import ArgumentParser

    ap = ArgumentParser()
    ap.add_argument(dest='device',
                    help='<tty> | <host>:<port> | <unix domain socket>')
    ap.add_argument('--speed', type=int, default=921600,
                    help='Use specified baudrate for serial device')
    ap.add_argument('--repeat', type=int, default=1,
                    help='Repeat connect/disconnect this many times')
    ap.add_argument('ssid', default='innophi', help='SSID to connect to')
    ap.add_argument('passphrase', nargs='?',
                    help='Passphrase (WPA) or Key (WEP) to use')

    opt = ap.parse_args()

    dev = hostio.open(opt.device, speed=opt.speed, registry=base.registry)
    if dev is None:
        print('{}: No such file'.format(opt.device))
        sys.exit(1)

    w = WCM(dev)
    w.connect(opt.ssid, opt.passphrase)
    print('Connected to {!r}'.format(opt.ssid))
    try:
        while True:
            cmd = input('> ')
            if len(cmd) == 0:
                pass
            elif cmd.startswith('q'):
                break
            elif cmd.startswith('d'):
                w.disconnect()
            elif cmd.startswith('c'):
                w.connect(opt.ssid, opt.passphrase)
            else:
                pass
    except (KeyboardInterrupt, EOFError) as e:
        pass

    w.disconnect()
    w.close()

    sys.exit(0)


if __name__ == '__main__':
    main()
