#!/usr/bin/env python3
"""
HIO debug module

can be used to force T2 into panic mode so coredumps can be retrieved

"""


import sys
import argparse
import hostio
from api import base
from api import debug


def get_dev(opt):
    dev = hostio.open(opt.device, raw=True, speed=opt.speed, registry=base.registry)
    if not dev:
        print("Failed to open hostio device {}".format(opt.device))
        sys.exit(1)
    return dev


def panic(parser, opt):
    dev = get_dev(opt)
    dev.call_async(debug.panic)
    print('panic request sent! device should now be awake and in state "ASSERTION FAILED"')
    print('you should now be able to create a coredump if krn.coredump=1 was set at boot?')


def parse_args(args):
    parser = argparse.ArgumentParser()
    parser.add_argument('--device', default='/dev/ttyUSB2', help='Hostio device name')
    parser.add_argument('--speed', type=int, default=921600, help='Serial baudrate')
    parser.set_defaults(func=None)
    cmdparser = parser.add_subparsers()

    subparser = cmdparser.add_parser('panic', help=debug.panic.__doc__)
    subparser.set_defaults(func=panic)

    opt = parser.parse_args(args)
    return parser, opt


def main():
    parser, opt = parse_args(sys.argv[1:])
    if opt.func is None:
        parser.print_help(sys.stdout)
    else:
        opt.func(parser, opt)


if __name__ == "__main__":
    main()

# vim: sw=4:expandtab
