//📝** ЗАДАНИЕ 1:(продвинутый уровень)
//«Потокобезопасная обработка данных из файла с использованием глобального массива» * *
//📌 Условие
//
//Дан текстовый файл input.txt, содержащий N целых чисел(одно число в строке).
//Создать глобальный массив int data[], размер которого равен количеству чисел в файле.
//
//Написать две функции, выполняемые в потоках :
//Поток 1 (producer — читатель) :
//	*Открывает файл
//	* Последовательно читает числа
//	* Помещает их в глобальный массив
//	* После записи каждого числа отправляет сигнал другому потоку через std::condition_variable
//	* Завершает поток, установив глобальный флаг finished = true и уведомив все ожидающие потоки
//
//	Поток 2 (consumer — обработчик) :
//	*Ожидает появления новых чисел в глобальном массиве
//	* Как только появляется новое число — выводит его квадрат
//	* Завершает работу, когда finished == true и новые элементы больше не поступают
//
//	📌 Требования
//
//	Не использовать using namespace std; (можно using std::cout и т.п.)
//	Использовать:
//
//std::mutex
//std::condition_variable
//глобальный массив
//файл
//
//Обязательно использовать предикат в cv.wait()
//Добавить комментарии

#include <condition_variable>
#include <mutex>
#include <vector>
#include <thread>
#include <fstream>
#include <string>
#include <iostream>

const int size = 10;
std::mutex m;
std::condition_variable cv;
std::vector<int> data[size];
bool finished = false;

void producer()
{
	std::ifstream in("input.txt", std::ios::trunc);
	std::string buff;

	if (in.is_open())
	{
		while (std::getline(in, buff))
		{
			{
				std::lock_guard<std::mutex> lock(m);
				data->push_back(stoi(buff));
			}
			cv.notify_one();
		}

		{
			std::lock_guard<std::mutex> lock(m);
			finished = true;
		}
		cv.notify_all();
	}
	else
	{
		std::cout << "Could not open the file" << std::endl;
	}
}

void consumer()
{
	for (;;)
	{
		std::unique_lock<std::mutex> lock(m);

		cv.wait(lock, [] { return !data->empty() || finished; });

		if (!data->empty()) {
			// Забираем элемент работы
			int v = q.front();
			q.pop();
			
			// Выходим из критической секции до "тяжёлой" обработки
			lock.unlock();
			
			// Обработка вне мьютекса — это уменьшает блокировки для других потоков
			std::cout << "Consumer " << id << " got " << v << "\n";
		}
		else if (finished) {
			break;
		}
	}
}


int main()
{

}