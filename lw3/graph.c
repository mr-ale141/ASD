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
#include <wchar.h>

#define MAX_STRING 200

typedef enum machineEnum
{
    lathe,                      // токарные работы
    fraser,                     // фрезерные работы
    drilling,                   // сверлильный станок
    welding,                    // сварочные работы
    laser,                      // лазерная резка
    listBending,                // станок для гибки листов металла
    wireBending,                // станок для гибки проволоки
    grinding,                   // шлифовальные работы
    painting,                   // покраска
    plastic,                    // литье пластика
    sewing,                     // швейные работы
    soldering,                  // пайка
    programming,                // программирование
    manual,                     // ручная сборка
    testing                     // тестирование и отладка
} machine;

typedef enum statusEnum
{
    new,
    inProgress,
    completed,
} status;

typedef struct operationStruct {
    wchar_t* result;
    float minuteLimit;
    machine operationType;
    status operationStatus;
    unsigned next;
} operation;

void print_menu()
{
    wprintf(L"Select a menu item:\n");
    wprintf(L"      0 - exit;\n");
    wprintf(L"      1 - open file and write in file;\n");
    wprintf(L"Your answer >: ");
}

int get_answer()
{
    const int MAX_ANSWER = 1;
    int answer;
    do {
        if (wscanf(L"%d", &answer) == 0)
        {
            wprintf(L"--------------------------------------------------------------\n");
            wprintf(L"Your answer is not digit, correct from 0 to %d, try again.\n", MAX_ANSWER);
            wprintf(L"--------------------------------------------------------------\n");
            while (getwchar() != L'\n')
                continue;
            print_menu();
        }
        else if (answer < 0 || answer > MAX_ANSWER)
        {
            wprintf(L"--------------------------------------------------------------\n");
            wprintf(L"Your answer (%d) is incorrect, correct from 0 to %d, try again.\n", answer, MAX_ANSWER);
            wprintf(L"--------------------------------------------------------------\n");
            print_menu();
        }
    } while (answer < 0 || answer > MAX_ANSWER);
    while (getwchar() != L'\n')
        continue;
    return answer;
}

FILE* get_file_input()
{
    wchar_t str[MAX_STRING] = {0};
    FILE *file;
    wprintf(L"Insert file name for INPUT with address (max %d symbol):\n->: ", MAX_STRING);
    fgetws(str, MAX_STRING, stdin);
    (*wcschr(str, L'\n')) = L'\0';
    while (NULL == (file = _wfopen(str, L"r")))
    {
        wprintf(L"Incorrect file name \"%s\" for INPUT, try again:\n->: ", str);
        fgetws(str, MAX_STRING, stdin);
        (*wcschr(str, L'\n')) = '\0';
    }
    return file;
}

FILE* get_file_output()
{
    wchar_t str[MAX_STRING] = {0};
    FILE* file;
    wprintf(L"Insert file name for OUTPUT with address (max %d symbol):\n->: ", MAX_STRING);
    fgetws(str, MAX_STRING, stdin);
    (*wcschr(str, L'\n')) = L'\0';
    while (NULL == (file = _wfopen(str, L"w")))
    {
        printf("Incorrect file name for OUTPUT, try again:\n->: ");
        fgetws(str, MAX_STRING, stdin);
        (*wcschr(str, L'\n')) = L'\0';
    }
    return file;
}

int main()
{
    print_menu();
    int answer = get_answer();
    while(answer != 0)
    {
        FILE* file_in = get_file_input();
        FILE* file_out = get_file_output();
        int count = 0;
        wchar_t in_str[MAX_STRING] = {0};
        wchar_t out_str[MAX_STRING] = {0};
        while (fgetws(in_str, MAX_STRING, file_in))
        {
            (*wcschr(in_str, L'\n')) = L'\0';
            if (in_str[0] == L'\0' || in_str[0] == L'#') continue;
            count++;
            _swprintf(out_str, L"%d: \"%s\"\n", count, in_str);
            wprintf(L"%s\n", out_str);
            fputws(out_str, file_out);
            out_str[0] = L'\0';
            in_str[0] = L'\0';
        }
        if (0 == count)
        {
            wprintf(L"--------------------------------------------------------------\n");
            wprintf(L"Input file is empty!\n");
            wprintf(L"--------------------------------------------------------------\n");
        }
        else
        {
            wprintf(L"--------------------------------------------------------------\n");
            wprintf(L"Recorded %d elements.\n", count);
            wprintf(L"--------------------------------------------------------------\n");
        }
        fclose(file_in);
        fclose(file_out);
        print_menu();
        answer = get_answer();
    }
}