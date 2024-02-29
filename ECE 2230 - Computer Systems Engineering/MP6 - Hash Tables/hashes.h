/* hashes.h 
 * Michael Morris
 * mvmorri
 * Lab6: Hash Tables 
 * ECE 2230, Fall 2023
 *
 */
unsigned djb_hash(hashkey_t key);
unsigned djb_hash_xor(hashkey_t key);
unsigned sax_hash(hashkey_t key);
unsigned fnv_hash(hashkey_t key);
unsigned oat_hash(hashkey_t key);
unsigned jsw_hash(hashkey_t key);
unsigned elf_hash(hashkey_t key);
unsigned jen_hash(hashkey_t key);

