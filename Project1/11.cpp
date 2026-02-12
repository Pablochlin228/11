#include <condition_variable>
#include <mutex>
#include <vector>
#include <thread>
#include <fstream>
#include <string>
#include <iostream>

std::mutex m;
std::condition_variable cv;
std::vector<int> data;
int completeNumbersCount = 0; // счетчик обработанных чисел
bool finished = false;

void producer()
{
	std::ifstream in("input.txt");
	std::string buff;

	if (in.is_open())
	{
		while (std::getline(in, buff)) //getline считывает по одному числу
		{
			{
				std::lock_guard<std::mutex> lock(m);
				data.push_back(stoi(buff)); //Преобразуем строку в int(stoi) и добавляем в массив 
			}
			cv.notify_one(); // оповещаем обработчика о новом числе(работе)
		}

		{
			std::lock_guard<std::mutex> lock(m);
			finished = true;    // меняем состояние под мьютексом
		}
		cv.notify_all(); // оповещаем всех обработчиков
	}
	else
	{
		std::cout << "Could not open the file" << std::endl;
	}

	in.close();
}

void consumer()
{
	for (;;)
	{
		std::unique_lock<std::mutex> lock(m);

		// ждём, пока появятся элементы, либо всё окончательно завершится
		cv.wait(lock, [] { return completeNumbersCount < data.size() || finished; });

		// пока счетчик меньше размера масива выполняем действие
		if (completeNumbersCount < data.size()) {
			
			int num = data[completeNumbersCount];
			completeNumbersCount++;
			
			// выходим из критической секции до обработки
			lock.unlock();

			// выводим квадрат
			std::cout << "Square of number " << num << " is " << num * num << "\n";
		}
		else if (finished) {
			//  если новых чисел нет - выходим из цикла
			break;
		}
	}
}


int main()
{
	std::thread t1(producer);
	std::thread t2(consumer);

	t1.join();
	t2.join();
}