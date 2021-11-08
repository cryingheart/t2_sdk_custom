define R
    reset
    load
    bootargs_init
    set $i = 0
    while $i < $argc
        eval "bootargs_add $arg%d", $i
        set $i = $i + 1
    end
    bootargs_apply
    # do "cont" here to retain Ctrl-C support
    cont
end

document R
Short for "reset,load,bootargs_apply,cont"
Use Ctrl-C, set break point in app thread and do "cont".
end

# vim: sw=4 ts=8 expandtab syntax=gdb
