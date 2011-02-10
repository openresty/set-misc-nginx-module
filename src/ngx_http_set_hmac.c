#define DDEBUG 0
#include "ddebug.h"
#include    <ndk.h>

#include "ngx_http_set_hmac.h"
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>


/* this function's implementation is partly borrowed from
 * https://github.com/anomalizer/ngx_aws_auth */
ngx_int_t
ngx_http_set_misc_set_hmac_sha1_b64(ngx_http_request_t *r,
                                    ngx_str_t *res, ngx_http_variable_value_t *v)
{
    ngx_http_variable_value_t   *secret, *string_to_sign;
    unsigned int                md_len;
    unsigned char               md[EVP_MAX_MD_SIZE];
    const EVP_MD                *evp_md = EVP_sha1();

    secret = v;
    string_to_sign = v+1;

    dd("secret=%s, string_to_sign=%s", secret->data, string_to_sign->data);

    HMAC(evp_md, secret->data, secret->len, string_to_sign->data,
         string_to_sign->len, md, &md_len);

    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bmem = BIO_new(BIO_s_mem());
    BUF_MEM *bptr;

    b64 = BIO_push(b64, bmem);
    BIO_write(b64, md, md_len);
    if (BIO_flush(b64) !=1){
      return NGX_ERROR;
    };

    BIO_get_mem_ptr(b64, &bptr);

    ndk_palloc_re(res->data, r->pool, bptr->length);

    ngx_memcpy(res->data,
               bptr->data,
               bptr->length-1);
    res->data[bptr->length-1]='\0';
    res->len = bptr->length-1;
    BIO_free_all(b64);
    return NGX_OK;
}


