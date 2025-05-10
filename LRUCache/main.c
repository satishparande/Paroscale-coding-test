#include <stdio.h>
#include "lrucache.h"

int main() {
    LRUCache* cache = InitCache(3);

    FileInfo file1 = { "/dir1/f1", 100, time(NULL)};
    FileInfo file2 = { "/dir2/f2", 200, time(NULL)};
    FileInfo file3 = { "/dir3/f3", 300, time(NULL)};
    FileInfo file4 = { "/dir4/f4", 400, time(NULL)};

    Put(cache, file1.pstrFilePath, file1);
    Put(cache, file2.pstrFilePath, file2);
    Put(cache, file3.pstrFilePath, file3);
    Put(cache, file4.pstrFilePath, file4);

    FileInfo* f = Get(cache, "/dir1/f1");
    printf("File /dir1/f1: %s\n", f ? "Found" : "Not Found");
    f = Get(cache, "/dir4/f4");
    if (f) printf("File /dir4/f4 size: %zu\n", f->lfileSize);
    
    clear(cache);
    return 0;
}
