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
#define COUNT_MACHINES 16
#define EMPTY_OPERATIONS 0

typedef enum machineEnum
{
    lathe,       // 0 токарные работы
    fraser,      // 1 фрезерные работы
    drilling,    // 2 сверлильный станок
    welding,     // 3 сварочные работы
    laser,       // 4 лазерная резка
    listBending, // 5 станок для гибки листов металла
    wireBending, // 6 станок для гибки проволоки
    grinding,    // 7 шлифовальные работы
    painting,    // 8 покраска
    plastic,     // 9 литье пластика
    sewing,      // 10 швейные работы
    soldering,   // 11 пайка
    programming, // 12 программирование
    manual,      // 13 ручная сборка
    testing,     // 14 тестирование и отладка
    start        // 15 стартовая
} machineType;

typedef enum statusEnum
{
    new,
    waiting,
    completed,
} status;

typedef struct operationStruct
{
    char result[MAX_STRING];
    int minuteLimit;
    int maxLenToFinish;
    int countDependencies;
    machineType operationType;
    status operationStatus;
    int countNextOperations;
    int nextOperations[MAX_OPERATIONS];
} operation;

typedef struct timetableStruct {
    int timeStart;
    int timeFinish;
    int operationIndex;
    struct timetableStruct* next;
} timetableType;

// typedef struct st
// {
//     int num;
//     struct st* next;
// } stack;
// stack* headStack;

// typedef struct q
// {
//     int num;
//     struct q* next;
// } queue;
// queue* firstQ = NULL;
// queue* lastQ = NULL;

// void addQ(int num)
// {
//     queue* elt;
//     elt = malloc(sizeof(queue));
//     elt->num = num;
//     if (firstQ == NULL)
//     {
//         firstQ = elt;
//         lastQ = elt;
//     }
//     else
//     {
//         lastQ->next = elt;
//         elt->next = NULL;
//         lastQ = elt;
//     }
// }

// queue* getQ()
// {
//     queue* elt = NULL;
//     if (firstQ == NULL) return NULL;
//     elt = firstQ;
//     firstQ = elt->next;
//     return elt;
// }

// void push(int num)
// {
//     stack* elt;
//     elt = malloc(sizeof(stack));
//     elt->num = num;
//     elt->next = headStack;
//     headStack = elt;
// }

// stack* pop()
// {
//     stack* elt;
//     if (NULL == headStack) return NULL;
//     elt = headStack;
//     headStack = elt->next;
//     return elt;
// }

void print_menu()
{
    printf("Select a menu item:\n");
    printf("      0 - exit;\n");
    printf("      1 - open graph from file;\n");
    printf("      2 - print list operations;\n");
    printf("      3 - print max len to finish;\n");
    printf("      4 - print general timetable;\n");
    printf("      5 - print timetable for machine;\n");
    printf("Your answer >: ");
}

void printErrorEmpty()
{
    printf("--------------------------------------------------------------\n");
    printf("Operations empty. Needed read file!\n");
    printf("--------------------------------------------------------------\n");
}

int get_answer(int maxAnswer)
{
    int answer;
    do {
        if (scanf("%d", &answer) == 0)
        {
            printf("--------------------------------------------------------------\n");
            printf("Your answer is not digit, correct from 0 to %d, try again.\n", maxAnswer);
            printf("--------------------------------------------------------------\n");
            while (getchar() != '\n')
                continue;
            print_menu();
        }
        else if (answer < 0 || answer > maxAnswer)
        {
            printf("--------------------------------------------------------------\n");
            printf("Your answer (%d) is incorrect, correct from 0 to %d, try again.\n", answer, maxAnswer);
            printf("--------------------------------------------------------------\n");
            print_menu();
        }
    } while (answer < 0 || answer > maxAnswer);
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

int isOperationString(char * str)
{
    int i = 0;
    while (str[i] != ' ') i++;
    return str[i+1] != '-';
}

int getMinuteLimit(char* in_str)
{
    char* start = strstr(in_str, " | ") + 3;
    char* end = strchr(in_str, '\0');
    return (int) strtol(start, &end, 10);
}

int getLastNum(char* in_str)
{
    char* start = strstr(in_str, " - ") + 3;
    char* end = strchr(in_str, '\0');
    return (int) strtol(start, &end, 10);
}

int getFirstNum(char* in_str)
{
    char* end = strchr(in_str, ' ');
    return (int) strtol(in_str, &end, 10);
}

machineType getTypeOperation(char* in_str)
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
    else if (ptr = strstr(in_str, "шитье"))
        return sewing;
    else if (ptr = strstr(in_str, "паяльные работы"))
        return soldering;
    else if (ptr = strstr(in_str, "программ"))
        return programming;
    else if (ptr = strstr(in_str, "ручная сборка"))
        return manual;
    else if (ptr = strstr(in_str, "тест"))
        return testing;
}

char* getNameMashine(machineType machine)
{
    if (machine == lathe)
        return "Lathe";
    else if (machine == fraser)
        return "Fraser";
    else if (machine == drilling)
        return "Drilling";
    else if (machine == welding)
        return "Welding";
    else if (machine == laser)
        return "Laser";
    else if (machine == listBending)
        return "ListBend";
    else if (machine == wireBending)
        return "WireBend";
    else if (machine == grinding)
        return "Grinding";
    else if (machine == painting)
        return "Painting";
    else if (machine == plastic)
        return "Plastic";
    else if (machine == sewing)
        return "Sewing";
    else if (machine == soldering)
        return "Soldering";
    else if (machine == programming)
        return "Program";
    else if (machine == manual)
        return "Manual";
    else if (machine == testing)
        return "Testing";
    else if (machine == start)
        return "Start";
}

void readOperationsFile(operation* operations, int* countOperations)
{
    FILE* file_in = get_file_input();
    char in_str[MAX_STRING] = {0};
    int operationStart = 0;
    operation* operation = &operations[operationStart];
    strcpy(operation->result, "0 Start");
    operation->operationType = start;
    operation->minuteLimit = 0;
    operation->operationStatus = waiting;
    operation->countDependencies = 0;
    operation->countNextOperations = 0;
    operation->maxLenToFinish = 0;
    (*countOperations)++;
    while (fgets(in_str, MAX_STRING, file_in))
    {
        (*strchr(in_str, '\n')) = '\0';
        if (in_str[0] == '\0' || in_str[0] == '#')
        {
            in_str[0] = '\0';
            continue;
        }
        if (isOperationString(in_str))
        {
            (*countOperations)++;
            int numOperation = getFirstNum(in_str);
            operation = &operations[numOperation];
            strncpy(operation->result, in_str, strstr(in_str, " - ") - in_str);
            operation->operationType = getTypeOperation(in_str);
            operation->minuteLimit = getMinuteLimit(in_str);
            operation->operationStatus = new;
            operation->countDependencies = 0;
            operation->countNextOperations = 0;
            operation->maxLenToFinish = 0;
        }
        else
        {
            int firstOperation = getFirstNum(in_str);
            int secondOperation = getLastNum(in_str);
            int* countNextOperations = &operations[firstOperation].countNextOperations;
            operations[firstOperation].nextOperations[*countNextOperations] = secondOperation;
            (*countNextOperations)++;
            operations[secondOperation].countDependencies++;
        }
    }
    if (0 == (*countOperations))
    {
        printf("--------------------------------------------------------------\n");
        printf("Input file is empty!\n");
        printf("--------------------------------------------------------------\n");
    }
    else
    {
        printf("--------------------------------------------------------------\n");
        printf("Readed %d operations.\n", (*countOperations) - 1);
        printf("--------------------------------------------------------------\n");
    }
    fclose(file_in);
}

void printOperations(operation* operations, const int countOperations)
{
    for (int i = 0; i < countOperations; i++)
    {
        printf("%2d | Type = %2d | Status = %2d | Limit = %2d min | Dep = %2d | Nexts = %2d | MaxLen = %2d | %s\n",
            i,
            operations[i].operationType,
            operations[i].operationStatus,
            operations[i].minuteLimit,
            operations[i].countDependencies,
            operations[i].countNextOperations,
            operations[i].maxLenToFinish,
            operations[i].result
        );
        if (operations[i].countNextOperations != 0)
        {
            printf("     NextList: ");
            for (int j = 0; j < operations[i].countNextOperations; j++)
            {
                printf("%2d, ", operations[i].nextOperations[j]);
            }
            printf("\n");
        }
    }
}

int getMaxLenToFinish(operation* operations, int numOperation)
{
    operations[numOperation].maxLenToFinish = operations[numOperation].minuteLimit;
    int lenNext = 0;
    if (operations[numOperation].countNextOperations != 0)
        for (int i = 0; i < operations[numOperation].countNextOperations; i++)
        {
            int len = getMaxLenToFinish(operations, operations[numOperation].nextOperations[i]);
            lenNext = (lenNext < len) ? len : lenNext;
        }
    operations[numOperation].maxLenToFinish += lenNext;
    return operations[numOperation].maxLenToFinish;
}

int isFinish(operation* operations, const int countOperations)
{
    int isFinish = 1;
    for (int i = 0; i < countOperations; i++)
        if (operations[i].operationStatus != completed)
            isFinish = 0;
    return isFinish;
}

void updateStatus(operation* operations, int operationIndex)
{
    int countNext = operations[operationIndex].countNextOperations;
    int* nextOperationsIndex = operations[operationIndex].nextOperations;
    for (int i = 0; i < countNext; i++)
    {
        operations[nextOperationsIndex[i]].countDependencies--;
        if (operations[nextOperationsIndex[i]].countDependencies == 0)
            operations[nextOperationsIndex[i]].operationStatus = waiting;
    }
}

void createTimetable(timetableType** timetables, operation* operations, const int countOperations)
{
    while (!isFinish(operations, countOperations))
    {
        int maxLens[COUNT_MACHINES] = {0};
        int nextOperation[COUNT_MACHINES];
        for (int i = 0; i < COUNT_MACHINES; i++)
            nextOperation[i] = countOperations + 1;
        for (int i = 0; i <= countOperations; i++)
        {
            int maxLenToFinish = operations[i].maxLenToFinish;
            int machine = operations[i].operationType;
            int status = operations[i].operationStatus;
            if (status == waiting && maxLenToFinish > maxLens[machine])
            {
                maxLens[machine] = maxLenToFinish;
                nextOperation[machine] = i;
            }
        }
        for (int machine = 0; machine < COUNT_MACHINES; machine++)
        {
            if (nextOperation[machine] != countOperations + 1)
            {
                int operationIndex = nextOperation[machine];
                operation* operation = &operations[operationIndex];
                int lastTimeFinish = 0;
                timetableType* timetable = timetables[machine];
                while (timetable != NULL)
                {
                    lastTimeFinish = timetable->timeFinish;
                    timetable = timetable->next;
                }
                timetable = malloc(sizeof(timetableType));
                timetable->operationIndex = operationIndex;
                timetable->timeStart = lastTimeFinish;
                timetable->timeFinish = lastTimeFinish + operation->minuteLimit;
                operation->operationStatus = completed;
                updateStatus(operations, operationIndex);
            }
        }
    }
}

void initTimetables(timetableType** timetables)
{
    for (int i = 0; i < COUNT_MACHINES; i++)
    {
        timetables[i] = NULL;
    }
}

int main()
{
    const int maxAnswer = 5;
    timetableType* timetables[COUNT_MACHINES];
    initTimetables(timetables);
    operation operations[MAX_OPERATIONS];
    int countOperations = EMPTY_OPERATIONS;
    int maxLenToFinish = 0;
    int indexStartOperation = 0;
    printf("Program for creating production timetable\n");
    print_menu();
    int answer = get_answer(maxAnswer);
    while(answer != 0)
    {
        switch (answer)
        {
        case 1:
            countOperations = EMPTY_OPERATIONS;
            readOperationsFile(operations, &countOperations);
            maxLenToFinish = getMaxLenToFinish(operations, indexStartOperation);
            createTimetable(timetables, operations, countOperations);
            break;
        case 2:
            if (countOperations)
                printOperations(operations, countOperations);
            else
                printErrorEmpty();
            break;
        case 3:
            if (countOperations)
            {
                printf("--------------------------------------------------------------\n");
                printf("Max len to finish: %d minute\n", maxLenToFinish);
                printf("--------------------------------------------------------------\n");
            }
            else
                printErrorEmpty();
            break;
        case 4:
            if (countOperations)
                ;// printCompanyTimetable(company, operations, countOperations, maxLenToFinish);
            else
                printErrorEmpty();
            break;
        case 5:
            if (countOperations)
                ;// printMachineTimetable(company, operations, countOperations, maxLenToFinish);
            else
                printErrorEmpty();
            break;
        default:
            break;
        }
        print_menu();
        answer = get_answer(maxAnswer);
    }
    return 0;
}
