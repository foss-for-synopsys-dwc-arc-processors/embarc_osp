#ifndef __MANIFEST_H
#define __MANIFEST_H

#include <stdint.h>

#define HASH_SIZE   32  // for SHA 256
#define KEY_SIZE    384 // for RSA 3072
#define MAX_MODULES 16


typedef struct _module_entry_t {
        uint32_t module_id;
        uint32_t module_ver;
        uint32_t module_flags; // TBD
        uint32_t size_in_bytes;
        uint32_t flash_address;
        uint32_t memory_address;
        uint32_t load_address;
        uint8_t  hash[HASH_SIZE];
} module_entry_t;

typedef struct _manifest_t {
        uint32_t version;
        uint32_t num_modules;
        uint32_t signature_algo;
        uint32_t key_size;
        uint32_t signature_size;
        module_entry_t modules[MAX_MODULES];
        uint8_t public_key[KEY_SIZE];
        uint8_t signature[KEY_SIZE];
} manifest_t;
        

#endif //_MANIFEST_H
