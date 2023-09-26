/*
                    Задача №24. (на языке Си)
    Имеется выражение в постфиксной форме, включающее
    обращение к функциям SIN, COS, EXP. Переменные заданы одной
    строчной латинской буквой. Запросить значения переменных и
    вычислить выражение. Представить его в инфиксной форме со
    скобками. Лишние скобки присутствовать не должны (11).

    ->: a b * c ^
--------------------------------------------------------------
Infix string: "a * b ^ c"
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

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

typedef struct st_calc
{
    float val;
    struct st_calc *next;
} stack_calc;
stack_calc *head_calc;

void push_calc(float val)
{
    stack_calc* elt;
    elt = malloc(sizeof(stack_calc));
    elt->val = val;
    elt->next = head_calc;
    head_calc = elt;
}

stack_calc* pop_calc()
{
    stack_calc* elt;
    if (NULL == head_calc) return NULL;
    elt = head_calc;
    head_calc = elt->next;
    return elt;
}

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
    printf("Insert postfix string (max %d symbol):\n->: ", MAX_STRING);
    fgets(str, 100, stdin);
    (*strchr(str, '\n')) = '\0';
}

FILE* get_file_input()
{
    char str[MAX_STRING] = {0};
    FILE *file;
    printf("Insert file name for INPUT with address (max %d symbol):\n->: ", MAX_STRING);
    fgets(str, 100, stdin);
    (*strchr(str, '\n')) = '\0';
    while (NULL == (file = fopen(str, "r")))
    {
        printf("Incorrect file name \"%s\" for INPUT, try again:\n->: ", str);
        fgets(str, 100, stdin);
        (*strchr(str, '\n')) = '\0';
    }
    return file;
}

FILE* get_file_output()
{
    char str[MAX_STRING] = {0};
    FILE* file;
    printf("Insert file name for OUTPUT with address (max %d symbol):\n->: ", MAX_STRING);
    fgets(str, 100, stdin);
    (*strchr(str, '\n')) = '\0';
    while (NULL == (file = fopen(str, "w")))
    {
        printf("Incorrect file name for OUTPUT, try again:\n->: ");
        fgets(str, 100, stdin);
        (*strchr(str, '\n')) = '\0';
    }
    return file;
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

void push_infix_in_head(char *postfix_str)
{
    for (int i = 0; i < strlen(postfix_str); i++)
    {
        stack *right_elt;
        stack *left_elt;
        operation type;
        char temp[2 * MAX_STRING + 3] = {0};
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
            if (is_var == left_elt->operation_type || type <= left_elt->operation_type)
                sprintf(temp, "%s %c ", left_elt->val, postfix_str[i]);
            else
                sprintf(temp, "(%s) %c ", left_elt->val, postfix_str[i]);
            if (is_var == right_elt->operation_type && (NULL == strchr(right_elt->val, '*') || postfix_str[i] == '/') && NULL == strchr(right_elt->val, '/') || postfix_str[i] == '*' && type <= right_elt->operation_type)
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
            if (is_var == left_elt->operation_type || type > left_elt->operation_type)
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
}

void print_stack()
{
    int count = 1;
    while (NULL != head)
    {
        stack *elt = pop();
        printf(" %3d: %s %d |", count, elt->val, elt->operation_type);
        if (0 == count % 10)
            printf("\n");
        free(elt);
        count++;
    }
}

void print_menu()
{
    printf("Select a menu item:\n");
    printf("      0 - exit;\n");
    printf("      1 - insert postfix string;\n");
    printf("      2 - print seved postfix string;\n");
    printf("      3 - print seved infix string;\n");
    printf("      4 - make a calculation;\n");
    printf("      5 - open file and write in file;\n");
    printf("Your answer >: ");
}

int get_answer()
{
    int answer;
    do {
        if (scanf("%d", &answer) == 0)
        {
            printf("Your answer is not digit, correct from 0 to 4, try again.\n");
            while (getchar() != '\n')
                continue;
            print_menu();
        }
        else if (answer < 0 || answer > 5)
        {
            printf("Your answer (%d) is incorrect, correct from 0 to 4, try again.\n", answer);
            print_menu();
        }
    } while (answer < 0 || answer > 5);
    while (getchar() != '\n')
        continue;
    return answer;
}

float get_val(char ch)
{
    float val;
    printf("Insert val of '%c': ", ch);
    while (scanf("%f", &val) == 0)
    {
        while (getchar() != '\n')
            continue;
        printf("Your answer is incorrect, try again.\n");
        printf("Insert val of '%c': ", ch);
    }
    return val;
}

void calc(char* postfix_str)
{
    free(head_calc);
    head_calc = NULL;
    stack_calc *right_elt;
    stack_calc *left_elt;
    char str[MAX_STRING] = {0};
    char var_name[MAX_STRING] = {0};
    float var_val[MAX_STRING] = {0};
    int var_index = 0;
    for (int i = 0; i < strlen(postfix_str); i++)
    {
        if (isalpha(postfix_str[i]) && ' ' == postfix_str[i + 1])
        {
            if (strchr(var_name, postfix_str[i]))
                push_calc(var_val[strchr(var_name, postfix_str[i]) - var_name]);
            else
            {
                var_name[var_index] = postfix_str[i];
                var_val[var_index] = get_val(postfix_str[i]);
                push_calc(var_val[var_index]);
                var_index++;
            }
            i++;
        }
        else if (isdigit(postfix_str[i]))
        {
            get_str_befor_space(postfix_str, &i, str);
            push_calc(strtof(str, NULL));
            str[0] = '\0';
        }
        else if (isalpha(postfix_str[i]) && isalpha(postfix_str[i + 1]))
        {
            right_elt = pop_calc();
            get_str_befor_space(postfix_str, &i, str);
            if (strstr(str, "sin"))
            {
                push_calc((float) sin((float) right_elt->val));
            }
            else if (strstr(str, "cos"))
            {
                push_calc((float)cos((float)right_elt->val));
            }
            else if (strstr(str, "exp"))
            {
                push_calc((float)exp((float)right_elt->val));
            }
            else if (strstr(str, "ln") || strstr(str, "log"))
            {
                push_calc((float)logf((float)right_elt->val));
            }
            free(right_elt);
        }
        else if ('+' == postfix_str[i])
        {
            right_elt = pop_calc();
            left_elt = pop_calc();
            push_calc(left_elt->val + right_elt->val);
            free(right_elt);
            free(left_elt);
        }
        else if ('-' == postfix_str[i])
        {
            right_elt = pop_calc();
            left_elt = pop_calc();
            push_calc(left_elt->val - right_elt->val);
            free(right_elt);
            free(left_elt);
        }
        else if ('*' == postfix_str[i])
        {
            right_elt = pop_calc();
            left_elt = pop_calc();
            push_calc(left_elt->val * right_elt->val);
            free(right_elt);
            free(left_elt);
        }
        else if ('/' == postfix_str[i])
        {
            right_elt = pop_calc();
            left_elt = pop_calc();
            push_calc(left_elt->val / right_elt->val);
            free(right_elt);
            free(left_elt);
        }
        else if ('^' == postfix_str[i])
        {
            right_elt = pop_calc();
            left_elt = pop_calc();
            push_calc(powf(left_elt->val, right_elt->val));
            free(right_elt);
            free(left_elt);
        }
    }
    printf("Answer = %.2f\n", (pop_calc())->val);
}

int main(void)
{
    char postfix_str[MAX_STRING] = {0};
    int answer;
    printf("!!!!!!!!Program for translation from postfix to infix!!!!!!!!!\n");
    printf("--------------------------------------------------------------\n");
    print_menu();
    answer = get_answer();
    while (answer != 0)
    {
        switch (answer)
        {
        case 1:
            free(head);
            head = NULL;
            get_postfix_string(postfix_str);
            push_infix_in_head(postfix_str);
            printf("--------------------------------------------------------------\n");
            printf("Infix string: \"%s\"\n", head->val);
            printf("--------------------------------------------------------------\n");
            break;
        case 2:
            printf("--------------------------------------------------------------\n");
            printf("Postfix string: \"%s\"\n", postfix_str);
            printf("--------------------------------------------------------------\n");
            break;
        case 3:
            printf("--------------------------------------------------------------\n");
            printf("Infix string: \"%s\"\n", head->val);
            printf("--------------------------------------------------------------\n");
            break;
        case 4:
            if ('\0' != postfix_str[0])
                calc(postfix_str);
            else
            {
                printf("--------------------------------------------------------------\n");
                printf("Postfix string is empty!!!\n");
                printf("--------------------------------------------------------------\n");
            }
            break;
        case 5:
            FILE* file_in = get_file_input();
            FILE* file_out = get_file_output();
            int count = 0;
            char p_str[MAX_STRING] = {0};
            char temp_str[MAX_STRING + MAX_STRING / 2] = {0};
            while (fgets(p_str, 100, file_in))
            {
                (*strchr(p_str, '\n')) = '\0';
                push_infix_in_head(p_str);
                count++;
                sprintf(temp_str, "%d:\n\tPostfix: %s\n", count, p_str);
                fputs(temp_str, file_out);
                temp_str[0] = '\0';
                sprintf(temp_str, "\tInfix  : %s\n", head->val);
                fputs(temp_str, file_out);
                temp_str[0] = '\0';
            }
            if (0 == count)
            {
                printf("--------------------------------------------------------------\n");
                printf("Input file is empty!\n");
                printf("--------------------------------------------------------------\n");
            }
            else
            {
                printf("--------------------------------------------------------------\n");
                printf("Recorded %d elements.\n", count);
                printf("--------------------------------------------------------------\n");
            }
                fclose(file_in);
                fclose(file_out);
                break;
        }
        printf("Done!\n");
        print_menu();
        answer = get_answer();
    }
    free(head);
    return 0;
}