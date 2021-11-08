#!/usr/bin/env python3
import sys
import argparse
import base64
import json
from elftools.elf.elffile import ELFFile

from Crypto.Hash import SHA256
from Crypto.PublicKey import ECC
from Crypto.Signature import DSS


# this should go someplace generic
def load_keyfile(fileobj):
    try:
        e = json.load(fileobj)
    except Exception:
        raise ValueError(f'{fileobj.name!r} must be a valid json file')

    pk = 'p256_private'

    if not isinstance(e, dict) \
       or pk not in e:
        raise ValueError(f'{fileobj.name!r} must be an object containing {pk!r}')

    result = {}

    d = bytes.fromhex(e[pk])
    d = int.from_bytes(d, 'big')
    ecc = ECC.construct(curve='p256', d=d)
    result[pk] = ecc

    return result


def elf_sections(elf):
    for name in ['.text', '.data', '.virt']:
        sect = elf.get_section_by_name(name)
        if sect is not None:
            yield sect


def compute_signature(fdesc, *, p256_private, verbose, **kw):
    elf = ELFFile(fdesc)
    hash = SHA256.new()

    for sect in elf_sections(elf):
        name = sect.name
        addr = sect.header.sh_addr
        data = sect.data()
        if verbose:
            print(f'{fdesc.name}: loading section {name} ({len(data)} bytes at {addr:#x})', file=sys.stderr)
        hash.update(data)

    hash.update((elf.header.e_entry).to_bytes(4, byteorder='little'))

    if verbose:
        print(f"SHA256({fdesc.name})", hash.hexdigest())

    dss = DSS.new(key=p256_private, mode='fips-186-3')
    return dss.sign(hash)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--output',
                    help='Signature output file')
    ap.add_argument('--key', required=True, type=argparse.FileType('r'),
                    help='Key filename')
    ap.add_argument('--verbose', action="store_true")
    ap.add_argument('filename', nargs='+', type=argparse.FileType('rb'),
                    help='ELF file to compute signature for')

    opt = ap.parse_args()

    if opt.output and len(opt.filename) > 1:
        print('Using --output with multiple input filenames is not supported',
              file=sys.stderr)
        sys.exit(1)

    secure_keys = load_keyfile(opt.key)

    for fd in opt.filename:
        signature = compute_signature(fd, **(vars(opt)), **secure_keys)
        if opt.output:
            open(opt.output, "wb").write(signature)

        if not opt.output or opt.verbose:
            pr_sign = base64.b64encode(signature).decode('ascii')
            print(f'ECDSA({fd.name})  {pr_sign}')


if __name__ == "__main__":
    main()
