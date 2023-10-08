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
#include <stdbool.h>

typedef struct nd
{
    int num;
    bool isLeaf;
    //!
} node;

void printlinkList(FILE* file_out, int nodesCount, int linkList[nodesCount][nodesCount])
{
    fprintf(file_out, "link     ");
    for (int i = 0; i < nodesCount; ++i)
        {
            fprintf(file_out, "%6d   ", i + 1);
        }
    fprintf(file_out, "\n-------+-");
    for (int i = 0; i < nodesCount; ++i)
        {
            fprintf(file_out, "-------+-", i + 1);
        }
    fprintf(file_out, "\n");
    for (int i = 0; i < nodesCount; ++i)
    {
        fprintf(file_out, "%-6d | ", i + 1);
        for (int j = 0; j < nodesCount; ++j)
        {
            fprintf(file_out, "%6d | ", linkList[i][j]);
        }
        fprintf(file_out, "\n");
    }
    fprintf(file_out, "-------+-");
    for (int i = 0; i < nodesCount; ++i)
        {
            fprintf(file_out, "-------+-", i + 1);
        }
    fprintf(file_out, "\n");
}

int main(int argc, char* argv[])
{
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
    int linkList[nodesCount][nodesCount];
    for (int i = 0; i < nodesCount; ++i)
    {
        for (int j = 0; j < nodesCount; ++j)
        {
            linkList[i][j] = nodesCount + 1;        // минимальное не возможное значение пути
        }
    }
    int n1, n2;
    while (fscanf(file_in, "%d %d", &n1, &n2) != EOF)
    {
        fgetc(file_in);
        linkList[n1 - 1][n2 - 1] = 0;
        linkList[n2 - 1][n1 - 1] = 0;
    }
    printlinkList(file_out, nodesCount, linkList);



    fclose(file_in);
    fclose(file_out);
    return 0;
}
