

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <lwip/netdb.h>
#include <errno.h>
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "ssl_wrap/inc/ssl_wrap.h"

typedef struct ssl_wrap_handle{
    mbedtls_net_context net;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt ca_cert;
    mbedtls_x509_crt client_cert;/**< Client / own cert*/
    mbedtls_pk_context client_key;/**< Client /own key*/
    struct ssl_wrap_handle *next;
} ssl_wrap_handle_c;

typedef struct ssl_wrap_crt_bundle {
    char *name;
    int name_len;
    int key_len;
    unsigned char *key;
} ssl_wrap_crt_bundle_t;

#define SSL_WRAP_DEFAULT_PDATA          "InnoT2"

#define BUNDLE_HEADER_OFFSET            2
#define CRT_HEADER_OFFSET               4
#define SSL_WRAP_MAX_CERTS_IN_BUNDLE    512

static ssl_wrap_handle_c *list;
static mbedtls_x509_crt ssl_wrap_dummy_crt;
static const char *ssl_wrap_ca_bundle = NULL;

static void 
ssl_wrap_set_sock_nonblocking(int fd) 
{
  int flags, rv;
  while ((flags = fcntl(fd, F_GETFL, 0)) == -1 && errno == EINTR);
  if (flags == -1) {
    os_printf("\nfcntl get failed...");
    while(1);
  }
  while ((rv = fcntl(fd, F_SETFL, flags | O_NONBLOCK)) == -1 && 
            errno == EINTR);
  if (rv == -1) {
    os_printf("\nfcntl set failed...\r\n");
    while(1);
  }
}

static void 
ssl_wrap_set_sock_blocking(int fd) 
{
  int flags, rv;
  
  while ((flags = fcntl(fd, F_GETFL, 0)) == -1 && errno == EINTR);
  if (flags == -1) {
    os_printf("\nfcntl get failed...");
    while(1);
  }
  while((rv = fcntl(fd, F_SETFL, flags & ~O_NONBLOCK)) == -1 && 
          errno == EINTR);  
  if (rv == -1) {
    os_printf("fcntl set failed...\r\n");
    while(1);
  }
}

static void
ssl_wrap_list_handle_add(ssl_wrap_handle_c *h)
{
    //TODO take mutex here
    /*insert at the head*/
    h->next = list;
    list = h;
    //TODO release mutex here
    
}

static void
ssl_wrap_list_handle_remove(ssl_wrap_handle_c *h)
{
    ssl_wrap_handle_c *p, *prev;
    
    //os_sem_wait(&ws_lock);
    p = prev = list ;
    if(p == h){
        list = h->next;
        goto exit;
    }
    while(p){
        if(p->next == h){
            p->next = h->next;
            break;
        }
        p = p->next;
    }
exit:    
    //os_sem_post(&ws_lock);
    return;
}
void
ssl_wrap_resource_free(ssl_wrap_handle_c *ssl_h)
{
    mbedtls_net_free(&ssl_h->net);
    mbedtls_ssl_free(&ssl_h->ssl);
    mbedtls_x509_crt_free(&ssl_h->ca_cert);
    mbedtls_x509_crt_free(&ssl_h->client_cert);    
    mbedtls_pk_free(&ssl_h->client_key);
    mbedtls_ssl_config_free(&ssl_h->conf);
    mbedtls_ctr_drbg_free(&ssl_h->ctr_drbg);
    mbedtls_entropy_free(&ssl_h->entropy);
    os_free(ssl_h);
}
/**
 * De-initialise the ssl context
 * @param ssl_h SSL context
 * @return allocate ssl context
 */
void
ssl_wrap_disconnect(ssl_wrap_handle_t handle)
{
    ssl_wrap_handle_c *ssl_h = (ssl_wrap_handle_c *)handle;
    if(NULL == ssl_h)
        return;
    ssl_wrap_list_handle_remove(ssl_h); 
    /*de-initialise the data structures and free memmory*/
    mbedtls_ssl_close_notify(&ssl_h->ssl);
    ssl_wrap_resource_free(ssl_h);
}

int
ssl_wrap_sock_fd_get(ssl_wrap_handle_t handle)
{
    ssl_wrap_handle_c *ssl_h = (ssl_wrap_handle_c *)handle;
    return ssl_h->net.fd;
}

/*Returns ssl handle for the supplied socket fd*/
ssl_wrap_handle_t
ssl_wrap_fd_to_handle(int sock_fd)
{
    ssl_wrap_handle_c *p;
  
    //os_sem_wait(&ws_lock);
    p = list ;
    while(p){        
        if(sock_fd == p->net.fd)
            break;
        p = p->next;
    }
    //os_sem_post(&ws_lock);
    return p;
}

static int ssl_wrap_verify_certificate(mbedtls_x509_crt *srvr_cert, 
                                       const uint8_t *pub_key_buf, 
                                       size_t pub_key_len)
{
    int ret = 0;
    mbedtls_x509_crt root;
    const mbedtls_md_info_t *md_info;
    unsigned char hash[MBEDTLS_MD_MAX_SIZE];

    mbedtls_x509_crt_init(&root);

    if ( (ret = mbedtls_pk_parse_public_key(&root.pk, pub_key_buf, 
                                             pub_key_len) ) != 0) {
        os_printf( "\nPK parse failed with error %X", ret);
        goto error_exit;
    }

    if (!mbedtls_pk_can_do(&root.pk, srvr_cert->sig_pk)) {
        os_printf( "\nSimple compare failed");
        ret = -1;
        goto error_exit;
    }

    md_info = mbedtls_md_info_from_type(srvr_cert->sig_md);
    if ( (ret = mbedtls_md( md_info, srvr_cert->tbs.p, srvr_cert->tbs.len, hash )) 
                            != 0 ) {
        os_printf( "\nInternal mbedTLS error %X", ret);
        goto error_exit;
    }

    if ( (ret = mbedtls_pk_verify_ext(srvr_cert->sig_pk, srvr_cert->sig_opts, 
                                       &root.pk,
                                       srvr_cert->sig_md, hash, 
                                       mbedtls_md_get_size( md_info ),
                                       srvr_cert->sig.p, srvr_cert->sig.len )) 
                                       != 0) {

        os_printf( "\nPK verify failed with error %X", ret);
        goto error_exit;
    }
error_exit:
    mbedtls_x509_crt_free(&root);

    return ret;
}

int ssl_wrap_crt_verify_callback(void *buf, mbedtls_x509_crt *srvr_crt, 
                                 int depth, 
                                 uint32_t *flags)
{
    bool crt_found = false;
    ssl_wrap_crt_bundle_t root_crt;
    char * ca_bundle;
    int rval;
    int num_certs;

    /* Weak signature hash algo is ignored as of now*/
    uint32_t flags_filtered = *flags & ~(MBEDTLS_X509_BADCERT_BAD_MD);

    if (flags_filtered != MBEDTLS_X509_BADCERT_NOT_TRUSTED) {
        return 0;
    }

    ca_bundle = (char *)ssl_wrap_ca_bundle;
    if (ca_bundle == NULL) {
        os_printf( "\nError: Certificates bundle not available");
        return MBEDTLS_ERR_X509_FATAL_ERROR;
    }

    num_certs = (ca_bundle[0] << 8) | ca_bundle[1];
    os_printf("\nNum certs = %d", num_certs);

    if(num_certs >= SSL_WRAP_MAX_CERTS_IN_BUNDLE){
        os_printf( "Error: Number of cert (%d) in bundle exceeds limit (%d)",
                    num_certs, SSL_WRAP_MAX_CERTS_IN_BUNDLE);
        return MBEDTLS_ERR_X509_FATAL_ERROR;
    }
    ca_bundle += BUNDLE_HEADER_OFFSET;
    
    for (int i = 0; i < num_certs; i++) {
        root_crt.name = ca_bundle + CRT_HEADER_OFFSET;

        root_crt.name_len = ca_bundle[0] << 8 | ca_bundle[1];
        root_crt.key_len = ca_bundle[2] << 8 | ca_bundle[3];
        root_crt.key = (unsigned char *)
                       (ca_bundle + CRT_HEADER_OFFSET + root_crt.name_len);
        ca_bundle += CRT_HEADER_OFFSET + root_crt.name_len + root_crt.key_len;
        
        rval = memcmp(srvr_crt->issuer_raw.p, root_crt.name, root_crt.name_len);
        if (rval == 0) {
            os_printf("\nFound the issuer (root) certificate");
            crt_found = true;
            break;
        }        
    }
    
    if (crt_found) {
        rval = ssl_wrap_verify_certificate(srvr_crt, root_crt.key, 
                                           root_crt.key_len);
        if (rval == 0) {
            os_printf( "\nCertificate validation success");
            *flags = 0;
            return 0;
        }
    }

    os_printf( "\nError: Failed to verify certificate");
    return MBEDTLS_ERR_X509_FATAL_ERROR;
}

ssl_wrap_handle_t 
ssl_wrap_connect(char *host_name, int port, ssl_wrap_cfg_t *cfg)
{
    int ret = 1;
    uint32_t flags;
    ssl_wrap_handle_c *ssl_h;
    char port_str[16];

    os_printf("  . Checking input configurations...\n");
    if(cfg->max_frag_len && 
        !( cfg->max_frag_len >= MBEDTLS_SSL_MAX_FRAG_LEN_512 && 
           cfg->max_frag_len <= MBEDTLS_SSL_MAX_FRAG_LEN_4096)){
            pr_err(" failed\n  !max fragment len config value error\n\n");
            return NULL;
    }
    ssl_h = os_alloc(sizeof(ssl_wrap_handle_c));
    if(NULL == ssl_h) {
        os_printf("\nError: %s os_alloc() failed", __FUNCTION__);
        return NULL;
        
    }   
    memset(ssl_h, 0, sizeof(ssl_wrap_handle_c));
    /*
     * 0. Initialize the RNG and the session data
     */
    mbedtls_net_init(&ssl_h->net);
    mbedtls_ssl_init(&ssl_h->ssl);
    mbedtls_ssl_config_init(&ssl_h->conf);
    mbedtls_x509_crt_init(&ssl_h->ca_cert);
    mbedtls_x509_crt_init(&ssl_h->client_cert);
    mbedtls_pk_init(&ssl_h->client_key);
    mbedtls_ctr_drbg_init(&ssl_h->ctr_drbg);
    
    if(NULL == cfg->p_data){
        cfg->p_data = SSL_WRAP_DEFAULT_PDATA;
    }
    
    os_printf("  . Seeding the random number generator...\n");
    mbedtls_entropy_init(&ssl_h->entropy);
    if((ret = mbedtls_ctr_drbg_seed(&ssl_h->ctr_drbg, mbedtls_entropy_func, 
                                     &ssl_h->entropy,
                                     (const unsigned char *) cfg->p_data,
                                    strlen(cfg->p_data))) != 0) {
        pr_err(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
        goto exit;
    }
    /*
     * 0. Initialize certificates
     */
    if(0 != cfg->ca_cert.len){
        os_printf("  . Loading the CA root certificate ...Cert Len = %d\n", 
                        cfg->ca_cert.len);
        ret = mbedtls_x509_crt_parse(&ssl_h->ca_cert, 
                                     (const unsigned char *) cfg->ca_cert.buf,
                                     cfg->ca_cert.len);
        if(ret < 0) {
            pr_err(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
            goto exit;
        }
    }
    
    if(0 != cfg->client_cert.len){
        if(0 ==  cfg->client_key.len){
            pr_err("Error: Client cert present but Client key not provided\n\n");
            goto exit;
        }
        /*Parse client(own) certificate*/        
        os_printf("  . Loading the Client(Own) certificate ...Cert Len = %d\n", 
                        cfg->client_cert.len);
        ret = mbedtls_x509_crt_parse(&ssl_h->client_cert, 
                                     (const unsigned char *) cfg->client_cert.buf,
                                     cfg->client_cert.len);
        if(ret < 0) {
            pr_err(" failed\n mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
            goto exit;
        }
        os_printf("  . Loading the Client(Own) Key ...Key Len = %d\n", 
                        cfg->client_key.len);
        ret = mbedtls_pk_parse_key(&(ssl_h->client_key),
                  					(const unsigned char*) cfg->client_key.buf,
                  					cfg->client_key.len /*+ 1*/, NULL, 0);
    	if (ret != 0) {
    		 pr_err(" failed\n mbedtls_pk_parse_key returned -0x%x\n\n", -ret);
            goto exit;
    	}
    }
        
    /*
     * 1. Start the connection
     */
    sprintf(port_str, "%d", port);
    os_printf("  . Connecting to tcp %s:%s...\n", host_name, port_str);
    if((ret = mbedtls_net_connect(&ssl_h->net, host_name,
                                  port_str, MBEDTLS_NET_PROTO_TCP)) != 0) {
        pr_err(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
        goto exit;
    }

    /*
     * 2. Setup stuff
     */
    os_printf("  . Setting up the SSL/TLS structure...\n");
    if((ret = mbedtls_ssl_config_defaults(&ssl_h->conf,
                                          MBEDTLS_SSL_IS_CLIENT,
                                          MBEDTLS_SSL_TRANSPORT_STREAM,
                                          MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        pr_err(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto exit;
    }

    /*set configuration */
    os_printf("        >setting configurations..\n");
    os_printf("        >auth mode = %d (0- skip, 1- optional, 2- required\n"
              "        >max fragment len = %d\n",
              cfg->auth_mode, cfg->max_frag_len);
    mbedtls_ssl_conf_authmode(&ssl_h->conf, cfg->auth_mode);
    
    if(ssl_wrap_ca_bundle){
        /* Configure a dummy certificate
         * This done so that ca_chain passes non-NULL check during handshake
         */
        mbedtls_x509_crt_init(&ssl_wrap_dummy_crt);
        mbedtls_ssl_conf_ca_chain(&ssl_h->conf, &ssl_wrap_dummy_crt, NULL);
        /* Register the call back. This call back will be called when server
           certificate need to be verified. The verification is done in the 
           call back*/
         os_printf("  . X.509 certificate bundle is used for verification...\n");
        mbedtls_ssl_conf_verify(&ssl_h->conf, ssl_wrap_crt_verify_callback, NULL);
    }else{
        /*ssl_h->ca_cert is the parsed certificate. This is parsed from the 
          cfg->ca_cert.buf using mbedtls_x509_crt_parse() at the begining*/
        mbedtls_ssl_conf_ca_chain(&ssl_h->conf, &ssl_h->ca_cert, NULL);
    }
    if(0 != cfg->client_cert.len){
        ret = mbedtls_ssl_conf_own_cert(&ssl_h->conf,
    			&(ssl_h->client_cert), &(ssl_h->client_key));
        if (ret  != 0) {
    		os_printf(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n",
    				  ret);
    		goto exit;
    	}
    }
    
    mbedtls_ssl_conf_rng(&ssl_h->conf, mbedtls_ctr_drbg_random, &ssl_h->ctr_drbg);
    if(cfg->max_frag_len){        
        mbedtls_ssl_conf_max_frag_len(&ssl_h->conf, MBEDTLS_SSL_MAX_FRAG_LEN_4096);
    }

    if((ret = mbedtls_ssl_setup(&ssl_h->ssl, &ssl_h->conf)) != 0) {
        pr_err(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        goto exit;
    }

    /*
     * In real life, we could use server_name as the hostname to check against 
     * the received server certificate.  In this example, however, the server 
     * certificate has a Common Name that probably won't be 
     * server_name: the test certificate has the Common Name "innotest"
     * and server_name is likely an ip address.
     */
    if((ret = mbedtls_ssl_set_hostname(&ssl_h->ssl, host_name)) != 0) {
        pr_err(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl_h->ssl, &ssl_h->net, mbedtls_net_send, 
                        mbedtls_net_recv, NULL);

    /*
     * 4. Handshake
     */
    os_printf("  . Performing the SSL/TLS handshake...\n");

    while((ret = mbedtls_ssl_handshake(&ssl_h->ssl)) != 0) {
        if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            pr_err(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret);
            goto exit;
        }
    }
    os_printf("\n ok\n");
    /*
     * 5. Verify the server certificate
     */
    os_printf("  . Verifying peer X.509 certificate...");
    /* In real life, we probably want to bail out when ret != 0 */
    if( /*cfg->skip_verify &&*/
        (flags = mbedtls_ssl_get_verify_result(&ssl_h->ssl)) != 0) {
        char * vrfy_buf = os_alloc(512);
        if(vrfy_buf != NULL) {
            pr_err(" failed\n");
            mbedtls_x509_crt_verify_info(vrfy_buf, 512, "  ! ", flags);
            pr_err("%s\n", vrfy_buf);
            os_free(vrfy_buf);
        }
    }

    ssl_wrap_list_handle_add(ssl_h); 
    return ssl_h;
exit:
    ssl_wrap_resource_free(ssl_h);
    return NULL;
}

int 
ssl_wrap_read(ssl_wrap_handle_t handle, unsigned char *buf, int len)
{
    ssl_wrap_handle_c *h = handle;
    return mbedtls_ssl_read(&h->ssl, buf, len);
}

int 
ssl_wrap_write(ssl_wrap_handle_t handle, unsigned char *buf, int len)
{
    ssl_wrap_handle_c *h = handle;
    return mbedtls_ssl_write(&h->ssl, buf, len);
}

int 
ssl_wrap_read_timeout(ssl_wrap_handle_t handle, unsigned char *buf, int len,
                      int timeout_ms)
{
    int ret, rcvd_bytes = 0;
    fd_set rs;
    struct timeval timeout = {0};
    ssl_wrap_handle_c *h = handle;    
    int sock_fd = h->net.fd;
    ssl_wrap_set_sock_nonblocking(sock_fd);
    
    do{
        ret = mbedtls_ssl_read(&h->ssl, (unsigned char *)buf, len);
        if(ret != MBEDTLS_ERR_SSL_WANT_READ && 
        ret != MBEDTLS_ERR_SSL_WANT_WRITE){
            rcvd_bytes = ret;
            break;
        }
        /*If ret is MBEDTLS_ERR_SSL_WANT_READ or MBEDTLS_ERR_SSL_WANT_WRITE
        * wait for the specified time n try again*/           
        FD_ZERO(&rs);
        FD_SET(h->net.fd, &rs);
        if(timeout_ms){
            timeout.tv_sec = (timeout_ms/1000)+1;
            timeout.tv_usec = 0;            
        }
        ret = select(h->net.fd+1, &rs, NULL, NULL, &timeout);        
        if(ret <= 0) {            
            os_printf("\n%s: Error timeout", __FUNCTION__);
            rcvd_bytes = -1;
            break;
        } 
    }while(1);  
    ssl_wrap_set_sock_blocking(sock_fd);
    return rcvd_bytes;
}

void 
ssl_wrap_crt_bundle_init( const char *ca_bundle)
{
    ssl_wrap_ca_bundle = ca_bundle;
}

void 
ssl_wrap_crt_bundle_deinit( void)
{
    ssl_wrap_ca_bundle = NULL;
}

