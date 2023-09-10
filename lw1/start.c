#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING 100                       // postfix string max 100 symbol
#define SIN_NAME "SIN"
#define COS_NAME "COS"
#define EXP_NAME "EXP"

typedef enum
{
    power = 3,
    func = 3,
    devide = 2,
    multiply = 2,
    minus = 1,
    plus = 1,
    is_var = 0
} operation;

typedef struct st
{
    char val[MAX_STRING];
    operation operation_type;
    struct st* next;
} stack;
stack* head;

void push(char* str, operation type)
{
    stack* elt;
    elt = malloc(sizeof(stack));
    strcpy(elt->val, str);
    elt->operation_type = type;
    elt->next = head;
    head = elt;
}

stack* pop()
{
    stack* elt;
    if (NULL == head) return NULL;
    elt = head;
    head = elt->next;
    return elt;
}

void get_postfix_string(char* str)
{
    int len = 0;
    printf("Insert postfix string (max %d symbol):\n->: ", MAX_STRING);
    scanf("%s", str);
    while ((len = (int)strlen(str)) > MAX_STRING)
    {
        printf("Max %d symbol, you gave %d symbol(s), try again:\n->: ", MAX_STRING, len);
        scanf("%s", str);
    }
}

int main(void)
{
    char postfix_str[MAX_STRING];
    get_postfix_string(postfix_str);
    for (int i=0; i<strlen(postfix_str); i++)
    {
        char ch[2] = {postfix_str[i], '\0'};
        push(ch, is_var);
    }
    int count = 1;
    while(NULL != head)
    {
        stack* elt = pop();
        printf(" %d: %s %d |", count, elt->val, elt->operation_type);
        count++;
    }
    return 0;
}