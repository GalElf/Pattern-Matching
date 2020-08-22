#include <stdio.h>
#include "slist.h"

// initialize a single list
void slist_init(slist_t *list)
{
    if (list == NULL)
    {
        return;
    }
    slist_head(list) = NULL;
    slist_tail(list) = NULL;
    slist_size(list) = 0;
}

// destroy and de-allocate the memory hold by the list
void slist_destroy(slist_t *list, slist_destroy_t flag)
{
    if (list == NULL)
    {
        return;
    }
    if (slist_head(list) == NULL)
    {
        return;
    }
    slist_node_t *temp;
    while (slist_head(list) != NULL)
    {
        temp = slist_head(list);
        slist_head(list) = slist_next(slist_head(list));
        if (flag == SLIST_FREE_DATA)
        { // free the data that was allocate
            free(slist_data(temp));
        }
        free(temp);
    }
}

// add data to the end of list
int slist_append(slist_t *list, void *data)
{
    if (list == NULL || data == NULL)
    {
        return -1;
    }
    slist_node_t *node = (slist_node_t *)malloc(sizeof(slist_node_t));
    if (node == NULL)
    {
        printf("node allocation was failed");
        return -1;
    }
    slist_data(node) = data;
    slist_next(node) = NULL;
    if (slist_size(list) == 0)
    { // check if the list is empty
        slist_size(list)++;
        slist_head(list) = node;
        slist_tail(list) = node;
        return 0;
    }
    slist_size(list)++;
    slist_next(slist_tail(list)) = node;
    slist_tail(list) = node;
    return 0;
}

// // add data to the start of list
int slist_prepend(slist_t *list, void *data)
{
    if (list == NULL || data == NULL)
    {
        return -1;
    }
    slist_node_t *node = (slist_node_t *)malloc(sizeof(slist_node_t));
    if (node == NULL)
    {
        printf("node allocation was failed");
        return -1;
    }
    slist_data(node) = data;
    slist_next(node) = NULL;
    if (slist_size(list) == 0)
    { // check if the list is empty
        slist_size(list)++;
        slist_head(list) = node;
        slist_tail(list) = node;
        return 0;
    }
    slist_size(list)++;
    slist_next(node) = slist_head(list);
    slist_head(list) = node;
    return 0;
}

// pop the first element in the list and return hid data
void *slist_pop_first(slist_t *list)
{
    if (slist_head(list) == NULL)
    {
        return NULL;
    }
    void *data;
    if (slist_size(list) == 1)
    { // check if there is only one node in the list
        slist_size(list)--;
        data = slist_data(slist_head(list));
        free(slist_head(list));
        slist_head(list) = NULL;
        slist_tail(list) = NULL;
        return data;
    }
    slist_size(list)--;
    data = slist_data(slist_head(list));
    slist_node_t *temp = slist_head(list);
    slist_head(list) = slist_next(slist_head(list));
    free(temp);
    return data;
}

// append elements from the second list to the first list
int slist_append_list(slist_t *list1, slist_t *list2)
{
    if (slist_head(list1) != NULL && slist_head(list2) == NULL)
    { // check if both list are not exists
        return 0;
    }
    else if (slist_head(list1) == NULL && slist_head(list2) == NULL)
    { // check if list2 are not exist
        return 0;
    }
    else if (slist_head(list1) == NULL)
    { // if list1 not initialize it will initialize list1 and then append list2 into list1
        slist_init(list1);
    }
    int i;
    slist_node_t *temp = slist_head(list2);
    for (i = 0; i < slist_size(list2); i++)
    {
        int checkAppend = slist_append(list1, slist_data(temp));
        if (checkAppend == -1)
            return -1;
        temp = slist_next(temp);
    }
    return 0;
}