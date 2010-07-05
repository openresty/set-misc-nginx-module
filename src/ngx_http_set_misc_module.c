#define DDEBUG 0
#include "ddebug.h"

#include <ndk.h>
#include "ngx_http_set_base32.h"
#include "ngx_http_set_default_value.h"
#include "ngx_http_set_hashed_upstream.h"
#include "ngx_http_set_unescape_uri.h"
#include "ngx_http_set_quote_sql.h"
#include "ngx_http_set_escape_uri.h"
#include "ngx_http_set_local_today.h"
#include "ngx_http_set_hash.h"

#define NGX_UNESCAPE_URI_COMPONENT  0

static  ndk_set_var_t  ngx_http_set_misc_set_sha1_filter = {
    NDK_SET_VAR_VALUE,
    ngx_http_set_misc_set_sha1,
    1,
    NULL
};

static  ndk_set_var_t  ngx_http_set_misc_unescape_uri_filter = {
    NDK_SET_VAR_VALUE,
    ngx_http_set_misc_unescape_uri,
    1,
    NULL
};

static ndk_set_var_t ngx_http_set_misc_escape_uri_filter = {
    NDK_SET_VAR_VALUE,
    ngx_http_set_misc_escape_uri,
    1,
    NULL
};

static  ndk_set_var_t  ngx_http_set_misc_quote_sql_str_filter = {
    NDK_SET_VAR_VALUE,
    ngx_http_set_misc_quote_sql_str,
    1,
    NULL
};

static  ndk_set_var_t  ngx_http_set_misc_encode_base32_filter = {
    NDK_SET_VAR_VALUE,
    ngx_http_set_misc_encode_base32,
    1,
    NULL
};

static  ndk_set_var_t  ngx_http_set_misc_decode_base32_filter = {
    NDK_SET_VAR_VALUE,
    ngx_http_set_misc_decode_base32,
    1,
    NULL
};

static ndk_set_var_t ngx_http_set_misc_local_today_filter = {
    NDK_SET_VAR_VALUE,
    ngx_http_set_local_today,
    0,
    NULL
};

static ngx_command_t  ngx_http_set_misc_commands[] = {
    {
        ngx_string ("set_sha1"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_SIF_CONF
            |NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE12,
        ndk_set_var_value,
        0,
        0,
        &ngx_http_set_misc_set_sha1_filter
    },
    {
        ngx_string ("set_unescape_uri"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_SIF_CONF
            |NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE12,
        ndk_set_var_value,
        0,
        0,
        &ngx_http_set_misc_unescape_uri_filter
    },
    {
        ngx_string ("set_escape_uri"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_SIF_CONF
            |NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE12,
        ndk_set_var_value,
        0,
        0,
        &ngx_http_set_misc_escape_uri_filter
    },
    {
        ngx_string ("set_quote_sql_str"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_SIF_CONF
            |NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE12,
        ndk_set_var_value,
        0,
        0,
        &ngx_http_set_misc_quote_sql_str_filter
    },
    {
        ngx_string ("set_if_empty"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_SIF_CONF
            |NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE2,
        ngx_http_set_if_empty,
        0,
        0,
        NULL
    },
    {
        ngx_string("set_hashed_upstream"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_SIF_CONF
            |NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE3,
        ngx_http_set_hashed_upstream,
        0,
        0,
        NULL
    },
    {
        ngx_string("set_encode_base32"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_SIF_CONF
            |NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE12,
        ndk_set_var_value,
        0,
        0,
        &ngx_http_set_misc_encode_base32_filter
    },
    {
        ngx_string("set_decode_base32"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_SIF_CONF
            |NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE12,
        ndk_set_var_value,
        0,
        0,
        &ngx_http_set_misc_decode_base32_filter
    },
    {
        ngx_string("set_local_today"),
        NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_SIF_CONF
            |NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF|NGX_CONF_TAKE1,
        ndk_set_var_value,
        0,
        0,
        &ngx_http_set_misc_local_today_filter
    },

    ngx_null_command
};


static ngx_http_module_t  ngx_http_set_misc_module_ctx = {
    NULL,                                 /* preconfiguration */
    NULL,                                 /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL,                                  /* merge location configuration */
};


ngx_module_t  ngx_http_set_misc_module = {
    NGX_MODULE_V1,
    &ngx_http_set_misc_module_ctx,          // module context
    ngx_http_set_misc_commands,             // module directives
    NGX_HTTP_MODULE,                        // module type
    NULL,                                   // init master
    NULL,                                   // init module
    NULL,                                   // init process
    NULL,                                   // init thread
    NULL,                                   // exit thread
    NULL,                                   // exit process
    NULL,                                   // exit master
    NGX_MODULE_V1_PADDING
};
