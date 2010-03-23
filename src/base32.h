#ifndef BASE32_H__
#define BASE32_H__

/*
 * Base32 编码将 5 个字节编码为 8 个字符:
 *  Low Addr                           High Addr
 * LSB    MSB                          LSB    MSB
 * 	01234567 01234567 01234567 01234567 01234567
 * 						||
 * 						\/
 * Low Addr                                 High Addr
 * LSB MSB                                   LSB MSB
 * 	01234 56701 23456 70123 45670 12345 67012 34567
 *
 * 非 5 整倍数长度的剩余字节串需要用 = 补齐, 规则如下:
 *  1 byte - 01234567 => 01234 567-- ===== ===== ===== ===== ===== =====
 *  2 bytes - 01234567 01234567 => 01234 56701 23456 7---- ===== ===== ===== =====
 *  3 bytes - 01234567 01234567 01234567 => 01234 56701 23456 70123 4567- ===== ===== =====
 *  4 bytes - 01234567 01234567 01234567 01234567 => 01234 56701 23456 70123 45670 12345 67--- =====
 *
 * 参考: http://www.faqs.org/rfcs/rfc3548.html
 * 	*/

#define base32_encoded_length(len) ((((len)+4)/5)*8)
#define base32_decoded_length(len) ((((len)+7)/8)*5)

void encode_base32(int slen, const char *src, int *dlen, char *dst);

int decode_base32(int slen, const char *src, int *dlen, char *dst);

#endif

/* vi:ts=4 sw=4 */

