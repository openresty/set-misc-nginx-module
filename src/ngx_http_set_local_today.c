#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"

#include <ndk.h>


ngx_int_t
ngx_http_set_local_today(ngx_http_request_t *r, ngx_str_t *res,
        ngx_http_variable_value_t *v)
{
    time_t           now;
    ngx_tm_t         tm;
    u_char          *p;

    /*t = ngx_timeofday();*/

    now = ngx_time();

    ngx_gmtime(now + ngx_cached_time->gmtoff * 60, &tm);

    dd("tm.ngx_tm_hour:%d", tm.ngx_tm_hour);

    p = ngx_palloc(r->pool, sizeof("yyyy-mm-dd") - 1);
    if (p == NULL) {
        return NGX_ERROR;
    }

    ngx_sprintf(p, "%04d-%02d-%02d", tm.ngx_tm_year, tm.ngx_tm_mon,
            tm.ngx_tm_mday);

    res->data = p;
    res->len = sizeof("yyyy-mm-dd") - 1;

    return NGX_OK;
}

