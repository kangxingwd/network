#ifndef __BASE_HASH_H__
#define __BASE_HASH_H__

unsigned int (*user_hash_func)(void* key, int key_len);

typedef struct hash {
    user_hash_func hash_func;
} hash_t;

hash_t hash_creat();
void hash_destroy(hash_t *h);

int hash_insert(hash_t* h, void* key, unsigned int key_len, void* val, unsigned int val_len);
void* hash_get(hash_t* h, void* key, unsigned int key_len);

void (*iterator_func)(hash_t* h, unsigned int key, void* val);
void hash_iterate(hash_t* h, );

#endif // __BASE_HASH_H__
