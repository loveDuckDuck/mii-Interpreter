// hash_table.h
#ifndef HASH_TABLE
#define HASH_TABLE

typedef struct
{
    char *key;
    char *value;
} ht_item;

// hash_table.h
typedef struct
{
    int size;
    int count;
    ht_item **items;
} ht_hash_table;

#define HT_PRIME_1 161
#define HT_PRIME_2 163

// hash_table.h
int hash_function(const char *s, const int a, const int num_buckets);
void ht_insert(ht_hash_table *ht, const char *key, const char *value);
char *ht_search(ht_hash_table *ht, const char *key);
void ht_delete(ht_hash_table *h, const char *key);
#endif