#define DDEBUG 0
#include "ddebug.h"

#include <ndk.h>
#include "ngx_http_set_hash.h"


ngx_int_t
ngx_http_set_misc_set_sha1(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v)
{
    u_char                  *p;

    p = ngx_palloc(r->pool, SHA1_DIGEST_LENGTH * 2);
    if (p == NULL) {
        return NGX_ERROR;
    }

    ndk_sha1_lower_hash((char *) p, (char *) v->data, v->len);

    res->data = p;
    res->len = SHA1_DIGEST_LENGTH * 2;

    return NGX_OK;
}

