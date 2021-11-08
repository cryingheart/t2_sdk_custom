"""
This module contains PUBLIC enums.

Used for code generation and lookup.
"""


from enum import IntEnum


class IEEE80211InformationElement(IntEnum):
    """
    >>> IEEE80211InformationElement(0).name
    'SSID'
    >>> IEEE80211InformationElement.SSID.value
    0
    """
    # fmt: off
    SSID                            = 0
    RATES                           = 1
    DS_PARAMS                       = 3
    CF_PARAMS                       = 4
    TIM                             = 5
    IBSS_PARAMS                     = 6
    COUNTRY                         = 7
    REQUEST                         = 10
    BSS_LOAD                        = 11
    EDCA_PARAM_SET                  = 12
    TSPEC                           = 13
    TCLAS                           = 14
    SCHEDULE                        = 15
    CHALLENGE                       = 16
    PWR_CONSTRAINT                  = 32
    PWR_CAPABILITY                  = 33
    TPC_REQUEST                     = 34
    TPC_REPORT                      = 35
    SUPPORTED_CHANNELS              = 36
    CHANNEL_SWITCH                  = 37
    MEASURE_REQUEST                 = 38
    MEASURE_REPORT                  = 39
    QUIET                           = 40
    IBSS_DFS                        = 41
    ERP_INFO                        = 42
    TS_DELAY                        = 43
    TCLAS_PROCESSING                = 44
    HT_CAP                          = 45
    QOS                             = 46
    RSN                             = 48
    EXT_RATES                       = 50
    AP_CHANNEL_REPORT               = 51
    NEIGHBOR_REPORT                 = 52
    RCPI                            = 53
    MOBILITY_DOMAIN                 = 54
    FAST_BSS_TRANSITION             = 55
    TIMEOUT_INTERVAL                = 56
    RIC_DATA                        = 57
    DSE_REGISTERED_LOCATION         = 58
    SUPPORTED_OPERATING_CLASSES     = 59
    EXT_CHANSWITCH_ANN              = 60
    HT_OPERATION                    = 61
    SECONDARY_CHANNEL_OFFSET        = 62
    BSS_AVERAGE_ACCESS_DELAY        = 63
    ANTENNA                         = 64
    RSNI                            = 65
    MEASUREMENT_PILOT_TRANSMISSION  = 66
    BSS_AVAILABLE_ADM_CAPA          = 67
    BSS_AC_ACCESS_DELAY             = 68
    TIME_ADVERTISEMENT              = 69
    RRM_ENABLED_CAPABILITIES        = 70
    MULTIPLE_BSSID                  = 71
    BSS_COEXISTENCE_20_40           = 72
    BSS_INTOLERANT_20_40            = 73
    OVERLAPPING_BSS_SCAN_PARAMS     = 74
    RIC_DESCRIPTOR                  = 75
    MME                             = 76
    EVENT_REQUEST                   = 78
    EVENT_REPORT                    = 79
    DIAGNOSTIC_REQUEST              = 80
    DIAGNOSTIC_REPORT               = 81
    LOCATION_PARAMETERS             = 82
    NONTRANSMITTED_BSSID_CAPA       = 83
    SSID_LIST                       = 84
    MLTIPLE_BSSID_INDEX             = 85
    FMS_DESCRIPTOR                  = 86
    FMS_REQUEST                     = 87
    FMS_RESPONSE                    = 88
    QOS_TRAFFIC_CAPABILITY          = 89
    BSS_MAX_IDLE_PERIOD             = 90
    TFS_REQ                         = 91
    TFS_RESP                        = 92
    WNMSLEEP                        = 93
    TIM_BROADCAST_REQUEST           = 94
    TIM_BROADCAST_RESPONSE          = 95
    COLLOCATED_INTERFERENCE_REPORT  = 96
    CHANNEL_USAGE                   = 97
    TIME_ZONE                       = 98
    DMS_REQUEST                     = 99
    DMS_RESPONSE                    = 100
    LINK_ID                         = 101
    WAKEUP_SCHEDULE                 = 102
    CHANNEL_SWITCH_TIMING           = 104
    PTI_CONTROL                     = 105
    TPU_BUFFER_STATUS               = 106
    INTERWORKING                    = 107
    ADV_PROTO                       = 108
    EXPEDITED_BANDWIDTH_REQ         = 109
    QOS_MAP_SET                     = 110
    ROAMING_CONSORTIUM              = 111
    EMERGENCY_ALERT_ID              = 112
    MESH_CONFIG                     = 113
    MESH_ID                         = 114
    MESH_LINK_METRIC_REPORT         = 115
    CONGESTION_NOTIFICATION         = 116
    PEER_MGMT                       = 117
    MESH_CHANNEL_SWITCH_PARAMETERS  = 118
    MESH_AWAKE_WINDOW               = 119
    BEACON_TIMING                   = 120
    MCCAOP_SETUP_REQUEST            = 121
    MCCAOP_SETUP_REPLY              = 122
    MCCAOP_ADVERTISEMENT            = 123
    MCCAOP_TEARDOWN                 = 124
    GANN                            = 125
    RANN                            = 126
    EXT_CAPAB                       = 127
    PREQ                            = 130
    PREP                            = 131
    PERR                            = 132
    PXU                             = 137
    PXUC                            = 138
    AMPE                            = 139
    MIC                             = 140
    DESTINATION_URI                 = 141
    U_APSD_COEX                     = 142
    DMG_WAKEUP_SCHEDULE             = 143
    EXTENDED_SCHEDULE               = 144
    STA_AVAILABILITY                = 145
    DMG_TSPEC                       = 146
    NEXT_DMG_ATI                    = 147
    DMG_CAPABILITIES                = 148
    DMG_OPERATION                   = 151
    DMG_BSS_PARAMETER_CHANGE        = 152
    DMG_BEAM_REFINEMENT             = 153
    CHANNEL_MEASUREMENT_FEEDBACK    = 154
    CCKM                            = 156
    AWAKE_WINDOW                    = 157
    MULTI_BAND                      = 158
    ADDBA_EXTENSION                 = 159
    NEXTPCP_LIST                    = 160
    PCP_HANDOVER                    = 161
    DMG_LINK_MARGIN                 = 162
    SWITCHING_STREAM                = 163
    SESSION_TRANSITION              = 164
    DYNAMIC_TONE_PAIRING_REPORT     = 165
    CLUSTER_REPORT                  = 166
    REPLAY_CAPABILITIES             = 167
    RELAY_TRANSFER_PARAM_SET        = 168
    BEAMLINK_MAINTENANCE            = 169
    MULTIPLE_MAC_SUBLAYERS          = 170
    U_PID                           = 171
    DMG_LINK_ADAPTATION_ACK         = 172
    MCCAOP_ADVERTISEMENT_OVERVIEW   = 174
    QUIET_PERIOD_REQUEST            = 175
    QUIET_PERIOD_RESPONSE           = 177
    QMF_POLICY                      = 181
    ECAPC_POLICY                    = 182
    CLUSTER_TIME_OFFSET             = 183
    INTRA_ACCESS_CATEGORY_PRIORITY  = 184
    SCS_DESCRIPTOR                  = 185
    QLOAD_REPORT                    = 186
    HCCA_TXOP_UPDATE_COUNT          = 187
    HIGHER_LAYER_STREAM_ID          = 188
    GCR_GROUP_ADDRESS               = 189
    ANTENNA_SECTOR_ID_PATTERN       = 190
    VHT_CAP                         = 191
    VHT_OPERATION                   = 192
    VHT_EXTENDED_BSS_LOAD           = 193
    VHT_WIDE_BW_CHSWITCH            = 194
    VHT_TRANSMIT_POWER_ENVELOPE     = 195
    VHT_CHANNEL_SWITCH_WRAPPER      = 196
    VHT_AID                         = 197
    VHT_QUIET_CHANNEL               = 198
    VHT_OPERATING_MODE_NOTIFICATION = 199
    UPSIM                           = 200
    REDUCED_NEIGHBOR_REPORT         = 201
    TVHT_OPERATION                  = 202
    DEVICE_LOCATION                 = 204
    WHITE_SPACE_MAP                 = 205
    FTM_PARAMETERS                  = 206
    VENDOR_SPECIFIC                 = 221
    CAG_NUMBER                      = 237
    AP_CSN                          = 239
    FILS_INDICATION                 = 240
    DILS                            = 241
    FRAGMENT                        = 242
    EXTENSION                       = 255


class IEEE80211ReasonCode(IntEnum):
    """
    >>> IEEE80211ReasonCode(1).name
    'UNSPECIFIED'
    >>> IEEE80211ReasonCode.UNSPECIFIED.value
    1
    """

    # fmt: off
    UNSPECIFIED                              = 1
    PREV_AUTH_NOT_VALID                      = 2
    DEAUTH_LEAVING                           = 3
    DISASSOC_DUE_TO_INACTIVITY               = 4
    DISASSOC_AP_BUSY                         = 5
    CLASS2_FRAME_FROM_NONAUTH_STA            = 6
    CLASS3_FRAME_FROM_NONASSOC_STA           = 7
    DISASSOC_STA_HAS_LEFT                    = 8
    STA_REQ_ASSOC_WITHOUT_AUTH               = 9
    PWR_CAPABILITY_NOT_VALID                 = 10
    SUPPORTED_CHANNEL_NOT_VALID              = 11
    BSS_TRANSITION_DISASSOC                  = 12
    INVALID_IE                               = 13
    MICHAEL_MIC_FAILURE                      = 14
    HANDSHAKE_4WAY_TIMEOUT                   = 15
    GROUP_KEY_UPDATE_TIMEOUT                 = 16
    IE_IN_4WAY_DIFFERS                       = 17
    GROUP_CIPHER_NOT_VALID                   = 18
    PAIRWISE_CIPHER_NOT_VALID                = 19
    AKMP_NOT_VALID                           = 20
    UNSUPPORTED_RSN_IE_VERSION               = 21
    INVALID_RSN_IE_CAPAB                     = 22
    IEEE_802_1X_AUTH_FAILED                  = 23
    CIPHER_SUITE_REJECTED                    = 24
    TDLS_TEARDOWN_UNREACHABLE                = 25
    TDLS_TEARDOWN_UNSPECIFIED                = 26
    SSP_REQUESTED_DISASSOC                   = 27
    NO_SSP_ROAMING_AGREEMENT                 = 28
    BAD_CIPHER_OR_AKM                        = 29
    NOT_AUTHORIZED_THIS_LOCATION             = 30
    SERVICE_CHANGE_PRECLUDES_TS              = 31
    UNSPECIFIED_QOS_REASON                   = 32
    NOT_ENOUGH_BANDWIDTH                     = 33
    DISASSOC_LOW_ACK                         = 34
    EXCEEDED_TXOP                            = 35
    STA_LEAVING                              = 36
    END_TS_BA_DLS                            = 37
    UNKNOWN_TS_BA                            = 38
    TIMEOUT                                  = 39
    PEERKEY_MISMATCH                         = 45
    AUTHORIZED_ACCESS_LIMIT_REACHED          = 46
    EXTERNAL_SERVICE_REQUIREMENTS            = 47
    INVALID_FT_ACTION_FRAME_COUNT            = 48
    INVALID_PMKID                            = 49
    INVALID_MDE                              = 50
    INVALID_FTE                              = 51
    MESH_PEERING_CANCELLED                   = 52
    MESH_MAX_PEERS                           = 53
    MESH_CONFIG_POLICY_VIOLATION             = 54
    MESH_CLOSE_RCVD                          = 55
    MESH_MAX_RETRIES                         = 56
    MESH_CONFIRM_TIMEOUT                     = 57
    MESH_INVALID_GTK                         = 58
    MESH_INCONSISTENT_PARAMS                 = 59
    MESH_INVALID_SECURITY_CAP                = 60
    MESH_PATH_ERROR_NO_PROXY_INFO            = 61
    MESH_PATH_ERROR_NO_FORWARDING_INFO       = 62
    MESH_PATH_ERROR_DEST_UNREACHABLE         = 63
    MAC_ADDRESS_ALREADY_EXISTS_IN_MBSS       = 64
    MESH_CHANNEL_SWITCH_REGULATORY_REQ       = 65
    MESH_CHANNEL_SWITCH_UNSPECIFIED          = 66
    # fmt: on


class IEEE80211StatusCode(IntEnum):
    """
    >>> IEEE80211StatusCode(0).name
    'SUCCESS'
    """

    # fmt: off
    SUCCESS                                  = 0
    UNSPECIFIED_FAILURE                      = 1
    TDLS_WAKEUP_ALTERNATE                    = 2
    TDLS_WAKEUP_REJECT                       = 3
    SECURITY_DISABLED                        = 5
    UNACCEPTABLE_LIFETIME                    = 6
    NOT_IN_SAME_BSS                          = 7
    CAPS_UNSUPPORTED                         = 10
    REASSOC_NO_ASSOC                         = 11
    ASSOC_DENIED_UNSPEC                      = 12
    NOT_SUPPORTED_AUTH_ALG                   = 13
    UNKNOWN_AUTH_TRANSACTION                 = 14
    CHALLENGE_FAIL                           = 15
    AUTH_TIMEOUT                             = 16
    AP_UNABLE_TO_HANDLE_NEW_STA              = 17
    ASSOC_DENIED_RATES                       = 18
    ASSOC_DENIED_NOSHORT                     = 19
    SPEC_MGMT_REQUIRED                       = 22
    PWR_CAPABILITY_NOT_VALID                 = 23
    SUPPORTED_CHANNEL_NOT_VALID              = 24
    ASSOC_DENIED_NO_SHORT_SLOT_TIME          = 25
    ASSOC_DENIED_NO_HT                       = 27
    R0KH_UNREACHABLE                         = 28
    ASSOC_DENIED_NO_PCO                      = 29
    ASSOC_REJECTED_TEMPORARILY               = 30
    ROBUST_MGMT_FRAME_POLICY_VIOLATION       = 31
    UNSPECIFIED_QOS_FAILURE                  = 32
    DENIED_INSUFFICIENT_BANDWIDTH            = 33
    DENIED_POOR_CHANNEL_CONDITIONS           = 34
    DENIED_QOS_NOT_SUPPORTED                 = 35
    REQUEST_DECLINED                         = 37
    INVALID_PARAMETERS                       = 38
    REJECTED_WITH_SUGGESTED_CHANGES          = 39
    INVALID_IE                               = 40
    GROUP_CIPHER_NOT_VALID                   = 41
    PAIRWISE_CIPHER_NOT_VALID                = 42
    AKMP_NOT_VALID                           = 43
    UNSUPPORTED_RSN_IE_VERSION               = 44
    INVALID_RSN_IE_CAPAB                     = 45
    CIPHER_REJECTED_PER_POLICY               = 46
    TS_NOT_CREATED                           = 47
    DIRECT_LINK_NOT_ALLOWED                  = 48
    DEST_STA_NOT_PRESENT                     = 49
    DEST_STA_NOT_QOS_STA                     = 50
    ASSOC_DENIED_LISTEN_INT_TOO_LARGE        = 51
    INVALID_FT_ACTION_FRAME_COUNT            = 52
    INVALID_PMKID                            = 53
    INVALID_MDIE                             = 54
    INVALID_FTIE                             = 55
    REQUESTED_TCLAS_NOT_SUPPORTED            = 56
    INSUFFICIENT_TCLAS_PROCESSING_RESOURCES  = 57
    TRY_ANOTHER_BSS                          = 58
    GAS_ADV_PROTO_NOT_SUPPORTED              = 59
    NO_OUTSTANDING_GAS_REQ                   = 60
    GAS_RESP_NOT_RECEIVED                    = 61
    STA_TIMED_OUT_WAITING_FOR_GAS_RESP       = 62
    GAS_RESP_LARGER_THAN_LIMIT               = 63
    REQ_REFUSED_HOME                         = 64
    ADV_SRV_UNREACHABLE                      = 65
    REQ_REFUSED_SSPN                         = 67
    REQ_REFUSED_UNAUTH_ACCESS                = 68
    INVALID_RSNIE                            = 72
    U_APSD_COEX_NOT_SUPPORTED                = 73
    U_APSD_COEX_MODE_NOT_SUPPORTED           = 74
    BAD_INTERVAL_WITH_U_APSD_COEX            = 75
    ANTI_CLOGGING_TOKEN_REQ                  = 76
    FINITE_CYCLIC_GROUP_NOT_SUPPORTED        = 77
    CANNOT_FIND_ALT_TBTT                     = 78
    TRANSMISSION_FAILURE                     = 79
    REQ_TCLAS_NOT_SUPPORTED                  = 80
    TCLAS_RESOURCES_EXCHAUSTED               = 81
    REJECTED_WITH_SUGGESTED_BSS_TRANSITION   = 82
    REJECT_WITH_SCHEDULE                     = 83
    REJECT_NO_WAKEUP_SPECIFIED               = 84
    SUCCESS_POWER_SAVE_MODE                  = 85
    PENDING_ADMITTING_FST_SESSION            = 86
    PERFORMING_FST_NOW                       = 87
    PENDING_GAP_IN_BA_WINDOW                 = 88
    REJECT_U_PID_SETTING                     = 89
    REFUSED_EXTERNAL_REASON                  = 92
    REFUSED_AP_OUT_OF_MEMORY                 = 93
    REJECTED_EMERGENCY_SERVICE_NOT_SUPPORTED = 94
    QUERY_RESP_OUTSTANDING                   = 95
    REJECT_DSE_BAND                          = 96
    TCLAS_PROCESSING_TERMINATED              = 97
    TS_SCHEDULE_CONFLICT                     = 98
    DENIED_WITH_SUGGESTED_BAND_AND_CHANNEL   = 99
    MCCAOP_RESERVATION_CONFLICT              = 100
    MAF_LIMIT_EXCEEDED                       = 101
    MCCA_TRACK_LIMIT_EXCEEDED                = 102
    DENIED_DUE_TO_SPECTRUM_MANAGEMENT        = 103
    ASSOC_DENIED_NO_VHT                      = 104
    ENABLEMENT_DENIED                        = 105
    RESTRICTION_FROM_AUTHORIZED_GDB          = 106
    AUTHORIZATION_DEENABLED                  = 107
    FILS_AUTHENTICATION_FAILURE              = 112
    UNKNOWN_AUTHENTICATION_SERVER            = 113
    UNKNOWN_PASSWORD_IDENTIFIER              = 123
    # fmt: on
