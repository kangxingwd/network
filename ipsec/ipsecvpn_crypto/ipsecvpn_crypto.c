#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "log.h"
#include "ipsecvpn_crypto.h"
#include "gmssl/sha2.h"
#include "gmssl/hmac.h"

void hex_dump(void *data, size_t size) {
    unsigned char *p = (unsigned char *)data;
    size_t i, j;
    char hex_part[50];
    char ascii_part[20];

    for (i = 0; i < size; i += 16) {
        // 初始化字符串
        hex_part[0] = '\0';
        ascii_part[0] = '\0';

        int count = 0;
        // 处理十六进制部分
        for (j = i; j < i + 16 && j < size; j++) {
            char temp[5];
            sprintf(temp, "%02x ", p[j]);
            strcat(hex_part, temp);
            count++;
            if (count == 8) {
                strcat(hex_part, " ");
                count = 0;
            }
        }
        for (; j < i + 16; j++)
            strcat(hex_part, "   ");

        // 处理 ASCII 部分
        for (j = i; j < i + 16 && j < size; j++) {
            ascii_part[j - i] = isprint(p[j])? p[j] : '.';
        }
        ascii_part[j - i] = '\0';

        // 输出整行
        LOG_DEBUG("%08lx: %s    %s\n", (unsigned long)i, hex_part, ascii_part);
    }
}

void hex_dump_line(void *data, size_t size) {
    unsigned char *p = (unsigned char *)data;
    char hex_str[size * 2 + 1];
    hex_str[size * 2] = '\0';

    for (size_t i = 0; i < size; i++) {
        sprintf(&hex_str[i * 2], "%02x", p[i]);
    }

    LOG_DEBUG("%s\n", hex_str);
}

int padding_len(int enc_method, size_t data_len) 
{   
    int padding = 0;
    int block_len = 16;

    switch (enc_method)
    {
    case ENC_AES_128_CBC:
    case ENC_AES_192_CBC:
    case ENC_AES_256_CBC:
        block_len = AES_BLOCK_LEN;
        break;

    default:
        break;
    }

    padding = block_len - data_len % block_len;
    return padding;
}

int ipsecvpn_crypto_init(struct ipsecvpn_crypto_s* ipsecvpn_crypto, int enc_method, char auth_method, const uint8_t *key, size_t keylen)
{
    return 0;
}

int aes_cbc_enc(uint8_t *in, size_t inlen, uint8_t *out, size_t *outlen, uint8_t *key, size_t keylen, uint8_t* iv, int padding)
{
    AES_KEY aes_key;
    int ret = 0;

    if (keylen != AES128_KEN_LEN && keylen != AES192_KEN_LEN && keylen != AES256_KEN_LEN) {
        LOG_DEBUG("aes_set_encrypt: keylen error keylen: %u, AES128_KEN_LEN: %u\n", keylen, AES128_KEN_LEN);
        return 1;
    }

    aes_set_encrypt_key(&aes_key, key, keylen);

    if (padding) {
        aes_cbc_padding_encrypt(&aes_key, iv, in, inlen, out, outlen);
    } else {
        aes_cbc_encrypt(&aes_key, iv, in, inlen/16, out);
        *outlen = inlen - inlen % 16;
    }
    return 0;
}

int aes_cbc_dec(uint8_t *in, size_t inlen, uint8_t *out, size_t *outlen, uint8_t *key, size_t keylen, uint8_t* iv, int padding)
{
    AES_KEY aes_key;
    int ret = 0;

    if (keylen != AES128_KEN_LEN && keylen != AES192_KEN_LEN && keylen != AES256_KEN_LEN) {
        LOG_DEBUG("aes_set_encrypt: keylen error keylen: %u\n", keylen);
        return 1;
    }

    aes_set_decrypt_key(&aes_key, key, keylen);
    if (padding) {
        aes_cbc_padding_decrypt(&aes_key, iv, in, inlen, out, outlen);
    } else {
        aes_cbc_decrypt(&aes_key, iv, in, inlen/16, out);
        *outlen = inlen - inlen % 16;
    }
    return 0;
}

int ipsecvpn_crypto_enc(int enc_method, uint8_t *in, size_t inlen, uint8_t *out, size_t *outlen, uint8_t *key, size_t keylen, uint8_t* iv, int padding)
{

    switch (enc_method)
    {
    case ENC_AES_128_CBC:
    case ENC_AES_192_CBC:
    case ENC_AES_256_CBC:
        return aes_cbc_enc(in, inlen, out, outlen, key, keylen, iv, padding);
        break;

    default:
        break;
    }

    return 0;
}


int ipsecvpn_crypto_dec(int enc_method, uint8_t *in, size_t inlen, uint8_t *out, size_t *outlen, uint8_t *key, size_t keylen, uint8_t* iv, int padding)
{

    switch (enc_method)
    {
    case ENC_AES_128_CBC:
    case ENC_AES_192_CBC:
    case ENC_AES_256_CBC:
        return aes_cbc_dec(in, inlen, out, outlen, key, keylen, iv, padding);
        break;

    default:
        break;
    }

    return 0;
}

int auth_len(int auth_method)
{
    switch (auth_method)
    {
    case AUTH_SHA256:
        return SHA256_DIGEST_SIZE;
    case AUTH_SHA512:
        return SHA512_DIGEST_SIZE;
    default:
        break;
    }

    return 0;
}

int ipsecvpn_crypto_auth(int auth_method, uint8_t *data, size_t datalen, uint8_t *key, size_t keylen, uint8_t *authdata, size_t *authdata_len)
{
    HMAC_CTX ctx;
    const DIGEST *digest = NULL;

    switch (auth_method)
    {
    case AUTH_SHA256:
        digest = DIGEST_sha256();
        break;
    case AUTH_SHA512:
        digest = DIGEST_sha512();
        break;
    default:
        break;
    }

    hmac_init(&ctx, digest, key, keylen);
	hmac_update(&ctx, data, datalen);
	hmac_finish(&ctx, authdata, authdata_len);

    if (*authdata_len !=  auth_len(auth_method)) {
		LOG_DEBUG_S("ipsecvpn_crypto_auth failed!\n");
		return 1;
	}
    return 0;
}
