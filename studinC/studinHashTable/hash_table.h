// hash_table.h
#ifndef HASH_TABLE
#define HASH_TABLE

#include <stddef.h>
#include <stdbool.h>
// --------------------
// Hash Table Item
// --------------------
typedef struct {
    char *key;
    char *value;
} ht_item;

// --------------------
// Hash Table Structure
// --------------------
typedef struct {
    int size;           // Current size of the hash table (number of buckets)
    int count;          // Number of items stored
    ht_item **items;    // Array of pointers to items
    int base_size;      // Base size for resizing
} ht_hash_table;

// --------------------
// Hash Table Constants
// --------------------
#define HT_PRIME_1 161
#define HT_PRIME_2 163
#define HT_INITIAL_BASE_SIZE 53

// --------------------
// Hash Table API
// --------------------

// Create a new hash table with default size
ht_hash_table *ht_new();

// Create a new hash table with a specific base size
ht_hash_table *ht_new_sized(const int base_size);

// Insert a key-value pair into the hash table
void ht_insert(ht_hash_table *ht, const char *key, const char *value);

// Search for a value by key in the hash table
char *ht_search(ht_hash_table *ht, const char *key);

// Delete a key-value pair from the hash table
void ht_delete_key_value(ht_hash_table *ht, const char *key);

// Resize the hash table to a new base size
void ht_resize(ht_hash_table *ht, const int base_size);

// Delete the entire hash table and free memory
void ht_delete_hash_table(ht_hash_table *ht);

// Print the hash table (for debugging)
void ht_print(ht_hash_table *ht);


void ht_resize_up_or_down(ht_hash_table *ht, const int base_size, bool upsize);

// --------------------
// Hash Table Item API
// --------------------

// Create a new hash table item
ht_item *ht_new_item(const char *k, const char *v);

// Free a hash table item
void ht_delete_item(ht_item *i);

// --------------------
// Hash Functions
// --------------------

// Hash function for a string with a given prime and number of buckets
int hash_function(const char *s, const int a, const int num_buckets);

// Double hashing: get hash for a given attempt
int ht_get_hash(const char *s, const int num_buckets, const int attempt);

#endif