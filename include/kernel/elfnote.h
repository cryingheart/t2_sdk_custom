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

#define ELFNOTE_TYPE_PATCH 1
#define ELFNOTE_TYPE_HWVERSION 2
