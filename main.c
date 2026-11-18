#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 2048
#define TABLE_MASK  (TABLE_SIZE - 1)

typedef struct Node { 
    char* key;
    char* value;
    struct Node* next;
} Node;

typedef struct {
    Node* buckets[TABLE_SIZE];
} HashTable;

HashTable db;

unsigned int hashString(const char* key){   
    unsigned int hash = 5381;
    int c;

    while((c = (unsigned char) *key++)){
        hash = ((hash << 5) + hash) ^ c;
    }
    return hash & TABLE_MASK;
}

void SET(const char* key, const char* value) {
    unsigned int hash = hashString(key);

    struct Node* current = db.buckets[hash]; 
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            free(current->value);
            current->value = strdup(value);
            return; 
        }
        current = current->next;
    }

    struct Node* newNode = malloc(sizeof(struct Node));
    if (newNode == NULL) {
        perror("Malloc failed");
        exit(EXIT_FAILURE);
    }

    newNode->key = strdup(key);
    newNode->value = strdup(value);
    newNode->next = NULL;

    if (!newNode->key || !newNode->value) {
        perror("Strdup failed");
        exit(EXIT_FAILURE);
    }

    if (db.buckets[hash] == NULL) {
        db.buckets[hash] = newNode;
    } else {
        struct Node* tail = db.buckets[hash];
        while (tail->next != NULL) {
            tail = tail->next;
        }
        tail->next = newNode;
    }
}

char* GET(const char* key) {
    unsigned int hash = hashString(key);
    struct Node* current = db.buckets[hash];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL; 
}

void DEL(const char* key){
    unsigned int hash = hashString(key);
    struct Node* head = db.buckets[hash];

    struct Node* dummy = malloc(sizeof(struct Node));

    if(dummy == NULL){
        perror("Malloc Failed");
        exit(EXIT_FAILURE);
    }

    dummy->next = head; 
    struct Node* left = dummy;
    struct Node* target = NULL;

    while(head != NULL){
        if(strcmp(head->key, key) == 0){
            target = head;
            left->next = head->next;
            break;
        }
        head = head->next;
        left = left->next;
    }

    db.buckets[hash] = dummy->next;

    free(dummy);

    if(target == NULL){
        printf("Key '%s' not found\n", key);
    }
    else{
        free(target->key);
        free(target->value);
        free(target);
        printf("Key '%s' successfully deleted\n", key);
    }
    
}

void shutdown_database() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        struct Node* current = db.buckets[i];
        
        while (current != NULL) {
            struct Node* next_node = current->next; 
            
            free(current->key);   
            free(current->value); 
            free(current); 
            
            current = next_node;
        }
        
        db.buckets[i] = NULL; 
    }
    printf("Database successfully shut down and all memory freed.\n");
}

int main() {
    SET("hello", "he");
    SET("world", "earth");
    SET("orld", "earh");
    
    char* val = GET("hello");
    if (val) {
        printf("GET hello: %s\n", val);
    }

    DEL("hello");

    shutdown_database();

    return 0;
}

