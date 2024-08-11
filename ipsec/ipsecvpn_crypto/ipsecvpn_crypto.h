#ifndef _IPSECVPN_CRYPTO_H_
#define _IPSECVPN_CRYPTO_H_

#include <stdint.h>
#include "gmssl/aes.h"

#define AES128_KEN_LEN AES128_KEY_SIZE
#define AES192_KEN_LEN AES192_KEY_SIZE
#define AES256_KEN_LEN AES256_KEY_SIZE
#define AES_BLOCK_LEN AES_BLOCK_SIZE

enum {
    ENC_AES_128_CBC =   1,
    ENC_AES_192_CBC,
    ENC_AES_256_CBC,
    ENC_MAX
};

enum {
    AUTH_SHA256 = 1,
    AUTH_SHA512,
    AUTH_MAX
};

struct ipsecvpn_crypto_s
{
    uint8_t enc_method;
    uint8_t auth_mothed;
    AES_KEY aes_key;

    int (*set_enc_key)(struct ipsecvpn_crypto_s*, const uint8_t *, size_t);
    int (*enc)(struct ipsecvpn_crypto_s*, const uint8_t iv[16], const uint8_t *, size_t, uint8_t *, size_t *);
    int (*dec)(struct ipsecvpn_crypto_s*, const uint8_t iv[16], const uint8_t *, size_t, uint8_t *, size_t *);
};

int ipsecvpn_crypto_init(struct ipsecvpn_crypto_s* ipsecvpn_crypto, int enc_method, char auth_method, const uint8_t *key, size_t keylen);
void hex_dump(void *data, size_t size);
void hex_dump_line(void *data, size_t size);


int ipsecvpn_crypto_enc(int enc_method, uint8_t *in, size_t inlen, uint8_t *out, size_t *outlen, uint8_t *key, size_t keylen, uint8_t* iv, int padding);
int ipsecvpn_crypto_dec(int enc_method, uint8_t *in, size_t inlen, uint8_t *out, size_t *outlen, uint8_t *key, size_t keylen, uint8_t* iv, int padding);
int ipsecvpn_crypto_auth(int auth_method, uint8_t *data, size_t datalen, uint8_t *key, size_t keylen, uint8_t *authdata, size_t *authdata_len);

#endif

