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

typedef struct n {
    int linkCount;
    int isVisited;
    int minDist;
} node;

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

void printLinks(FILE* file_out, int nodesCount, int links[nodesCount][nodesCount])
{
    fprintf(file_out, "Link     ");
    for (int i = 0; i < nodesCount; ++i)
        fprintf(file_out, "%6d   ", i + 1);
    fprintf(file_out, "\n-------+-");
    for (int i = 0; i < nodesCount; ++i)
        fprintf(file_out, "-------+-", i + 1);
    fprintf(file_out, "\n");
    for (int i = 0; i < nodesCount; ++i)
    {
        fprintf(file_out, "%-6d | ", i + 1);
        for (int j = 0; j < nodesCount; ++j)
        {
            if (links[i][j])
                fprintf(file_out, "%6d | ", links[i][j]);
            else
                fprintf(file_out, "%06d | ", links[i][j]);
        }
        fprintf(file_out, "\n");
    }
    fprintf(file_out, "-------+-");
    for (int i = 0; i < nodesCount; ++i)
        fprintf(file_out, "-------+-", i + 1);
    fprintf(file_out, "\n");
}

void printNodes(FILE* file_out, int nodesCount, node* nodes[nodesCount])
{
    fprintf(file_out, "Node     Links    Visited  minDist\n");
    fprintf(file_out, "-------+--------+--------+--------+-\n");
    for (int i = 0; i < nodesCount; ++i)
    {
        fprintf(file_out, "%-6d | %-6d | %-6d | %-6d | \n", i + 1, nodes[i]->linkCount, nodes[i]->isVisited, nodes[i]->minDist);
    }
    fprintf(file_out, "-------+--------+--------+--------+-\n");
}

void printQ(FILE* file_out)
{
    queue* elt;
    elt = firstQ;
    int count = 1;
    if (elt != NULL)
    {
        fprintf(file_out, "Queue#   Num PC\n", count, elt->numPC);
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

void printAnswer(FILE* file_out, int answer[2])
{
    fprintf(file_out, "Answer:\n");
    if (answer[1] == 0)
        fprintf(file_out, "1\n%d\n", answer[0]);
    else
    {
        int min = (answer[0] < answer[1]) ? answer[0] : answer[1];
        int max = (min == answer[0]) ? answer[1] : answer[0];
        fprintf(file_out, "2\n%d %d\n", min, max);
    }
}

void initNodes(int nodesCount, node* nodes[nodesCount])
{
    for (int i = 0; i < nodesCount; ++i)
    {
        node* elt;
        elt = malloc(sizeof(node));
        elt->isVisited = 0;
        elt->linkCount = 0;
        elt->minDist = nodesCount + 1;        // не возможное значение пути
        nodes[i] = elt;
    }
}

void initLinks(int nodesCount, int links[nodesCount][nodesCount])
{
    for (int i = 0; i < nodesCount; ++i)
    {
        for (int j = 0; j < nodesCount; ++j)
        {
            links[i][j] = 0;
        }
    }
}

void updateLinksNodes(FILE* file_in, int nodesCount, int links[nodesCount][nodesCount], node* nodes[nodesCount])
{
    int n1, n2;
    while (fscanf(file_in, "%d %d", &n1, &n2) != EOF)
    {
        fgetc(file_in);
        links[n1 - 1][n2 - 1] = 1;
        links[n2 - 1][n1 - 1] = 1;
        nodes[n1 - 1]->linkCount++;
        nodes[n2 - 1]->linkCount++;
    }
}

void addLeavesInQueue(int nodesCount, node* nodes[nodesCount])
{
    for (int i = 0; i < nodesCount; ++i)
    {
        if (nodes[i]->linkCount == 1) 
        {
            nodes[i]->minDist = 0;
            addQ(i + 1);
        }
    }
}

int main(int argc, char* argv[])
{
    const int not = 0;
    if (argc < 3) 
    {
        printf("For start ->: name_program.exe fale_in.txt file_out.txt\n");
        return 0;
    }
    FILE* file_in;
    FILE* file_out;
    if (NULL == (file_in = fopen(argv[1], "r"))) return 0;
    if (NULL == (file_out = fopen(argv[2], "w"))) return 0;
    int nodesCount;
    fscanf(file_in, "%d", &nodesCount);
    fgetc(file_in);
    node* nodes[nodesCount];
    initNodes(nodesCount, nodes);
    int links[nodesCount][nodesCount];
    initLinks(nodesCount, links);
    updateLinksNodes(file_in, nodesCount, links, nodes);
    addLeavesInQueue(nodesCount, nodes);
    int answer[2] ={0, 0};
    // printLinks(file_out, nodesCount, links);
    // printNodes(file_out, nodesCount, nodes);
    // printQ(file_out);
    queue* q;
    while (q = getQ())
    {
        if (nodes[q->numPC - 1]->isVisited == not)
        {
            nodes[q->numPC - 1]->isVisited++;
            for (int i = 0; i < nodesCount; ++i)
            {
                if (links[q->numPC - 1][i] == 1)
                {
                    if (nodes[i]->isVisited == not && nodes[i]->minDist == nodesCount + 1)
                        nodes[i]->minDist = nodes[q->numPC - 1]->minDist + 1;
                    else if (nodes[i]->minDist < nodes[q->numPC - 1]->minDist + 1 && nodes[i]->linkCount != 1)
                        nodes[i]->minDist = nodes[q->numPC - 1]->minDist + 1;
                    else if (nodes[i]->minDist == nodes[q->numPC - 1]->minDist)
                    {
                        answer[0] = i + 1;
                        answer[1] = q->numPC;
                    }
                    else if (nodes[i]->minDist + 1 == nodes[q->numPC - 1]->minDist || nodes[i]->minDist == nodes[q->numPC - 1]->minDist + 1)
                    {
                        answer[0] = i + 1;
                        answer[1] = 0;
                    }
                    links[i][q->numPC - 1]--;
                    links[q->numPC - 1][i]--;
                    if (nodes[q->numPC - 1]->linkCount == 1)
                        nodes[q->numPC - 1]->linkCount--;
                    nodes[i]->linkCount--;
                    if (nodes[i]->linkCount == 1)
                            addQ(i + 1);
                }
            }
        }
        free(q);
    }
    // printNodes(file_out, nodesCount, nodes);
    printAnswer(file_out, answer);
    fclose(file_in);
    fclose(file_out);
    return 0;
}
