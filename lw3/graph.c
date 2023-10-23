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
#define NO_FINISH -2
#define COUNT_MACHINES 15
#define MIN_STEP 0

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
} machineType;

typedef enum statusEnum
{
    new,
    waiting,
    inProgress,
    completed,
} status;

typedef struct operationStruct {
    char result[MAX_STRING];
    int minuteLimit;
    int stepToFinish;
    int dependencies;
    machineType operationType;
    status operationStatus;
    int next;
} operation;

typedef struct timetableStruct {
    int timeStart;
    int timeFinish;
    int operation;
    struct timetableStruct* next;
} timetableType;

typedef struct st
{
    int num;
    int countPoint;
    struct st* next;
} stack;
stack* headStack;

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

void printErrorEmpty()
{
    printf("--------------------------------------------------------------\n");
    printf("Operations empty. Needed read file!\n");
    printf("--------------------------------------------------------------\n");
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
        operation* currOperation = &operations[(*countOperations) - 1];
        operation* prevOperation = &operations[(*countOperations) - 2];
        int countPoint = getCountPoint(in_str);
        strncpy(
                currOperation->result,
                in_str + countPoint,
                strstr(in_str, " - ") - in_str - countPoint
            );
        currOperation->operationType = getTypeOperation(in_str);
        currOperation->minuteLimit = getMinuteLimit(in_str);
        currOperation->operationStatus = new;
        currOperation->dependencies = 0;
        if (countPoint == 0)
        {
            currOperation->next = NO_NEXT;
            pushFather((*countOperations) - 1, countPoint);
            currOperation->stepToFinish = 1;
        }
        else if (countPoint > prevCountPoint)
        {
            currOperation->next = headStack->num;
            operations[headStack->num].dependencies++;
            pushFather((*countOperations) - 1, countPoint);
        }
        else if (countPoint == prevCountPoint)
        {
            prevOperation->operationStatus = waiting;
            currOperation->next = headStack->next->num;
            operations[headStack->next->num].dependencies++;
            free(popFather());
            pushFather((*countOperations) - 1, countPoint);
        }
        else if (countPoint < prevCountPoint)
        {
            prevOperation->operationStatus = waiting;
            while (headStack->countPoint != countPoint)
                free(popFather());
            currOperation->next = headStack->next->num;
            operations[headStack->next->num].dependencies++;
            free(popFather());
            pushFather((*countOperations) - 1, countPoint);
        }
        currOperation->stepToFinish = operations[currOperation->next].stepToFinish + 1;
        in_str[0] = '\0';
        prevCountPoint = countPoint;
    }
    operation* lastOperation = &operations[(*countOperations) - 1];
    lastOperation->operationStatus = waiting;
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

void printOperations(operation* operations, const int countOperations)
{
    for (int i = 0; i < countOperations; i++)
    {
        printf("%2d | Type = %2d | Status = %2d | Limit = %2d min | Next = %2d | Steps = %2d | Dep = %2d | %s\n",
            i,
            operations[i].operationType,
            operations[i].operationStatus,
            operations[i].minuteLimit,
            operations[i].next,
            operations[i].stepToFinish,
            operations[i].dependencies,
            operations[i].result
        );
    }
}

int getOperationIndex(const machineType machine, operation* operations, const int countOperations)
{
    int operationIndex = NO_NEXT;
    int stepToFinish = MIN_STEP;
    for (int i = 0; i < countOperations; i++)
    {
        if (operations[i].operationType == machine && operations[i].operationStatus == waiting && operations[i].stepToFinish > stepToFinish)
        {
            stepToFinish = operations[i].stepToFinish;
            operationIndex = i;
        }
    }
    return operationIndex;
}

void startCompany(timetableType* company, operation* operations, const int countOperations, int* time)
{
    *time = 0;
    for (int machine = lathe; machine <= testing; machine++)
    {
        int operationIndex = getOperationIndex(machine, operations, countOperations);
        company[machine].timeStart = *time;
        company[machine].timeFinish = NO_FINISH;
        company[machine].next = NULL;
        company[machine].operation = operationIndex;
        if (operationIndex >= 0)
        {
            operations[operationIndex].operationStatus = inProgress;
            company[machine].timeFinish = *time + operations[operationIndex].minuteLimit;
        }
    }
}

int isProductFinish(operation* operations, const int countOperations)
{
    int isFinish = 1;
    for (int i = 0; i < countOperations; i++)
        if (operations[i].operationStatus != completed)
            isFinish = 0;
    return isFinish;
}

void createProduct(timetableType* company, operation* operations, const int countOperations, int* time)
{
    startCompany(company, operations, countOperations, time);
    while (!isProductFinish(operations, countOperations))
    {
        (*time)++;
        for (int machine = lathe; machine <= testing; machine++)
        {
            timetableType* currElt = &company[machine];
            while (currElt->next != NULL)
                currElt = currElt->next;
            if (currElt->timeFinish == *time)
            {
                operations[currElt->operation].operationStatus = completed;
                int next = operations[currElt->operation].next;
                if (next != NO_NEXT)
                    operations[next].dependencies--;
                if (next != NO_NEXT && operations[next].dependencies == 0)
                    operations[next].operationStatus = waiting;
                timetableType* newElt = malloc(sizeof(timetableType));
                newElt->next = NULL;
                int newOperation = getOperationIndex(machine, operations, countOperations);
                newElt->operation = newOperation;
                newElt->timeStart = *time;
                if (newOperation != NO_NEXT)
                {
                    operations[newOperation].operationStatus = inProgress;
                    newElt->timeFinish = operations[newOperation].minuteLimit + *time;
                }
                else
                    newElt->timeFinish = NO_FINISH;
                currElt->next = newElt;
            }
            else if (currElt->operation == NO_NEXT)
            {
                currElt->timeFinish = *time;
                int newOperation = getOperationIndex(machine, operations, countOperations);
                if (newOperation != NO_NEXT)
                {
                    timetableType* newElt = malloc(sizeof(timetableType));
                    newElt->next = NULL;
                    newElt->operation = newOperation;
                    newElt->timeStart = *time;
                    operations[newOperation].operationStatus = inProgress;
                    newElt->timeFinish = operations[newOperation].minuteLimit + *time;
                    currElt->next = newElt;
                }
            }
            
        }
    }
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
}

int getMaxResultStrLen(operation* operations, const int countOperations)
{
    int maxLen = 0;
    for (int i = 0; i < countOperations; i++)
    {
        int len = strlen(operations[i].result);
        if (len > maxLen)
            maxLen = len;
    }
    return maxLen;
}

machineType choiseMachine()
{
    printf("--------------------------------------------------------------\n");
    printf("Select machine:\n");
    printf("      1  - lathe;\n");
    printf("      2  - fraser;\n");
    printf("      3  - drilling;\n");
    printf("      4  - welding;\n");
    printf("      5  - laser;\n");
    printf("      6  - listBending;\n");
    printf("      7  - wireBending;\n");
    printf("      8  - grinding;\n");
    printf("      9  - painting;\n");
    printf("      10 - plastic;\n");
    printf("      11 - sewing;\n");
    printf("      12 - soldering;\n");
    printf("      13 - programming;\n");
    printf("      14 - manual;\n");
    printf("      15 - testing;\n");
    printf("Your answer >: ");
    int answer = get_answer(15);
    if (answer == 1)
        return lathe;
    else if (answer == 2)
        return fraser;
    else if (answer == 3)
        return drilling;
    else if (answer == 4)
        return welding;
    else if (answer == 5)
        return laser;
    else if (answer == 6)
        return listBending;
    else if (answer == 7)
        return wireBending;
    else if (answer == 8)
        return grinding;
    else if (answer == 9)
        return painting;
    else if (answer == 10)
        return plastic;
    else if (answer == 11)
        return sewing;
    else if (answer == 12)
        return soldering;
    else if (answer == 13)
        return programming;
    else if (answer == 14)
        return manual;
    else if (answer == 15)
        return testing;
}

void printMachineTimetable(timetableType* company, operation* operations, int countOperations, int timeFinish)
{
    machineType machine = choiseMachine();
    timetableType* currMachine = &company[machine];
    printf("---------------------------%9s--------------------------\n", getNameMashine(machine));
    printf("Start | Finish | Operation\n");
    printf("------+--------+----------------------------------------------\n");
    while (currMachine != NULL)
    {
        char operation[MAX_STRING];
        if (currMachine->operation != NO_NEXT)
            printf("%5d | %6d | %s\n", currMachine->timeStart, currMachine->timeFinish, operations[currMachine->operation].result);
        else
            printf("%5d | %6d | %s\n", currMachine->timeStart, currMachine->timeFinish, "Ожидание");
        currMachine = currMachine->next;
    }
    printf("--------------------------------------------------------------\n");
}

void printCompanyTimetable(timetableType* company, operation* operations, int countOperations, int timeFinish)
{
    // int maxLen = getMaxResultStrLen(operations, countOperations);
    // printf("%*d |", maxLen, maxLen);
    // for (machineType machine = lathe; machine < COUNT_MACHINES; machine++)
    //     printf(" %9s |", getNameMashine(machine));
    // printf("\n");
    // for (int i = 0; i <= maxLen; i++)
    //     printf("-");
    // printf("+");
    // for (machineType machine = lathe; machine < COUNT_MACHINES; machine++)
    //     printf("-----------+");
    // printf("\n");

    // printf("%*s |", maxLen, "Time Start");
    // for (machineType machine = lathe; machine < COUNT_MACHINES; machine++)
    //     printf(" %9d |", company[machine].timeStart);
    // printf("\n");

    // printf("%*s |", maxLen, "Time Finish");
    // for (machineType machine = lathe; machine < COUNT_MACHINES; machine++)
    //     printf(" %9d |", company[machine].timeFinish);
    // printf("\n");
    int start = 0;
    int finish = timeFinish + 1;
    while (finish != timeFinish)
    {
        finish = timeFinish + 1;
        printf("--------------------------------------------------------------\n");
        printf("Start = %d\n", start);
        for (machineType machine = lathe; machine < COUNT_MACHINES; machine++)
        {
            timetableType* currMachine = &company[machine];
            while (currMachine->timeFinish <= start)
                currMachine = currMachine->next;
            if (finish > currMachine->timeFinish)
                finish = currMachine->timeFinish;
            int finished = 0;
            if (currMachine->operation != NO_NEXT)
                finished = currMachine->timeFinish;
            printf("          %9s | Finished: %3d | %s\n", getNameMashine(machine), finished, operations[currMachine->operation].result);
        }
        printf("Finish = %d\n", finish);
        printf("--------------------------------------------------------------\n");
        start = finish;
    }
}

int main()
{
    const int maxAnswer = 5;
    timetableType company[COUNT_MACHINES];
    operation operations[MAX_OPERATIONS];
    int countOperations = 0;
    int timeFinish = NO_FINISH;
    printf("Program for creating production timetable\n");
    print_menu();
    int answer = get_answer(maxAnswer);
    while(answer != 0)
    {
        switch (answer)
        {
        case 1:
            countOperations = 0;
            readOperationsFile(operations, &countOperations);
            createProduct(company, operations, countOperations, &timeFinish);
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
                printf("For create product neded: %d minute\n", timeFinish);
                printf("--------------------------------------------------------------\n");
            }
            else
                printErrorEmpty();
            break;
        case 4:
            if (countOperations)
                printCompanyTimetable(company, operations, countOperations, timeFinish);
            else
                printErrorEmpty();
            break;
        case 5:
            if (countOperations)
                printMachineTimetable(company, operations, countOperations, timeFinish);
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
