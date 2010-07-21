
#ifndef NGX_SET_QUOTE_JSON
#define NGX_SET_QUOTE_JSON


#include <ngx_core.h>
#include <ngx_config.h>
#include <ngx_http.h>


ngx_int_t ngx_http_set_misc_quote_json_str(ngx_http_request_t *r, ngx_str_t *res, ngx_http_variable_value_t *v);

uintptr_t ngx_http_set_misc_escape_json_str(u_char *dst, u_char *src, size_t size);


#endif /* NGX_SET_QUOTE_SQL */

