#!/usr/bin/env python3


import sys
if sys.version_info.major != 3 or sys.version_info.minor < 7:
    print('Python version 3.7 or higher required (you have {0.major}.{0.minor}.{0.micro})'.format(sys.version_info))
    sys.exit(1)
    pass
import io
import json
import time
import struct
import argparse
import hostio
import api.base
import api.hio
import bits.bootargs
import bits.flash
from reset import reset
from elftools.elf.elffile import ELFFile
try:
    from Crypto.Cipher import AES
    from Crypto.Hash import SHA256
except ImportError:
    AES = None
    SHA256 = None
try:
    from Crypto.PublicKey import ECC
    from Crypto.Signature import DSS
except ImportError:
    ECC = None
    DSS = None


def is_virtual_address(addr):
    return 0x100000 <= addr < 0xfc0000

def is_ram_address(addr):
    return 0x40000 <= addr < 0xc0000

def bxor(bytes1, bytes2): # use xor for bytes
    return bytes(b1^b2 for b1, b2 in zip(bytes1, bytes2))

def gf2_mul(a):
    result = bytearray()
    carry = 0
    for x in a:
        carry = 2*x+carry
        result.append(carry & 0xff)
        carry >>= 8

    if carry:
        result[0] ^= 0x87

    return bytes(result)

def chop(d, n):
    while len(d):
        a, d = d[0:n], d[n:]
        yield a

def xex_encrypt(*, k1, k2, data, init):
    result = bytearray()

    X = k2.encrypt(init)

    for p in chop(bytearray(data), 16):
        c = bxor(p, X)
        c = k1.decrypt(c)
        c = bxor(c, X)
        result.extend(c)
        X = gf2_mul(X)

    return bytes(result)


class Memory():
    class Range():
        def __init__(self, start, end):
            self.start = start
            self.end = end
            pass
        def sort_key(self):
            return (self.start, self.end)
        pass

    def __init__(self):
        self.ranges = []

    def add(self, start, end):
        # there is probably a better way to do this
        tmp = self.ranges
        self.ranges = []

        tmp.append(Memory.Range(start, end))
        tmp.sort(key=Memory.Range.sort_key)

        rlast = None
        for r in tmp:
            if rlast:
                if rlast.end == r.start:
                    rlast.end = r.end
                    continue
                if r.start < rlast.end:
                    print('overlapping memory regions', file=sys.stderr)
                    rlast = max(rlast.end, r.end)
                    continue
                self.ranges.append(rlast)
                pass
            rlast = r
            pass
        if rlast:
            self.ranges.append(rlast)
            pass
        return
    pass


# [[[cog
# import cog
# import subprocess
# import tempfile
# import binascii
#
# def generate_table(filename):
#     print=cog.outl
#     objfile = tempfile.NamedTemporaryFile()
#
#     subprocess.check_call(['arm-none-eabi-gcc',
#                            '-mcpu=cortex-m3', '-mthumb',
#                            '-c', '-fPIC',
#                            '-o',  objfile.name,
#                            filename])
#
#     data = subprocess.check_output(['arm-none-eabi-objcopy',
#                                     '-O', 'binary', objfile.name, '/dev/stdout'])
#
#     print("trampoline = bytearray.fromhex('''")
#     while data:
#         l = data[:8]
#         r = data[8:16]
#         l = ' '.join(f'{_:02x}' for _ in l)
#         r = ' '.join(f'{_:02x}' for _ in r)
#         print(f'{l}  {r}')
#         data = data[16:]
#         pass
#     print("''')")
#
#     for l in subprocess.check_output(['nm', '-n', objfile.name],
#                                      encoding='ascii').splitlines():
#         l = l.split()
#         if len(l) == 3:
#             for sym in ('mac_reset', 'table', 'dest'):
#                 if l[-1] == sym:
#                     v = int(l[0], 16)
#                     print(f'{sym}_offset = {v:#x}')
#                     pass
#                 pass
#             pass
#         pass
#     return
# ]]]
# [[[end]]]

def make_bank3_trampoline(address, dst, src, size, entry, argc, argv):
    # see kernel/arm/bank3_workaround.S

    # [[[cog
    # generate_table('kernel/arm/bank3_workaround.S')
    # ]]]
    trampoline = bytearray.fromhex('''
    06 48 07 49 07 4a 88 42  04 d0 50 f8 04 3b 42 f8
    04 3b f8 e7 05 48 06 49  df f8 0c f0 00 00 00 00
    00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
    00 00 00 00 4f f4 7c 01  91 f8 79 01 30 f0 04 00
    81 f8 79 01 91 f8 7a 01  50 f0 04 00 81 f8 7a 01
    03 48 c1 f8 0c 01 03 49  03 48 08 60 00 bf fd e7
    00 00 00 00 0c ed 00 e0  04 00 fa 05
    ''')
    table_offset = 0x1c
    mac_reset_offset = 0x35
    dest_offset = 0x60
    # [[[end]]]


    table = bytearray()
    table.extend(src.to_bytes(4, 'little'))
    table.extend((src+size).to_bytes(4, 'little'))
    table.extend(dst.to_bytes(4, 'little'))
    table.extend(entry.to_bytes(4, 'little'))
    table.extend(argc.to_bytes(4, 'little'))
    table.extend(argv.to_bytes(4, 'little'))

    trampoline[table_offset:table_offset+len(table)] = table
    trampoline[dest_offset:dest_offset+4] = (address|1).to_bytes(4, 'little')

    fake_entry = address + mac_reset_offset

    return fake_entry | 1, trampoline


class BootCommand(bytearray):
    @property
    def sync(self):
        return self[0]
    @property
    def op(self):
        return self[1]
    @property
    def length(self):
        return int.from_bytes(self[2:4], 'little')
    @property
    def addr(self):
        return int.from_bytes(self[4:8], 'little')
    @property
    def data(self):
        return self[8:]

    @classmethod
    def from_stream(cls, stream):
        pos = stream.tell()
        hdr = stream.read(8)
        if not hdr:
            return
        if len(hdr) < 8:
            raise ValueError(f'truncated stream at position {pos}')
        obj = cls(hdr)
        if obj.sync != 0x7e:
            raise ValueError(f'invalid sync at position {pos}')

        if obj.op == BootStream.WRITE:
            data = stream.read(obj.length)
            if len(data) != obj.length:
                raise ValueError(f'truncated stream at position {pos}')
            obj.extend(data)
            pass
        return obj

    @classmethod
    def iter_from_stream(cls, stream):
        while True:
            cmd = cls.from_stream(stream)
            if cmd is None:
                break
            yield cmd
            pass
        return
    pass

class BootStream():
    NOP     = 0
    READ    = 1
    WRITE   = 2
    ENTRY   = 3
    SPEED   = 4
    ARGS    = 7
    SIGN    = 8

    def __init__(self, writable=None, *, bank3_workaround=False):
        if writable:
            self.buf = writable
        else:
            self.buf = io.BytesIO()
            pass

        # workaround for RAM reset issue
        self.bank3_workaround = bank3_workaround
        self.memory = Memory()
        self.bank3_data = io.BytesIO()

    def __bool__(self):
        return len(self.buf.getvalue()) > 0

    def bytes(self):
        return self.buf.getvalue()

    def write(self, data):
        return self.buf.write(data)

    def cmd(self, op, length, addr):
        return self.write(struct.pack("<BBHI", 0x7e, op, length, addr))

    def cmd_nop(self):
        return self.cmd(BootStream.NOP, 0, 0)

    def cmd_read(self, addr, length):
        return self.cmd(BootStream.READ, length, addr)

    def bank3_fixup(self, entry, argc, argv):
        if not self.bank3_workaround:
            return entry
        data = self.bank3_data.getvalue()
        end = 0
        for r in self.memory.ranges:
            end = max(end, r.end)
            pass
        end = max(end, 0x80000)
        end += -end % 256
        start = end

        for d in chunked(data, 32768):
            self.cmd_write(end, d)
            end += len(d)
            pass

        end += -end % 4
        entry, d = make_bank3_trampoline(end,
                                         0x70000, start, end-start,
                                         entry, argc, argv)

        self.cmd_write(end, d)

        return entry

    def bank3_memory_ranges(self, addr, data):
        if self.bank3_workaround:
            # yield list of memory not in bank3
            # bank3 data is saved for later
            mem_start = addr
            mem_end = addr + len(data)
            bank3_start = max(0x70000, mem_start)
            bank3_end = min(0x80000, mem_end)

            if bank3_start < bank3_end:
                if mem_start < bank3_start:
                    yield mem_start, data[:bank3_start-mem_start]
                self.bank3_data.seek(bank3_start-0x70000)
                self.bank3_data.write(data[bank3_start-mem_start:bank3_end-mem_start])
                if mem_end > bank3_end:
                    yield bank3_end, data[bank3_end-mem_start:]
                    pass
                return
            pass
        yield addr, data
        return

    def cmd_write(self, addr, data):
        if not data:
            return 0
        total = 0
        for a, d in self.bank3_memory_ranges(addr, data):
            n = self.cmd(BootStream.WRITE, len(d), a)
            n += self.write(d)
            total += n
            self.memory.add(a, a+len(d))
        return total

    def cmd_sign(self, data):
        return 0

    def cmd_entry(self, addr):
        return self.cmd(BootStream.ENTRY, 0, addr)

    def cmd_speed(self, speed):
        return self.cmd(BootStream.SPEED, 0, speed)

    def cmd_args(self, argc, argv):
        return self.cmd(BootStream.ARGS, argc, argv)


class SecureBootStream(BootStream):
    def __init__(self, *, cipher_key, p256_private, **kw):
        super().__init__(**kw)
        self.hash = SHA256.new()
        self.data_key = AES.new(cipher_key[0:16], AES.MODE_ECB)
        self.tweak_key = AES.new(cipher_key[16:32], AES.MODE_ECB)
        self.dss = DSS.new(key=p256_private, mode='fips-186-3')
        pass

    def cmd_write(self, addr, data):
        if not data:
            return 0
        # FIXME: This is utterly broken, we need multiples of 16
        # bytes, but where are these supposed to come from? Let's pad
        # here, with the risk of overwriting something with padding.
        total = 0
        for a, d in self.foo(addr, data):

            padding = -len(d) % 16
            d += b'\x00' * padding
            assert len(d) % 16 == 0

            self.hash.update(a.to_bytes(4, byteorder='little'))
            self.hash.update(d)

            data = xex_encrypt(k1=self.data_key, k2=self.tweak_key,
                               data=d,
                               init=a.to_bytes(16, byteorder='little'))

            total += super().cmd_write(a, d)
            pass

        return total

    def cmd_sign(self, data):
        n = self.cmd(BootStream.SIGN, len(data), 0)
        n += self.write(data)
        return n

    def cmd_entry(self, addr):
        self.hash.update(addr.to_bytes(4, byteorder='little'))

        signature = self.dss.sign(self.hash)
        self.cmd_sign(signature)
        return super().cmd_entry(addr)

    def cmd_args(self, argc, argv):
        self.hash.update(argv.to_bytes(4, byteorder='little'))
        self.hash.update(argc.to_bytes(4, byteorder='little'))
        return super().cmd_args(argc, argv)


def chunked(seq, size):
    return (seq[pos:pos + size] for pos in range(0, len(seq), size))

def chunked_write(dev, data):
    written = 0
    for chunk in chunked(data, 4096):
        written += dev.write(chunk)
        pass
    return written

def elf_segments(elf):
    for seg in elf.iter_segments():
        if seg.header.p_type != 'PT_LOAD':
            continue
        yield seg

def read_reg(dev, addr, size):
    BootStream(dev).cmd_read(addr, size)
    data = dev.readmsg()
    return int.from_bytes(data, 'little')

def parse_read_cmd(cmd):
    if ',' in cmd:
        addr, size = cmd.split(',', 1)
        size = int(size)
    elif '/' in cmd:
        addr, size = cmd.split('/', 1)
        size = int(size)
    else:
        size = 1
    addr = int(addr, 0)
    return addr, size

def write_reg(dev, addr, value, size):
    data = value.to_bytes(size, 'little')
    BootStream(dev).cmd_write(addr, data)
    return

def parse_write_cmd(cmd):
    addr, value = cmd.split('=', 1)
    addr = int(addr, 0)
    if ',' in value:
        value, size = value.split(',', 1)
        size = int(size)
    elif '/' in value:
        value, size = value.split('/', 1)
        size = int(size)
    else:
        size = 1
    value = int(value, 0)
    return addr, value, size


# this should go someplace generic
def load_keyfile(fileobj):
    try:
        e = json.load(fileobj)
    except Exception:
        raise ValueError(f'{fileobj.name!r} must be a valid json file')

    ck = 'cipher_key'
    pk = 'p256_private'

    if not isinstance(e, dict) \
       or ck not in e \
       or pk not in e:
        raise ValueError(f'{fileobj.name!r} must be an object containing {ck!r} and {pk!r} keys')

    result = {}
    result[ck] = bytes.fromhex(e[ck])

    d = bytes.fromhex(e[pk])
    d = int.from_bytes(d, 'big')
    ecc = ECC.construct(curve='p256', d=d)
    result[pk] = ecc

    return result


def create_default_partition_table(tflash):
    table = [
        dict(index=0, type=bits.flash.PartitionType.BOOT, sect_start=1, sect_count=63),
        dict(index=1, type=bits.flash.PartitionType.VIRT, sect_start=64, sect_count=128),
    ]
    if any(tflash.partitions()):
        return False
    print('No partition table found, creating default')
    for e in table:
        print(f'    adding {e["type"].name} partition at sectors {e["sect_start"]}..{e["sect_start"]+e["sect_count"]-1}')
        tflash.add_partition(**e)
        pass
    tflash.save_partition_table()
    return True


def start_flash_helper(dev, devname, speed):
    args = bits.bootargs.BootArguments()
    args.add(f'hio.baudrate={speed}')
    gordon_stream = BootStream()
    try:
        _ = parse_boot_seq(gordon_stream,
                           'load,entry',
                           'apps/gordon/bin/gordon.elf',
                           args)
    except:
        _ = parse_boot_seq(gordon_stream,
                           'load,entry',
                           'apps/gordon/bin/gordon.elf.strip',
                           args)
    chunked_write(dev, gordon_stream.bytes())

    dev.close()

    dev = hostio.open(devname, registry=api.base.registry, speed=speed)
    if not dev:
        print("could not open hio")
        sys.exit(1)

    rsp = dev.call(api.hio.query)
    if rsp is None:
        print("Flash helper is not responding, check your cables")
        sys.exit(1)
    return dev

def write_stream_to_part(tflash, name, ptype, blob):
    part = tflash.get_partition_object(ptype=ptype)
    if part is None:
        if not create_default_partition_table(tflash):
            print(f'No {name} partition defined, please add one')
            return False
        part = tflash.get_partition_object(ptype=ptype)
        if part is None:
            print('Attempted to add {name} partition, but it\'s still undefined')
            return False
        pass

    required = len(blob)
    if part.capacity < required:
        print(f'{name} image does not fit in partition {part.capacity} < {required}')
        return False

    padding = -len(blob) % tflash.sector_size
    padding = b'\xff' * padding
    print(f'Writing {name} image to flash ({len(blob)} bytes)')
    if sys.stdout.isatty():
        tflash.register_progress(bits.flash.progress)
    part.pwrite(data=blob+padding, offset=0)
    tflash.flush()
    if sys.stdout.isatty():
        tflash.register_progress(None)
        print('')
    return True

def parse_boot_seq(stream, seq, image, args):
    cmd_list = seq.split(',')
    vm = None
    imageFile = open(image, 'rb') if image else None
    elf = ELFFile(imageFile) if imageFile else None
    entry = elf.header.e_entry if elf else None

    if elf is not None:
        for seg in elf_segments(elf):
            if is_virtual_address(seg.header.p_vaddr):
                vm = seg.data()

    while cmd_list:
        cmd = cmd_list.pop(0)
        if cmd == 'nop':
            stream.cmd_nop()
        elif cmd == 'cont':
            stream.cmd_entry(0xffffffff)
        elif cmd == 'next':
            stream.cmd_entry(0)
        elif cmd == 'clken':
            pass
        elif cmd == 'entry' and elf is not None:
            print(f'Entrypoint {entry:#x}', file=sys.stderr)
            stream.cmd_entry(entry)
        elif cmd == 'load' and elf is not None:
            print(f'Loading {image}', file=sys.stderr)
            for seg in elf_segments(elf):
                if not is_virtual_address(seg.header.p_vaddr):
                    addr = seg.header.p_vaddr
                    for data in chunked(seg.data(), 32768):
                        stream.cmd_write(addr, data)
                        addr += len(data)

            args.encode(sizealign=16)

            entry = stream.bank3_fixup(entry, args.argc, args.argv)

            stream.cmd_write(args.load_address, args.blob)
            stream.cmd_args(args.argc, args.argv)

        elif '=' in cmd:
            try:
                addr, value, size = parse_write_cmd(cmd)
                stream.cmd_pbwrite(addr, value, size)
            except ValueError:
                print(f'Bad format for write command: {cmd}', file=sys.stderr)
        elif len(cmd) > 0:
            print(f'Ignoring unknown command {cmd!r}', file=sys.stderr)

    if imageFile:
        imageFile.close()
    return vm


def write_stream_to_memory(dev, stream, verify=False):
    if not verify:
        return chunked_write(dev, stream.bytes())
    else:
        written = 0
        output = BootStream(dev)
        for cmd in BootCommand.iter_from_stream(io.BytesIO(stream.bytes())):
            written += chunked_write(dev, cmd)
            if cmd.op == BootStream.WRITE and is_ram_address(cmd.addr):
                output.cmd_read(cmd.addr, cmd.length)
                read_data = dev.readmsg()
                if read_data != cmd.data:
                    raise ValueError(
                        f'failed validation at [{cmd.addr:x}:{cmd.addr+cmd.length:x})')
            pass
        pass

    return written

def read_rom_version(dev):
    table = [
        (16, 0x3b369, 0x2f),
        (15, 0x3bb38, 0x2f),
        (14, 0x3bab8, 0x2f),
        (13, 0x36960, 0x2f),
        (12, 0x39b04, 0x2f),
        (11, 0x35b30, 0x2f)
    ]
    with dev.timeout_context(1.0):
        for v, a, l in table:
            BootStream(dev).cmd_read(a, l)
            version = dev.readmsg()
            if version is None:
                print('Unable to get a response from the boot ROM.')
                print('Has the system been properly reset?')
                raise SystemExit('Timeout reading ROM')
            if version.startswith(b'@(#) $RomVersion: ') \
               and version.endswith(b' $\x00'):
                version = version[18:-3]
                version = version.decode('ascii')
                return version
            pass
    return


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--device', default='/dev/ttyUSB2',
                    help='Interface boot device')
    ap.add_argument('--output',
                    help='Output boot image to filename')
    ap.add_argument('--seq', default='load,entry',
                    help="Set boot command sequence. Default is 'load,entry'")
    ap.add_argument('--gdb', dest='seq', action='store_const', const='',
                    help="Don\'t download to RAM, only flash virtual image. (equal to --seq='')")
    ap.add_argument('--speed', type=int, default=921600,
                    help='Set interface speed (default: %(default)d)')
    ap.add_argument('--reset', default='none',
                    help='Perform reset using the specified method')
    ap.add_argument('--xtal', type=int, choices=(40, 80), default=40,
                    help='Board crystal in MHz (default: %(default)d)')
    ap.add_argument('--identify', action='store_true',
                    help='Identify chip type')
    ap.add_argument('--rom-version', action='store_true',
                    help='Show ROM version')
    ap.add_argument('--check-clock', action="store_true",
                    help=argparse.SUPPRESS)
    ap.add_argument('--check-rom', action="store_true",
                    help='Compute hash checksum of ROM image')
    ap.add_argument('--rd', action='append',
                    help='Read register. e.g. --rd <addr>[,<size>]')
    ap.add_argument('--wr', action='append',
                    help='Write register, e.g --wr <addr>=<val>[,<size>]')
    ap.add_argument('--test', action="store_true",
                    help='Run test sequence')
    ap.add_argument('--verify', action="store_true",
                    help=argparse.SUPPRESS)

    ap.add_argument('--bank3-workaround',
                    dest='bank3_workaround',
                    action="store_true",
                    default=True,
                    help=argparse.SUPPRESS)

    ap.add_argument('--no-bank3-workaround',
                    dest='bank3_workaround',
                    action="store_false",
                    help='Disable workaround for RAM bank 3 reset issue')

    ap.add_argument('--keyfile', type=argparse.FileType('r'),
                    default=None, help='Key filename')
    ap.add_argument('--flash', choices=('all', 'vm', 'nope'),
                    help=('Flash complete image (all), '
                        'only virtual memory (vm) part, or none (nope). '
                        'Default is virtual memory (vm).'),
                    default='vm')
    ap.add_argument('--SN', default=None, help='''ftdi serial.
            Used by reset to identify a selected board if using multiple boards.
            Use ftdi_urls.py form the pyftdi package to list serial numbers''')
    ap.add_argument('image', nargs='?',
                    help='Image in .elf format to be loaded and run')
    bits.bootargs.BootArguments.add_arguments(ap)

    opt = ap.parse_args()

    if opt.verify and opt.output:
        sys.exit('--output and --verify are incompatible')

    DEFAULT_SPEED = 115200

    if opt.keyfile:
        if ECC is None:
            msg = '''
The PyCryptodome package is required to use secure boot.
You may be able to install this with a command like
    "pip3 install --user pycryptodome"
'''
            raise NotImplementedError(msg)

        secure_keys = load_keyfile(opt.keyfile)
        stream = SecureBootStream(**secure_keys,
                                  bank3_workaround=opt.bank3_workaround)
    else:
        stream = BootStream(bank3_workaround=opt.bank3_workaround)

    args = bits.bootargs.BootArguments.factory(options=opt)

    if opt.output:
        outp = open(opt.output, 'wb')
        if not outp:
            print(f'{opt.output}: Bad output file')
            sys.exit(1)
    else:
        bootspeed = DEFAULT_SPEED * (2 if opt.xtal==80 else 1)
        dev = hostio.open(opt.device, speed=bootspeed, raw=True)
        if dev is None:
            print(f'{opt.device}: Bad output device')
            sys.exit(1)

    vm = parse_boot_seq(stream, opt.seq, opt.image, args)

    if opt.output:
        payload = stream.bytes()
        outp.write(payload)
        print(f'Wrote bootimage to {opt.output} {len(payload):_} bytes')

        if vm:
            with open(opt.output + '.vm', 'wb') as outvm:
                outvm.write(vm)
            print(f'Wrote vmimage to {opt.output}.vm {len(vm):_} bytes')
        return

    if opt.reset:
        reset(opt.reset, ftdi_serial=opt.SN, dev=dev)
        if isinstance(dev, hostio.FtdiSPI):
            dev.close()
            dev = hostio.open(opt.device, speed=bootspeed, raw=True)

    # Drop any garbage bytes from input
    time.sleep(0.1)
    dev.flushrx()

    if opt.speed != DEFAULT_SPEED:
        print(f'Change speed to {opt.speed}')
        BootStream(dev).cmd_speed(opt.speed)
        dev.close()
        dev = hostio.open(opt.device, raw=True, speed=opt.speed*(2 if opt.xtal==80 else 1))
        if dev is None:
            print(f'{opt.device}: Failed to open with baudrate {opt.speed}')
            sys.exit(1)

    if opt.rd:
        for c in opt.rd:
            addr, size = parse_read_cmd(c)
            value = read_reg(dev, addr, size)
            width = 2+2*size
            print(f'[{addr:#x}] = {value:#0{width}x} ({value:d})')
            pass
        pass

    if opt.wr:
        for c in opt.wr:
            addr, value, size = parse_write_cmd(c)
            write_reg(dev, addr, value, size)
            width = 2+2*size
            print(f'[{addr:#x}] = {value:#0{width}x} ({value:d})')
            pass
        pass

    if opt.identify:
        chipid = read_reg(dev, 0xfc016c, 4)
        maskid = read_reg(dev, 0xfc0170, 1)
        print(f'CHIPID: {chipid:#x} MASKID: {maskid:#x}')
        pass

    # Always read ROM version, but only print it on request. This is a
    # simple way to test connection.
    rom_version = read_rom_version(dev)
    if opt.rom_version:
        print('ROM Version:', rom_version)

    if opt.check_rom:
        cksum = SHA256.new()
        chunk = 1024
        for addr in range(0x00000, 0x40000, chunk):
            BootStream(dev).cmd_read(addr, chunk)
            data = dev.readmsg()
            cksum.update(data)
            print(f'{addr:08x}', endl='\r')
        print('SHA256', cksum.hexdigest())
        pass

    if opt.check_clock:
        clock_sel = read_reg(dev, 0xfc0106, 1)
        sys_cntr0 = read_reg(dev, 0xfc0450, 4)
        time.sleep(2)
        sys_cntr1 = read_reg(dev, 0xfc0450, 4)
        dt = sys_cntr1 - sys_cntr0
        if dt < 0:
            dt += 2**32
            pass
        f = dt / 2
        if (clock_sel & 1) == 0:
            f /= 2
            pass
        f /= 1e6

        print(f'Nominal clock frequency {f:.1f} MHz')
        pass

    if opt.test:
        wdata = bytes(range(64))
        BootStream(dev).cmd_write(0x60000, wdata)

        fail = None
        for i in range(63):
            BootStream(dev).cmd_read(0x60000 + i, len(wdata) - i)
            data = dev.readmsg()
            if data != wdata[i:]:
                fail = i
                break

        if fail is None:
            print("PASS")
        else:
            print(f'FAIL ({fail})')

    if opt.flash != 'nope' and not (opt.flash == 'vm' and vm is None):
        if opt.reset is None:
            print("You need to specify reset method")
            sys.exit(1)

        dev = start_flash_helper(dev, opt.device, opt.speed)
        tflash = bits.flash.TargetFlash(dev)

        if opt.flash == 'all':
            if not write_stream_to_part(tflash, 'boot',
                                        bits.flash.PartitionType.BOOT, stream.bytes()):
                sys.exit(1)
                pass
            pass

        if vm:
            if not write_stream_to_part(tflash, 'VM',
                                        bits.flash.PartitionType.VIRT, vm):
                sys.exit(1)
                pass
            pass

        reset(opt.reset, ftdi_serial=opt.SN, dev=dev)
        time.sleep(0.1)
        dev.close()
        dev = hostio.open(opt.device, speed=DEFAULT_SPEED * (2 if opt.xtal==80 else 1), raw=True)

        if opt.speed:
            BootStream(dev).cmd_speed(opt.speed)
            dev.close()
            dev = hostio.open(opt.device, raw=True, speed=opt.speed)

    if stream and opt.flash != 'all':
        written = write_stream_to_memory(dev, stream, opt.verify)
        print(f'Wrote {written} bytes to {opt.device!r}')

    if opt.speed and (not opt.seq or 'entry' not in opt.seq):
        BootStream(dev).cmd_speed(opt.speed)
        dev.close()
        dev = hostio.open(opt.device, speed=opt.speed, raw=True)
        if dev is None:
            print(f'{opt.device}: Failed to open with baudrate {opt.speed}')
            sys.exit(1)

    dev.close()


if __name__ == "__main__":
    main()

# vim: sw=4:expandtab
#
