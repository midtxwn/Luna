#ifndef LIST_H
#define LIST_H

typedef struct list list_t;

// Create a new empty list
list_t* list_create();

// Destroy the list and free all memory (LOS ELEMENTOS DEBEN DE SER LIBERADOS MANUALMENTE)
void list_destroy(list_t* list);

// Insert a new element at the end of the list
void list_append(list_t* list, void* data);

// Delete the first occurrence of data in the list
// Returns 1 if deleted, 0 if not found
int list_delete(list_t* list, void* data);

// Reset the iterator to the beginning
void list_reset_iterator(list_t* list);

// Check if there is a next element
int list_has_next(list_t* list);

//Return lenght of list
int list_get_length(list_t* list);

// Get the next element and advance the iterator
void* list_next(list_t* list);

// Get the current element without advancing
void* list_current(list_t* list);

// Delete the current element and advance to the next
// Returns 1 if deleted, 0 if no current element
int list_delete_current(list_t* list);

//Returns last element of the list, in case it exists
void* list_last(list_t* list);

#endif // LIST_H