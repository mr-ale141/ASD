/*
            Деревья. Задача №24. (на языке С)
В офисе фирмы  Megasoft  установлены  N  компьютеров  с 
номерами от 1 до N, некоторые из них  соединены  между  собой. 
Сообщение между соединенными компьютерами проходит в любом  из 
двух направлений за 1 с. Компьютер,  получив  сообщение, сразу 
отправляет  его  всем  соединенным  с  ним  компьютерам.  Cеть 
устроена так, что между любыми двумя компьютерами  есть  путь, 
причем только один. Найти номера всех компьютеров,  с  которых 
главный программист Гилл Бейтс может отправить сообщение  так, 
чтобы максимальная задержка в  получении  сообщения  была  как 
можно меньше. 
    Ввод из файла INPUT.TXT. В первой строке вводится значение N 
(1<=N<=10^5). В каждой из следующих N-1  строк  вводится через 
пробел пара номеров компьютеров, обозначающая соединение.
    Вывод в файл OUTPUT.TXT. В первой строке выводится количество
искомых компьютеров M. Во второй строке выдаются через пробел в
порядке возрастания номера искомых компьютеров.
    Время счета не должно превышать 2 сек.
    Пример
    Ввод
4
1 2
4 3
2 3
    Вывод
2
2 3
    Указание. Предложить структуру данных, обеспечивающую быстрое
нахождение листьев бескорневого дерева из условия задачи (14).
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef struct structListPC {
    int numPC;
    struct structListPC* next;
} typeListPC;

typedef struct structPC {
    int linkCount;
    int minDist;
    typeListPC* listLinks;
} typePC;

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

void printQ(FILE* file_out)
{
    queue* elt;
    elt = firstQ;
    int count = 1;
    if (elt != NULL)
    {
        fprintf(file_out, "Queue#   Num PC\n");
        fprintf(file_out, "-------+--------+-\n");
        while (elt != NULL)
        {
            fprintf(file_out, "%-6d | %6d |\n", count, elt->numPC);
            elt = elt->next;
            count++;
        }
        fprintf(file_out, "-------+--------+-\n");
    }
    else
        fprintf(file_out, "Queue is empty!\n");
}

void addPCInLinks(typePC* targetPC, int numPC)
{
    targetPC->linkCount += 1;
    typeListPC* oldListPC = targetPC->listLinks;
    targetPC->listLinks = malloc(sizeof(typeListPC));
    targetPC->listLinks->numPC = numPC;
    targetPC->listLinks->next = oldListPC;
}

void createNetwork(FILE* file_in, const int nodesCount, typePC* network[nodesCount])
{
    int n1, n2;
    while (fscanf(file_in, "%d %d", &n1, &n2) != EOF)
    {
        fgetc(file_in);
        addPCInLinks(network[n1 - 1], n2);
        addPCInLinks(network[n2 - 1], n1);
    }
}

void initNetwork(int nodesCount, typePC* network[nodesCount])
{
    for (int i = 0; i < nodesCount; ++i)
    {
        typePC* elt;
        elt = malloc(sizeof(typePC));
        elt->linkCount = 0;
        elt->minDist = INT_MAX;
        elt->listLinks = NULL;
        network[i] = elt;
    }
}

void printNetwork(FILE* file_out, int nodesCount, typePC* network[nodesCount])
{
    fprintf(file_out, " PC      Links    minDist      Links\n");
    fprintf(file_out, "-------+--------+------------+-\n");
    for (int i = 0; i < nodesCount; ++i)
    {
        fprintf(file_out, "%-6d | %-6d | %-10d | ", i + 1, network[i]->linkCount, network[i]->minDist);
        typeListPC* listLinks = network[i]->listLinks;
        while(listLinks != NULL)
        {
            fprintf(file_out, " %d |", listLinks->numPC);
            listLinks = listLinks->next;
        }
        fprintf(file_out, "\n");
    }
    fprintf(file_out, "-------+--------+------------+-\n");
}

void printAnswer(FILE* file_out, int answer[2])
{
    if (answer[1] == 0)
        fprintf(file_out, "1\n%d\n", answer[0]);
    else
    {
        int min = (answer[0] < answer[1]) ? answer[0] : answer[1];
        int max = (min == answer[0]) ? answer[1] : answer[0];
        fprintf(file_out, "2\n%d %d\n", min, max);
    }
}

void addLeavesInQueue(int nodesCount, typePC* network[nodesCount])
{
    for (int i = 0; i < nodesCount; ++i)
    {
        if (network[i]->linkCount == 1) 
        {
            network[i]->minDist = 0;
            addQ(i + 1);
        }
    }
}

void deleteLinkInList(int numCurrPC, typePC* currPC, typePC* linkPC)
{
    currPC->linkCount--;
    linkPC->linkCount--;
    free(currPC->listLinks);
    currPC->listLinks = NULL;
    typeListPC* link = linkPC->listLinks;
    typeListPC* oldLink = NULL;
    while (link->numPC != numCurrPC)
    {
        oldLink = link;
        link = oldLink->next;
    }
    if (oldLink != NULL)
        oldLink->next = link->next;
    else
        linkPC->listLinks = link->next;
    free(link);
}

int main(int argc, char* argv[])
{
    double timerStart = (double) clock();
    if (argc < 3) 
    {
        printf("For start ->: name_program.exe fale_in.txt file_out.txt\n");
        return 0;
    }
    FILE* file_in;
    FILE* file_out;
    if (NULL == (file_in = fopen(argv[1], "r"))) return 1;
    if (NULL == (file_out = fopen(argv[2], "w"))) return 1;
    int nodesCount;
    fscanf(file_in, "%d", &nodesCount);
    fgetc(file_in);
    typePC* network[nodesCount];
    initNetwork(nodesCount, network);
    createNetwork(file_in, nodesCount, network);
    addLeavesInQueue(nodesCount, network);
    // printNetwork(file_out, nodesCount, network);
    // printQ(file_out);
    int answer[2] ={0, 0};
    queue* q = NULL;
    while (q = getQ())
    {
        int currPC = q->numPC;
        if (network[currPC - 1]->listLinks == NULL) break;
        int linkPC = network[currPC - 1]->listLinks->numPC;
        if (network[linkPC - 1]->minDist == INT_MAX)
            network[linkPC - 1]->minDist = network[currPC - 1]->minDist + 1;
        else if (network[linkPC - 1]->minDist < network[currPC - 1]->minDist + 1 && network[linkPC - 1]->linkCount != 1)
            network[linkPC - 1]->minDist = network[currPC - 1]->minDist + 1;
        else if (network[linkPC - 1]->minDist == network[currPC - 1]->minDist)
        {
            answer[0] = linkPC;
            answer[1] = currPC;
        }
        else if (network[linkPC - 1]->minDist + 1 == network[currPC - 1]->minDist || network[linkPC - 1]->minDist == network[currPC - 1]->minDist + 1)
        {
            answer[0] = linkPC;
            answer[1] = 0;
        }
        deleteLinkInList(currPC, network[currPC - 1], network[linkPC - 1]);
        if (network[linkPC - 1]->linkCount == 1)
            addQ(linkPC);
        free(q);
    }
    // printQ(file_out);
    // printNetwork(file_out, nodesCount, network);
    printAnswer(file_out, answer);
    fclose(file_in);
    fclose(file_out);
    double timerStop = (double) clock();
    double timeDiff = timerStop / CLOCKS_PER_SEC - timerStart / CLOCKS_PER_SEC;
    printf("%lf\n", timeDiff);
    return 0;
}
