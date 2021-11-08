"""Traffic agent interface

Traffic agent corresponding to the one specified in WFA CAPI.
"""


from api.base import message, status
from api.base import uint8, uint16, uint32, uint64
from api.base import int8, int16, int32
from api.base import string


GROUP_NAME = 'traffic_agent'
GROUP_ID = 10


class rx_config(message):
    '''Create an UDP rx stream'''
    req   = [
        uint8('destination', array=16, doc='destination address, used for multicast, set zero otherwize'),
        uint16('destinationlen', doc='size of the address IPv4(4)/Pv6(16)'),
        uint16('destination_port', doc='The listen UDP port'),
        uint8('source', array=16, doc='source address'),
        uint16('sourcelen', doc='size of the address IPv4(4)/Pv6(16)'),
        uint16('source_port', doc='UDP port of the sending traffic agent'),
        uint32('payload_size', doc='Number of bytes per frame'),
        uint32('echo', doc='Echo incoming packets'),
    ]
    rsp   = [
        status(),
        uint32('stream_id', doc='Identifier of the created stream')
    ]

class rx_start(message):
    '''Start an UDP rx stream'''
    req   = [
        uint32('stream_id', doc='Identifier of the stream')
    ]
    rsp   = [
        status(),
    ]

class rx_stop(message):
    '''Stop an UDP rx stream'''
    req   = [
        uint32('stream_id', doc='Identifier of the stream')
    ]
    rsp   = [
        status(),
        uint32('rx_frames', doc='Number of frames received'),
        uint32('rx_bytes', doc='Number of bytes received'),
        uint32('tx_frames', doc='Number of frames transmitted'),
        uint32('tx_bytes', doc='Number of bytes transmitted')
    ]

class tx_config(message):
    '''Create an UDP tx stream'''
    req   = [
        uint8('destination', array=16, doc='destination address'),
        uint16('destinationlen', doc='size of the address IPv4(4)/Pv6(16)'),
        uint16('destination_port', doc='The listen UDP port'),
        uint16('source_port', doc='UDP port of the sending traffic agent'),
        uint16('start_delay', doc='Number of seconds befor start'),
        uint32('frame_rate', doc='Number of frames per second'),
        uint32('payload_size', doc='Number of bytes per frame'),
        uint32('tos', doc='IP Type Of Service field'),
        uint32('duration', doc='Duration in seconds'),
    ]
    rsp   = [
        status(),
        uint32('stream_id', doc='Identifier of the created stream')
    ]

class tx_send(message):
    '''Start an UDP tx stream'''
    req   = [
        uint32('stream_id', doc='Identifier of the stream')
    ]
    rsp   = [
        status(),
    ]

class tx_ready(message):
    '''Check if an UDP tx stream is done'''
    req   = [
        uint32('stream_id', doc='Identifier of the stream')
    ]
    rsp   = [
        status(),
        uint32('ready', doc='1 if tx stream has completed'),
        uint32('tx_frames', doc='Number of frames sent'),
        uint32('tx_bytes', doc='Number of bytes sent')
    ]

class reset(message):
    '''Remove all streams'''
    req   = [
    ]
    rsp   = [
        status(),
    ]
