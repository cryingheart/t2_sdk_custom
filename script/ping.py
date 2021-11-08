#!/usr/bin/env python3
import sys
import time
import struct
import hostio
import statistics as pstats
from api import base, hio, sock
from wcm import WCMError, WCM
import threading
from ipaddress import ip_address, IPv4Address, IPv6Address
from argparse import ArgumentParser
from enum import IntEnum

class ICMP_Type(IntEnum):
    ECHO_REQUEST = 8
    ECHO_REPLY   = 0

class ICMPv6_Type(IntEnum):
    ECHO_REQUEST = 128
    ECHO_REPLY   = 129

class ICMP_IPHdr(IntEnum):
    VERSION     = 4
    ADDR_LEN    = 4
    HDR_LEN     = 20
    PROTO_ICMP  = 1

class ICMPv6_IPHdr(IntEnum):
    VERSION     = 6
    ADDR_LEN    = 16
    HDR_LEN     = 40
    PROTO_ICMPv6= 58

class ICMP:
    def __init__(self, **kwargs):
        if 'buf' in kwargs:
            buf = kwargs.get('buf')
            hdr = struct.unpack(">BBHHH", buf[:8])
            self.payload = buf[8:]
            self.type = hdr[0]
            self.code = hdr[1]
            self.cksum = hdr[2]
            self.id = hdr[3]
            self.seq = hdr[4]
            pass
        else:
            self.type = kwargs.get('type', 8)
            self.code = kwargs.get('code', 0)
            self.seq = kwargs.get('seq', 1)
            self.id = kwargs.get('id', 0x1234)
            self.payload = kwargs.get('payload', bytes(i%256 for i in range(kwargs.get('size', 32))))
            hdr = struct.pack(">BBHHH", self.type, self.code, 0, self.id, self.seq)
            if self.type == ICMPv6_Type.ECHO_REQUEST:
                '''
                TODO: compute correct checksum with pseudo-header
                NOTE: RFC2460 is obsoloted by RFC8200
                NOTE: RFC2463 is obsoloted by RFC4443
                '''
                ip6zero = IPv6Address(bytes([0]*16))
                ip6src, ip6dst = kwargs.get('ip6src', ip6zero), kwargs.get('ip6dst', ip6zero)
                icmpv6len      =  40 + len(hdr) + len(self.payload)
                pseudohdr = ip6src.packed + ip6dst.packed + struct.pack(">IHBB", icmpv6len, 0, 0, 58)
                #print('pseudohdr:{}'.format(list(pseudohdr)))
                cksum = 0#ICMP.checksum(pseudohdr + hdr + self.payload)
                #cksum = ICMP.checksum(hdr, cksum)
                #cksum = ICMP.checksum(self.payload, cksum)
            else:
                cksum = ICMP.checksum(hdr)
                cksum = ICMP.checksum(self.payload, cksum)
            self.buffer = struct.pack(">BBHHH", self.type, self.code, ~cksum & 0xffff, self.id, self.seq) + self.payload
        return

    @staticmethod
    def checksum(buf, s=0):
        length = len(buf)
        pad = b'\x00' if length % 2 != 0 else b''
        length += len(pad)
        s += sum(struct.unpack(">{:d}H".format(length//2), buf + pad))
        s = ((s >> 16) & 0xffff) + (s & 0xffff)
        return s

    def __len__(self):
        return len(self.buffer)

    def __bytes__(self):
        return self.buffer

    def __repr__(self):
        return 'ICMP[type={},code={},id={},seq={}]'.format(self.type,self.code,self.id, self.seq)

class Ping:
    def __init__(self, dev):
        self.dev = dev
        self.ready_sem = threading.Semaphore(0)
        self.socket = None
        self.tracker = {}
        self.dev.set_ind_handler(sock.notification, lambda msg:self.ready_sem.release())

    def setup(self, host, psize=32):
        try:
            self.ip_version = ip_address(host).version
            assert self.ip_version in  [ICMP_IPHdr.VERSION, ICMPv6_IPHdr.VERSION]
            if self.ip_version == ICMPv6_IPHdr.VERSION:
                self.ip6dst = IPv6Address(host)
                self.icmp_size = ICMPv6_IPHdr.HDR_LEN + 8 + psize
                self.dstaddr, self.dstaddrlen, self.rem_host = \
                    (bytes(self.ip6dst.packed), ICMPv6_IPHdr.ADDR_LEN, f'ip6://[{host}]:{ICMPv6_IPHdr.PROTO_ICMPv6}')
            else:
                ip6src = [0]*16
                self.icmp_size = ICMP_IPHdr.HDR_LEN + 8 + psize
                self.ip6dst = self.ip6src = IPv6Address(bytes(ip6src))
                self.dstaddr, self.dstaddrlen, self.rem_host = \
                    (bytes(IPv4Address(host).packed + bytes(12)), \
                        ICMP_IPHdr.ADDR_LEN, f'ip://{host}:{ICMP_IPHdr.PROTO_ICMP}')
            rsp = self.dev.call(sock.client, bytes(self.rem_host, 'ascii'))
            if rsp.status != 0:
                return False
            self.socket = rsp.socket
            '''get local host info (addr, port)'''
            if self.ip_version == ICMPv6_IPHdr.VERSION:
                rsp = self.dev.call(sock.getlocal, self.socket)
                if rsp.status != 0:
                    print ('invalid local address')
                    self.dev.call(sock.close, self.socket)
                    return False
                if rsp.addrlen == ICMPv6_IPHdr.ADDR_LEN:
                    self.ip6src = IPv6Address(bytes(rsp.addr[0:]))
                    self.ip6dst = IPv6Address(host)
                else:
                    print('invalid remote address. Please enable support for IPv6')
                    self.dev.call(sock.close, self.socket)
                    return False
            rsp = self.dev.call(sock.notification, self.socket, 0, 0)
            return True
        except Exception as ex:
            print('{}'.format(str(ex)))
        self.dev.call(sock.close, self.socket)
        return False

    def get_reply(self):
        t1 = time.time()
        self.last_reply = time.time()
        while True:
            rsp = self.dev.call(sock.recvfrom, self.socket, self.icmp_size, 0)
            if rsp.status != 0:
                break
            if rsp.addrlen == ICMP_IPHdr.ADDR_LEN:
                iphdr    = struct.unpack(">BBHHHBBHII", rsp.data[:ICMP_IPHdr.HDR_LEN])
                echo_rsp = ICMP(buf=rsp.data[ICMP_IPHdr.HDR_LEN:])
                sender   = str(IPv4Address(bytes(rsp.addr[:ICMP_IPHdr.ADDR_LEN])))
                ttl = iphdr[5]
            else:
                iphdr    = struct.unpack(">BBHHBBIIIIIIII", rsp.data[:ICMPv6_IPHdr.HDR_LEN])
                echo_rsp = ICMP(buf=rsp.data[ICMPv6_IPHdr.HDR_LEN:])
                sender   = str(IPv6Address(bytes(rsp.addr[0:])))
                ttl = iphdr[5]
            '''echo reply Type = 0(IPv4,ICMP) 129(IPv6,ICMP6) and 8,128(IPv4,6,ICMP self Ping)'''
            echo_rsp_types = [ICMP_Type.ECHO_REPLY, ICMP_Type.ECHO_REQUEST, \
                                ICMPv6_Type.ECHO_REPLY, ICMPv6_Type.ECHO_REQUEST]
            if echo_rsp.type in echo_rsp_types:
                self.num_reply += 1 #received is indeed a response
                if echo_rsp.seq in self.tracker:
                    seqno, sndt0 = self.tracker[echo_rsp.seq]
                    del self.tracker[echo_rsp.seq]
                    rtt = 1000 * (t1 - sndt0)
                    print("{:d} bytes from {:s}: icmp_seq={:d} ttl={:d} time={:.2f}ms".format(len(echo_rsp.payload), sender, seqno, ttl, rtt))
                    self.rtt_stats.append(rtt)
                else:
                    print("Unknown response from {:s} with icmp_seq={:d}".format(sender, echo_rsp.seq))

    def ping(self, host, count=3, interval=1.0, psize=32):
        if not self.setup(host, psize):
            return 0
        self.num_reply = 0
        self.rtt_stats = list()
        start_time  = time.time()
        try:
            for i in range(count):
                seqno_key = i%65536
                echo_req = ICMP(type=ICMP_Type.ECHO_REQUEST \
                                if self.ip_version == ICMP_IPHdr.VERSION \
                                    else ICMPv6_Type.ECHO_REQUEST,
                                seq=seqno_key, size=psize, ip6src=self.ip6src, ip6dst=self.ip6dst)
                t0 = start_time+i*interval # t0 is wanted start time
                self.tracker[seqno_key] = (seqno_key, time.time()) # Use actual time
                rsp = self.dev.call(sock.sendto, self.socket, len(echo_req), 0, self.dstaddr, self.dstaddrlen, 0, bytes(echo_req))
                while True:
                    wait_time = t0+interval-time.time()
                    if wait_time > 0:
                        if self.ready_sem.acquire(blocking=True, timeout=wait_time):
                            self.get_reply()
                    elif self.ready_sem.acquire(blocking=False):
                        self.get_reply()
                    else:
                        break
            while len(self.tracker) != 0:
                # Wait for the last response(s)
                if self.ready_sem.acquire(blocking=True, timeout=2):
                    self.get_reply()
                else:
                    break
        except KeyboardInterrupt as kbi:
            print('{}'.format(str(kbi)))
        self.dev.call(sock.close, self.socket)
        print("\n--- {} ping statistics ---".format(host))
        loss = ((i+1)-self.num_reply) if (i+1) >= self.num_reply else 0
        print("packets tx: {}, rx: {}, loss: {} ({:.1f}%), time: {:.2f} ms"\
            .format(i+1, self.num_reply, loss, 100*(1.*loss/(i+1)), 1000*(self.last_reply-start_time)))
        if len(self.rtt_stats):
            rtt_min, rtt_avg, rtt_max, rtt_sdev = min(self.rtt_stats), pstats.mean(self.rtt_stats), max(self.rtt_stats), pstats.stdev(self.rtt_stats)
            print("rtt min/avg/max/mdev = {:.3f}/{:.3f}/{:.3f}/{:.3f} ms".format(rtt_min, rtt_avg, rtt_max, rtt_sdev))
        print("Elapsed Time:{:.2f} ms".format(1000*(time.time()-start_time)))
        return self.num_reply


def main():
    ap = ArgumentParser()
    ap.add_argument(dest='device',
                    help='<tty> | <host>:<port> | <unix domain socket>')
    ap.add_argument('--speed', type=int, default=921600,
                    help='Use specified baudrate for serial device')
    ap.add_argument('--ssid', default='innophi',
                    help='SSID to connect to')
    ap.add_argument('--passphrase', default='innophase',
                    help='Passphrase (WPA) or Key (WEP) to use')
    ap.add_argument('--size', type=int, default=32,
                    help='Size of payload')
    ap.add_argument('--count', type=int, default=3,
                    help='Number of PINGs')
    ap.add_argument('--interval', type=float, default=1.0,
                    help='Interval in seconds between sending PINGs')
    ap.add_argument('--suspend',
                    help='Enable suspend mode', default=0)
    ap.add_argument('destination',
                    help='Destination host IP address')
    opt = ap.parse_args()

    dev = hostio.open(opt.device, speed=opt.speed, registry=base.registry)
    if dev is None:
        print('{}: No such file'.format(opt.device))
        sys.exit(1)

    w = WCM(dev)
    w.connect(opt.ssid, opt.passphrase)
    if opt.suspend != 0:
        dev.enable_suspend(1)
    ping = Ping(dev)
    ping.ping(opt.destination, count=opt.count, interval=opt.interval, psize=opt.size)


if __name__ == '__main__':
    main()
