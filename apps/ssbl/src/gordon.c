
#if DEVEL_PACKAGE==0
/* SDK Release package.
 * files are at gordon/src/ directory
 */
#include "apps/gordon/src/gordon.c"
#include "apps/gordon/src/part.c"
#include "apps/gordon/src/sector_cache.c"

#else /* SDK_RELEASE_PACKAGE */
/* Development package. embedded_apps + sdk setup.
 * files are at gordon/ directory
 */
#include "apps/gordon/gordon.c"
#include "apps/gordon/part.c"
#include "apps/gordon/sector_cache.c"

#endif /* SDK_RELEASE_PACKAGE */

int
enroll(struct secure_boot *sb, uint8_t mode, const uint8_t *secret,
       const uint8_t *public_key, const uint8_t *cipher_key)
{
    return 1;
}

bool gordon_hio_in_use(void)
{
    return spi_initialized;
}
