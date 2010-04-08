#ifndef _NGX_HTTP_SET_ESCAPE_URI
#define _NGX_HTTP_SET_ESCAPE_URI


ngx_int_t
ngx_http_set_misc_escape_uri(ngx_http_request_t *r,
		ngx_str_t *res, ngx_http_variable_value_t *v);

#endif
