#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <time.h>
#include <sys/stat.h>

/*
 * File info stored in cache
 */
typedef struct FileInfo {
    char   *pstrFilePath;
    size_t lfileSize;
    time_t accessTime;
    // commented to simplify 
    //time_t creationTime;
    //ino_t  inodeNumber;
} FileInfo;

/*
 * Doubly link list to maintain the cache and usage order
 * frequently accessed at the front and least recently at end
 */
typedef struct CacheNode {
    char* key;
    FileInfo *value;
    struct CacheNode* prev;
    struct CacheNode* next;
    struct CacheNode* hnext; // chain of items for hash collisions
} CacheNode;

/*
 * LRU Cache
 */
typedef struct LRUCache {
    size_t lcapacity;
    size_t lsize;
    struct CacheNode* head;
    struct CacheNode* tail;
    struct CacheNode** ppHashTable; 
} LRUCache;

LRUCache* InitCache(size_t capacity);
void ClearCache(LRUCache* cache);
void Put(LRUCache* cache, const char* key, FileInfo value);
FileInfo* Get(LRUCache* cache, const char* key);

#endif // LRUCACHE_H

// This is a simple LRU Cache implementation in C.
// It uses a doubly linked list to maintain the order of usage
// and a hash table for fast access to the nodes.
// The cache has a fixed capacity, and when it reaches that capacity,
// the least recently used item is removed to make space for a new one.
// The cache is initialized with a specified capacity, and items can be added
// and retrieved using the Put and Get functions.
// The cache is cleared using the ClearCache function, which frees all memory
// associated with the cache and its items.

// To build the library, you can use the following command:
// gcc -fPIC -c lrucache.c -o lrucache.o
// gcc -shared -o liblrucache.so lrucache.o
// To use the library, you can link it with your application using:
// gcc -o lruapp main.c -L. -llrucache
// run  ./lruapp