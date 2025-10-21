#include "dList.h"

struct dList *dListCreateList()
{

    struct dList *node = (struct dList *)malloc(sizeof(struct dList));
    if (node == NULL)
        return NULL;
    else
    {
        node->next = NULL;
        node->previus = NULL;
        node->val = 1;
        return node;
    }
}

bool dListInsertList(struct dList *list)
{
    if (list == NULL)
    {
        return false;
    }
    else
    {
        struct dList *node = (struct dList *)malloc(sizeof(struct dList));
        struct dList *temp = list;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        node->val = temp->val + 1;
        node->next = NULL;
        node->previus = temp;
        temp->next = node;
        return true;
    }
}
bool dListDeleteList(struct dList *list)
{
    struct dList * temp = NULL;
    while (list!=NULL)
    {
        temp = list;
        list = list->next;
        free(temp);
    }
    
    return (list == NULL) ? true : false;
}

void regularPrint(struct dList *list)
{
    printf("\nCLASSIC PRINT: ");
    struct dList *temp = list;
    while (temp != NULL)
    {
        printf("%d ", temp->val);
        temp = temp->next;
    }
}
void reversePrint(struct dList *list)
{
    printf("\nREVERSE PRINT: ");

    struct dList *temp = list;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    while (temp != NULL)
    {

        printf("%d ", temp->val);
        temp = temp->previus;
    }
}