#define DDEBUG 0
#include "ddebug.h"
#include <ndk.h>
#include "ngx_http_set_base32.h"


#define base32_encoded_length(len) ((((len)+4)/5)*8)
#define base32_decoded_length(len) ((((len)+7)/8)*5)


static void encode_base32(int slen, const char *src, int *dlen, char *dst);
static int decode_base32(int slen, const char *src, int *dlen, char *dst);


ngx_int_t
ngx_http_set_misc_encode_base32(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v)
{
    size_t                   len;
    u_char                  *p;
    u_char                  *src, *dst;

    len = base32_encoded_length(v->len);

    dd("estimated dst len: %d", len);

    p = ngx_palloc(r->pool, len);
    if (p == NULL) {
        return NGX_ERROR;
    }

    src = v->data; dst = p;

    encode_base32((int)v->len, (const char *)src, (int *)&len, (char *)dst);

    res->data = p;
    res->len = len;

    dd("res (len %d): %.*s", res->len, res->len, res->data);

    return NGX_OK;
}


ngx_int_t
ngx_http_set_misc_decode_base32(ngx_http_request_t *r,
        ngx_str_t *res, ngx_http_variable_value_t *v)
{
    size_t                   len;
    u_char                  *p;
    u_char                  *src, *dst;
    int                      ret;

    len = base32_decoded_length(v->len);

    dd("estimated dst len: %d", len);

    p = ngx_palloc(r->pool, len);
    if (p == NULL) {
        return NGX_ERROR;
    }

    src = v->data; dst = p;

    ret = decode_base32((int)v->len, (const char *)src, (int *)&len,
            (char *)dst);

    if (ret == 0 /* OK */) {
        res->data = p;
        res->len = len;

        return NGX_OK;
    }

    /* failed to decode */

    res->data = NULL;
    res->len = 0;

    return NGX_OK;
}


/* 实现参考 src/core/ngx_string.c 中的 ngx_(encode|decode)_base64()
 * 例程 */

/**
 * 将给定字符串转换成对应的 Base32 编码形式.
 * 目标字符串必须保证有充足的空间容纳编码后的数据.
 * 可以用宏 base32_encoded_length() 预估编码后数据
 * 长度并预先为目标字符串分配空间.
 * <code>
 *     char *src, *dst;
 *     int slen, dlen;
 *     slen = sizeof("hello") - 1;
 *     src = (char*) "hello";
 *     dst = malloc(base32_encoded_length(slen));
 *     encode_base32(slen, src, &(dlen), dst);
 * </code>
 * @param slen 源数据串长度.
 * @param src 原数据串指针.
 * @param dlen 目标数据串长度指针, 保存 Base32 编码后数据长度.
 * @param dst 目标数据串指针, 保存 Base32 编码后数据.
 * */
static void
encode_base32(int slen, const char *src, int *dlen, char *dst)
{
    static unsigned char basis32[] = "0123456789abcdefghijklmnopqrstuv";

    int len;
    const unsigned char *s;
    unsigned char *d;

    len = slen;
    s = (const unsigned char*)src;
    d = (unsigned char*)dst;

    while (len > 4) {
        *d++ = basis32[s[0] >> 3];
        *d++ = basis32[((s[0] & 0x07) << 2) | (s[1] >> 6)];
        *d++ = basis32[(s[1] >> 1) & 0x1f];
        *d++ = basis32[((s[1] & 1) << 4) | (s[2] >> 4)];
        *d++ = basis32[((s[2] & 0x0f) << 1) | (s[3] >> 7)];
        *d++ = basis32[(s[3] >> 2) & 0x1f];
        *d++ = basis32[((s[3] & 0x03) << 3) | (s[4] >> 5)];
        *d++ = basis32[s[4] & 0x1f];

        s += 5;
        len -= 5;
    }

    if (len) {
        *d++ = basis32[s[0] >> 3];

        if (len == 1) {
            /* 剩余 1 个字节 */
            *d++ = basis32[(s[0] & 0x07) << 2];

            /* 到结束为止补 6 个 = */
            *d++ = '=';
            *d++ = '=';
            *d++ = '=';
            *d++ = '=';
            *d++ = '=';
        } else {
            *d++ = basis32[((s[0] & 0x07) << 2) | (s[1] >> 6)];
            *d++ = basis32[(s[1] >> 1) & 0x1f];

            if (len == 2) {
                /* 剩余 2 个字节 */
                *d++ = basis32[(s[1] & 1) << 4];

                /* 到结束为止补 4 个 = */
                *d++ = '=';
                *d++ = '=';
                *d++ = '=';
            } else {
                *d++ = basis32[((s[1] & 1) << 4) | (s[2] >> 4)];

                if (len == 3) {
                    /* 剩余 3 个字节 */
                    *d++ = basis32[(s[2] & 0x0f) << 1];

                    /* 到结束为止补 3 个 = */
                    *d++ = '=';
                    *d++ = '=';
                } else {
                    /* 剩余 4 个字节 */
                    *d++ = basis32[((s[2] & 0x0f) << 1) | (s[3] >> 7)];
                    *d++ = basis32[(s[3] >> 2) & 0x1f];
                    *d++ = basis32[(s[3] & 0x03) << 3];

                    /* 到结束为止补 1 个 = */
                }
            }
        }

        *d++ = '=';
    }

    *dlen = d - (unsigned char*)dst;
}


static int
decode_base32(int slen, const char *src, int *dlen, char *dst)
{
    static unsigned char basis32[] = {
        /* 0 - 15 */
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,

        /* 16 - 31 */
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,

        /* 32 - 47 */
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,

        /* 48 - 63 */
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 77, 77, 77, 77, 77, 77,

        /* 64 - 79 */
        77, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,

        /* 80 - 95 */
        25, 26, 27, 28, 29, 30, 31, 77, 77, 77, 77, 77, 77, 77, 77, 77,

        /* 96 - 111 */
        77, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,

        /* 112 - 127 */
        25, 26, 27, 28, 29, 30, 31, 77, 77, 77, 77, 77, 77, 77, 77, 77,

        /* 128 - 143 */
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,

        /* 144 - 159 */
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,

        /* 160 - 175 */
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,

        /* 176 - 191 */
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,

        /* 192 - 207 */
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,

        /* 208 - 223 */
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,

        /* 224 - 239 */
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77,

        /* 240 - 255 */
        77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77
    };

    int len, mod;
    const unsigned char *s = (const unsigned char*)src;
    unsigned char *d = (unsigned char*)dst;

    for (len = 0; len < slen; len++) {
        if (s[len] == '=') {
            break;
        }

        if (basis32[s[len]] == 77) {
            return -1;
        }
    }

    mod = len % 8;

    if (mod == 1 || mod == 3 || mod == 6) {
        /* bad Base32 digest length */
        return -1;
    }

    while (len > 7) {
        *d++ = (basis32[s[0]] << 3) | ((basis32[s[1]] >> 2) & 0x07);

        *d++ = ((basis32[s[1]] & 0x03) << 6) | (basis32[s[2]] << 1) |
            ((basis32[s[3]] >> 4) & 1);

        *d++ = ((basis32[s[3]] & 0x0f) << 4) | ((basis32[s[4]] >> 1) & 0x0f);

        *d++ = ((basis32[s[4]] & 1) << 7) | ((basis32[s[5]] & 0x1f) << 2) |
            ((basis32[s[6]] >> 3) & 0x03);
        *d++ = ((basis32[s[6]] & 0x07) << 5) | (basis32[s[7]] & 0x1f);

        s += 8;
        len -= 8;
    }

    if (len) {
        /* 2 bytes left */
        *d++ = (basis32[s[0]] << 3) | ((basis32[s[1]] >> 2) & 0x07);

        if (len > 2) {
            /* 4 bytes left */
            *d++ = ((basis32[s[1]] & 0x03) << 6) | ((basis32[s[2]] & 0x1f) << 1)
                | ((basis32[s[3]] >> 4) & 1);

            if (len > 4) {
                /* 5 bytes left */
                *d++ = ((basis32[s[3]] & 0x0f) << 4) |
                    ((basis32[s[4]] >> 1) & 0x0f);

                if (len > 5) {
                    /* 7 bytes left */
                    *d++ = ((basis32[s[4]] & 1) << 7) |
                        ((basis32[s[5]] & 0x1f) << 2) |
                        ((basis32[s[6]] >> 3) & 0x03);
                }
            }
        }
    }

    *dlen = d - (unsigned char*)dst;

    return 0;
}

