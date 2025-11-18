// main.c
#include "hash_table.h"
#include <stdio.h>


int main() {
    ht_hash_table* ht = ht_new();
    for(int i = 0; i < 100; i++) {
        char key[10];
        char value[10];
        snprintf(key, sizeof(key), "key%d", i);
        snprintf(value, sizeof(value), "value%d", i);
        ht_insert(ht, key, value);
    }
    ht_print(ht);
    ht_delete_hash_table(ht);
}