#include "base32.h"

/* 实现参考 src/core/ngx_string.c 中的 ngx_(encode|decode)_base64() 例程 */

/**
 * 将给定字符串转换成对应的 Base32 编码形式. 目标字符串必须保证有充足的空间容纳编码后的数据.
 * 可以用宏 base32_encoded_length() 预估编码后数据长度并预先为目标字符串分配空间.
 * <code>
 * 	char *src, *dst;
 * 	int slen, dlen;
 * 	slen = sizeof("hello") - 1;
 * 	src = (char*) "hello";
 * 	dst = malloc(base32_encoded_length(slen));
 * 	encode_base32(slen, src, &(dlen), dst);
 * </code>
 * @param slen 源数据串长度.
 * @param src 原数据串指针.
 * @param dlen 目标数据串长度指针, 保存 Base32 编码后数据长度.
 * @param dst 目标数据串指针, 保存 Base32 编码后数据.
 * */
void
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
		/*
		 * According to RFC 3548, The layout for input data is:
		 *
		 *  Lower Addr --------------------> Higher Addr
		 * 	msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb
		 * 	76543210 76543210 76543210 76543210 76543210
		 *
		 * After segmenting:
		 *
		 * 	Lower Addr -----------------------------------------------> Higher Addr
		 * 	msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb
		 * 	---76543 ---21076 ---54321 ---07654 ---32107 ---65432 ---10765 ---43210
		 *
		 * */
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

	/* 处理非 5 整倍数的剩余字节串 */
	/**
	 * Remain 1 byte:
	 *
	 * 	Lower Addr -----------------------------------------------> Higher Addr
	 * 	msb  lsb msb  lsb padding0 padding1 padding2 padding3 padding4 padding5
	 * 	---76543 ---210-- ======== ======== ======== ======== ======== ========
	 *
	 * Remain 2 bytes:
	 *
	 * 	Lower Addr -----------------------------------------------> Higher Addr
	 * 	msb  lsb msb  lsb msb  lsb msb  lsb padding0 padding1 padding2 padding3
	 * 	---76543 ---21076 ---54321 ---0---- ======== ======== ======== ========
	 *
	 * Remain 3 bytes:
	 *
	 * 	Lower Addr -----------------------------------------------> Higher Addr
	 * 	msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb padding0 padding1 padding2
	 * 	---76543 ---21076 ---54321 ---07654 ---3210- ======== ======== ========
	 *
	 * Remain 4 bytes:
	 *
	 * 	Lower Addr -----------------------------------------------> Higher Addr
	 * 	msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb padding0
	 * 	---76543 ---21076 ---54321 ---07654 ---32107 ---65432 ---10--- ========
	 *
	 **/
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


/**
 * 将给定的 Base32 编码(大小写不敏感)数据转换成对应的原始字符串. 目标字符串必须保证有充足的空间容纳解码后的数据.
 * 可以用宏 base32_decoded_length() 预估解码后数据长度并预先为目标字符串分配空间.
 * <code>
 * 	char *src, *dst;
 * 	int slen, dlen;
 * 	slen = sizeof("6RRU6H5CI3======") - 1;
 * 	src = (char*)"6RRU6H5CI3======";
 * 	dst = malloc(base32_decoded_length(slen));
 * 	if (!decode_base32(slen, src, &(dlen), dst)) {
 * 		// do something with dst
 * 	} else {
 * 		// decoding error
 * 	}
 * </code>
 * @param slen 源数据串长度.
 * @param src 源数据串指针.
 * @param dlen 目标数据长度指针, 保存 Base32 解码后的数据长度.
 * @param dst 目标数据串指针, 保存 Base32 解码后的数据.
 * @retval 解码成功时返回0，失败时返回-1.
 * */
int
decode_base32(int slen, const char *src, int *dlen, char *dst)
{
	static unsigned char basis32[] = {
		77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 	/* 0 - 15 */
		77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 	/* 16 - 31 */
		77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 	/* 32 - 47 */
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 77, 77, 77, 77, 77, 77, 	/* 48 - 63 */
		77, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 	/* 64 - 79 */
		25, 26, 27, 28, 29, 30, 31, 77, 77, 77, 77, 77, 77, 77, 77, 77, 	/* 80 - 95 */
		77, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 	/* 96 - 111 */
		25, 26, 27, 28, 29, 30, 31, 77, 77, 77, 77, 77, 77, 77, 77, 77, 	/* 112 - 127 */

		77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 	/* 128 - 143 */
		77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 	/* 144 - 159 */
		77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 	/* 160 - 175 */
		77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 	/* 176 - 191 */
		77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 	/* 192 - 207 */
		77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 	/* 208 - 223 */
		77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 	/* 224 - 239 */
		77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77, 77 		/* 240 - 255 */
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
		/* Base32 编码串有效长度错误 */
		return -1;
	}

	while (len > 7) {
		/*
		 * According to RFC 3548, The layout for input data is:
		 *
		 * 	Lower Addr -----------------------------------------------> Higher Addr
		 * 	msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb
		 * 	---76543 ---21076 ---54321 ---07654 ---32107 ---65432 ---10765 ---43210
		 *
		 * After assembling:
		 *
		 *  Lower Addr --------------------> Higher Addr
		 * 	msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb
		 * 	76543210 76543210 76543210 76543210 76543210
		 *
		 * */
		*d++ = (basis32[s[0]] << 3) | ((basis32[s[1]] >> 2) & 0x07);
		*d++ = ((basis32[s[1]] & 0x03) << 6) | (basis32[s[2]] << 1) | ((basis32[s[3]] >> 4) & 1);
		*d++ = ((basis32[s[3]] & 0x0f) << 4) | ((basis32[s[4]] >> 1) & 0x0f);
		*d++ = ((basis32[s[4]] & 1) << 7) | ((basis32[s[5]] & 0x1f) << 2) | ((basis32[s[6]] >> 3) & 0x03);
		*d++ = ((basis32[s[6]] & 0x07) << 5) | (basis32[s[7]] & 0x1f);

		s += 8;
		len -= 8;
	}

	/* 处理非 8 整倍数的 Base32 编码串 */
	/**
	 * Remain 1 byte:
	 *
	 * 	Lower Addr -----------------------------------------------> Higher Addr
	 * 	msb  lsb msb  lsb padding0 padding1 padding2 padding3 padding4 padding5
	 * 	---76543 ---210-- ======== ======== ======== ======== ======== ========
	 *
	 * Remain 2 bytes:
	 *
	 * 	Lower Addr -----------------------------------------------> Higher Addr
	 * 	msb  lsb msb  lsb msb  lsb msb  lsb padding0 padding1 padding2 padding3
	 * 	---76543 ---21076 ---54321 ---0---- ======== ======== ======== ========
	 *
	 * Remain 3 bytes:
	 *
	 * 	Lower Addr -----------------------------------------------> Higher Addr
	 * 	msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb padding0 padding1 padding2
	 * 	---76543 ---21076 ---54321 ---07654 ---3210- ======== ======== ========
	 *
	 * Remain 4 bytes:
	 *
	 * 	Lower Addr -----------------------------------------------> Higher Addr
	 * 	msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb msb  lsb padding0
	 * 	---76543 ---21076 ---54321 ---07654 ---32107 ---65432 ---10--- ========
	 *
	 **/
	if (len) {
		/* 剩余 2 个字节 */
		*d++ = (basis32[s[0]] << 3) | ((basis32[s[1]] >> 2) & 0x07);

		if (len > 2) {
			/* 剩余 4 个字节 */
			*d++ = ((basis32[s[1]] & 0x03) << 6) | ((basis32[s[2]] & 0x1f) << 1) | ((basis32[s[3]] >> 4) & 1);

			if (len > 4) {
				/* 剩余 5 个字节 */
				*d++ = ((basis32[s[3]] & 0x0f) << 4) | ((basis32[s[4]] >> 1) & 0x0f);

				if (len > 5) {
					/* 剩余 7 个字节 */
					*d++ = ((basis32[s[4]] & 1) << 7) | ((basis32[s[5]] & 0x1f) << 2) | ((basis32[s[6]] >> 3) & 0x03);
				}
			}
		}
	}

	*dlen = d - (unsigned char*)dst;

	return 0;
}

/* vi:ts=4 sw=4 */

