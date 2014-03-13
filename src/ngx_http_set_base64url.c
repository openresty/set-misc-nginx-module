#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"

#include <ndk.h>
#include "ngx_http_set_base64url.h"


ngx_int_t
ngx_http_set_misc_set_decode_base64url(ngx_http_request_t *r, ngx_str_t *res,
    ngx_http_variable_value_t *v)
{
    u_int     i;
    ngx_str_t src;

    src.len = v->len;
    src.data = v->data;

    res->len = ngx_base64url_decoded_length(v->len);
    res->data = ngx_palloc(r->pool, res->len);
    if (res->data == NULL) {
        return NGX_ERROR;
    }

#if defined(nginx_version) && nginx_version >= 1005010
    if (ngx_decode_base64url(res, &src) != NGX_OK) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                      "set_decode_base64url: invalid value");
        return NGX_ERROR;
    }
#else
    for (i=0;i<src.len;i++) {
        switch (src.data[i]) {
            case '-': src.data[i]='+'; break;
            case '_': src.data[i]='/'; break;
        }
    }
    if (ngx_decode_base64(res, &src) != NGX_OK) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                      "set_decode_base64url: invalid value");
        return NGX_ERROR;
    }
#endif

    return NGX_OK;
}


ngx_int_t
ngx_http_set_misc_set_encode_base64url(ngx_http_request_t *r, ngx_str_t *res,
    ngx_http_variable_value_t *v)
{
    u_int i;
    ngx_str_t        src;

    src.len = v->len;
    src.data = v->data;

    res->len = ngx_base64url_encoded_length(v->len);
    res->data = ngx_palloc(r->pool, res->len);
    if (res->data == NULL) {
        return NGX_ERROR;
    }

#if defined(nginx_version) && nginx_version >= 1005010
    ngx_encode_base64url(res, &src);
#else
    ngx_encode_base64(res, &src);
    for (i=0;i<res->len;i++) {
        switch (res->data[i]) {
            case '+': res->data[i]='-'; break;
            case '/': res->data[i]='_'; break;
            case '=': res->len--; break;
        }
    }
#endif

    return NGX_OK;
}

