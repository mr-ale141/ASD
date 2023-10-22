/*  UTF-8 cmd 'chcp 65001'
                        2.4. Графы

    Общими требованиями к лабораторной работе являются:
    1) вводить  граф из файла в понятной для пользователя форме
(не в виде матрицы смежности, без дублирования информации);
    2) обеспечить   возможность   многократных   запросов   без
повторного запуска программы.

24. Информация  о  некотором  изделии  задана   с   помощью
ориентированного графа.  Вершина графа соответствует отдельной
операции,  а дуги показывают очередность  выполнения  операций
(каждая   операция   может   начаться   не   ранее   окончания
предшествующих операций).  Продолжительность  каждой  операции
известна.  Имеется  один  станок  для  выполнения каждого типа
операций.  Если станок освобождается,  то производится  та  из
готовых  к выполнению операций,  для которой максимальный путь
от соответствующей вершины до конца  имеет  наибольшую  длину.
Определить  время  изготовления  изделия.  Выдать общий график
выполнения операций и график загрузки каждого станка (13).
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING 200
#define MAX_OPERATIONS 100
#define NO_NEXT -1

typedef enum machineEnum
{
    lathe,                      // 0 токарные работы
    fraser,                     // 1 фрезерные работы
    drilling,                   // 2 сверлильный станок
    welding,                    // 3 сварочные работы
    laser,                      // 4 лазерная резка
    listBending,                // 5 станок для гибки листов металла
    wireBending,                // 6 станок для гибки проволоки
    grinding,                   // 7 шлифовальные работы
    painting,                   // 8 покраска
    plastic,                    // 9 литье пластика
    sewing,                     // 10 швейные работы
    soldering,                  // 11 пайка
    programming,                // 12 программирование
    manual,                     // 13 ручная сборка
    testing                     // 14 тестирование и отладка
} machine;

typedef enum statusEnum
{
    new,
    inProgress,
    completed,
} status;

typedef struct operationStruct {
    char result[MAX_STRING];
    int minuteLimit;
    machine operationType;
    status operationStatus;
    int next;
} operation;

typedef struct st
{
    int num;
    int countPoint;
    struct st* next;
} stack;
stack* headStack;

typedef struct q
{
    int numPC;
    struct q* next;
} queue;
queue* firstQ = NULL;
queue* lastQ = NULL;

void addQ(int numPC)
{
    queue* elt;
    elt = malloc(sizeof(queue));
    elt->numPC = numPC;
    if (firstQ == NULL)
    {
        firstQ = elt;
        lastQ = elt;
    }
    else
    {
        lastQ->next = elt;
        elt->next = NULL;
        lastQ = elt;
    }
}

queue* getQ()
{
    queue* elt = NULL;
    if (firstQ == NULL) return NULL;
    elt = firstQ;
    firstQ = elt->next;
    return elt;
}

void pushFather(int num, int countPoint)
{
    stack* elt;
    elt = malloc(sizeof(stack));
    elt->num = num;
    elt->countPoint = countPoint;
    elt->next = headStack;
    headStack = elt;
}

stack* popFather()
{
    stack* elt;
    if (NULL == headStack) return NULL;
    elt = headStack;
    headStack = elt->next;
    return elt;
}

void print_menu()
{
    printf("Select a menu item:\n");
    printf("      0 - exit;\n");
    printf("      1 - open graph from file;\n");
    printf("      2 - print list operations;\n");
    printf("      3 - print all time for create product;\n");
    printf("      4 - print general timetable;\n");
    printf("      5 - print timetable for machine;\n");
    printf("Your answer >: ");
}

int get_answer()
{
    const int MAX_ANSWER = 5;
    int answer;
    do {
        if (scanf("%d", &answer) == 0)
        {
            printf("--------------------------------------------------------------\n");
            printf("Your answer is not digit, correct from 0 to %d, try again.\n", MAX_ANSWER);
            printf("--------------------------------------------------------------\n");
            while (getchar() != '\n')
                continue;
            print_menu();
        }
        else if (answer < 0 || answer > MAX_ANSWER)
        {
            printf("--------------------------------------------------------------\n");
            printf("Your answer (%d) is incorrect, correct from 0 to %d, try again.\n", answer, MAX_ANSWER);
            printf("--------------------------------------------------------------\n");
            print_menu();
        }
    } while (answer < 0 || answer > MAX_ANSWER);
    while (getchar() != '\n')
        continue;
    return answer;
}

FILE* get_file_input()
{
    char str[MAX_STRING] = {0};
    FILE *file;
    printf("Insert file name for INPUT with address (max %d symbol):\n->: ", MAX_STRING);
    fgets(str, MAX_STRING, stdin);
    (*strchr(str, '\n')) = '\0';
    while (NULL == (file = fopen(str, "r")))
    {
        printf("Incorrect file name \"%s\" for INPUT, try again:\n->: ", str);
        fgets(str, MAX_STRING, stdin);
        (*strchr(str, '\n')) = '\0';
    }
    return file;
}

FILE* get_file_output()
{
    char str[MAX_STRING] = {0};
    FILE* file;
    printf("Insert file name for OUTPUT with address (max %d symbol):\n->: ", MAX_STRING);
    fgets(str, MAX_STRING, stdin);
    (*strchr(str, '\n')) = '\0';
    while (NULL == (file = fopen(str, "w")))
    {
        printf("Incorrect file name for OUTPUT, try again:\n->: ");
        fgets(str, MAX_STRING, stdin);
        (*strchr(str, '\n')) = '\0';
    }
    return file;
}

int getCountPoint(char* in_str)
{
    int count = 0;
    for (int i = 0; in_str[i] != '\0'; i++)
    {
        if (in_str[i] == '.')
            count++;
        else
            break;
    }
    return count;
}

machine getTypeOperation(char* in_str)
{
    char* ptr;
    if (ptr = strstr(in_str, "токар"))
        return lathe;
    else if (ptr = strstr(in_str, "фрезер"))
        return fraser;
    else if (ptr = strstr(in_str, "сверл"))
        return drilling;
    else if (ptr = strstr(in_str, "свар"))
        return welding;
    else if (ptr = strstr(in_str, "лазер"))
        return laser;
    else if (ptr = strstr(in_str, "гибка листов"))
        return listBending;
    else if (ptr = strstr(in_str, "гибка проволоки"))
        return wireBending;
    else if (ptr = strstr(in_str, "шлиф"))
        return grinding;
    else if (ptr = strstr(in_str, "краск"))
        return painting;
    else if (ptr = strstr(in_str, "лить"))
        return plastic;
    else if (ptr = strstr(in_str, "швей"))
        return sewing;
    else if (ptr = strstr(in_str, "пайк"))
        return soldering;
    else if (ptr = strstr(in_str, "программ"))
        return programming;
    else if (ptr = strstr(in_str, "ручная сборка"))
        return manual;
    else if (ptr = strstr(in_str, "тест"))
        return testing;
}

int getMinuteLimit(char* in_str)
{
    char* start = strstr(in_str, " | ") + 3;
    char* end = strchr(in_str, '\0');
    return (int) strtol(start, &end, 10);
}

void readOperationsFile(operation* operations, int* countOperations)
{
    FILE* file_in = get_file_input();
    int countString = 0;
    int prevCountPoint = 0;
    char in_str[MAX_STRING] = {0};
    while (fgets(in_str, MAX_STRING, file_in))
    {
        (*strchr(in_str, '\n')) = '\0';
        if (in_str[0] == '\0' || in_str[0] == '#')
        {
            in_str[0] = '\0';
            continue;
        }
        (*countOperations)++;
        int countPoint = getCountPoint(in_str);
        strncpy(
                operations[(*countOperations) - 1].result,
                in_str + countPoint,
                strstr(in_str, " - ") - in_str - countPoint
            );
        operations[(*countOperations) - 1].operationType = getTypeOperation(in_str);
        operations[(*countOperations) - 1].minuteLimit = getMinuteLimit(in_str);
        operations[(*countOperations) - 1].operationStatus = new;
        if (countPoint == 0)
        {
            operations[(*countOperations) - 1].next = NO_NEXT;
            pushFather((*countOperations) - 1, countPoint);
        }
        else if (countPoint > prevCountPoint)
        {
            operations[(*countOperations) - 1].next = headStack->num;
            pushFather((*countOperations) - 1, countPoint);
        }
        else if (countPoint == prevCountPoint)
        {
            operations[(*countOperations) - 2].operationStatus = inProgress;
            operations[(*countOperations) - 1].next = headStack->next->num;
            free(popFather());
            pushFather((*countOperations) - 1, countPoint);
        }
        else if (countPoint < prevCountPoint)
        {
            operations[(*countOperations) - 2].operationStatus = inProgress;
            while (headStack->countPoint != countPoint)
                free(popFather());
            operations[(*countOperations) - 1].next = headStack->next->num;
            free(popFather());
            pushFather((*countOperations) - 1, countPoint);
        }
        in_str[0] = '\0';
        prevCountPoint = countPoint;
    }
    operations[(*countOperations) - 1].operationStatus = inProgress;
    if (0 == (*countOperations))
    {
        printf("--------------------------------------------------------------\n");
        printf("Input file is empty!\n");
        printf("--------------------------------------------------------------\n");
    }
    else
    {
        printf("--------------------------------------------------------------\n");
        printf("Readed %d operations.\n", (*countOperations));
        printf("--------------------------------------------------------------\n");
    }
    fclose(file_in);
}

void printOperations(operation* operations, int countOperations)
{
    for (int i = 0; i < countOperations; i++)
    {
        printf("%2d | Type = %2d | Status = %2d | Limit = %2d min | Next = %2d | %s\n",
            i,
            operations[i].operationType,
            operations[i].operationStatus,
            operations[i].minuteLimit,
            operations[i].next,
            operations[i].result
        );
    }
}

int main()
{
    printf("Program for creating production timetable\n");
    print_menu();
    operation operations[MAX_OPERATIONS];
    int countOperations = 0;
    int answer = get_answer();
    while(answer != 0)
    {
        switch (answer)
        {
        case 1:
            readOperationsFile(operations, &countOperations);
            break;
        case 2:
            if (countOperations)
                printOperations(operations, countOperations);
            else
            {
                printf("--------------------------------------------------------------\n");
                printf("Operations empty. Needed read file!\n");
                printf("--------------------------------------------------------------\n");
            }
            break;
        case 3:
            if (countOperations)
                ;
            else
            {
                printf("--------------------------------------------------------------\n");
                printf("Operations empty. Needed read file!\n");
                printf("--------------------------------------------------------------\n");
            }
            break;
        case 4:
            if (countOperations)
                ;
            else
            {
                printf("--------------------------------------------------------------\n");
                printf("Operations empty. Needed read file!\n");
                printf("--------------------------------------------------------------\n");
            }
            break;
        case 5:
            if (countOperations)
                ;
            else
            {
                printf("--------------------------------------------------------------\n");
                printf("Operations empty. Needed read file!\n");
                printf("--------------------------------------------------------------\n");
            }
            break;
        default:
            break;
        }
        print_menu();
        answer = get_answer();
    }
    return 0;
}
