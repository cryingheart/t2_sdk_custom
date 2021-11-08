#pragma once

typedef void * ssl_wrap_handle_t;

typedef enum {
    SSL_WRAP_VERIFY_NONE,    /**<peer certificate is not checked
                            (default on server)
                            (insecure on client)*/

    SSL_WRAP_VERIFY_OPTIONAL,/**<peer certificate is checked, however the
                         handshake continues even if verification failed;
                         mbedtls_ssl_get_verify_result() can be called after the
                         handshake is complete.*/

    SSL_WRAP_VERIFY_REQUIRED /**<peer *must* present a valid certificate,
                         handshake is aborted if verification failed.
                         (default on client)*/

} ssl_auth_mode_t;

typedef struct {
    char *path;/**<not used currently. path of the certificate in file sys*/
    char *buf;
    int len;
}ssl_wrap_cert_t;
typedef struct{    
    ssl_wrap_cert_t ca_cert;
    ssl_wrap_cert_t client_cert;/**< own ie client certificate info*/
    ssl_wrap_cert_t client_key;/**< own ie client key information*/
    ssl_auth_mode_t auth_mode;
    int max_frag_len;/*Value shall be >= MBEDTLS_SSL_MAX_FRAG_LEN_512 and
                        <= MBEDTLS_SSL_MAX_FRAG_LEN_4096*/
    char *p_data;/**< */
}ssl_wrap_cfg_t;


ssl_wrap_handle_t 
ssl_wrap_connect(char *host_name, int port, ssl_wrap_cfg_t *cfg);

int 
ssl_wrap_read(ssl_wrap_handle_t handle, unsigned char *buf, int len);

int 
ssl_wrap_write(ssl_wrap_handle_t handle, unsigned char *buf, int len);

int 
ssl_wrap_read_timeout(ssl_wrap_handle_t handle, unsigned char* buf, int len,
                  int timeout_ms);

void
ssl_wrap_disconnect(ssl_wrap_handle_t handle);

int
ssl_wrap_sock_fd_get(ssl_wrap_handle_t handle);

/*Returns ssl handle for the supplied socket fd*/
ssl_wrap_handle_t
ssl_wrap_fd_to_handle(int sock_fd);

void 
ssl_wrap_crt_bundle_init( const char *ca_bundle);

void 
ssl_wrap_crt_bundle_deinit( void);


