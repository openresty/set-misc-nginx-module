#include <ngx_core.h>
#include <ngx_config.h>
#include <ngx_http.h>

#ifndef _NGX_HTTP_SET_UNESCAPE_URI
#define _NGX_HTTP_SET_UNESCAPE_URI


ngx_int_t ngx_http_set_misc_unescape_uri(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v);

void ngx_unescape_uri_patched(u_char **dst, u_char **src,
        size_t size, ngx_uint_t type);


#endif
