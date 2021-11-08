'''Sockets interface

The sockets interface provides the host with a BSD sockets like interface for
implementing network clients and servers.
'''


from api.base import message, status
from api.base import uint8, uint16, uint32, uint64
from api.base import int8, int16, int32
from api.base import string
from api.base import ipaddr6


GROUP_NAME = 'sock'
GROUP_ID = 5


class server(message):
    '''Create a server socket that binds to the protocol, address and port specified by the URI'''
    req   = [
        string('uri', array=64,
               doc='URI on the form `<protocol>://<host>:<port>` where protocol may be tcp or udp'),
    ]
    rsp   = [
        status(),
        uint32('socket', doc='handle to the created socket if successful'),
    ]

class client(message):
    '''Create a client socket that connects to the protocol, address and port specified by the URI'''
    req   = [
        string('uri', array=64,
               doc='URI on the form `<protocol>://<host>:<port>` where protocol may be tcp or udp'),
    ]
    rsp   = [
        status(),
        uint32('socket', doc='handle to the created socket if successful'),
    ]

class getlocal(message):
    '''Returns the local IPv4/IPv6 address and port'''
    req  = [
        uint32('socket',  doc='handle to socket to return name for'),
    ]
    rsp  = [
        status(),
        uint16('port',    doc='local endpoint port'),
        uint16('addrlen', doc='size of the address IPv4(4)/IPv6(16)'),
        ipaddr6('addr',   doc='returned local or remote address'),
    ]

class getpeer(message):
    '''Returns the remote IPv4/IPv6 address and port'''
    req  = [
        uint32('socket',  doc='handle to socket to return name for'),
    ]
    rsp  = [
        status(),
        uint16('port', doc='remote endpoint port'),
        uint16('addrlen', doc='size of the address IPv4(4)/IPv6(16)'),
        ipaddr6('addr', doc='returned local or remote address'),
    ]

class getavailable(message):
    '''Get the number of bytes available for read'''
    req  = [
        uint32('socket',  doc='handle to socket to read data from'),
    ]
    rsp  = [
        status(),
        uint32('available', doc='number of bytes available for reading'),
    ]

class connection(message):
    '''Notities about a new server connection.'''
    ind   = [
        status(),
        uint32('socket', doc='handle to socket for the new connection'),
    ]

class close(message):
    '''Closes and destroys the socket specified by the handle'''
    req   = [
        uint32('socket', doc='handle to socket that will be closed and destroyed'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        status(),
        uint32('socket', doc='handle of socket to close'),
    ]

class notification(message):
    '''Request notification when socket is readable'''
    req   = [
        uint32('socket', doc='handle to socket that will be closed and destroyed'),
        uint32('threshold', doc='Fire notification when abount of readable data is above threshold'),
        uint32('flags', doc='reserved for future use'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        status(),
        uint32('socket', doc='handle to socket that can be read'),
        uint32('flags', doc='reserved for future use'),
    ]

class recvfrom(message):
    '''Reads up to `size` data bytes from the socket. May return less than `size`. If not data is available the `actual` size will be 0 and status != 0'''
    req   = [
        uint32('socket', doc='socket descriptor'),
        uint32('size', doc='number of bytes to receive'),
        uint32('flags', doc='reserved for future use'),
    ]
    rsp   = [
        status(),
        int32('actual', doc='number of bytes received from the socket'),
        ipaddr6('addr', doc='source address'),
        uint16('addrlen', doc='size of the address IPv4(4)/IPv6(16)'),
        uint16('port', doc='source port'),
        uint8('data', array=0, doc='buffer with received data'),
    ]

class sendto(message):
    '''Writes `size` data bytes into the socket. Used on non-connected sockets'''
    req   = [
        uint32('socket', doc='socket descriptor'),
        uint32('size',   doc='size of data to write to the socket'),
        uint32('flags',  doc='reserved for future use'),
        ipaddr6('addr',  doc='destination address'),
        uint16('addrlen', doc='size of the address IPv4(4)/IPv6(16)'),
        uint16('port', doc='destination port'),
        uint8('data', array=0, doc='buffer containing the data to write to the socket'),
    ]
    rsp   = [
        status(),
        uint32('actual', doc='number of bytes written to the socket'),
    ]

class resolve(message):
    '''Returns the IPv4/IPv6 address of a hostname'''
    req   = [
        uint8('family', doc=' Address family of socket IPv4/IPv6 <0,4,6,46,64> default is 0'),
        string('hostname', array=0, doc='the hostname to resolve'),
    ]
    rsp   = [
        status(),
        ipaddr6('ipaddr', doc='the IPv4/IPv6 address of the hostname'),
        uint16('addrlen', doc='size of the address IPv4(4)/IPv6(16)'),
    ]
