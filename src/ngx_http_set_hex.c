#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"

#include <ndk.h>
#include "ngx_http_set_hex.h"


ngx_int_t
ngx_http_set_misc_set_encode_hex_dash(ngx_http_request_t *r, ngx_str_t *res,
    ngx_http_variable_value_t *v)
{

    u_char      *p;
    ngx_uint_t   i;
    size_t       len;

    if (v->len % 2 != 0) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                      "set_encode_hex_dash: invalid value");
        return NGX_ERROR;
    }

    p = v->data;
    len = v->len << 1;

    res->data = ngx_palloc(r->pool, len);
    if (res->data == NULL) {
        return NGX_ERROR;
    }

    for (i = 0; i < len; i=i+2) {
        res->data[(i*2)+0] = (u_char) 92;
        res->data[(i*2)+1] = (u_char) 120;
        res->data[(i*2)+2] = p[i];
        res->data[(i*2)+3] = p[i+1];
    }

    res->len = len;
    return NGX_OK;
}

ngx_int_t
ngx_http_set_misc_set_decode_hex(ngx_http_request_t *r, ngx_str_t *res,
    ngx_http_variable_value_t *v)
{
    u_char      *p;
    ngx_int_t    n;
    ngx_uint_t   i;
    size_t       len;

    if (v->len % 2 != 0) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                      "set_decode_hex: invalid value");
        return NGX_ERROR;
    }

    p = v->data;
    len = v->len >> 1;

    res->data = ngx_palloc(r->pool, len);
    if (res->data == NULL) {
        return NGX_ERROR;
    }

    for (i = 0; i < len; i++) {
        n = ngx_hextoi(p, 2);
        if (n == NGX_ERROR || n > 255) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                          "set_decode_hex: invalid value");
            return NGX_ERROR;
        }

        p += 2;
        res->data[i] = (u_char) n;
    }

    res->len = len;
    return NGX_OK;
}

ngx_int_t
ngx_http_set_misc_set_encode_hex(ngx_http_request_t *r, ngx_str_t *res,
    ngx_http_variable_value_t *v)
{
    res->len = v->len << 1;
    res->data = ngx_palloc(r->pool, res->len);
    if (res->data == NULL) {
        return NGX_ERROR;
    }

    ngx_hex_dump(res->data, v->data, v->len);
    return NGX_OK;
}
