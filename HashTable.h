//Struct Definitions

typedef struct HashTableObjectTag HashTableObject;
typedef HashTableObject *HashTablePTR;
typedef struct HashTableInfoTag
{
    unsigned int bucketCount; // current number of buckets
    float loadFactor; // ( number of entries / number of buckets )
    float useFactor; // ( number of buckets with one or more entries / number of buckets )
    unsigned int largestBucketSize; // number of entries in the bucket containing the most entries
    int dynamicBehaviour; // whether or not the Hash Table will resize dynamically
    float expandUseFactor; // the value of useFactor that will trigger an expansion of the number of buckets
    float contractUseFactor; // the value of useFactor that will trigger a contraction in the number of buckets
} HashTableInfo;


int CreateHashTable( HashTablePTR *, unsigned int );
 
int DestroyHashTable( HashTablePTR * );
 
int InsertEntry( HashTablePTR , char *, void *, void **);
 
int DeleteEntry( HashTablePTR , char *, void ** );
 
int FindEntry( HashTablePTR , char *, void ** );
 
int GetKeys( HashTablePTR , char ***, unsigned int * );
 
int GetLoadFactor( HashTablePTR , float * );
	
int SetResizeBehaviour( HashTablePTR, int, float, float );

int GetHashTableInfo( HashTablePTR, HashTableInfo *);

