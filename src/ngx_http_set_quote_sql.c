#define DDEBUG 0
#include "ddebug.h"
#include <ndk.h>
#include "ngx_http_set_quote_sql.h"


ngx_int_t
ngx_http_set_misc_quote_sql_str(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v)
{
    size_t                   len;
    u_char                  *p;
    size_t                   escape;

    if (v->not_found || v->len == 0) {
        res->data = (u_char *) "null";
        res->len = sizeof("null") - 1;
        return NGX_OK;
    }

    escape = ngx_http_set_misc_escape_sql_str(NULL, v->data, v->len);

    len = sizeof("''") - 1
        + v->len
        + escape;

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

