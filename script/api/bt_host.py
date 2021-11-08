"""BT host interface

This interface is used to run BT in host mode.
See BT specification, Vol 3: Host Volume, part C: GAP, chapter 9.2, 9.3 and 9.10 for
GAP discoverable, GAP discovery, GAP connectable, GAP connection, GAP bonding modes and
GAP termination.

"""


from api.base import message, status
from api.base import uint8, uint16, uint32, uint64
from api.base import int8, int16, int32
from api.base import macaddr
from api.base import string


GROUP_NAME = 'bt_host'
GROUP_ID = 9


class gap_addr_set(message):
    '''set address for given address type '''
    req   = [
        uint8("addr_type", doc='address type to set: 0=public, 1=random'),
        macaddr("addr", doc='address'),
    ]
    rsp   = [
        status(),
    ]
class gap_addr_get(message):
    '''get address for given address type '''
    req   = [
        uint8("addr_type", doc='address type to get: 0=public, 1=random'),
    ]
    rsp   = [
        status(),
        macaddr("addr", doc='address used for the given address type'),
    ]
class gap_cfg_adv(message):
    '''configure advertise'''
    req   = [
        uint16("adv_fast_period", doc='In ms (0 = infinit), using adv_fast_int for adv_fast_period, then using adv_slow_int (default: 0)'),
        uint16("adv_slow_period", doc='In ms (0 = infinit), using adv_slow_int for adv_slow_period, then disable advertise (default: 0)'),
        uint16("adv_fast_int", doc='In 625 us, range: 0x0020 to 0x4000 (default: 160)'),
        uint16("adv_slow_int", doc='In 625 us, range: 0x0020 to 0x4000 (default: 1600)'),
        int8("adv_tx_power", doc='In dBm, range: -127 to 10, and 127 (127=no preference) (default: 127)'),
        uint8("adv_ch_map", doc='Channel map used: bit0=ch37, bit1=ch38, bit2=ch39 (default: 0x7)'),
    ]
    rsp   = [
        status(),
    ]
class gap_set_adv_data(message):
    '''set advertising data'''
    req   = [
        uint8("length", doc='number of significant octets in the advertising data (1..31)'),
        uint8("data", array=31, doc='advertising data'),
    ]
    rsp   = [
        status(),
    ]
class gap_set_srsp_data(message):
    '''set scan response data'''
    req   = [
        uint8("length", doc='number of significant octets in the scan response data (1..31)'),
        uint8("data", array=31, doc='scan response data'),
    ]
    rsp   = [
        status(),
    ]
class gap_cfg_scan(message):
    '''configure scan'''
    req   = [
        uint16("scan_period", doc='In ms, foreground scanning (no connected link) (default: 10240)'),
        uint16("scan_int", doc='In 625 us, range: 0x0004 to 0x4000 (default: 96)'),
        uint16("scan_win", doc='In 625 us, range: 0x0004 to 0x4000 (default: 48)'),
        uint16("scan_bkg_int", doc='In 625 us, range: 0x0004 to 0x4000 (default: 2048)'),
        uint16("scan_bkg_win", doc='In 625 us, range: 0x0004 to 0x4000 (default: 18)'),
        uint8("scan_filter_duplicates", doc='1=True, 0=False (default: 1)')
    ]
    rsp   = [
        status(),
    ]
class gap_cfg_conn(message):
    '''configure connection'''
    req   = [
        uint16("conn_interval", doc='in 1.25 ms, range: 0x0006 to 0x0C80 (default: 80)'),
        uint16("conn_latency", doc='in intervals, range: 0x0000 to 0x01F3 (default: 0)'),
        uint16("conn_timeout", doc=' in ms, range: 0x000A to 0x0C80 (default: 2000)'),
        uint16("conn_params_reject", doc='Reject parameter update, 1=True, 0=False (default: 0)'),
        uint16("conn_params_int_min", doc='in 1.25 ms, parameter update min connection interval (default: 6)'),
        uint16("conn_params_int_max", doc='in 1.25 ms, parameter update max connection interval (default: 800)'),
    ]
    rsp   = [
        status(),
    ]
class gap_cfg_smp(message):
    '''configure security (smp)'''
    req   = [
        uint8("io_cap", doc='I/O-capabilities: 0-display_only, 1-display_yes_no, 2-keyboard_only, 3-no_input_no_output, 4-keyboard_display (default: 0)'),
        uint8("oob", doc='OOB exists: 1=True, 0=False (default: 0)'),
        uint8("bondable", doc='bondable: 1=True, 0=False (default: 0)'),
        uint8("mitm", doc='MITM protection: 1=True, 0=False (default: 0)'),
        uint8("sc", doc='Secure connection: 1=True, 0=False (default: 0)'),
        uint8("keypress", doc='Send keypress: 1=True, 0=False (default: 0)'),
        uint8("key_size", doc='Smallest key size (7..16 octets) (default: 16)'),
        uint8("encrypt", doc='Automatically encrypt link at connection setup if key exists: 1=True, 0=False (default: 0)'),
    ]
    rsp   = [
        status(),
    ]
class gap_cfg_ext(message):
    '''configure extended'''
    req   = [
        uint8("ext_use", doc='Extended (5.0) usage: 1-legacy, 2-extended, 3-legacy+extended (default: 1)'),
        uint8("adv_pri_phy", doc='Primary PHY: 1-LE 1M, 3-LE Coded (default: 3)'),
        uint8("adv_sec_phy", doc='Secondary PHY:  1-LE 1M, 2-LE 2M, 3-LE Coded (default: 3)'),
        uint8("adv_sid", doc='Advertising SID (0..15) (default: 13)'),
        uint8("conn_phy", doc='Preferred phy(s) during connection - bit0: 1M, bit1: 2M, bit2: CodedS2, bit3: CodedS8, 0=no preference (default: 0x1)'),
        uint8("conn_len", doc='Max length of transmitted data during connection (27..251) (default: 27)'),
    ]
    rsp   = [
        status(),
    ]
class gap_broadcast(message):
    '''broadcast mode'''
    req   = [
        uint8("mode", doc='0=disable, 1=enable'),
        uint8("own_type", doc='own address type: 0=public, 1=random, 2=resolvable (or public), 3=resolvable (or random)'),
        uint8("peer_type", doc='peer address type: 0=public, 1=random'),
        macaddr("peer_addr", doc='peer address'),
    ]
    rsp   = [
        status(),
    ]
class gap_discoverable(message):
    '''discoverable mode'''
    req   = [
        uint8("mode", doc='0=disable, 1=non, 2=limited, 3=general'),
        uint8("own_type", doc='own address type: 0=public, 1=random, 2=resolvable (or public), 3=resolvable (or random)'),
        uint8("peer_type", doc='peer address type: 0=public, 1=random'),
        macaddr("peer_addr", doc='peer address'),
    ]
    rsp   = [
        status(),
    ]
class gap_discovery(message):
    '''discovery mode'''
    req   = [
        uint8("mode", doc='0=disable, 1=limited, 2=general, 3=name'),
        uint8("own_type", doc='own address type: 0=public, 1=random, 2=resolvable (or public), 3=resolvable (or random)'),
        uint8("peer_type", doc='peer address type (only for mode "name"): 0=public, 1=random, 2=public identity, 3=random identity'),
        macaddr("peer_addr", doc='peer address (only for mode "name")'),
    ]
    rsp   = [
        status(),
    ]
class gap_connectable(message):
    '''connectable mode'''
    req   = [
        uint8("mode", doc='0=disable, 1=non, 2=direct, 3=undirect'),
        uint8("own_type", doc='own address type: 0=public, 1=random, 2=resolvable (or public), 3=resolvable (or random)'),
        uint8("peer_type", doc='peer address type: 0=public, 1=random'),
        macaddr("peer_addr", doc='peer address'),
    ]
    rsp   = [
        status(),
    ]
class gap_connection(message):
    '''connection mode'''
    req   = [
        uint8("mode", doc='0=disable, 1=auto, 2=general, 3=selective, 4=direct ("auto" and "selective" require a white list)'),
        uint8("own_type", doc='own address type: 0=public, 1=random, 2=resolvable (or public), 3=resolvable (or random)'),
        uint8("peer_type", doc='peer address type: 0=public, 1=random, 2=public identity, 3=random identity'),
        macaddr("peer_addr", doc='peer address'),
    ]
    rsp   = [
        status(),
    ]
class gap_terminate(message):
    '''terminate connection'''
    req   = [
        uint8("handle", doc='connection handle'),
    ]
    rsp   = [
        status(),
    ]
class gap_authenticate(message):
    '''authenticate connection with provided settings'''
    req   = [
        uint8("handle", doc='connection handle'),
        uint8("oob", doc='OOB protection: 1=True, 0=False'),
        uint8("bondable", doc='bondable: 1=True, 0=False'),
        uint8("mitm", doc='MITM protection: 1=True, 0=False'),
        uint8("sc", doc='Secure connection: 1=True, 0=False'),
        uint8("key128", doc='128-bits key required: 1=True, 0=False'),
    ]
    rsp   = [
        status(),
   ]
class gap_connection_update(message):
    '''update existing connection with new connection parameters'''
    req   = [
        uint16("handle", doc='connection handle'),
        uint16("interval_min", doc='in 1.25 ms, range: 0x0006 to 0x0C80'),
        uint16("interval_max", doc='in 1.25 ms, range: 0x0006 to 0x0C80'),
        uint16("latency", doc='in intervals, range: 0x0000 to 0x01F3'),
        uint16("timeout", doc=' in ms, range: 0x000A to 0x0C80'),
    ]
    rsp   = [
        status(),
    ]
class gap_add_device_to_white_list(message):
    '''add device to white list'''
    req   = [
        uint8("addr_type", doc='address type: 0=public, 1=random'),
        macaddr("addr", doc='public or random device address'),
    ]
    rsp   = [
        status(),
    ]
class gap_remove_device_from_white_list(message):
    '''remove device from white list'''
    req   = [
        uint8("addr_type", doc='address type: 0=public, 1=random'),
        macaddr("addr", doc='public or random device address'),
    ]
    rsp   = [
        status(),
    ]
class gap_clear_white_list(message):
    '''clear white list'''
    req   = [
    ]
    rsp   = [
        status(),
    ]
class gap_add_device_to_resolving_list(message):
    '''add device to resolving list'''
    req   = [
        uint8("addr_type", doc='address type: 0=public, 1=random'),
        macaddr("addr", doc='public or random device address'),
        uint8("peer_irk", array=16, doc='IRK of the peer device'),
        uint8("local_irk", array=16, doc='IRK of the local device'),
    ]
    rsp   = [
        status(),
    ]
class gap_remove_device_from_resolving_list(message):
    '''remove device from resolving list'''
    req   = [
        uint8("addr_type", doc='address type: 0=public, 1=random'),
        macaddr("addr", doc='public or random device address'),
    ]
    rsp   = [
        status(),
    ]
class gap_clear_resolving_list(message):
    '''clear resolving list'''
    req   = [
    ]
    rsp   = [
        status(),
    ]
class gap_set_address_resolution_enable(message):
    '''enable address resolution'''
    req   = [
        uint16("timeout", doc='Resolvable private address timeout in s (default: 900s)'),
        uint8("enable", doc='Enable: 1=True, 0=False (default: 0)'),
    ]
    rsp   = [
        status(),
    ]
class gap_event(message):
    '''hci event'''
    ind   = [
        uint8("payload", array=0, doc='hci event, see BT specification, Vol 2: Core, part E: HCI, chapter 7.7.'),
    ]
class smp_passkey(message):
    '''enter passkey (or oob) for smp pairing'''
    req   = [
        uint32("key0", doc='20 bits passkey or OOB0 (bits 0..31)'),
        uint32("oob1", doc='OOB1 (bits 32..63)'),
        uint32("oob2", doc='OOB2 (bits 64..95)'),
        uint32("oob3", doc='OOB3 (bits 96..127)'),
    ]
    rsp   = [
        status(),
    ]
class gatt_exchange_mtu(message):
    '''This sub-procedure is used by the client to set the ATT_MTU to the maximum possible value that can be supported by both
    devices when the client supports a value greater than the default ATT_MTU for the Attribute protocol.'''
    req   = [
        uint16("size", doc='client rx mtu size (23..251) (default: 23)'),
    ]
    rsp   = [
        status(),
    ]
class gatt_discover_all_primary_services(message):
    '''This sub-procedure is used by a client to discover all the primary services on a server.'''
    req   = [
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint16("attr_handle", doc='handle for the service declaration'),
        uint16("end_handle", doc='handle of the last attribute within the service definition'),
        uint16("size", doc='service uuid size in bytes, 2-uuid16, 16-uuid128'),
        uint8("uuid", array=16, doc='service uuid - 16 or 128 bits'),
    ]
class gatt_discover_primary_service_by_service_uuid(message):
    '''This sub-procedure is used by a client to discover a specific primary service on a server when only the Service UUID is known.'''
    req   = [
        uint16("size", doc='uuid size in bytes, 2-uuid16, 16-uuid128'),
        uint8("uuid", array=16, doc='uuid - 16 or 128 bits'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint16("start_handle", doc='starting handle of the service definition'),
        uint16("end_handle", doc='ending handle of the service definition'),
    ]
class gatt_find_included_services(message):
    '''This sub-procedure is used by a client to find include service declarations within a service definition on a server.'''
    req   = [
        uint16("start_handle", doc='starting handle of the specified service'),
        uint16("end_handle", doc='ending handle of the specified service'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint16("attr_handle", doc='attribute handle'),
        uint16("isa_handle", doc=' handle'),
        uint16("end_handle", doc='end group handle'),
        uint16("size", doc='uuid size in bytes, 2-uuid16, 0-not included (=uuid128)'),
        uint16("uuid", doc='uuid16 (if size is 2, else not valid)'),
    ]
class gatt_discover_all_characteristics_of_a_service(message):
    '''This sub-procedure is used by a client to find all the characteristic declarations within a service definition on a server
    when only the service handle range is known.'''
    req   = [
        uint16("start_handle", doc='starting handle of the specified service'),
        uint16("end_handle", doc='ending handle of the specified service'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint16("handle", doc='handle for the characteristic declaration'),
        uint16("prop", doc='characteristic properties'),
        uint16("value_handle", doc='characteristic value handle'),
        uint16("size", doc='characteristic uuid size in bytes, 2-uuid16, 16-uuid128'),
        uint8("uuid", array=16, doc='characteristic uuid - 16 or 128 bits'),
    ]
class gatt_discover_characteristics_by_uuid(message):
    '''This sub-procedure is used by a client to discover service characteristics on a server when only the service handle ranges are known
    and the characteristic UUID is known.'''
    req   = [
        uint16("start_handle", doc='starting handle for the service handle range'),
        uint16("end_handle", doc='ending handle for the service handle range'),
        uint16("size", doc='uuid size in bytes, 2-uuid16, 16-uuid128'),
        uint8("uuid", array=16, doc='uuid - 16 or 128 bits'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint16("attr_handle", doc='handle for the characteristic declaration'),
        uint16("prop", doc='characteristic properties'),
        uint16("value_handle", doc='characteristic value handle'),
        uint16("size", doc='characteristic uuid size in bytes, 2-uuid16, 16-uuid128'),
        uint8("uuid", array=16, doc='characteristic uuid - 16 or 128 bits'),
    ]
class gatt_discover_all_characteristic_descriptors(message):
    '''This sub-procedure is used by a client to find all the characteristic descriptor's Attribute Handles and Attribute Types
    within a characteristic definition when only the characteristic handle range is known.'''
    req   = [
        uint16("start_handle", doc='starting handle of the specified service'),
        uint16("end_handle", doc='ending handle of the specified service'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint16("attr_handle", doc='handle for the characteristic descriptor declaration'),
        uint16("size", doc='characteristic uuid size in bytes, 2-uuid16, 16-uuid128'),
        uint8("uuid", array=16, doc='characteristic uuid - 16 or 128 bits'),
    ]
class gatt_read_characteristic_value(message):
    '''This sub-procedure is used to read a Characteristic Value from a server when the client knows the Characteristic Value Handle.'''
    req   = [
        uint16("value_handle", doc='value_handle to be read from remote server'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint16("handle", doc='handle for the read characteristic'),
        uint16("size", doc='size of value in bytes'),
        uint8("value", array=0, doc='value of read attribute'),
    ]
class gatt_read_using_characteristic_uuid(message):
    '''This sub-procedure is used to read a Characteristic Value from a server when the client only knows the characteristic UUID.'''
    req   = [
        uint16("start_handle", doc='starting handle of the specified service'),
        uint16("end_handle", doc='ending handle of the specified service'),
        uint16("size", doc='uuid size in bytes, 2-uuid16, 16-uuid128'),
        uint8("uuid", array=16, doc='uuid - 16 or 128 bits'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint16("handle", doc='handle for the read characteristic'),
        uint16("size", doc='size of value in bytes'),
        uint8("value", array=0, doc='value of read attribute'),
    ]
class gatt_read_long_characteristic_value(message):
    '''Same as gatt_read_characteristic_value, but for long values.'''
    req   = [
        uint16("value_handle", doc='value_handle to be read from remote server'),
        uint16("value_offset", doc='value_offset to be read'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint16("handle", doc='handle for the read characteristic'),
        uint16("size", doc='size of value in bytes'),
        uint8("value", array=0, doc='value of read attribute'),
    ]
class gatt_read_multiple_characteristic_values(message):
    '''This sub-procedure is used to read multiple Characteristic Values from a server when the client knows the Characteristic Value Handles.'''
    req   = [
        uint16("nof_handles", doc='number of handles to be read'),
        uint8("handles", array=0, doc='handles to be read (two bytes per handle (lsb,msb))'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint16("handle", doc='handle for the read characteristic'),
        uint16("size", doc='size of value in bytes'),
        uint8("value", array=0, doc='value of read attribute'),
    ]
class gatt_read_characteristic_descriptor(message):
    '''This sub-procedure is used to read a characteristic descriptor from a server when the client knows the
    characteristic descriptor declaration's Attribute handle.'''
    req   = [
        uint16("handle", doc='handle for the characteristic descriptor'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint16("handle", doc='handle for the read characteristic descriptor'),
        uint16("size", doc='size of value in bytes'),
        uint8("value", array=0, doc='value of read characteristic descriptor'),
    ]
class gatt_read_long_characteristic_descriptor(message):
    '''Same as gatt_read_characteristic_descriptor, but for long values.'''
    req   = [
        uint16("handle", doc='handle for the characteristic descriptor'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint16("handle", doc='handle for the read characteristic descriptor'),
        uint16("size", doc='size of value in bytes'),
        uint8("value", array=0, doc='value of read characteristic descriptor'),
    ]
class gatt_write_without_response(message):
    '''This sub-procedure is used to write a Characteristic Value to a server when the client knows the Characteristic
    Value Handle and the client does not need an acknowledgment that the write was successfully performed.'''
    req   = [
        uint16("value_handle", doc='value_handle to be written'),
        uint8("value", array=0, doc='value to write'),
    ]
    rsp   = [
        status(),
    ]
class gatt_signed_write_without_response(message):
    '''This sub-procedure is used to write a Characteristic Value to a server when the client knows the Characteristic
    Value Handle and the ATT bearer is not encrypted.'''
    req   = [
        uint16("value_handle", doc='value_handle to be written'),
        uint8("value", array=0, doc='value to write'),
    ]
    rsp   = [
        status(),
    ]
class gatt_write_characteristic_value(message):
    '''This sub-procedure is used to write a Characteristic Value to a server when the client knows the Characteristic Value Handle.'''
    req   = [
        uint16("value_handle", doc='value_handle to be written'),
        uint8("value", array=0, doc='value to write'),
    ]
    rsp   = [
        status(),
    ]
class gatt_write_characteristic_descriptor(message):
    '''This sub-procedure is used to write a characteristic descriptor value to a server when the client knows the characteristic descriptor handle.'''
    req   = [
        uint16("handle", doc='handle for the characteristic descriptor'),
        uint8("value", array=0, doc='value to write'),
    ]
    rsp   = [
        status(),
    ]
class gatt_notification(message):
    '''This sub-procedure is used when a server is configured to notify a Characteristic Value to a client without expecting any Attribute Protocol layer
    acknowledgment that the notification was successfully received.'''
    req   = [
        uint8("value", doc='value in notification'),
    ]
    rsp   = [
        status(),
    ]
class gatt_indication(message):
    '''This sub-procedure is used when a server is configured to indicate a Characteristic Value to a client and expects an Attribute Protocol layer
    acknowledgment that the indication was successfully received.'''
    req   = [
        uint8("value", doc='value in indication'),
    ]
    rsp   = [
        status(),
    ]
class gatt_service_changed(message):
    '''This sub-procedure is used to trigger a service changed indication (the client needs to write the CCCD first).'''
    req   = [
    ]
    rsp   = [
        status(),
    ]
class bt_iperf(message):
    '''This sub-procedure is used to read a Characteristic Value from a server when the client only knows the characteristic UUID.'''
    req   = [
        uint8("handle", doc='connection handle'),
        uint8("mode", doc='0=OFF, 1=RX, 2=TX, 3=RX and TX'),
        uint16("bandwidth", doc='transmitted bandwidth in kbits/s'),
        uint16("time", doc='time for test in seconds (0=unlimited)'),
    ]
    rsp   = [
        status(),
    ]
    ind   = [
        uint16("bandwidth", doc='measured bandwidth in kbits/s (65535 means completed)'),
        int8("rssi", doc='rssi in dBm (127 means no value)'),
    ]
class smp_bond_list(message):
    '''list bond at index in persistent memory'''
    req   = [
        uint32("index", doc='index in list (0..4)'),
    ]
    rsp   = [
        status(),
        string("name", array=64, doc='friendly name or address'),
    ]
class smp_bond_remove_one(message):
    '''remove bond at index in persistent memory'''
    req   = [
        uint32("index", doc='index in list (0..4)'),
    ]
    rsp   = [
        status(),
    ]
class smp_bond_remove_all(message):
    '''remove all bonds from persistent memory'''
    req   = [
    ]
    rsp   = [
        status(),
    ]
