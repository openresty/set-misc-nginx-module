#ifndef NGX_HTTP_SET_HASHED_UPSTREAM
#define NGX_HTTP_SET_HASHED_UPSTREAM


#include <ngx_core.h>
#include <ngx_config.h>
#include <ngx_http.h>
#include <ndk.h>


typedef enum {
    ngx_http_set_misc_distribution_modula,
    ngx_http_set_misc_consistent_hash,
    ngx_http_set_misc_distribution_random /* XXX not used */
} ngx_http_set_misc_distribution_t;

typedef struct {
    ngx_str_t*                   name;
    uint32_t                     hash;
} ngx_http_set_hashed_upstream_consistent_hash_node;

typedef struct {
    ndk_upstream_list_t                                     *ul;
    ngx_http_set_hashed_upstream_consistent_hash_node       *hashNodes;
    uint32_t                                                nodeLen;
    ngx_http_set_misc_distribution_t                        hashType;
} ngx_http_set_hashed_upstream_conf;


ngx_uint_t ngx_http_set_misc_apply_distribution(ngx_log_t *log, ngx_uint_t hash,
                                                ngx_http_set_hashed_upstream_conf *hashConf,
                                                ngx_http_set_misc_distribution_t type);

uint32_t ngx_http_set_misc_bsearch(ngx_http_set_hashed_upstream_consistent_hash_node *nodes, uint32_t nodeLen,
                                   uint32_t target);

char *ngx_http_set_hashed_upstream_distribution_modula(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

char *ngx_http_set_hashed_upstream_consistent_hash(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

char *ngx_http_set_hashed_upstream_hashtype(ngx_conf_t *cf, ngx_command_t *cmd, void *conf,
                                            ngx_http_set_misc_distribution_t hashType);

ngx_int_t ngx_http_set_misc_set_hashed_upstream(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v, void *data);


#endif /* NGX_HTTP_SET_HASHED_UPSTREAM */
