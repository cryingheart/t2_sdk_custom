"""HIO interface

This message group deals with control type messages, such as
capability discovery, and device configuration.

"""


from api.base import message, status
from api.base import uint8, uint16, uint32
from api.base import string


GROUP_NAME = 'hio'
GROUP_ID = 0


class query(message):
    '''Query device about available procedures.'''
    req   = [
    ]
    rsp   = [
        status(),
        uint8("version", doc='protocol version'),
        uint8("ngroups", doc='length of nmsg vector'),
        uint16("maxsize", doc='max request size'),
        string("fw_rev", array=10, doc='Firware revision string'),
        string("patch_rev", array=10, doc='ROM patch revision string'),
        uint16("nmsg", array=0, doc='supported hostio interfaces, coded as 256*group + num_api'),
    ]

class config(message):
    '''Configure wake-up and interrupt pins.'''
    req   = [
        uint8("wakeup_pin", doc='GPIO pin to use for wakeup, zero to disable'),
        uint8("wakeup_level", doc='wakeup level high(1) or low(0)'),
        uint8("irq_pin", doc='GPIO pin to use for interrupt'),
        uint8("irq_mode", doc='Interrupt mode to use: none(0), high(1), or toggle(2)'),
    ]
    rsp   = [
        status()
    ]

class sleep(message):
    '''Indicate that device may go to sleep.'''
    req   = [
    ]

class irqack(message):
    '''Acknowledge level interrupt.'''
    req   = [
    ]

class ping(message):
    '''Solicit response from device.'''
    req   = [
        uint8("payload", array=0, doc='Ping message content.'),
    ]
    rsp   = [
        uint8("payload", array=0, doc='Ping response content.'),
    ]

class wakeup(message):
    '''Informs host that device is awake. Keep wakeup_pin set until wakeup_ind is received.'''
    ind   = [
        uint32("reserved")
    ]

class console(message):
    '''Debug console output.'''
    ind   = [
        string("str", array=0, doc='console message'),
    ]
