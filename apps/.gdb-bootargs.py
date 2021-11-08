import io
import struct


class Singleton:
    instance = None

    class __Singleton:
        pass

    def __init__(self):
        if not Singleton.instance:
            Singleton.instance = Singleton.__Singleton()

    def __getattr__(self, name):
        return getattr(self.instance, name)

    def __setattr__(self, name, v):
        return setattr(self.instance, name, v)


class bootargs_init_cmd(gdb.Command):
    def __init__ (self):
        super (bootargs_init_cmd, self).__init__ ("bootargs_init", gdb.COMMAND_NONE, gdb.COMPLETE_NONE)

    def invoke (self, arg, from_tty):
        Singleton().argv = []


class bootargs_add_cmd(gdb.Command):
    def __init__ (self):
        super (bootargs_add_cmd, self).__init__ ("bootargs_add", gdb.COMMAND_NONE, gdb.COMPLETE_NONE)

    def invoke (self, arg, from_tty):
        for s in arg.split():
            Singleton().argv.append(s)


class Arglist():
    def __init__(self):
        self.args = io.BytesIO()
        self.argv = []
        pass

    def add(self, arg):
        self.argv.append(self.args.tell())
        self.args.write(arg.encode('utf-8') + b'\0')
        return

    @staticmethod
    def align(n, a):
        return n + (a - n % a) % a

    def encode(self):
        if not self.argv:
            # No boot argument was supplied
            # Set argc to 0
            gdb.execute('set $r0=0')
            # Set argv to NULL
            gdb.execute('set $r1=0')
            return
        argc = len(self.argv)
        strsize = self.align(self.args.tell(), 4)
        totsize = strsize + (argc + 1) * 4
        addr = 0xc0000 - self.align(totsize, 256)

        self.args.write(b'\0'*(strsize - self.args.tell()))
        for a in self.argv:
            self.args.write(struct.pack('<I', (a + addr)))
            pass
        self.args.write(struct.pack('<I', 0))
        gdb.inferiors()[0].write_memory(addr, self.args.getvalue())
        gdb.execute('set $r0=%d' % (argc))
        gdb.execute('set $r1=%d' % (addr + strsize))
        return


class bootargs_cmd(gdb.Command):
    def __init__ (self):
        super (bootargs_cmd, self).__init__ ("bootargs_apply", gdb.COMMAND_NONE, gdb.COMPLETE_NONE)

    def invoke (self, arg, from_tty):
        al = Arglist()
        for s in arg.split():
            al.add(s)
        for s in Singleton().argv:
            al.add(s)

        al.encode()


bootargs_init_cmd()
bootargs_add_cmd()
bootargs_cmd()
