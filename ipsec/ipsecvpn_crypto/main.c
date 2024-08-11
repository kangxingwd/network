#include <stdio.h>
#include <string.h>

#include <gmssl/hex.h>

#include "log.h"
#include "ipsecvpn_crypto.h"

struct {
	char *key;
	char *data;
	char *hmac_sha256;
	char *hmac_sha512;
} hmac_tests[] = {
	{
		"0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b",
		"d487435e6a85f75b6a4847a5fe27ace091111",
        "5139785b2bca88726ac8e5d91b8e3769c29ff3257bc707f56d4e6b79a33f2556",
        "c86230cac84438282d612c57df19c62294d0e3bc8e692fc887b046ec3f016cb01f27c3099f568ec9308bab5b8d19d7984578fe6d4d55d362c4822d8fdde12127"
	}
};

void test_aes_128_cbc()
{   
    LOG_DEBUG_S("\n");
    LOG_DEBUG_S("########################## test_aes_128_cbc start\n");

    uint8_t in[] = "12345678901234561234567890123456";
    size_t inlen = strlen(in);
    uint8_t enc_str[1024] = "d487435e6a85f75b6a4847a5fe27ace098ca3f5581ee5453d3e89df76e29c480";

    uint8_t enc_bytes[1024] = {0};
    size_t enc_bytes_len;
    hex_to_bytes(enc_str, strlen(enc_str), enc_bytes, &enc_bytes_len);
    
    LOG_DEBUG_S("in hex:\n");
    hex_dump_line(in, inlen);
    LOG_DEBUG_S("ok enc hex:\n");
    LOG_DEBUG("%s\n", enc_str);

    uint8_t out[1024] = {0};
    size_t out_len = 0;
    uint8_t out2[1024] = {0};
    size_t out2_len = 0;

    uint8_t iv[16] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
    };
    // 0123456789abcdef
    uint8_t key128[AES128_KEN_LEN] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef
    };

    // LOG_DEBUG_S("enc, pad\n");
    // ipsecvpn_crypto_enc(ENC_AES_128_CBC, in, inlen, out, &out_len, key128, AES128_KEN_LEN, iv, 1);
    // hex_dump(out, out_len);
    // LOG_DEBUG("enc, pad, end, out_len=%u\n", out_len);

    // LOG_DEBUG_S("dec, pad\n");
    // ipsecvpn_crypto_dec(ENC_AES_128_CBC, out, out_len, out2, &out2_len, key128, AES128_KEN_LEN, iv, 1);
    // hex_dump(out2, out2_len);
    // out2[out2_len] = '\0';
    // LOG_DEBUG("dec, pad, end, out2_len=%u, out2: %s\n", out2_len, out2);

    ipsecvpn_crypto_enc(ENC_AES_128_CBC, in, inlen, out, &out_len, key128, AES128_KEN_LEN, iv, 0);
    LOG_ERROR("test_aes_128_cbc enc %s \n", (memcmp(out, enc_bytes, enc_bytes_len) != 0)? "failed":"success");
    hex_dump_line(out, out_len);
    

    ipsecvpn_crypto_dec(ENC_AES_128_CBC, out, out_len, out2, &out2_len, key128, AES128_KEN_LEN, iv, 0);
    LOG_ERROR("test_aes_128_cbc dec %s \n", (memcmp(out2, in, inlen) != 0)? "failed":"success");
    hex_dump_line(out2, out2_len);
}

void test_hmac()
{
    LOG_DEBUG_S("\n");
    LOG_DEBUG_S("########################## test_hmac start\n");

    uint8_t out[1024];
    size_t outlen;

    uint8_t ok_data[1024];
    size_t oklen;


    // int ipsecvpn_crypto_auth(int auth_method, uint8_t *data, size_t datalen, uint8_t *key, size_t keylen, uint8_t *authdata, size_t *authdata_len)
    int i;
	for (i = 0; i < sizeof(hmac_tests)/sizeof(hmac_tests[0]); i++) {
        
        hex_to_bytes(hmac_tests[i].hmac_sha256, strlen(hmac_tests[i].hmac_sha256), ok_data, &oklen);
		ipsecvpn_crypto_auth(AUTH_SHA256, hmac_tests[i].data, strlen(hmac_tests[i].data), hmac_tests[i].key, strlen(hmac_tests[i].key), out, &outlen);
        LOG_ERROR("hmac sha256 test %s \n", (memcmp(out, ok_data, oklen) != 0)? "failed":"success");

        hex_to_bytes(hmac_tests[i].hmac_sha512, strlen(hmac_tests[i].hmac_sha512), ok_data, &oklen);
        ipsecvpn_crypto_auth(AUTH_SHA512, hmac_tests[i].data, strlen(hmac_tests[i].data), hmac_tests[i].key, strlen(hmac_tests[i].key), out, &outlen);
        LOG_ERROR("hmac sha512 test %s \n", (memcmp(out, ok_data, oklen) != 0)? "failed":"success");
	}
}

int main()
{

    LOG_INIT(LOG_MODE_STDOUT, LOG_LEVEL_DEBUG, NULL);
    LOG_DEBUG_S("main start ...\n");

    test_aes_128_cbc();

    test_hmac();

    return 0;
}

