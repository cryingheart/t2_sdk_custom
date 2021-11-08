"""WCM Interface.

This is an interface to the WIFI Connection Manager. This interface containts operations for:

<ul>
  <li> creating and destroying interfaces </li>
  <li> scanning </li>
  <li> connecting and disconnecting </li>
  <li> querying connection information </li>
</ul>
"""


from api.base import message, status
from api.base import uint8, uint16, uint32, uint64
from api.base import int8, int16, int32
from api.base import string
from api.base import macaddr
from api.base import ipaddr4, ipaddr6


GROUP_NAME = 'wcm'
GROUP_ID = 2


class if_create(message):
    '''Create a WiFi network interface'''
    req   = [
        macaddr("addr", doc='''MAC address of the WIFI interface. If an invalid MAC address is specified (e.g. all zero), the device will generate a random address.'''),
    ]
    rsp   = [
        uint32("handle", doc='interface handle of created interface'),
        macaddr("hwaddr", doc='The MAC address used by the interface'),
    ]

class if_destroy(message):
    '''Shutdown and cleanup a WiFi Connection Manager interface.'''
    req   = [
        uint32("handle", doc='interface handle'),
    ]
    rsp   = [
        status(),
    ]

class if_notify(message):
    '''Connection manager notification.'''
    ind   = [
        uint32("type", doc='''Notification type: Link up (200),  Link down (201),  Address (202)'''),
        string("attr", array=0, doc='''Notification data''')
    ]

class if_scan(message):
    '''Scan for WiFi networks.'''
    req   = [
        uint32("handle", doc='interface handle'),
        string("ssid", array=33, doc='Zero terminated SSID to scan for, empty for broadcast scan.'),
        uint8("max", doc='Max number of responses to return.'),
        uint16("num_probes", doc='Max number of probes to send in active scan.'),
        uint16("idleslots", doc='Max number of idle slots to decide if we should keep listening'),
        uint16("min_listen_time",
              doc='The minimum amount of time (in milliseconds) to stay on the channel after transmitting the probe request and listening for responses.'),
        uint16("max_listen_time",
              doc='The maximum amount of time (in milliseconds) to stay on the channel after transmitting the probe request and listening for responses.'),
        uint16("probe_tx_timeout",
               doc='The time (in milliseconds) a (2nd) probe request is aborted if transmission was not possible.'),
        uint16("wait_time",
               doc='The time to wait after finished probing one channel and proceeding to the next.'),
        uint16("rate",
               doc='The rate as defined by @ref rate_t used to transmit the probe request. If this field is set to the value 0xffff, no probes will be sent and the scan is only passive (ony listening)'),
    ]
    rsp   = [
        status(),
        uint8("num", doc='Number of scan responses received (may be larger than max in request).'),
    ]
    ind   = [
        macaddr("bssid", doc='BSSID'),
        uint16("caps", doc='Capability information'),
        uint64("timestamp", doc='TSF timestamp from Beacon/Probe-Rsp'),
        uint16("interval", doc='Beacon interval'),
        uint16('fc', doc='Frame control field'),
        int16("rssi", doc='Estimated RSSI for the station'),
        int16("rate", doc='Transmission rate for the received frame'),
        uint8("ielist", array=0, doc='List of information elements'),
    ]

class if_connect(message):
    '''Connect to WiFI network'''
    req   = [
        uint32("handle", doc='interface handle'),
        string("ssid", array=33, doc='SSID to connect to'),
        string("passphrase", array=65, doc='passphrase for RSN, key for WEP or empty string for unencrypted connection'),
    ]
    rsp   = [
        status(),
    ]

class if_disconnect(message):
    '''Disconnect from WiFI network'''
    req   = [
        uint32("handle", doc='interface handle'),
    ]
    rsp   = [
        status(),
    ]

class if_set_addr4(message):
    '''Set interface IPv4 ,netmask, gateway and dns addresses'''
    req   = [
        uint32("handle",     doc='interface handle'),
        ipaddr4("ipaddr4",    doc='address, as big-endian integer'),
        ipaddr4("netmask",    doc='netmask, as big-endian integer'),
        ipaddr4("gw",         doc='default-route, as big-endian integer'),
        ipaddr4("dns_server", doc='DNS server, as big-endian integer'),
    ]
    rsp   = [
        status(),
    ]

class if_set_addr6(message):
    '''Set interface IPv6 address number '''
    req   = [
        uint32("handle", doc='interface handle'),
        uint32('addr6_idx', doc='address index and MUST be >= 0. NOTE that index 0 is link-local IPv6 address'),
        ipaddr6("ipaddr6",  doc='interface IPv6 address'),
    ]
    rsp   = [
        status(),
    ]

class if_get_addr4(message):
    '''Get current IPv4/IPv6 addresses'''
    req   = [
        uint32("handle", doc='interface handle'),
    ]
    rsp   = [
        status(),
        ipaddr4("ipaddr4", doc='address, as big-endian integer'),
        ipaddr4("netmask", doc='netmask, as big-endian integer'),
        ipaddr4("gw",      doc='default-route, as big-endian integer'),
        ipaddr4("dns",     doc='DNS server, as big-endian integer'),
    ]

class if_get_addr6(message):
    '''Get current IPv4/IPv6 addresses'''
    req   = [
        uint32("handle", doc='interface handle'),
    ]
    rsp   = [
        status(),
        uint32('numaddr6', doc='number of returned addr6'),
        ipaddr6("ipaddr6", numaddr=3, doc='interface list of IPv6 addresses'),
    ]

class if_set_pmconfig(message):
    '''Configure WiFi power-save parameters'''
    req   = [
        uint32("handle", doc='interface handle'),
        uint32("listen_interval", doc='Listen interval in units of beacon intervals'),
        uint32("traffic_tmo", doc='Traffic timeout (in ms)'),
        uint8("ps_poll", doc='Set 1 to use ps poll when beacon was missed'),
        uint8("dyn_listen_int", doc='Set 1 to listen to all beacons if there was traffic recently'),
    ]
    rsp   = [
        status(),
    ]

class if_get_rssi(message):
    '''Return the current RSSI'''
    req   = [
        uint32("handle", doc='interface handle'),
    ]

    rsp   = [
        int32("rssi", doc="Current average RSSI (zero if not assocaited)"),
    ]

class if_get_counters(message):
    '''Return link stats counters'''
    req   = [
        uint32("handle", doc='interface handle'),
    ]
    rsp   = [
        uint64("txbytes", doc="number of transmitted bytes"),
        uint32("txframe", doc="number of transmitted frames"),
        uint32("txmframe", doc="number of transmitted multicast frames"),
        uint32("txnoack", doc="number of frames without ACK"),
        uint32("txfail", doc="number of transmitted frame failures"),
        uint32("txretry", doc="number of with at least one retry"),
        uint32("txretries", doc="number of with multiple retries"),
        uint64("rxbytes", doc="number of received bytes"),
        uint32("rxframe", doc="number of received frames"),
        uint32("rxmframe", doc="number of received multicast frames"),
        uint32("rxdup", doc="number of dupicate frames"),
        uint32("rxfcserr", doc="number of frames with FCS error"),
    ]

class if_txpower(message):
    '''Set TX power'''
    req   = [
        uint32("handle", doc='interface handle'),
        int8("tx_power", doc='''TX power in dBm (-10..20(max))'''),
        uint8("reserved1", doc='''Reserved. Must be zero'''),
        uint16("reserved2", doc='''Reserved. Must be zero'''),
    ]
    rsp   = [
        status(),
    ]

class if_get_bssid(message):
    '''Get current BSSID'''
    req   = [
        uint32("handle", doc='interface handle'),
    ]
    rsp   = [
        status(),
        macaddr("bssid", doc='''BSSID of the network if associated, zero otherwize'''),
    ]

class if_add_network(message):
    '''Add network to connect to (asynchronously)'''
    req   = [
        uint32("handle", doc='interface handle'),
        string("ssid", array=33, doc='SSID of the network or empty string if bssid is set.'),
        macaddr("bssid", doc='''BSSID of the network, set to all zeroes if ssid is set'''),
        string("passphrase", array=65, doc='passphrase for RSN, key for WEP or empty string for unencrypted connection'),
    ]
    rsp   = [
        status(),
    ]

class if_remove_network(message):
    '''Remove network.'''
    req   = [
        uint32("handle", doc='interface handle'),
        string("ssid", array=33, doc='SSID of the network or empty string if bssid is set.'),
        macaddr("bssid", doc='''BSSID of the network, set to all zeroes if ssid is set'''),
    ]
    rsp   = [
        status(),
    ]

class if_auto_connect(message):
    '''Enable/Disable async connect configured by if_add_network'''
    req   = [
        uint32("handle", doc='interface handle'),
        uint32("enabled", doc='allow wcm to connect. 1=enabled, 0=disabled'),
    ]
    rsp   = [
        status(),
    ]

class if_add_network_uri(message):
    '''Add network to connect to (asynchronously)'''
    req   = [
        uint32("handle", doc='interface handle'),
        string("uri", array=0,
               doc="""A "WIFI" style URI specifying the network to connect to."""),
    ]
    rsp   = [
        status(),
    ]

class set_mfp_mode(message):
    '''Set the desired mode for management frame protection.'''
    req = [
        uint32("handle", doc="interface handle"),
        uint32("mfp_required", doc="make MFP required if non-zero")
    ]
    rsp   = [
        status()
    ]

class testmode(message):
    '''Configure test mode.'''
    req = [
        uint32("handle", doc="interface handle"),
        uint32("mode", doc="undocumented mode selector."),
        uint32("control", doc="undocumented per-mode control parameter."),
    ]
    rsp   = [
        status()
    ]

class set_regulatory_domain(message):
    '''Set regulatory domain, with channels and power limits'''
    req   = [
        uint32("handle", doc='interface handle'),
        string("domain", array=64, doc='FCC, ETSI, TELEC, KCC, SRCC (or e.g. 1..11@20)'),
    ]
    rsp   = [
        status(),
    ]
