#include "list.h"
#include <stdlib.h>

typedef struct node {
    void* data;
    struct node* next;
    struct node* prev;
} node_t;

struct list {
    node_t* head;
    node_t* tail;
    node_t* current;  // for iterator
    size_t size;
};

list_t* list_create() {
    list_t* list = malloc(sizeof(list_t));
    if (list == NULL) {
        return NULL;
    }
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->size = 0;
    return list;
}

void list_destroy(list_t* list) {
    if (list == NULL) {
        return;
    }
    node_t* current = list->head;
    while (current != NULL) {
        node_t* next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

int list_get_length(list_t* list)
{
    if(!list) return 0;
    return list->size;
}

void list_append(list_t* list, void* data) {
    if (list == NULL) {
        return;
    }
    node_t* new_node = malloc(sizeof(node_t));
    if (new_node == NULL) {
        return; // or handle error
    }
    new_node->data = data;
    new_node->next = NULL;
    new_node->prev = list->tail;
    if (list->tail != NULL) {
        list->tail->next = new_node;
    } else {
        list->head = new_node;
    }
    list->tail = new_node;
    list->size++;
}

int list_delete(list_t* list, void* data) {
    if (list == NULL || list->head == NULL) {
        return 0;
    }
    node_t* current = list->head;
    while (current != NULL) {
        if (current->data == data) {
            if (current->prev != NULL) {
                current->prev->next = current->next;
            } else {
                list->head = current->next;
            }
            if (current->next != NULL) {
                current->next->prev = current->prev;
            } else {
                list->tail = current->prev;
            }
            if (list->current == current) {
                list->current = current->next;
            }
            free(current);
            list->size--;
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void list_reset_iterator(list_t* list) {
    if (list != NULL) {
        list->current = list->head;
    }
}

int list_has_next(list_t* list) {
    return list != NULL && list->current != NULL;
}

void* list_next(list_t* list) {
    if (list == NULL || list->current == NULL) {
        return NULL;
    }
    void* data = list->current->data;
    list->current = list->current->next;
    return data;
}

void* list_current(list_t* list) {
    if (list == NULL || list->current == NULL) {
        return NULL;
    }
    return list->current->data;
}

void* list_last(list_t* list) {
    if (list == NULL || list->tail == NULL) {
        return NULL;
    }
    return list->tail->data;
}


int list_delete_current(list_t* list) {
    if (list == NULL || list->current == NULL) {
        return 0;
    }
    node_t* to_delete = list->current;
    if (to_delete->prev != NULL) {
        to_delete->prev->next = to_delete->next;
    } else {
        list->head = to_delete->next;
    }
    if (to_delete->next != NULL) {
        to_delete->next->prev = to_delete->prev;
    } else {
        list->tail = to_delete->prev;
    }
    list->current = to_delete->next;
    free(to_delete);
    list->size--;
    return 1;
}