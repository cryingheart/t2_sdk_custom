#pragma once

enum cipher_suite {
    CIPHER_SUITE_USE_GROUP    = 0x000fac00, /* Use group cipher suite */
    CIPHER_SUITE_WEP40        = 0x000fac01, /* WEP-40 */
    CIPHER_SUITE_TKIP         = 0x000fac02, /* TKIP */
    CIPHER_SUITE_CCMP         = 0x000fac04, /* CCMP-128 */
    CIPHER_SUITE_WEP104       = 0x000fac05, /* WEP-104 */
    CIPHER_SUITE_BIP_CMAC_128 = 0x000fac06, /* BIP-CMAC-128 */
    CIPHER_SUITE_NO_GROUP     = 0x000fac07, /* Group traffic not allowed */
    CIPHER_SUITE_GCMP_128     = 0x000fac08, /* GCMP-128 */
    CIPHER_SUITE_GCMP_256     = 0x000fac09, /* GCMP-256 */
    CIPHER_SUITE_CCMP_256     = 0x000fac0a, /* CCMP-256 */
    CIPHER_SUITE_BIP_GMAC_128 = 0x000fac0b, /* BIP-GMAC-128 */
    CIPHER_SUITE_BIP_GMAC_256 = 0x000fac0c, /* BIP-GMAC-256 */
    CIPHER_SUITE_BIP_CMAC_256 = 0x000fac0d, /* BIP-CMAC-256 */
};
