#ifndef NGX_HTTP_SET_IP_MATCHES
#define NGX_HTTP_SET_IP_MATCHES

#include <ngx_core.h>
#include <ngx_config.h>
#include <ngx_http.h>

ngx_int_t ngx_http_set_misc_set_ip_matches(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v);

#endif /* NGX_HTTP_SET_IP_MATCHES */
