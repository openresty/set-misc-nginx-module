#define DDEBUG 0
#include "ddebug.h"
#include <ndk.h>
#include "ngx_http_set_escape_uri.h"
#include "ngx_string.h"

ngx_int_t
ngx_http_set_misc_escape_uri(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v)
{
    size_t        len;
    u_char        *src, *dst;

    if (v->len == 0) {
        res->len = 0;
        res->data = NULL;
        return NGX_OK;
    }

    src = v->data;

    dd("before escape:%.*s", v->len, v->data);
    len = v->len + 2 * ngx_escape_uri(NULL, src, v->len, NGX_ESCAPE_URI);
    dd("escaped string len:%zu", len);

    dst = ngx_palloc(r->pool, len);

    if (dst == NULL) {
        return NGX_ERROR;
    }

    ngx_escape_uri(dst, src, v->len, NGX_ESCAPE_URI);

    res->data = dst;
    res->len = len;
    dd("after eacape:%.*s", res->len, res->data);
    return NGX_OK;
}

