#include <openssl/md5.h>
#include <string.h>
#include <stdio.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>

void genMD5(unsigned char *output_md5, const char *msg)
{
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, msg, strlen(msg));
    MD5_Final(output_md5, &ctx);
}

void genSHA256(unsigned char *output_sha256, const char *msg)
{
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, msg, strlen(msg));
    SHA256_Final(output_sha256, &ctx);
}

void genSHA512(unsigned char *output_sha512, const char *msg)
{
    SHA512_CTX ctx;
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, msg, strlen(msg));
    SHA512_Final(output_sha512, &ctx);
}

void genSHA384(unsigned char *output_sha384, const char *msg)
{
    SHA512_CTX ctx;
    SHA384_Init(&ctx);
    SHA384_Update(&ctx, msg, strlen(msg));
    SHA384_Final(output_sha384, &ctx);
}
void genHMAC(unsigned char *output_hmac, unsigned int *output_hmac_length, const char *msg, const char *key, const char *engine)
{
    // engine可以为：sha256, sha512, sha384,
    const EVP_MD *md = NULL;

    if (strcasecmp("sha256", engine) == 0)
    {
        md = EVP_sha256();
    }
    else if (strcasecmp("sha384", engine) == 0)
    {
        md = EVP_sha384();
    }
    else if (strcasecmp("sha512", engine) == 0)
    {
        md = EVP_sha512();
    }

    HMAC_CTX* ctx = HMAC_CTX_new();
    HMAC_Init_ex(ctx, key, strlen(key), md, NULL);
    HMAC_Update(ctx, msg, strlen(msg));
    HMAC_Final(ctx, output_hmac, output_hmac_length);
    HMAC_CTX_free(ctx);
}

int main()
{
    // =========== md5 example ===================
    unsigned char output_md5[16]; // 128bit = 16char = 32位16进制数
    memset(output_md5, 0, sizeof(output_md5));
    const char *msg = "hello";

    genMD5(output_md5, msg);

    int i = 0;
    for (i = 0; i < 16; i++)
    {
        printf("%02X", output_md5[i]);
    }
    printf("\n");

    // =========== sha256 example ===================
    unsigned char output_sha256[32]; // 256bit = 32char = 64位16进制数
    memset(output_sha256, 0, sizeof(output_sha256));

    genSHA256(output_sha256, msg);

    for (i = 0; i < 32; i++)
    {
        printf("%02X", output_sha256[i]);
    }
    printf("\n");

    // =========== sha512 example ===================
    unsigned char output_sha512[64]; // 512bit = 64char = 128位16进制数
    memset(output_sha512, 0, sizeof(output_sha512));

    genSHA512(output_sha512, msg);

    for (i = 0; i < 64; i++)
    {
        printf("%02X", output_sha512[i]);
    }
    printf("\n");
    // =========== sha384 example ===================
    unsigned char output_sha384[48]; // 384bit = 48char = 96位16进制数
    memset(output_sha384, 0, sizeof(output_sha384));

    genSHA384(output_sha384, msg);

    for (i = 0; i < 48; i++)
    {
        printf("%02X", output_sha384[i]);
    }
    printf("\n");

    //========================== hmac example =========================
    unsigned char output_hmac[48]; // 384bit = 48char = 96位16进制数
    memset(output_hmac, 0, sizeof(output_hmac));

    unsigned int output_hmac_length = 0;
    char *key = "12345678";
    char *key2 = "helloworld";
    char *engine = "sha384";

    genHMAC(output_hmac, &output_hmac_length, msg, key, engine);

    for (i = 0; i < 48; i++)
    {
        printf("%02X", output_hmac[i]);
    }
    printf("\n");
    printf("hmac output length : %d\n", output_hmac_length);

    // ==== modify key ====
    genHMAC(output_hmac, &output_hmac_length, msg, key2, engine);

    for (i = 0; i < 48; i++)
    {
        printf("%02X", output_hmac[i]);
    }
    printf("\n");
    printf("hmac output length : %d\n", output_hmac_length);

    return 0;
}