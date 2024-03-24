
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct LinkedList
{
     char text[64];
    struct LinkedList *next;
};

//Basic linked list that can point to the next entry in the list by utilizing a pointer to anohter struct LinkedList
int linkedlist(void)
{

    struct LinkedList* first = malloc(sizeof(struct LinkedList));
    struct LinkedList* second = malloc(sizeof(struct LinkedList));
    struct LinkedList* head = malloc(sizeof(struct LinkedList));

    strcpy(first->text,"Dustyn");
    strcpy(second->text,"Jim");
    strcpy(head->text,"head");

    head-> next = first;
    first->next = second;
    second->next = NULL;

    struct LinkedList *current = head;
    while(current != NULL)
    {
        printf("%s\n",current->text);
        current = current->next;
    }
    free(first);
    free(second);
    free(head);
    free(current);


    return 0;
}
