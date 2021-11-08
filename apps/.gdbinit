# -*- gdb-script -*-

# T2 memory regions
define mem_t2
    # ROM
    mem 0 0x40000 ro
    # RAM
    mem 0x40000 0xc0000 rw
    # Virtual
    mem 0x100000 0x200000 ro
    # Registers
    mem 0xfc0000 0x1000000 rw
    # Fictive load addresses for virtual image
    mem 0x2000000 0x2100000 rw
end

define ocd
  dont-repeat
  if $argc == 0
    target remote localhost:3333
  else
    target remote $arg0:3333
  end
end

document ocd
Connect to OpenOCD server on specified host (or localhost with out arguments).
This is just a wrapper around "target remote".
end

define reset
  # Route sysresetreq to CPU reset (not PoR since that will reset PMU and undo the XTAL fix)

  set *(int*)0xfc0104 = 8
  monitor reset halt
  set *(short*)0xfc0100 = 1
end

document reset
Resets the Talaria TWO chip.
end

define restart
  reset
  load
  cont
end

document restart
Start current application by performing a reset, reloading the
ELF-file and restarting.
end

source .gdb-bootargs.py
source .gdb-bootargs.gdb

# Setup memory regions for T2
mem_t2
