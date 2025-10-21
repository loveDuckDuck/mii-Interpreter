#include <stdio.h>
#include "dList.h"
int main()
{

    struct dList *list = dListCreateList();
    if (list != NULL)
    {
        printf("created list\n");
        printf("val head : %d\n", list->val);
    }
    else
        printf("errors");

    printf("%s\n", dListInsertList(list) ? "add" : "no");
    printf("%s\n", dListInsertList(list) ? "add" : "no");

    printf("%s\n", dListInsertList(list) ? "add" : "no");

    regularPrint(list);
    reversePrint(list);
    printf("\n%s\n", dListDeleteList(list) ? "delete all" : "error while deleting");

    return 0;
}