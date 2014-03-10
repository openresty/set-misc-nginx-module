#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"

#include <ndk.h>

#include "ngx_http_set_expired.h"

ngx_int_t
ngx_http_set_misc_set_expired(ngx_http_request_t *r, ngx_str_t *res,
    ngx_http_variable_value_t *v)
{
    ngx_http_variable_value_t *expires;
    time_t                    now, deadline;

    expires = v;
    dd("expires=%.*s",(int) expires->len, expires->data);

    now = ngx_time();
    deadline = ngx_atotm(expires->data, expires->len);
    dd("now=%lld, deadline=%lld", (long long) now, (long long) deadline);

    res->len = 1;
    res->data = ngx_palloc(r->pool, res->len);
    if (res->data == NULL) {
        return NGX_ERROR;
    }
    res->data[0] = (u_char) ((deadline && now < deadline) ? '0' : '1');

    return NGX_OK;
 }
