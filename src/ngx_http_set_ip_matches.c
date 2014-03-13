#ifndef DDEBUG
#define DDEBUG 1
#endif
#include "ddebug.h"

#include <ndk.h>

#include "ngx_http_set_ip_matches.h"

ngx_int_t
ngx_http_set_misc_set_ip_matches(ngx_http_request_t *r, ngx_str_t *res,
    ngx_http_variable_value_t *v)
{
    ngx_http_variable_value_t *network_var, *ip_var;
    ngx_str_t                 network_str, ip_str;
    ngx_cidr_t                network, ip;
    size_t                    len;
    u_char                    *ip_addr, *ip_mask, *network_addr, *network_mask, result;
    u_int                     i;

    network_var = v;
    ip_var = v+1;

    network_str.len = network_var->len;
    network_str.data = network_var->data;

    ip_str.len = ip_var->len;
    ip_str.data = ip_var->data;

    if (ngx_ptocidr(&network_str, &network) == NGX_ERROR) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "network: invalid address or cidr");
        return NGX_ERROR;
    }

    if (ngx_ptocidr(&ip_str, &ip) == NGX_ERROR) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ip: invalid address");
        return NGX_ERROR;
    }

    if (network.family != ip.family) {
          ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "IPv4 and IPv6 cannot be mixed");
        return NGX_ERROR;
    }

    switch (ip.family) {
        case AF_INET:
            len = 4;
            ip_addr = (u_char *) &ip.u.in.addr;
            ip_mask = (u_char *) &ip.u.in.mask;
            network_addr = (u_char *) &network.u.in.addr;
            network_mask = (u_char *) &network.u.in.mask;
            break;
        case AF_INET6:
            len = 16;
            ip_addr = ip.u.in6.addr.__in6_u.__u6_addr8;
            ip_mask = ip.u.in6.mask.__in6_u.__u6_addr8;
            network_addr = network.u.in6.addr.__in6_u.__u6_addr8;
            network_mask = network.u.in6.mask.__in6_u.__u6_addr8;
            break;
        default:
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "invalid address family");
            return NGX_ERROR;
    }

    for (i=0;i<len;i++) {
    	if (ip_mask[i] != 255) {
    		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "ip: cidr notation not allowed");
    		return NGX_ERROR;
    	}
    }

    result = 0;
    for (i=0;i<len;i++) {
    	result |= (network_addr[i] & network_mask[i]) ^ (ip_addr[i] & network_mask[i]);
    }

    res->len = 1;
    res->data = ngx_palloc(r->pool, res->len);
    if (res->data == NULL) {
        return NGX_ERROR;
    }
    res->data[0] = result==0 ? '1' : '0';

    return NGX_OK;
 }
