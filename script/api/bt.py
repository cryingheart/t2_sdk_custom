"""BT HCI interface
This interface is used to control BT via HCI.
Two formats are supported: "hci" - the standard hci format, "explicit" - user friendly with more obvious parameters.

    Sending HCI commands
    E.g. hci_reset
    hci 1,3,12,0               <-- hci: packet indicator "1", opcode "3,12(=3<<4)", length "0"
    cmd 3 3 0 0,               <-- explicit: ocf "3", ogf "3", length "0", parameters "0,"

    Sending ACL data:
    hci 2,0,0,5,0,0,1,2,3,4,   <-- hci: pi "2", handle "0,0", length "5,0", data "0,1,2,3,4,"
    dat 0 0 0 5 0,1,2,3,4,     <-- explicit: handle "0", pbf "0", bcf "0, length "5", data "0,1,2,3,4,"

    Receiving HCI events and ACL data:
    The "hci" format is used until the first HCI command is sent. The format of the first
    HCI command will then be used for all HCI events and ACL data.
"""


from api.base import message, status
from api.base import uint8, uint16, uint32, uint64


GROUP_NAME = 'bt'
GROUP_ID = 4


class hci(message):
    '''Send command/data (req) and receive event/data (rsp, ind) - hci format'''
    req   = [
        uint8("payload", array=0, doc='hci command or acl data'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint8("payload", array=0, doc='hci event  or acl data'),
    ]

class cmd(message):
    '''Send HCI command (req) and receive HCI event (rsb, ind) - explicit format'''
    req   = [
        uint8("ocf", doc='opcode command field'),
        uint8("ogf", doc='opcode group field'),
        uint8("len", doc='parameter length (0..255)'),
        uint8("param", array=0, doc='parameters'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint8("code", doc='event code'),
        uint8("len", doc='parameter length (0..255)'),
        uint8("param", array=0, doc='parameters'),
    ]

class dat(message):
    '''Send ACL data (req) and receive ACL data (ind) - explicit format'''
    req   = [
        uint8("handle", doc='connection handle'),
        uint8("pbf", doc='packet boundary flags: 0-start, 1-cont'),
        uint8("bcf", doc='broadcast flags: 0-point-to-point (no broadcast)'),
        uint8("len", doc='data length (0..255)'),
        uint8("data", array=0, doc='data'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint8("handle", doc='connection handle'),
        uint8("pbf", doc='packet boundary flags: 0-start, 1-cont'),
        uint8("bcf", doc='broadcast flags: 0-point-to-point (no broadcast)'),
        uint8("len", doc='data length (0..255)'),
        uint8("data", array=0, doc='data'),
    ]
