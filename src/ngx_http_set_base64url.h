#ifndef NGX_HTTP_SET_BASE64URL
#define NGX_HTTP_SET_BASE64URL

#include <ngx_core.h>
#include <ngx_config.h>
#include <ngx_http.h>

#ifndef ngx_base64url_decoded_length
#define ngx_base64url_decoded_length(len)  ((((len + 3) / 4) * 3) - (len % 4 ? 4 - len % 4 : 0))
#endif

#ifndef ngx_base64url_encoded_length
#define ngx_base64url_encoded_length(len)  ((((len + 2) / 3) * 4) + (len % 3 ? len % 3 + 1 : 0))
#endif

ngx_int_t ngx_http_set_misc_set_encode_base64url(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v);

ngx_int_t ngx_http_set_misc_set_decode_base64url(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v);

#endif /* NGX_HTTP_SET_BASE64URL */
