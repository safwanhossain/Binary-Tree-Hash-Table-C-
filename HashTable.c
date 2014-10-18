#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"HashTable.h"

struct HashObject	//The binary trees
{	struct HashObject* left;
	struct HashObject* right;
	char *key;
	void *value;
};

//Typedef Commands
typedef struct HashObject small_struct;		
typedef struct HashObject * small_struct_PTR;

struct HashTableObjectTag	//Holds everything
{
	int sentinel;
	struct HashObject** pointer_to_hash;	//Pointer to a pointer to a small_struct
	unsigned int hash_table_size;	//holds how big the hash table is. Used in hash_creator function as "value"
	unsigned int insert_del_count;	//the algebraic sum of all the inserts and deletes
	int dynamicBehaviour; // whether or not the Hash Table will resize dynamically
    	float expandUseFactor; // the value of useFactor that will trigger an expansion of the number of buckets
    	float contractUseFactor; // the value of useFactor that will trigger a contraction in the number of buckets
};

// Hash Function - The Same as Simple String Hash in Midterm 2
unsigned int hash_creator( char *string, unsigned int upper_lim )	
{	unsigned char *up = (unsigned char *)string;	
	int sum = 0;
	int value = (int)upper_lim;
	
	while ( *up != '\0' )
	{	sum += *up; 
		up += 1;
	}
	if ( (sum % value) < 0 )
		return ( (unsigned int)(-1 * (sum % value)) );
	return ( (unsigned int)(sum % value) );
}


//////////////////////////////////////////////////////////////////// The Functions from Lab 09 and other helper functions ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Creates a new Node
static small_struct_PTR NewTreeNode( char* key, void* data )		
{	small_struct_PTR newNode = malloc( sizeof(small_struct) );
	
	unsigned long key_length = strlen(key);
	newNode->key = malloc( key_length + 1 );
	strcpy( newNode->key, key);
	newNode->value = data;
	newNode->left = NULL;
	newNode->right  = NULL;

	return newNode;
}

//This is a helper function made to check if duplicate keys exist. If so, returns the address of the node where thay are located
small_struct_PTR dup_local;		//dup_local holds the address of the node where duplicates occur. It is global but is always set to NULL in the InsertEntry function where it is used
int CheckDuplicateKey( small_struct_PTR root, char* key )		
{	small_struct_PTR check = root;
	
	if( check == NULL)
		return 0;
	if( strcmp(check->key, key) == 0)
		dup_local = check;							
	
	CheckDuplicateKey(check->left, key);
	CheckDuplicateKey(check->right, key);
	return 0;
}

//Helper Function for inserting nodes 
int InsertNode( small_struct_PTR *root, char* key, void* data)
{	if (*root == NULL)				//Base Case condition 
	{	*root = NewTreeNode( key, data );
		if (*root == NULL)			//checks if enough space existed to create this node.
			return -2;
		return 0;
	}	

	small_struct_PTR check = *root;			//local var check
	
	if( (strcmp(key, check->key) < 0) && (check->left == NULL) )	//if given key is smaller than what's being compared to 
	{	small_struct_PTR newNode = NewTreeNode( key, data );
		if (newNode == NULL)
			return -2;
		check->left = newNode;
		return 0;
	}

	if( (strcmp(key, check->value) > 0) && (check->right == NULL) )	//if given key is larger than what's being compared to
	{	small_struct_PTR newNode = NewTreeNode( key, data );
		if (newNode == NULL)
			return -2;
		check->right = newNode;
		return 0;
	}

	if( strcmp(key, check->key) < 0 )					//condition for less than
		InsertNode( &(check->left), key, data);

	if( strcmp(key, check->key) > 0 )					//condition for greater than
		InsertNode( &(check->right), key, data);

	return 0;
}

// Helper function for Delete. It doesn't use recursion. Istead it walks the tree iteratively and utilizes a trailing pointer
int DeleteNode(small_struct_PTR* rootHandle, char* target, void** dataHandle)
{	small_struct_PTR* prev = NULL;	//holds the previous nodes
	int side;		//holds which side of the node the next node we are going to is located
	
	while (*rootHandle != NULL)
	{	if ( strcmp((*rootHandle)->key, target) == 0 )	//Target node found
		{	*dataHandle = (*rootHandle)->value;	//give back to data handle whats at that node
			
			if( ((*rootHandle)->left == NULL) && ((*rootHandle)->right == NULL) )		//if the node to be deleted is a leaf
			{	free( (*rootHandle)->key );
				free(*rootHandle);
				*rootHandle = NULL;
				return 0;
			}
			
			if( (*rootHandle)->left == NULL )						//if node to be deleted has only one child on the right
			{	small_struct_PTR temp = (*rootHandle)->right;
				free( (*rootHandle)->key );
				free(*rootHandle);
				*rootHandle = NULL;
				
				if ( prev == NULL )							//if the node to be deleted is the root with only child on right
				{	*rootHandle = temp;
					return 0;
				}
				if (side == 0)								//this links the contents of the deleted node to the correct side
					(*prev)->right = temp;
				if (side == 1)
					(*prev)->left = temp;

				return 0;
			}
			
			if( (*rootHandle)->right == NULL )						//if node to be deleted has only one child on the left
			{	small_struct_PTR temp = (*rootHandle)->left;
				free( (*rootHandle)->key );
				free(*rootHandle);
				*rootHandle = NULL;
				
				if ( prev == NULL )							//if the node to be deleted is the root with only child on left
				{	*rootHandle = temp;
					return 0;
				}
				if (side == 0)
					(*prev)->right = temp;
				if (side == 1)
					(*prev)->left = temp;
				
				return 0;		
			}				
		
			if( ((*rootHandle)->right != NULL) && ((*rootHandle)->left != NULL) )		//if node has two children
			{	small_struct_PTR *target_location = rootHandle;				//keeps location of where target is (because roothandle is always changing)
				small_struct_PTR *prev2 = rootHandle;					//holds the previous nodes in the forthcoming loop. For this block only
				
				rootHandle = &((*rootHandle)->left);					//go on the left side
											
				while ((*rootHandle)->right != NULL)
				{	prev2 = rootHandle;
					rootHandle = &((*rootHandle)->right);				
				}
 
				free( (*target_location)->key );		//empty out whatever was there previously
				(*target_location)->key = malloc( strlen((*rootHandle)->key) + 1);	//allocate the necessary space
				strcpy( (*target_location)->key, (*rootHandle)->key);			//copy the key in
				(*target_location)->value = (*rootHandle)->value;	//assign target_location the corresponding value. What was there before has been passed into dataHandle

				small_struct_PTR temp = (*rootHandle)->left;			//this checks whether there is anything to the left			
				
				free( (*rootHandle)->key );
				free( (*rootHandle) );
				*rootHandle = NULL;
				
				if ( temp == NULL )
					return 0;

				if ( strcmp((*prev2)->key, (temp->key)) < 0 )
					(*prev2)->right = temp;
	
				if ( strcmp((*prev2)->key, (temp->key)) > 0 )
					(*prev2)->left = temp;
	
				return 0;
			}				
		}
		prev = rootHandle;
		char* key = (*rootHandle)->key;

		if( strcmp(target, key) < 0)							//right side is 0, left side is 1
		{	rootHandle = &((*rootHandle)->left);
			side = 1;
		}
		if( strcmp(target, key) > 0)
		{	rootHandle = &((*rootHandle)->right);
			side = 0;
		}
	}
	return -1;
}

//Helper fucntion for FindItem
small_struct_PTR FindItem(small_struct_PTR root, char* target)
{	small_struct_PTR check = root;
	
	if( check == NULL )	//Base Case
		return NULL;

	if( strcmp(check->key, target) == 0 )
		return check;

	if( strcmp(target, check->key) < 0 )
		return FindItem( check->left, target );

	if( strcmp(target, check->key) > 0 )
		return FindItem( check->right, target );

	return NULL;		//just there to get past Wconversion	
}

//Helper Fucntion for delete. Utilizes a post order search
int FreeTree(small_struct_PTR* root)
{	small_struct_PTR* check = root;
	
	if( *check == NULL)	//Base Case
		return 0;

	FreeTree(&((*check)->left));
	FreeTree(&((*check)->right));
	free((*check)->key);
	free(*check);
	check = NULL;
	
	return 0;	//to get by Wconversion
}

//Get Keys Helper Fucntion
unsigned int KeysArrayCount;	//This is a global variable for the function below meant to keep count of the indicies on the keysArray. It is initilaized and reset in the GetKeys function
int GetKeysHelper(small_struct_PTR root, char ***keysArrayHandle)
{	small_struct_PTR check = root;		//just a renamed variable
	
	if( check == NULL)	//base Case
		return 0;
	
	*(*keysArrayHandle + KeysArrayCount) = malloc( strlen(check->key) + 1 );			// allocate memory for each key
	strcpy( *(*keysArrayHandle + KeysArrayCount), check->key);					//copy the key in
	KeysArrayCount++;		//increment GetKeysCount. Remember this is global so it wont lose the increment until it leaves the GetKeys function

	GetKeysHelper(check->left, keysArrayHandle);
	GetKeysHelper(check->right, keysArrayHandle);
	return 0;
}

//Helper function for LargestBucketSize
unsigned int entryCount;	//This is a global variable for the function below meant to keep count of the number of entries in each bucket. Used in LargestBucketSize function  
int BucketSizeHelper(small_struct_PTR root)
{	small_struct_PTR check = root;		//just a renamed variable
	
	if( check == NULL)
		return 0;
	
	entryCount++;		//increment entrycount
	BucketSizeHelper(check->left);
	BucketSizeHelper(check->right);
	return 0;
}

//Finds the Use factor for the hashtable. Used in numerous API fucntions
int GetUseFactor( HashTablePTR hashTable, float *UseFactor )
{	int *pointer = (int *)(hashTable);
	if (*pointer != 0xDEADBEEF)			//check if the struct given is correct
		return -1;
	
	int count;
	float nonEmptyBuckets = 0;	//holds the number of non empty buckets
	for (count = 0; count < (hashTable->hash_table_size); count++)		//walks the hash table
	{	if ( *(hashTable->pointer_to_hash + count) != NULL )
			nonEmptyBuckets += 1;	

	}
	*UseFactor = nonEmptyBuckets / ((float)hashTable->hash_table_size);
	return 0;
}

//Finds the largest Bucket (one with the most keys) Used in multiple API fucntions
int LargestBucketSize( HashTablePTR hashTable, unsigned int *entries )
{	int *pointer = (int *)(hashTable);
	if (*pointer != 0xDEADBEEF)			//check if the struct given is correct
		return -1;
	
	int count;
	unsigned int max = 0;	//holds the maximum number of entries
	for (count = 0; count < (hashTable->hash_table_size); count++)		//walks the hash table
	{	entryCount = 0;			//initializes this global var. Must be initialized before each bucket (see BucketSizeHelper )
		BucketSizeHelper(*(hashTable->pointer_to_hash + count) );
		if( entryCount > max )
			max = entryCount;
	}
	*entries = max;
	return 0;
}

//This is exactly the same as InsertEntry except that it doesn't call ResizeExpand. The ResizeExpand and ResizeContract fucntions needs to use insert. However, they can't use InsertEntry because InsertEntry itself calls ResizeExpand and the two CAN get locked into a Recursive loop that is problematic. Thus, ResizeExpand and ResizeContract calls HelperInsert Instead. 
int HelperInsert( HashTablePTR hashTable, char *key, void *data, void **previousDataHandle )
{	int *pointer = (int *)(hashTable);
	if (*pointer != 0xDEADBEEF)			//check if the struct given is correct
		return -1;

	unsigned int hash_value = hash_creator(key, hashTable->hash_table_size);			
	
	//Check if the key already exists by walking all the buckets
	int count;
	for (count = 0; count < (hashTable->hash_table_size); count++)	
	{	dup_local = NULL;
		CheckDuplicateKey( *(hashTable->pointer_to_hash + count), key );
		
		if ( dup_local != NULL )
		{	*previousDataHandle = dup_local->value;		//give the old contents to previousdatahandle
			dup_local->value = data;
			return 2;
		}		
	}
	(hashTable->insert_del_count) += 1;		//Keeps tarck of number of keys; in the above code, no new keys are created, so its placed down here
	
	if ( (*(hashTable->pointer_to_hash + hash_value) == NULL) )		//if there exits nothing there at that index	
	{	InsertNode( (hashTable->pointer_to_hash + hash_value), key, data);		
		return 0;
	}
	else								//If there is a hash collision. IE there is already something in the that bucket
	{	InsertNode( (hashTable->pointer_to_hash + hash_value), key, data);
		return 1;
	}
}

//This is the function that Checks if the table needs to expanded. If so, it expands it. It is called by InsertEntry Function
int ResizeExpand( HashTablePTR hashTable )
{	int *pointer = (int *)(hashTable);
	if (*pointer != 0xDEADBEEF)			//check if the struct given is correct
		return -1;
	
	int dynamicBehaviour = hashTable->dynamicBehaviour;	//Get the information from the hashtable struct; 
	float expandUseFactor = hashTable->expandUseFactor;

	if( dynamicBehaviour == 0 )			//check for dynamic behaviour being true
		return -2;

	float Usefactor;	//get the current use factor
	GetUseFactor( hashTable, &Usefactor );
	unsigned int newSize;						//the size of the new hash table that is to be made
	unsigned int oldSize = (hashTable->hash_table_size);		//the size of the old hash table
	
	if( Usefactor > expandUseFactor )	//check if hashtable needs to made larger				
	{	//This is a formula discussed in class. It is garunteed to expand to to the proper limit. Thus no "while" clause is needed to check if the Resizing was correct.
		double temp_fl = (( (double)(hashTable->insert_del_count) / (expandUseFactor) ) * 1.5);	// 1.5 can be thought of as a "safety" factor	
		newSize = (unsigned int)temp_fl;
	}
	else
		return -3;	//if no resizing is necessary

	unsigned int keyCount;		//number of keys in the current hashTable
	char **keys_array;		// array of all the current keys
	GetKeys( hashTable, &keys_array, &keyCount );	
	void **dataArray = malloc( sizeof(void *) * keyCount );		//array to holds the data pointers

	int count;
	for( count = 0; count < keyCount; count++)		//iterate over the keys
	{	char* key = *(keys_array + count);
		FindEntry( hashTable, key, (dataArray + count) );		//finds that data associated with the certain key and puts it in the dataArray	
	}

	for (count = 0; count < oldSize; count++)		//Free all the keys and the nodes
		FreeTree( hashTable->pointer_to_hash + count );

	free(hashTable->pointer_to_hash);	//free the old space
	hashTable->pointer_to_hash = malloc(sizeof(small_struct_PTR) * newSize);	//reallocate new space
	hashTable->hash_table_size = newSize;		//update the number of buckets
	
	for (count = 0; count < newSize; count++)
		*(hashTable->pointer_to_hash + count) = NULL;	//NULLS all the pointers

	for( count = 0; count < keyCount; count++)
	{	char* key = *(keys_array + count);
		void* prevData = NULL;				//made because HelperInsert needs it
		HelperInsert( hashTable, key, *(dataArray + count), prevData );
	}	
	
	for ( unsigned int count2 = 0 ; count2 < keyCount; count2++)		//free the keys array
		free( *(keys_array + count2) );
	
	free( keys_array);		//free key array
	free(dataArray);		//free the data array
	return 0;
}

//This is the function that Checks if the table needs to contracted. If so, it contracts it. It is called by DeleteEntry Function
int ResizeContract( HashTablePTR hashTable )
{	int *pointer = (int *)(hashTable);
	if (*pointer != 0xDEADBEEF)			//check if the struct given is correct
		return -1;
	
	int dynamicBehaviour = hashTable->dynamicBehaviour;	//Get the information in the hashtable struct; 
	float contractUseFactor = hashTable->contractUseFactor;

	if( dynamicBehaviour == 0 )			//check for dynamic behaviour being true
		return -2;

	float Usefactor;	//get the current use factor
	GetUseFactor( hashTable, &Usefactor );
	unsigned int newSize;						//the size of the new hash table
	unsigned int oldSize = (hashTable->hash_table_size);		//the size of the old hash table
	
	if( Usefactor < contractUseFactor )	//if hashtable needs to made smaller
	{	//This is a formula discussed in class. It is garunteed to expand to to the proper limit. Thus no "while" clause is needed to check if the Resizing was correct.
		double temp_fl = ( ((double)1) / contractUseFactor) / 1.5;	//1.5 can be thought as a "safety" factor	
		newSize = (unsigned int)temp_fl;
	}
	else
		return 0;	// the table doesn't need contracting
	
	//////////////////////////// After this point, Code is exactly the same as ResizeExpand /////////////////////////// 
	unsigned int keyCount;		//number of keys in the current hashTable
	char **keys_array;		// array of all the current keys
	GetKeys( hashTable, &keys_array, &keyCount );	
	void **dataArray = malloc( sizeof(void *) * keyCount );		//array to holds the data pointers

	int count;
	for( count = 0; count < keyCount; count++)		//iterate over the keys
	{	char* key = *(keys_array + count);
		FindEntry( hashTable, key, (dataArray + count) );		//finds that data associated with the certain key and puts it in the dataArray	
	}

	for (count = 0; count < oldSize; count++)		//Free all the keys and the nodes
		FreeTree( hashTable->pointer_to_hash + count );

	free(hashTable->pointer_to_hash);	//free the old space
	hashTable->pointer_to_hash = malloc(sizeof(small_struct_PTR) * newSize);	//reallocate new space
	hashTable->hash_table_size = newSize;		//update the number of buckets
	
	for (count = 0; count < newSize; count++)
		*(hashTable->pointer_to_hash + count) = NULL;	//NULLS all the pointers

	for( count = 0; count < keyCount; count++)
	{	char* key = *(keys_array + count);
		void* prevData = NULL;				//made because HelperInsert needs it
		HelperInsert( hashTable, key, *(dataArray + count), prevData );
	}	
	
	for ( unsigned int count2 = 0 ; count2 < keyCount; count2++)		//free the keys array
		free( *(keys_array + count2) );
	
	free( keys_array);		//free key array
	free(dataArray);		//free the data array
	return 0;	
}

//////////////////////////////////////////////////////////////////////////////// The API Funtions ///////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int CreateHashTable( HashTablePTR *hashTableHandle, unsigned int initialSize )		
{	*hashTableHandle = malloc( sizeof(HashTableObject) );	//CReate Hash Table	
	if ( *hashTableHandle == NULL )
		return -1;

	(*hashTableHandle)->sentinel = (int)0xDEADBEEF;		//set sentinel
	(*hashTableHandle)->pointer_to_hash = malloc( sizeof(small_struct_PTR) * initialSize);	//malloc enough pointers to buckets
	if ( ((*hashTableHandle)->pointer_to_hash ) == NULL )	//if not enough space
		return -1;
	
	(*hashTableHandle)->hash_table_size = initialSize;		//initialize hash_table_size
	(*hashTableHandle)->insert_del_count = 0;			//initialize the insert del count
	(*hashTableHandle)->dynamicBehaviour = 1;			//initialize dynamic behaviour
	(*hashTableHandle)->expandUseFactor = (float).70;			//initialize expand use factor
	(*hashTableHandle)->contractUseFactor = (float).20;			//initialize contract use factor

	int count;
	for (count = 0; count < initialSize; count++)
		*( (*hashTableHandle)->pointer_to_hash + count ) = NULL;	//NULLS all the pointers
	 
	return 0;
}

int InsertEntry( HashTablePTR hashTable, char *key, void *data, void **previousDataHandle )
{	int *pointer = (int *)(hashTable);
	if (*pointer != 0xDEADBEEF)			//check if the struct given is correct
		return -1;

	unsigned int hash_value = hash_creator(key, hashTable->hash_table_size);	//get the hash value, by hashing the key		
	
	//Check if the key already exists by walking all the binary trees
	int count;
	for (count = 0; count < (hashTable->hash_table_size); count++)	
	{	dup_local = NULL;
		CheckDuplicateKey( *(hashTable->pointer_to_hash + count), key );	//call Diplicate keys
	
		if ( dup_local != NULL )
		{	*previousDataHandle = dup_local->value;		//give the old contents to previousdatahandle
			dup_local->value = data;
			return 2;
		}		
	}
	(hashTable->insert_del_count) += 1;		//Keeps track and updates the number of keys; in the above code, no new keys are created, so its placed down here
	
	if ( (*(hashTable->pointer_to_hash + hash_value) == NULL) )		//if there exits nothing there at that index	
	{	if (-2 == InsertNode( (hashTable->pointer_to_hash + hash_value), key, data))	
			return -2;	//if not enough space
		//since new key was created, check if table needs resizing
		ResizeExpand( hashTable );
		return 0;
	}

	else								//Go through the tree located at that index until an empty position is found. Assign object to it
	{	if( -2 == InsertNode( (hashTable->pointer_to_hash + hash_value), key, data))
			return -2;	//not enough space
		//since new key was created, check if table needs resizing
		ResizeExpand( hashTable );
		return 1;
	}
}

int DeleteEntry( HashTablePTR hashTable, char *key, void **dataHandle)
{	int *pointer = (int *)(hashTable);
	if (*pointer != 0xDEADBEEF)			//check if the struct given is correct
		return -1;
	
	int count;
	for (count = 0; count < (hashTable->hash_table_size); count++)	//Check each of the buckets
	{	if ( DeleteNode( (hashTable->pointer_to_hash + count), key, dataHandle) == 0)
		{	(hashTable->insert_del_count) -= 1;		// Since something is being deleted, decrease the count

			//since a key was deleted, check if table needs resizing
			ResizeContract( hashTable );
			return 0;
		}
	}
	return -2;	//Key not found
}

int FindEntry( HashTablePTR hashTable, char *key, void **dataHandle )
{	int *pointer = (int *)(hashTable);
	if (*pointer != 0xDEADBEEF)			//check if the struct given is correct
		return -1;
	
	int count;
	for (count = 0; count < (hashTable->hash_table_size); count++)	//Check every bucket
	{	small_struct_PTR check = FindItem(*(hashTable->pointer_to_hash + count), key);
		
		if( check != NULL )
		{	*dataHandle = check->value;
			return 0;
		}
	}
	return -2;	//Key not found
}

int DestroyHashTable( HashTablePTR *hashTableHandle )
{	if (*hashTableHandle == NULL)	//check if pointer given is null. (for safety)
		return -1;

	int *pointer = (int *)(*hashTableHandle);
	if (*pointer != 0xDEADBEEF)			//check if the struct given is correct
		return -1;

	int count;
	for (count = 0; count < ((*hashTableHandle)->hash_table_size); count++)	//Free all the keys and the nodes 
		FreeTree( (*hashTableHandle)->pointer_to_hash + count );
		
	free( (*hashTableHandle)->pointer_to_hash );	//free the pointer that points to buckets. It has allocated to it all the space for the binary tree pointers
	(*hashTableHandle)->pointer_to_hash = NULL;

	free( *hashTableHandle );			//free the actual hashTable
	*hashTableHandle = NULL;

	return 0;
}

int GetKeys( HashTablePTR hashTable, char ***keysArrayHandle, unsigned int *keyCount )
{	int *pointer = (int *)(hashTable);
	if (*pointer != 0xDEADBEEF)			//check if the struct given is correct
		return -1;
	
	*keysArrayHandle = malloc( sizeof(char *) * (hashTable->insert_del_count) );	//Allocate memory for the number of keys; determined by number of inserts and deletes
	if (*keysArrayHandle == NULL )
		return -2;		//check if there is sufficient memory

	int count;	//go over all the buckets
	KeysArrayCount = 0;	//(global var) This is for the whole hashTable and not just individual trees (see GetkeysHelper fucntion)
	for (count = 0; count < (hashTable->hash_table_size); count++)
		GetKeysHelper( *(hashTable->pointer_to_hash + count), keysArrayHandle );

	*keyCount = KeysArrayCount;	
	return 0;
}

int GetLoadFactor( HashTablePTR hashTable, float *loadFactor )
{	int *pointer = (int *)(hashTable);
	if (*pointer != 0xDEADBEEF)			//check if the struct given is correct
		return -1;

	float entry_count = (float)(hashTable->insert_del_count);	//Since insert_del_count keeps track of all inserts and deletes, it holds the number of current entries 
	float bucket_size = (float)(hashTable->hash_table_size);	//Just some renaming. Number of buckets

	*loadFactor = entry_count / bucket_size;	
	return 0;
}

int SetResizeBehaviour( HashTablePTR hashTable, int dynamicBehaviour, float expandUseFactor, float contractUseFactor )		//ASSUMING WE CAN TAKE A HANDLE
{	int *pointer = (int *)(hashTable);
	if (*pointer != 0xDEADBEEF)			//check if the struct given is correct
		return -1;

	if( contractUseFactor > expandUseFactor )
		return 1;

	hashTable->dynamicBehaviour = dynamicBehaviour;	//place the information in the hashtable struct;  
	hashTable->expandUseFactor = expandUseFactor;
	hashTable->contractUseFactor = contractUseFactor;
	return 0;
}

int GetHashTableInfo( HashTablePTR hashTable, HashTableInfo *pHashTableInfo )
{	int *pointer = (int *)(hashTable);
	if (*pointer != 0xDEADBEEF)			//check if the struct given is correct
		return -1;

	pHashTableInfo->bucketCount = hashTable->hash_table_size;	//number of buckets
	GetLoadFactor( hashTable, &(pHashTableInfo->loadFactor) );	//load factor
	GetUseFactor( hashTable, &(pHashTableInfo->useFactor) );	//use Factor
	LargestBucketSize( hashTable, &(pHashTableInfo->largestBucketSize) );	//largest bucket size
	pHashTableInfo->dynamicBehaviour = hashTable->dynamicBehaviour;		//dynamic behaviour
	pHashTableInfo->expandUseFactor = hashTable->expandUseFactor;		//expand Use Factor
	pHashTableInfo->contractUseFactor = hashTable->contractUseFactor;	//contract use factor
	return 0;
}



//////////////////////////////////////////////////////////////////// End /////////////////////////////////////////////////////////////////////////////////////////////////////
