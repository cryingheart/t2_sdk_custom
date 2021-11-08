#!/usr/bin/env python3
"""
Interface to OpenOCD daemon.

This module implements an interface class to the OpenOCD daemon and provides
functions for controlling the target execution and reading and writing memory.
"""
import sys
import os
import socket
from argparse import ArgumentParser

class OpenOcd:

    COMMAND_TOKEN = '\x1a'

    def __init__(self, verbose=False, notify=lambda x:None):
        self.verbose = verbose
        self.notify = notify
        self.sock = None

    def connect(self, host="localhost", port=6666):
        """Connect to the OpenOCD daemon at specified host and port."""
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((host, port))
        self.chipname = self.send("set x $CHIPNAME")
        self.notify(True)

    def disconnect(self):
        """Disconnect from the OpenOCD daemon."""
        self.notify(False)
        if self.sock:
            self.send("exit")
            self.sock.close()

    def send(self, cmd):
        """Send a command string to TCL parser. Return the result that was read."""
        assert self.sock, "Not connected"
        data = (cmd + OpenOcd.COMMAND_TOKEN).encode("ascii")
        if self.verbose: print("<- ", data)
        self.sock.send(data)
        return self.recv()

    def recv(self):
        """Read from the stream until COMMAND_TOKEN is received."""
        assert self.sock, "Not connected"
        data = bytes()
        while True:
            chunk = self.sock.recv(4096)
            data += chunk
            if bytes(OpenOcd.COMMAND_TOKEN, encoding="ascii") in chunk:
                break
        if self.verbose: print("-> ", data)
        data = data.decode("ascii").strip()
        data = data[:-1] # strip trailing \x1a
        return data

    def reset(self):
        """Reset and halt the target."""
        self.send("reset halt")

    def halt(self):
        """Halt the target."""
        return self.send('capture "ocd_halt"')

    def resume(self):
        """Resume execution of the target."""
        self.send('resume')

    def state(self):
        """Return the current execution state of the target; 'running' or 'halted'"""
        return self.send("{}.cpu curstate".format(self.chipname))

    def read8(self, addr):
        """Read a 8-bit value from specified address."""
        raw = self.send("ocd_mdb {:#x}".format(addr)).strip().split(": ")
        return None if len(raw) != 2 else int(raw[1], 16)

    def read16(self, addr):
        """Read a 16-bit value from specified address."""
        raw = self.send("ocd_mdh {:#x}".format(addr)).strip().split(": ")
        return None if len(raw) != 2 else int(raw[1], 16)

    def read32(self, addr):
        """Read a 32-bit value from specified address."""
        raw = self.send("ocd_mdw {:#x}".format(addr)).strip().split(": ")
        return None if len(raw) != 2 else int(raw[1], 16)

    def readx(self, addr, size):
        """Read value from 'addr' using specified access size (in bytes)"""
        read_fn = getattr(self, "read{:d}".format(8*size))
        return read_fn(addr)

    def write8(self, addr, value):
        """Write 8-bit value to the specifed address."""
        self.send("mwb {:#x} {:#x}".format(addr, value))

    def write16(self, addr, value):
        """Write 16-bit value to the specifed address."""
        self.send("mwh {:#x} {:#x}".format(addr, value))

    def write32(self, addr, value):
        """Write 32-bit value to the specifed address."""
        self.send("mww {:#x} {:#x}".format(addr, value))

    def writex(self, addr, value, size):
        """Write 'value' to 'addr' using specified access size (in bytes)"""
        write_fn = getattr(self, "write{:d}".format(8*size))
        return write_fn(addr, value)

    def readmem(self, addr, n, size = 1):
        """Read a range of 'n' consecutive locations of specified 'size',
        starting at 'addr', and return the values as a list."""
        self.send("array unset input")
        self.send("mem2array input {:d} {:#x} {:d}".format(8*size, addr, n))
        output = self.send("ocd_echo $input").split(" ")
        # Array is returned on the form <idx> <val> <idx> <val> ... <idx> <val>
        indices = map(int, output[::2])
        values  = map(int, output[1::2])
        pairs = dict(zip(indices, values))
        return [int(pairs[i]) for i in range(len(output)//2)]

    def writemem(self, addr, values, size = 1):
        """Write a range of consecutive locations of specified 'size',
        starting at 'addr', using a sequence of values."""
        N = 1024 # Size of chunks to write
        if os.name == 'nt': N = 128 # Smaller chunks for Windows
        for chunk in [values[i:i+N] for i in range(0, len(values), N)]:
            array = " ".join([("{:d} {:#x}".format(a, b)) for a, b in enumerate(chunk)])
            self.send("array unset output")
            self.send("array set output {{ {:s} }}".format(array))
            self.send("array2mem output {:d} {:#x} {:d}".format(8*size, addr, len(chunk)))
            addr += len(chunk) * size


def main():
    parser = ArgumentParser()
    parser.add_argument('--halt', action='store_true', help='Halt target')
    parser.add_argument('--reset', action='store_true', help='Reset and run target')
    parser.add_argument('hostname', default='localhost', help='Hostname for OpenOCD server')
    opt = parser.parse_args(sys.argv[1:])

    ocd = OpenOcd()
    ocd.connect(opt.hostname)

    if opt.halt:
        ocd.halt()
    elif opt.reset:
        ocd.reset()
        ocd.resume()
    else:
        print(ocd.state())

    sys.exit(0)


if __name__ == '__main__':
    main()
