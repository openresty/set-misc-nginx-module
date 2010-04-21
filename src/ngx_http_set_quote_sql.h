#include <ngx_core.h>
#include <ngx_config.h>
#include <ngx_http.h>

#ifndef _NGX_SET_QUOTE_SQL
#define _NGX_SET_QUOTE_SQL


uintptr_t ngx_http_set_misc_escape_sql_str(u_char *dst, u_char *src,
        size_t size);


ngx_int_t ngx_http_set_misc_quote_sql_str(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v);


#endif
