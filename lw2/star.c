/*
            Деревья. Задача №24. (на языке Си)
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