#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"

#include <ndk.h>
#include "ngx_http_set_hex.h"

ngx_int_t
ngx_http_set_misc_set_decode_hex(ngx_http_request_t *r,
                                 ngx_str_t *res, ngx_http_variable_value_t *v)
{

    u_char      *p;
    ngx_int_t   n;
    ngx_int_t   i;

    if (v->len % 2 != 0) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                      "set_decode_hex: invalid value");
        return NGX_ERROR;
    }
    p = v->data;
    ndk_palloc_re(res->data, r->pool, v->len/2);
    for (i = 0; i < v->len/2; i++) {
        n = ngx_hextoi(p, 2);
        if (n == NGX_ERROR || n > 255) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                          "set_decode_hex: invalid value");
            return NGX_ERROR;
        }
        p += 2;
        res->data[i] = (u_char) n;
    }
    res->len = v->len/2;
    return NGX_OK;
}


ngx_int_t
ngx_http_set_misc_set_encode_hex(ngx_http_request_t *r,
                                 ngx_str_t *res, ngx_http_variable_value_t *v)
{
    res->len = (v->len)*2;
    ndk_palloc_re(res->data, r->pool, res->len);
    ngx_hex_dump(res->data, v->data, v->len);
    return NGX_OK;
}
