#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashTable.h"

int main(void){
    
    char command[80];
    HashTablePTR table = NULL;
    int returnValue;
    char** data = NULL;
    unsigned long numOfData = 0;
    char r;
    FILE * read = stdin;
    
    do{
    fprintf(stdout, "Read from standard in? (y or n): ");
    fscanf(read, " %c", &r);
    }while(r != 'y' && r != 'n');

    if(r == 'n'){
        do{
            read = stdin;
            char file[30];
            
            fprintf(stdout, "\nIf you changed your mind, cuz you can't decide for life... type in, IgnoreMe\n");

            fprintf(stdout, "Enter text file name (with extension): ");
            fscanf(read, "%s", file);
            if(!strcmp(file, "IgnoreMe")){
                break;
            }
    
            read = fopen(file, "r");
            if(read == NULL){
                fprintf(stdout, "Stop wasting my time. Don't give me a file that doesn't exist. Make sure the file is in the same directory and try again kiddo.\n");
    
            }

        }while(read == NULL);

    }



    fprintf(stdout, "Command: ");
    fscanf(read, "%s", command);

    while(strcmp(command, "quit")){
        
        if(!strcmp(command, "create")){
            unsigned int size;
            fprintf(stdout, "Size: ");
            fscanf(read, "%u", &size);

            returnValue = CreateHashTable(&table, size);
            if(returnValue == 0){
                fprintf(stdout, "Successfully created a hash table.\n");
            }else if(returnValue == -1){
                fprintf(stdout, "Insufficient memory. Give your computer to your grandpa.\n");
            }else{
                fprintf(stdout, "Go fix your program!\n");
            }

        }else if(!strcmp(command, "destroy")){
            
            returnValue = DestroyHashTable(&table);

            if(returnValue == 0){
                fprintf(stdout, "Successfully destroyed hashtable.\n");

            }else if(returnValue == -1){
                fprintf(stdout, "Doesn't point to hash table.\n");
            }else{
                fprintf(stdout, "Go fix your program!\n");
            }

        }else if(!strcmp(command, "insert")){

            char value[80];
            void *prevData = NULL;

            fprintf(stdout,"Key: ");
            fscanf(read, "%s", command);
            fprintf(stdout, "Value: ");
            fscanf(read, "%s", value);

            data = realloc(data, sizeof(char*)*(numOfData+1));
            data[numOfData] = malloc(strlen(value)+1);
            strcpy(data[numOfData], value);
            
            returnValue = InsertEntry(table, command, data[numOfData++], &prevData);

            if(returnValue == 0){
                fprintf(stdout, "Inserted in a new bucket\n");
            }else if(returnValue == 2){
                fprintf(stdout, "Key already exists\n");
                fprintf(stdout, "Old data is %s\n", (char *)prevData);
            }else if(returnValue == 1){
                fprintf(stdout, "Hash collision\n");
            }else if(returnValue == -1){
                fprintf(stdout, "Doesn't point to hash table\n");
            }else if(returnValue == -2){
                fprintf(stdout, "Insufficient memory. Give your computer to your grandma\n");
            }else{
                fprintf(stdout, "Go fix your program\n");
            }
        }else if(!strcmp(command, "delete")){

            void *prevData = NULL;

            fprintf(stdout,"Key: ");
            fscanf(read, "%s", command);

            returnValue = DeleteEntry(table, command, &prevData);

            if(returnValue == 0){
                fprintf(stdout, "Key deleted\n");
            }else if(returnValue == -2){
                fprintf(stdout, "Key is not found\n");
            }else if(returnValue == -1){
                fprintf(stdout, "Doesn't point to hash table\n");
            }else{
                fprintf(stdout, "Go fix your program\n");
            }
        }else if(!strcmp(command, "find")){
            void *found = NULL;
            
            fprintf(stdout,"Key: ");
            fscanf(read, "%s", command);

            returnValue = FindEntry(table, command, &found);

            if(returnValue == 0){
                fprintf(stdout, "Value is: %s\n", (char*)found);
            }else if(returnValue == -2){
                fprintf(stdout, "Key is not found!\n");
            }else if(returnValue == -1){
                fprintf(stdout, "Doesn't point to hash table\n");
            }else{
                fprintf(stdout, "Go fix your program\n");
            }

        }else if(!strcmp(command, "print")){
            char **array;
            unsigned int count = 0;
            returnValue = GetKeys(table, &array, &count);

            if(returnValue == 0){
                fprintf(stdout, "\nKeys are:\n");
                void *value;

                for(int i=0; i<count; i++){
                    fprintf(stdout, "%s: ", array[i]);
                    FindEntry(table, array[i], &value); 
                    fprintf(stdout, "%s\n", (char *) value);
                    free(array[i]);
                }
                fprintf(stdout, "\n");
                free(array);
            }else if(returnValue == -2){
                fprintf(stdout, "Insufficient memory to get keys\n");
            }else if(returnValue == -1){
                fprintf(stdout, "Doesn't point to hash table\n");
            }else{
                fprintf(stdout, "Go fix your program\n");
            }
            
        }else if(!strcmp(command, "get")){
            HashTableInfo info;
            returnValue = GetHashTableInfo(table, &info);

            if(returnValue == 0){

                fprintf(stdout, "\nBucket Count: %d\n", info.bucketCount);
                fprintf(stdout, "Load Factor: %f\n", info.loadFactor);
                fprintf(stdout, "Use Factor: %f\n", info.useFactor);
                fprintf(stdout, "Largest bucket size: %u\n", info.largestBucketSize);
                fprintf(stdout, "Dynamic Behaviour: %d\n", info.dynamicBehaviour);
                fprintf(stdout, "Expand Use factor: %f\n", info.expandUseFactor);
                fprintf(stdout, "Contract Use factor: %f\n\n", info.contractUseFactor);
            }else if(returnValue == -1){
                fprintf(stdout, "Doesn't point to hash table\n");
            }else{
                fprintf(stdout, "Go fix your program\n");
            }

        }else if(!strcmp(command, "set")){
            int dynamic;
            float expand, contract;
            fprintf(stdout, "Shoud it expand? (0:No, 1:Yes): ");
            fscanf(read, "%d", &dynamic);
            fprintf(stdout, "Expand use factor: ");
            fscanf(read, "%f", &expand);
            fprintf(stdout, "Contract use factor: ");
            fscanf(read, "%f", &contract);

            returnValue = SetResizeBehaviour(table, dynamic, expand, contract);
            if(returnValue == 0){
                fprintf(stdout, "Values have been updated\n");
            }else if(returnValue == 1){
                fprintf(stdout, "Expand use factor is not greater than contract use factor\n");
            }else if(returnValue == -1){
                fprintf(stdout, "Doesn't point to hash table\n");
            }else{
                fprintf(stdout, "Go fix your program\n");
            }
        }

        fprintf(stdout, "Command: ");
        fscanf(read, "%s", command);
    }

    for(int i=0;i<numOfData; i++){
        free(data[i]);
    }
    free(data);
	DestroyHashTable(&table);
}
