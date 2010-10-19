#define DDEBUG 0
#include "ddebug.h"

#include <ndk.h>
#include "ngx_http_set_hash.h"

#ifndef NGX_HTTP_SET_HASH
ngx_int_t
ngx_http_set_misc_set_sha1(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v)
{
    u_char                  *p;

    p = ngx_palloc(r->pool, SHA_DIGEST_LENGTH * 2);
    if (p == NULL) {
        return NGX_ERROR;
    }

    ndk_sha1_hash(p, (char *) v->data, v->len);

    res->data = p;
    res->len = SHA_DIGEST_LENGTH * 2;

    return NGX_OK;
}

ngx_int_t
ngx_http_set_misc_set_md5(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v)
{
    u_char                  *p;
    p = ngx_palloc(r->pool, MD5_DIGEST_LENGTH * 2);
    if (p == NULL) {
        return NGX_ERROR;
    }

    ndk_md5_hash(p, (char *) v->data, v->len);

    res->data = p;
    res->len = MD5_DIGEST_LENGTH * 2;

    return NGX_OK;
}
#endif
