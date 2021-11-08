#!/usr/bin/env python3

"""
This module is used by the "cog" code generator

make recog

"""


import sys
import importlib
from argparse import ArgumentParser
from api.base import registry
from textwrap import wrap


class Emitter:

    INDENT = "    "

    def __init__(self, outfn):
        self.outfn = outfn
        self.level = 0

    def put(self, s, **kwargs):
        level = kwargs.get('indent', self.level)
        self.outfn(Emitter.INDENT * level + s + "\n")

    def begin(self, s):
        self.put(s)
        self.level += 1

    def end(self, s):
        self.level -= 1
        self.put(s)

def emit_define(out, group, cls):
    out.put("")
    cname = cls.__name__.upper()
    if getattr(cls, 'req', None) is not None:
        out.put("#define {}_{}_REQ 0x{:02x}".format(group, cname, cls.msgid))
    if getattr(cls, 'rsp', None) is not None:
        out.put("#define {}_{}_RSP 0x{:02x}".format(group, cname, 0x80 | cls.msgid))
    if getattr(cls, 'ind', None) is not None:
        out.put("#define {}_{}_IND 0x{:02x}".format(group, cname, 0xc0 | cls.msgid))

def emit_struct(out, group, suffix, cls, doc=''):
    out.put("")
    if doc:
        out.put("/** {}".format(doc))
        out.put(" */")
        pass
    out.begin("struct {}_{}_{} {{".format(group, cls.__name__, suffix))
    for field in cls.get_fields(suffix):
        doc = ' /**< {} */'.format(field.doc) if field.doc else ''
        out.put("{};{}".format(field.get_decl(), doc))
        for name,value in field.get_enum():
            enum_name = "{}_{}_{}".format(group, field.name, name).upper()
            out.put("#define {:30s} {:d}".format(enum_name, value), indent=0)
    out.end("};")

def emit_createfn(out, group, suffix, cls):
    name = "{}_{}_{}".format(group, cls.__name__, suffix)
    decl = cls.get_declaration(suffix, qual="const")
    args = ', '.join(decl) if len(decl) > 0 else "void"

    out.put("""
static inline struct packet *
alloc_{name}(struct {name} **{suffix})
{{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **{suffix});
    if (pkt) {{
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *{suffix} = packet_insert_tail(pkt, sizeof **{suffix});
    }}
    return pkt;
}}""".format(**locals()))

    out.begin("""
static inline struct packet *
create_{name}({args})
{{
    struct {name} *{suffix};
    struct packet *pkt = alloc_{name}(&{suffix});""".format(**locals()))

    out.begin("if (pkt) {")
    for field in cls.get_fields(suffix):
        if field.name.startswith('reserved'):
            out.put(field.get_assign("{}->{}".format(suffix, field.name), "0"))
        else:
            out.put(field.get_assign("{}->{}".format(suffix, field.name), field.name))
    out.end("}")
    out.put("return pkt;")
    out.end("}")


def try_import_sys_group(name):
    # this is a hack for supporting old code
    if name not in registry._sys_modules:
        return False
    try:
        m = importlib.import_module(f'api.{name}')
    except ImportError as e:
        return False

    return True


def info(out, g):
    print("Number of modules: {}".format(registry._num_modules))
    print("Modules: {}".format(registry._modules))
    if g not in registry.groups():
        print("No such interface '{}'".format(g))
        return 1
    for cls in registry.messages(g):
        print("Message {0}({group},{msgid})".format(cls.__name__, **cls.__dict__))
    return 0

def header(out, g, struct_only=False):
    if g not in registry.groups() and not try_import_sys_group(g):
        print("No such interface '{}'".format(g))
        return 1

    out.put("#define HIO_GROUP_{} {:d}".format(g.upper(), registry.messages(g)[0].group))
    out.put("")
    for cls in registry.messages(g):
        out.put("/*")
        out.put(" * Message {}".format(cls.__name__))
        out.put(" */")
        emit_define(out, g.upper(), cls)
        for suffix in ['req', 'rsp', 'ind']:
            if cls.get_fields(suffix) is not None:
                if cls.__doc__:
                    # XXX ugly
                    doc = cls.__doc__.rstrip()
                    if not doc.endswith('.'):
                        # XXX even more ugly
                        doc += '.'
                        pass
                    if suffix == 'req':
                        doc = doc + ' Request structure.'
                    elif suffix == 'rsp':
                        doc = doc + ' Response structure.'
                    elif suffix == 'ind':
                        doc = doc + ' Indication structure.'
                else:
                    doc = ''
                emit_struct(out, g, suffix, cls, doc=doc)
                if not struct_only: emit_createfn(out, g, suffix, cls)
    return 0

def doxygen(out, g):
    if g not in registry.groups() and not try_import_sys_group(g):
        print("No such interface '{}'".format(g))
        return 1
    m = sys.modules[registry.messages(g)[0].__module__]
    if m.__doc__ is None:
        h = b = ''
    else:
        d = m.__doc__.split('\n', 1)
        if len(d) == 2:
            h, b = d
        else:
            h = d[0]
            b = ''
            pass
        pass
    h = h.strip()
    b = b.strip()
    out.put('/**')
    out.put(' * @addtogroup hioapi_{}'.format(g))
    out.put(' * @{')
    out.put(' *')
    for x in b.splitlines():
        if not x:
            out.put(' *') # Special case to avoid trailing whitespace
        else:
            out.put(' * ' +  x)

    def tag(t, seq):
        return ''.join(['<{}>{}</{}>'.format(t,s,t) for s in seq])

    headers = ['OPERATION', 'REQ', 'RSP', 'IND']
    msgid_msb = {'req' : 0, 'rsp' : 0x80, 'ind' : 0xc0}
    group_id = registry.group_id(g)
    out.put(' *')
    out.put(' * <table>')
    out.put(' * <caption>Message summary (group={})</caption>'.format(group_id))
    out.put(' * <tr>{}</tr>'.format(tag('th', headers)))
    for cls in registry.messages(g):
        row = []
        name = cls.__name__
        row.append(name)
        for direction in ['req','rsp','ind']:
            if cls.get_fields(direction) is None:
                row.append('')
            else:
                msgid = cls.msgid | msgid_msb[direction]
                row.append('0x{:02x} - {}_{}_{}'.format(msgid, g, name, direction))
        out.put(' * <tr>{}</tr>'.format(tag('td', row)))
    out.put(' * </table>')
    out.put(' */')
    return 0

def api(out, g):
    if g not in registry.groups() and not try_import_sys_group(g):
        print("No such interface '{}'".format(g))
        return 1
    req_list = []
    for cls in registry.messages(g):
        if cls.get_fields('req') is None:
            req_list.append("NULL /* {} */".format(cls.__name__))
        else:
            req_list.append("{}_{}".format(g,cls.__name__))
    handlers = ',\n        '.join(req_list)
    group_id = registry.messages(g)[0].group
    n = len(registry.messages(g))
    out.put("""static const struct hio_api {g}_api = {{
    .group = {group_id},
    .num_handlers = {n},
    .handler = {{
        {handlers},
    }}
}};""".format(**locals()))
    return 0


def main():
    operations = {'info':info, 'header':header, 'api':api, 'doxygen':doxygen}
    modules = sorted(registry._modules)
    ap = ArgumentParser()
    ap.add_argument('operation', choices=operations.keys())
    if len(modules) == 1:
        ap.add_argument('module', nargs='?', choices=modules, default=modules[0])
    else:
        ap.add_argument('module', choices=modules)
    opt = ap.parse_args()

    out = Emitter(sys.stdout.write)
    op = operations.get(opt.operation)
    rc = op(out, opt.module)
    sys.exit(rc)


def load_system_modules():
    for g in registry._sys_modules:
        try_import_sys_group(g)
    return


if __name__ == '__main__':
    load_system_modules()
    main()
