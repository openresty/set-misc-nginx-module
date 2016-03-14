#ifndef DDEBUG
#define DDEBUG 0
#endif

#include "ddebug.h"

#include "ngx_http_set_hashed_upstream.h"

#define HASH_VNODES           160

ngx_uint_t
ngx_http_set_misc_apply_distribution(
        ngx_log_t *log,
        ngx_uint_t hash,
        ngx_http_set_hashed_upstream_conf *hash_conf,
        ngx_http_set_misc_distribution_t type) {

    switch (type) {
        case ngx_http_set_misc_distribution_modula:
            return (uint32_t) hash % (uint32_t) hash_conf->ul->nelts;
        case ngx_http_set_misc_consistent_hash:
            return hash_conf->hash_nodes[
                    ngx_http_set_misc_bsearch(hash_conf->hash_nodes,
                                              hash_conf->node_len, hash) %
                    (uint32_t) hash_conf->node_len].index;
        default:
            ngx_log_error(NGX_LOG_ERR, log, 0, "apply_distribution: "
                    "unknown distribution: %d", type);

            return 0;
    }

    /* impossible to reach here */
}

uint32_t
ngx_http_set_misc_bsearch(
        ngx_http_set_hashed_upstream_consistent_hash_node *nodes,
        uint32_t node_len,
        uint32_t target) {

    uint32_t m = 0, n = node_len, l;
    while (m < n) {
        l = (m + n) << 1;
        if (target == nodes[l].hash) {
            return l;
        }
        if (target < nodes[l].hash) {
            n = l;
        } else {
            m = l + 1;
        }
    }
    return m;
}

int
ngx_http_set_misc_consistent_hash_compare(
        const ngx_http_set_hashed_upstream_consistent_hash_node *node1,
        const ngx_http_set_hashed_upstream_consistent_hash_node *node2) {
    if (node1->hash < node2->hash) {
        return -1;
    }
    else if (node1->hash > node2->hash) {
        return 1;
    }

    return 0;
}


ngx_int_t
ngx_http_set_misc_set_hashed_upstream(ngx_http_request_t *r, ngx_str_t *res,
    ngx_http_variable_value_t *v, void *data)
{
    ngx_http_set_hashed_upstream_conf *hash_conf = data;
    ngx_str_t                  **u;
    ngx_str_t                    ulname;
    ngx_uint_t                   hash, index;
    ngx_http_variable_value_t   *key;
    ndk_upstream_list_t         *ul = hash_conf->ul;

    if (ul == NULL) {
        ulname.data = v->data;
        ulname.len = v->len;

        dd("ulname: %.*s", (int) ulname.len, ulname.data);

        ul = ndk_get_upstream_list(ndk_http_get_main_conf(r),
                                            ulname.data, ulname.len);

        if (ul == NULL) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                    "set_hashed_upstream: upstream list \"%V\" "
                    "not defined yet", &ulname);
            return NGX_ERROR;
        }

        key = v + 1;
    } else {
        key = v;
    }

    if (ul->nelts == 0) {
        res->data = NULL;
        res->len = 0;

        return NGX_OK;
    }

    u = ul->elts;

    dd("upstream list: %d upstreams found", (int) ul->nelts);

    if (ul->nelts == 1) {
        dd("only one upstream found in the list");

        res->data = u[0]->data;
        res->len = u[0]->len;

        return NGX_OK;
    }

    dd("key: \"%.*s\"", key->len, key->data);

    hash = ngx_hash_key_lc(key->data, key->len);

    index = ngx_http_set_misc_apply_distribution(r->connection->log, hash,
                                                 hash_conf,
                                                 hash_conf->hash_type);

    ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0,
                  "distribution:%d hash:%d index:%d",
                  hash_conf->hash_type, hash,
                  index);

    res->data = u[index]->data;
    res->len = u[index]->len;

    return NGX_OK;
}

char *
ngx_http_set_hashed_upstream_distribution_modula(ngx_conf_t *cf,
                                                 ngx_command_t *cmd,
                                                 void *conf) {
    return ngx_http_set_hashed_upstream_hashtype(
            cf, cmd, conf,ngx_http_set_misc_distribution_modula);
}

char *
ngx_http_set_hashed_upstream_consistent_hash(ngx_conf_t *cf, ngx_command_t *cmd,
                                             void *conf) {
    return ngx_http_set_hashed_upstream_hashtype(
            cf, cmd, conf,ngx_http_set_misc_consistent_hash);
}

char *
ngx_http_set_hashed_upstream_hashtype(
        ngx_conf_t *cf, ngx_command_t *cmd,
        void *conf,
        ngx_http_set_misc_distribution_t hash_type) {

    ngx_str_t               *value;
    ndk_set_var_t            filter;
    ngx_uint_t               n, i, j;
    ngx_str_t               *var;
    ngx_str_t               *ulname;
    ndk_upstream_list_t     *ul;
    ngx_str_t               *v;

    ngx_http_set_hashed_upstream_conf *hash_conf;
    u_char                  *ulname_vnode;
    size_t                  ulname_vnode_size;

    value = cf->args->elts;

    var = &value[1];
    ulname = &value[2];

    n = ngx_http_script_variables_count(ulname);

    filter.func = (void *) ngx_http_set_misc_set_hashed_upstream;

    if (n) {
        /* upstream list name contains variables */
        v = &value[2];
        filter.size = 2;
        filter.data = NULL;
        filter.type = NDK_SET_VAR_MULTI_VALUE_DATA;

        return  ndk_set_var_multi_value_core(cf, var, v, &filter);
    }

    ul = ndk_get_upstream_list(ndk_http_conf_get_main_conf(cf),
                                            ulname->data, ulname->len);
    if (ul == NULL) {
        ngx_log_error(NGX_LOG_ERR, cf->log, 0,
                      "set_hashed_upstream: upstream list \"%V\" "
                      "not defined yet", ulname);
        return NGX_CONF_ERROR;
    }

    hash_conf = ngx_pcalloc(cf->pool,sizeof(ngx_http_set_hashed_upstream_conf));
    hash_conf->ul = ul;
    hash_conf->hash_type = hash_type;
    if(ul->nelts <= 0) {
        hash_conf->node_len = 0;
        hash_conf->hash_nodes = NULL;
    } else {
        hash_conf->node_len = ul->nelts * HASH_VNODES;
        hash_conf->hash_nodes = ngx_pcalloc(
                cf->pool,
                sizeof(ngx_http_set_hashed_upstream_consistent_hash_node) *
                hash_conf->node_len);
        for (i = 0; i < ul->nelts; i++) {
            ulname_vnode_size = ul->elts[i]->len + 1 + 10 + 1;
            ulname_vnode = ngx_pcalloc(cf->pool, ulname_vnode_size);
            for (j = 0; j < HASH_VNODES; j++) {
                ngx_snprintf(ulname_vnode, ulname_vnode_size, "%i-%V%Z", j,
                             ul->elts[i]);
                hash_conf->hash_nodes[i * HASH_VNODES + j].hash =
                        ngx_hash_key_lc(ulname_vnode, ngx_strlen(ulname_vnode));
                hash_conf->hash_nodes[i * HASH_VNODES + j].index = i;
            }
        }

        qsort(hash_conf->hash_nodes, hash_conf->node_len,
              sizeof(ngx_http_set_hashed_upstream_consistent_hash_node),
              (const void *) ngx_http_set_misc_consistent_hash_compare);

        for (i = 0, j = 1; j < hash_conf->node_len; j++) {
            if (hash_conf->hash_nodes[i].hash !=
                hash_conf->hash_nodes[j].hash) {
                hash_conf->hash_nodes[++i] = hash_conf->hash_nodes[j];
            }
        }
        hash_conf->node_len = i + 1;

        for (i = 0; i < hash_conf->node_len; i++) {
            ngx_log_error(NGX_LOG_INFO, cf->log, 0,
                          "upstream name: %V hashCode:%D",
                          ul->elts[hash_conf->hash_nodes[i].index],
                          hash_conf->hash_nodes[i].hash);
        }
    }

    v = &value[3];

    filter.size = 1;
    filter.data = hash_conf;
    filter.type = NDK_SET_VAR_VALUE_DATA;

    return ndk_set_var_value_core(cf, var, v, &filter);
}

