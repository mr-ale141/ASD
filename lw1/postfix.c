/*
                    Задача №24. (на языке Си)
    Имеется выражение в постфиксной форме, включающее
    обращение к функциям SIN, COS, EXP. Переменные заданы одной
    строчной латинской буквой. Запросить значения переменных и
    вычислить выражение. Представить его в инфиксной форме со
    скобками. Лишние скобки присутствовать не должны (11).

    1. Отрицательные числа, переменные
    2. Минус перед скобками
    3. Очеред с делением/умножением
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STRING 100                       // postfix string max 100 symbol

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
    gets(str);
    while ((len = (int)strlen(str)) > MAX_STRING)
    {
        printf("Max %d symbol, you gave %d symbol(s), try again:\n->: ", MAX_STRING, len);
        gets(str);
    }
}

void get_str_befor_space(char *in_str, int *i, char *temp)
{
    int pos_temp = 0;
    temp[pos_temp] = in_str[*i];
    temp[pos_temp + 1] = '\0';
    (*i)++;
    pos_temp++;
    while (' ' != in_str[*i] && '\0' != in_str[*i])
    {
        temp[pos_temp] = in_str[*i];
        temp[pos_temp + 1] = '\0';
        (*i)++;
        pos_temp++;
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
        if (isalpha(postfix_str[i]) && isalpha(postfix_str[i + 1]))
        {
            type = func;
            get_str_befor_space(postfix_str, &i, temp);
            right_elt = pop();
            if (is_var == right_elt->operation_type || (type == right_elt->operation_type && NULL == strchr(right_elt->val, '^')))
            {
                strcat(temp, " ");
                strcat(temp, right_elt->val);
            }
            else
            {
                strcat(temp, "(");
                strcat(temp, right_elt->val);
                strcat(temp, ")");
            }
            push(temp, type);
            temp[0] = '\0';
            free(right_elt);
        }
        else if (isalpha(postfix_str[i]) && (' ' == postfix_str[i + 1] || '\0' == postfix_str[i + 1]))
        {
            type = is_var;
            temp[0] = postfix_str[i];
            temp[1] = '\0';
            push(temp, type);
            temp[0] = '\0';
            i++;
        }
        else if (isdigit(postfix_str[i]))
        {
            type = is_var;
            get_str_befor_space(postfix_str, &i, temp);
            push(temp, type);
            temp[0] = '\0';
        }
        else if ('+' == postfix_str[i])
        {
            type = plus;
            right_elt = pop();
            left_elt = pop();
            sprintf(temp, "%s %c %s", left_elt->val, postfix_str[i], right_elt->val);
            push(temp, type);
            free(left_elt);
            free(right_elt);
            temp[0] = '\0';
        }
        else if ('-' == postfix_str[i])
        {
            type = minus;
            right_elt = pop();
            left_elt = pop();
            sprintf(temp, "%s %c ", left_elt->val, postfix_str[i]);
            if (type == right_elt->operation_type)
            {
                strcat(temp, "(");
                strcat(temp, right_elt->val);
                strcat(temp, ")");
            }
            else
                strcat(temp, right_elt->val);
            push(temp, type);
            free(left_elt);
            free(right_elt);
            temp[0] = '\0';
        }
        else if ('*' == postfix_str[i] || '/' == postfix_str[i])
        {
            type = umnozhenie;
            right_elt = pop();
            left_elt = pop();
            if (is_var == left_elt->operation_type || NULL == strchr(left_elt->val, '/') && type <= left_elt->operation_type)
                sprintf(temp, "%s %c ", left_elt->val, postfix_str[i]);
            else
                sprintf(temp, "(%s) %c ", left_elt->val, postfix_str[i]);
            if (is_var == right_elt->operation_type || NULL == strchr(right_elt->val, '/') && type <= right_elt->operation_type)
                strcat(temp, right_elt->val);
            else
            {
                strcat(temp, "(");
                strcat(temp, right_elt->val);
                strcat(temp, ")");
            }
            push(temp, type);
            free(left_elt);
            free(right_elt);
            temp[0] = '\0';
        }
        else if ('^' == postfix_str[i])
        {
            type = stepen;
            right_elt = pop();
            left_elt = pop();
            if (is_var == left_elt->operation_type || type == left_elt->operation_type)
                sprintf(temp, "%s %c ", left_elt->val, postfix_str[i]);
            else
                sprintf(temp, "(%s) %c ", left_elt->val, postfix_str[i]);
            if (is_var == right_elt->operation_type || type == right_elt->operation_type)
                strcat(temp, right_elt->val);
            else
            {
                strcat(temp, "(");
                strcat(temp, right_elt->val);
                strcat(temp, ")");
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