/**
 * @addtogroup args
 * @{
 */
#pragma once

#define ELFNOTE(_name, _type, _desc)                         \
    asm(".pushsection .note." #_name ", \"\", \"note\";"     \
        ".balign 4;"                                         \
        ".long 2f - 1f;"                                     \
        ".long 4f - 3f;"                                     \
        ".long "#_type ";"                                   \
        "1:;"                                                \
        ".asciz \"" #_name "\";"                             \
        "2:;"                                                \
        ".balign 4;"                                         \
        "3:;"                                                \
        #_desc ";"                                           \
        "4:;"                                                \
        ".balign 4;"                                         \
        ".popsection")

#define BOOTARG_TYPE_STRING  0
#define BOOTARG_TYPE_BOOL    1
#define BOOTARG_TYPE_INTEGER 2
#define BOOTARG_TYPE_OLDNAME 256
#define BOOTARG_TYPE_SUPERSEDED 257

#define _BOOTARG_PSTR(_s) .byte 11f - 10f; 10:; .ascii _s; 11:;

#define _BOOTARG_ELFNOTE(...)                   \
    ELFNOTE(innophase, 3, __VA_ARGS__)

#define _BOOTARG(_name, _type, _metavar, _flags, _description)         \
    _BOOTARG_ELFNOTE(                                                   \
            .long 0;                                                    \
            .long _type;                                                \
            .long _flags;                                               \
            _BOOTARG_PSTR(_name);                                       \
            _BOOTARG_PSTR(_metavar);                                    \
            _BOOTARG_PSTR(_description);                                \
        )

#define BOOTARG_INT(_name, _metavar, _descr)\
    _BOOTARG(_name, BOOTARG_TYPE_INTEGER, _metavar, 0, _descr)

#define BOOTARG_BOOL(_name, _descr)\
    _BOOTARG(_name, BOOTARG_TYPE_BOOL, "0|1", 0, _descr)

#define BOOTARG_STRING(_name, _metavar, _descr)          \
    _BOOTARG(_name, BOOTARG_TYPE_STRING, _metavar, 0, _descr)

/* _oldname is now called _newname */
#define BOOTARG_OLDNAME(_oldname, _newname)\
    _BOOTARG(_oldname, BOOTARG_TYPE_OLDNAME, _newname, 0, "")

/* _oldname has been removed, and similar functionality is now set via
 * _newname, but values do not map one-to-one */
#define BOOTARG_SUPERSEDED(_oldname, _newname)\
    _BOOTARG(_oldname, BOOTARG_TYPE_SUPERSEDED, _newname, 0, "")

const char* os_get_boot_arg(const char *name);

int os_get_boot_arg_int(const char *name, int defval);

const char *os_get_boot_arg_str(const char *name);

int os_parse_boot_arg_int(const char *strval, int *value);

void os_print_boot_args(void);

/** @} */
