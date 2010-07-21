#define DDEBUG 0
#include "ddebug.h"
#include <ndk.h>
#include "ngx_http_set_quote_sql.h"
#define         true            1
#define         false           0

char *
ngx_http_set_quote_sql_str(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ndk_set_var_t               filter;
    ngx_str_t                   *value;
    ngx_str_t                   result[3];
    ngx_str_t                   *bad_arg;

    filter.type = NDK_SET_VAR_MULTI_VALUE;
    filter.size = 1;

    value = cf->args->elts;
    result[0] = value[1];

    if (cf->args->nelts == 2) {
        dd("set_quote_sql $foo");
        filter.func = &ngx_http_set_misc_quote_sql_str;
        return ndk_set_var_multi_value_core(cf, &result[0], &value[1], &filter);
    }

    if (value[1].len >= sizeof("for=") -1 && (ngx_strncasecmp((u_char *) "for=", value[1].data, sizeof("for=") -1))==0)
    {
        result[1] = value[2];
        result[0].data += sizeof("for=") -1;

        if (ngx_strncasecmp((u_char *) "pg", result[0].data, sizeof("pg") - 1) == 0) {
             filter.func = &ngx_http_set_misc_quote_pgsql_str;
        } else {
            bad_arg = &result[0];
        }

        if (cf->args->nelts > 4){
            bad_arg = &value[4];
        } else {
            return ndk_set_var_multi_value_core(cf, &result[1], &value[2], &filter);
        }
    } else if (cf->args->nelts == 3){
        dd("set_quote_sql $foo $foo");
        filter.func = &ngx_http_set_misc_quote_sql_str;
        return ndk_set_var_multi_value_core(cf, &result[0], &value[1], &filter);
    } else {
        bad_arg = &value[1];
    }

    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
            "%V: unexpected argument \"%V\"",
            &cmd->name, bad_arg);
    return NGX_CONF_ERROR;
}


ngx_int_t
ngx_http_set_misc_quote_pgsql_str(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v)
{
    u_char                   *pstr;
    ngx_int_t               length;

    if (v->not_found || v->len ==0) {
        res->data = (u_char *) "null";
        res->len = sizeof("null") - 1;
        return NGX_OK;
    }

    ngx_http_set_misc_quote_sql_str(r, res, v);
    length  = res->len;
    pstr    = ngx_palloc(r->pool, length + 1);
    *pstr   = 'E';
    memcpy(pstr + 1, res->data, length);
    res->data   = pstr;
    res->len    = length + 1;

    if (ngx_http_pg_utf_islegal(res->data, res->len)) {
       return NGX_OK;
    }

    res = ngx_http_pg_utf_escape(r, res);
    return NGX_OK;
}


ngx_int_t
ngx_http_pg_utf_mblen(const unsigned char *s)
{
    int len;

    if ((*s & 0x80) == 0)
        len = 1;
    else if ((*s & 0xe0) == 0xc0)
        len = 2;
    else if ((*s & 0xf0) == 0xe0)
        len = 3;
    else if ((*s & 0xf8) == 0xf0)
        len = 4;
#ifdef NOT_USED
    else if ((*s & 0xfc) == 0xf8)
        len = 5;
    else if ((*s & 0xfe) == 0xfc)
        len = 6;
#endif
    else
        len = 1;
    return len;
}


ngx_int_t
ngx_http_pg_utf_islegal(const unsigned char *s, ngx_int_t len)
{
    ngx_int_t               mblen;
    ngx_int_t               slen;
    u_char                  a;

    slen = len;

    while (slen > 0) {
        mblen = ngx_http_pg_utf_mblen(s);
        if (slen < mblen)
            return false;

        switch(mblen)
        {
            default:
                return false;
            case 4:
                a = *(s + 3);
                if (a < 0x80 || a > 0xBF)
                    return false;
            case 3:
                a = *(s + 2);
                if (a < 0x80 || a > 0xBF)
                    return false;
            case 2:
                a = *(s + 1);
                switch (*s)
                {
                    case 0xE0:
                        if (a < 0xA0 || a > 0xBF)
                            return false;
                        break;
                    case 0xED:
                        if (a < 0x80 || a > 0x9F)
                            return false;
                        break;
                    case 0xF0:
                        if (a < 0x90 || a > 0xBF)
                            return false;
                        break;
                    case 0xF4:
                        if (a < 0x80 || a > 0x8F)
                            return false;
                        break;
                    default:
                        if (a < 0x80 || a > 0xBF)
                            return false;
                        break;
                }
            case 1:
                a = *s;
                if (a >= 0x80 && a < 0xC2)
                    return false;
                if (a > 0xF4)
                    return false;
                break;
        }
        s       += mblen;
        slen    -= mblen;
    }
    return true;
}


ngx_str_t *
ngx_http_pg_utf_escape(ngx_http_request_t *r, ngx_str_t *res)
{
    ngx_str_t               *result;
    ngx_int_t                l, count;
    u_char                  *d, *p, *p1;

    l           = res->len;
    d           = res->data;
    result      = res;
    count       = 0;

    while (l-- > 0) {
        if (*d & 0x80) {
            count += 4;
        }
        d++;
        count++;
    }

    d   = res->data;
    l   = res->len;
    p   = ngx_palloc(r->pool, count);
    if (p == NULL) {
        return result;
    }

    p1  = p;
    while (l-- > 0) {
        if ((*d & 0x80)) {
            *p++ = '\\';
            *p++ = '\\';
            *p++ = (*d >> 6) + '0';
            *p++ = ((*d >> 3) & 07) + '0';
            *p++ = (*d & 07) + '0';
        } else {
            *p++ = *d;
        }
        d++;
    }

    result->len     = count;
    result->data    = p1;

    return result;
}


ngx_int_t
ngx_http_set_misc_quote_sql_str(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v)
{
    size_t                   len;
    u_char                  *p;
    size_t                   escape;

    if (v->not_found || v->len == 0) {
        res->data   = (u_char *) "null";
        res->len    = sizeof("null") - 1;
        return NGX_OK;
    }

    escape = ngx_http_set_misc_escape_sql_str(NULL, v->data, v->len);

    len = sizeof("''") - 1 + v->len + escape;

    p = ngx_palloc(r->pool, len);
    if (p == NULL) {
        return NGX_ERROR;
    }

    res->data = p;
    res->len = len;

    *p++ = '\'';

    if (escape == 0) {
        p = ngx_copy(p, v->data, v->len);
    } else {
        p = (u_char *) ngx_http_set_misc_escape_sql_str(p, v->data, v->len);
    }

    *p++ = '\'';

    if (p != res->data + res->len) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                "set_quote_sql_str: buffer error");
        return NGX_ERROR;
    }

    return NGX_OK;
}


uintptr_t
ngx_http_set_misc_escape_sql_str(u_char *dst, u_char *src,
        size_t size)
{
    ngx_uint_t               n;

    if (dst == NULL) {
        /* find the number of chars to be escaped */
        n = 0;
        while (size) {
            /* the highest bit of all the UTF-8 chars
             * is always 1 */
            if ((*src & 0x80) == 0) {
                switch (*src) {
                    case '\r':
                    case '\n':
                    case '\\':
                    case '\'':
                    case '"':
                    case '\032':
                        n++;
                        break;
                    default:
                        break;
                }
            }
            src++;
            size--;
        }

        return (uintptr_t) n;
    }

    while (size) {
        if ((*src & 0x80) == 0) {
            switch (*src) {
                case '\r':
                    *dst++ = '\\';
                    *dst++ = 'r';
                    break;

                case '\n':
                    *dst++ = '\\';
                    *dst++ = 'n';
                    break;

                case '\\':
                    *dst++ = '\\';
                    *dst++ = '\\';
                    break;

                case '\'':
                    *dst++ = '\\';
                    *dst++ = '\'';
                    break;

                case '"':
                    *dst++ = '\\';
                    *dst++ = '"';
                    break;

                case '\032':
                    *dst++ = '\\';
                    *dst++ = *src;
                    break;

                default:
                    *dst++ = *src;
                    break;
            }
        } else {
            *dst++ = *src;
        }
        src++;
        size--;
    } /* while (size) */

    return (uintptr_t) dst;
}

