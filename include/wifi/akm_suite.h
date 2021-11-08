#pragma once

enum akm_suite {
    AKM_SUITE_1X           = 0x000fac01, /* 802.1X (PMKSA caching) */
    AKM_SUITE_PSK          = 0x000fac02, /* PSK */
    AKM_SUITE_FT           = 0x000fac03, /* FT over 802.1X */
    AKM_SUITE_FT_PSK       = 0x000fac04, /* FT using PSK */
    AKM_SUITE_1X_SHA256    = 0x000fac05, /* 802.1X (PMKSA caching) */
    AKM_SUITE_PSK_SHA256   = 0x000fac06, /* PSK */
    AKM_SUITE_TDLS         = 0x000fac07, /* TDLS */
    AKM_SUITE_SAE          = 0x000fac08, /* SAE (PMKSA caching) */
    AKM_SUITE_FT_SAE       = 0x000fac09, /* FT over SAE */
    AKM_SUITE_AP_PEER_KEY  = 0x000fac0a, /* APPeerKey (PMKSA caching) */
    AKM_SUITE_1X_SB_SHA256 = 0x000fac0b, /* 802.1X w/ SuiteB (PMKSA caching) */
    AKM_SUITE_1X_SB_SHA384 = 0x000fac0c, /* 802.1X w/ SuiteB (PMKSA caching) */
    AKM_SUITE_FT_SHA384    = 0x000fac0d, /* FT over 802.1X */
    AKM_SUITE_OWE          = 0x000fac12, /* Opportunistic Wireless Encryption */
};
