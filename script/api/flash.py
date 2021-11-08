"""Flash programming interface

This message group defines operations to program serial flash.
"""


from api.base import message, status
from api.base import macaddr, uuid
from api.base import uint8, uint16, uint32


GROUP_NAME = 'flash'
GROUP_ID = 8


class identify(message):
    '''Check presence of supported serial flash device.'''
    req   = [
    ]
    rsp   = [
        status(),
        uint16('page_size', doc='Size of flash page'),
        uint16('sector_size', doc='Size of flash erase sector'),
        uint32('num_pages', doc='Device capacity in units of flash pages'),
        uint32('idcode', doc='Flash device manufacturer and device code'),
    ]

class read(message):
    '''Read from flash.'''
    req   = [
        uint32('address', doc='Sector address to start reading from'),
        uint32('length', doc='Number of bytes to read'),
    ]
    rsp   = [
        status(),
        uint8('data', array=0, doc='Data read from flash'),
    ]

class write(message):
    '''Write to flash.'''
    req   = [
        uint32('address', doc='Sector address where write should start'),
        uint32('length', doc='Number of bytes to write'),
        uint8('data', array=0, doc='Data to write to flash'),
    ]
    rsp   = [
        status(),
    ]

class enroll(message):
    '''Enrollment message.'''
    req   = [
        uint8('secureboot_mode', doc='PUF or efuse'),
        uint8('secret', array=33, doc='Secret key to store in PUF'),
        uint8('ecdsa_key', array=64, doc='Public key for ECDSA signature validation'),
        uint8('fw_key', array=32, doc='Key for FW encryption'),
    ]
    rsp   = [
        status(),
    ]

class file_mount(message):
    '''Mount smartfs filesystem.'''
    req   = [
        uint32('address', doc='Sector address where filesystem starts'),
        uint32('size',    doc='Size of filesystem'),
    ]
    rsp   = [
        status(),
    ]

class file_open(message):
    '''Open file.'''
    req   = [
        uint32('mode', doc='File mode'),
        uint8('path', array=0, doc='File path'),
    ]
    rsp   = [
        status(),
    ]

class file_remove(message):
    '''Open file.'''
    req   = [
        uint32('path_len', doc='Length of path'),
        uint8('path', array=0, doc='File path'),
    ]
    rsp   = [
        status(),
    ]

class file_read(message):
    '''Read from file.'''
    req   = [
        uint32('length', doc='Number of bytes to read'),
    ]
    rsp   = [
        status(),
        uint8('data', array=0, doc='Data read from flash'),
    ]

class file_write(message):
    '''Write to file.'''
    req   = [
        uint32('length', doc='Number of bytes to write'),
        uint8('data', array=0, doc='Data to write to flash'),
    ]
    rsp   = [
        status(),
    ]

class file_close(message):
    '''Close file.'''
    req   = [
    ]
    rsp   = [
        status(),
    ]

class file_stat(message):
    '''Return some info about file.'''
    req   = [
        uint32('path_len', doc='Length of path'),
        uint8('path', array=0, doc='File path'),
    ]
    rsp   = [
        status(),
        uint32('mode', doc='Some info about file'),
    ]

class file_dir_open(message):
    '''Open a directory.'''
    req   = [
        uint8('path', array=0, doc='File path'),
    ]
    rsp   = [
        status(),
    ]

class file_dir_read(message):
    '''Read a entry from the opened directory.'''
    req   = [
    ]
    rsp   = [
        status(),
        uint32('mode', doc='Some info about file'),
        uint8('data', array=0, doc='Data read from flash'),
    ]

class hash(message):
    '''calculate hash of flash.'''
    req   = [
        uint32('address', doc='Sector address to start reading from'),
        uint32('length', doc='Number of bytes to read'),
    ]
    rsp   = [
        status(),
        uint8('data', array=0, doc='Calculated hash of flash'),
    ]

class boot(message):
    '''Write to sram.'''
    req   = [
        uint32('address', doc='SRAM address where write should start'),
        uint32('length', doc='Number of bytes to write'),
        uint8('data', array=0, doc='Data to write to ram'),
    ]
    rsp   = [
        status(),
    ]

class flush(message):
    '''Flushes any unsaved state to flash.'''
    req   = [
    ]
    rsp   = [
        status(),
    ]

class get_hwaddr(message):
    '''Retreives the hardware address stored in flash.'''
    req   = [
    ]
    rsp   = [
        status(),
        macaddr("hwaddr", doc='''Hardware address stored in flash.''')
    ]

class set_hwaddr(message):
    '''Stores hardware address to flash.'''
    req   = [
        macaddr("hwaddr", doc='''Hardware address to store in flash.''')
    ]
    rsp   = [
        status()
    ]

class get_uuid(message):
    '''Retreives the UUID stored in flash.'''
    req   = [
    ]
    rsp   = [
        status(),
        uuid("uuid", doc='''UUID stored in flash.''')
    ]

class set_uuid(message):
    '''Stores a UUID to flash.'''
    req   = [
        uuid("uuid", doc='''UUID to store in flash.''')
    ]
    rsp   = [
        status()
    ]

class load_partition_table(message):
    '''Loads partition table into RAM. To commit any changes it has to be saved.'''
    req = [
        uint32('revert', doc='''Forcefully loads table even if modified in memory.''')
    ]
    rsp = [
        status()
    ]

class save_partition_table(message):
    '''Saves a previously loaded partition table, along with any changes.'''
    req = [
    ]
    rsp = [
        status()
    ]

class get_partition(message):
    '''Retreives information about single partition.'''
    req = [
        uint32('index', doc='Index to retreive.')
    ]
    rsp = [
        status(),
        uint32('type', doc='Partition type'),
        uint32('sect_start', doc='Starting sector number'),
        uint32('sect_count', doc='Sector count')
    ]

class add_partition(message):
    '''Adds a partition at the given index.'''
    req = [
        uint32('index', doc='Partition index to add.'),
        uint32('type', doc='Partition type'),
        uint32('sect_start', doc='Starting sector number'),
        uint32('sect_count', doc='Sector count'),
    ]
    rsp = [
        status()
    ]

class delete_partition(message):
    '''Marks the partition at the given index as deleted.'''
    req = [
        uint32('index', doc='Partition index to delete.')
    ]
    rsp = [
        status()
    ]

class erase_by_sector(message):
    '''Erase the chip by sector number'''
    req   = [
        uint32("address",doc='Sector number between 1 to 511'),
    ]
    rsp   = [
        status(),
    ]
