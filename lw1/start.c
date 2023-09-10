#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STRING 100                       // postfix string max 100 symbol
#define SIN_NAME "SIN"
#define COS_NAME "COS"
#define EXP_NAME "EXP"

typedef enum
{
    stepen = 3,
    func = 3,
    delenie = 2,
    umnozhenie = 2,
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
    char postfix_str[MAX_STRING] = {0};
    get_postfix_string(postfix_str);
    for (int i = 0; i < strlen(postfix_str); i++)
    {
        stack* right_elt;
        stack* left_elt;
        operation type;
        char temp[MAX_STRING + MAX_STRING / 2] = {0};
        char ch = postfix_str[i];
        if (isalpha(ch))
        {
            char ch_str[2] = {ch, '\0'};
            push(ch_str, is_var);
        } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^')
        {
            right_elt = pop();
            left_elt = pop();
            switch (ch)
            {
            case '+': type = plus; break;
            case '-': type = minus; break;
            case '*': type = umnozhenie; break;
            case '/': type = delenie; break;
            case '^': type = stepen; break;
            }
            if (left_elt->operation_type != 0 && left_elt->operation_type < type)
                sprintf(temp, "(%s)%c", left_elt->val, ch);
            else 
                sprintf(temp, "%s%c", left_elt->val, ch);
            if (right_elt->operation_type != 0 && right_elt->operation_type < type)
            {
                strcat(temp, "(");
                strcat(temp, right_elt->val);
                strcat(temp, ")");
            } 
            else 
            {
                strcat(temp, right_elt->val);
            }
            push(temp, type);
            free(left_elt);
            free(right_elt);
            temp[0] = '\0';
        }
    }
    printf("%s\n", (pop())->val);
    free(head);
    /*
    int count = 1;
    while(NULL != head)
    {
        stack* elt = pop();
        printf(" %3d: %s %d |", count, elt->val, elt->operation_type);
        if (0 == count % 10) printf("\n");
        free(elt);
        count++;
    }
    */
    return 0;
}