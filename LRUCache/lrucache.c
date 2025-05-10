#include "lrucache.h"
#include <stdlib.h>

// DJB2 hash 
static size_t hash(const char* key, size_t hashSize) {
    unsigned int hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;
    return hash % hashSize;
}

static CacheNode* CreateNode(const char* filePath, FileInfo fileInfo) {
    CacheNode* node = malloc(sizeof(CacheNode));
    node->key = strdup(filePath);
    node->value = malloc(sizeof(FileInfo));
    node->value->pstrFilePath = strdup(fileInfo.pstrFilePath);
    node->value->lfileSize = fileInfo.lfileSize;
    node->value->accessTime = fileInfo.accessTime;
    node->prev = node->next = node->hnext = NULL;
    return node;
}

/* Move a node to the front of the list
 *
 * scenarios:
 * new node: 
 *   cache empty: get added as head node
 *   non empty: prev and next of new node are null
 *              new node gets added at front 
 *              node->next = cache->head;
 *              cache->head->prev = node;
 *              cache->head = node;
 * key already exist
 *   new node at front position: do nothing
 *   new node at intermediate position
 *      adjust the prev nodes next pointer and next nodes prev pointer accordingly
 *      add node at the head (front) position with prev pointing to null and next pointing to prev head
 *   new node at tail position
 *      tail should be changed to nodes prev
 *      add this node at the head position
 */

static void MoveToFront(LRUCache* cache, CacheNode* node) {
    if (cache->head == node) 
    {
      return;
    }

    // Remove node
    if (node->prev)
    {
      node->prev->next = node->next;
    }
    if (node->next) 
    {
      node->next->prev = node->prev;
    }
    if (cache->tail == node)
    {
      cache->tail = node->prev;
    }

    // Insert at front
    node->next = cache->head;
    node->prev = NULL;
    if (cache->head)
    {
      cache->head->prev = node;
    }
    cache->head = node;
    if (!cache->tail) 
    {
      cache->tail = node;
    }
}

// Remove the oldest accessed node
static void RemoveTail(LRUCache* cache) {
    if (!cache->tail) 
    {
      return;
    }

    CacheNode* tail = cache->tail;
    size_t hashIndex = hash(tail->key, cache->lcapacity);

    // Remove from hash table
    CacheNode** ptr = &cache->ppHashTable[hashIndex];
    //tail is being removed so if collision list exist in hash table adjust it appropriately
    while (*ptr && *ptr != tail) 
    {
      ptr = &((*ptr)->hnext);
    }
    if (*ptr) 
    {
      *ptr = tail->hnext;
    }

    // Remove from list
    if (tail->prev) 
    { 
      tail->prev->next = NULL;
    }
    cache->tail = tail->prev;
    if (cache->head == tail) 
    {
      cache->head = NULL;
      cache->tail = NULL;
    }
    free(tail->key);
    free(tail);
    cache->lsize--;
}


// Create a new LRU cache
LRUCache* InitCache(size_t capacity) 
{
    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    cache->lcapacity = capacity;
    cache->lsize = 0;
    cache->head = NULL;
    cache->tail = NULL;
    cache->ppHashTable = (CacheNode**)calloc(cache->lcapacity, sizeof(CacheNode*));
    return cache;
}

// Destroy the LRU cache
void ClearCache(LRUCache* cache) 
{
    CacheNode* current = cache->head;
    while (current) 
    {
        CacheNode* temp = current;
        current = current->next;
        free(temp);
    }
    free(cache->ppHashTable);
    free(cache);
}


// Add or update file information in the cache
void Put(LRUCache* cache, const char* key, FileInfo value) 
{
    size_t hashIndex = hash(key, cache->lcapacity);
    CacheNode* node = cache->ppHashTable[hashIndex];

    // Check if the file is already present at the hash index and in the collision chain
    while (node) 
    {
        if (strcmp(node->key, key) == 0) 
        {
            // Found; update the value
            node->value->pstrFilePath = strdup(value.pstrFilePath);
            node->value->lfileSize = value.lfileSize;
            node->value->accessTime = value.accessTime;
            MoveToFront(cache, node);
            return;
        }
        node = node->hnext;
    }

    if (cache->lsize >= cache->lcapacity)
    {
        RemoveTail(cache);
    }

    CacheNode* newNode = CreateNode(key, value);
    newNode->hnext = cache->ppHashTable[hashIndex];
    cache->ppHashTable[hashIndex] = newNode;
    MoveToFront(cache, newNode);
    cache->lsize++;
}

// Get file information from the cache
FileInfo* Get(LRUCache* cache, const char* key) 
{
    size_t hashIndex = hash(key, cache->lcapacity);
    CacheNode* node = cache->ppHashTable[hashIndex];

    while (node) {
        if (strcmp(node->key, key) == 0) 
        {
            MoveToFront(cache, node);
            return node->value;
        }
        node = node->hnext;
    }
    return NULL;
}
