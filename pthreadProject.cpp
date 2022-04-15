#define HAVE_STRUCT_TIMESPEC

#include <iostream>
#include <pthread.h>
#include <cmath>

int N;
int squareThreadCount;
int simpleThreadCount;
int squareCount = 0;
int simpleCount = 0;
int squareElements;
int naturalElements;
pthread_t* simpleThreads;
pthread_t* squareThreads;
pthread_mutex_t mutex;

void* SquareThreadFunc(void* args)
{
    int index = *(static_cast<int*>(args));
    double temp;
    for (int i = 0; i < squareElements; i++)
    {
        if (squareElements * index + i > N) return 0;
        if (modf(sqrt(squareElements * index + i), &temp) == 0)
        {
            pthread_mutex_lock(&mutex);
            squareCount++;
            pthread_mutex_unlock(&mutex);
        }
    }

    return 0;
}

void* SimpleThreadFunc(void* args)
{
    int index = *(static_cast<int*>(args));
    double temp;
    for (int i = index * naturalElements; i <= (index + 1) * naturalElements; i++)
    {
        if (i == 0) i = 1;
        if (i == 2) { simpleCount++; continue; }
        if (i == 3) { simpleCount++; continue; }
        for (int j = 2; j < sqrt((index + 1) * naturalElements + i); j++)
        {
            if (modf((double)i / j, &temp) == 0) goto endOfFor;
        }
        simpleCount++;
    endOfFor:
        continue;
    }

    return 0;
}

void findSquareCount()
{
    int* threadIndexes = new int[squareThreadCount];
    for (int i = 0; i < squareThreadCount; i++)
        threadIndexes[i] = i;

    squareElements = floor((double)N / squareThreadCount);
    for (int i = 0; i < squareThreadCount; i++)
        pthread_create(&squareThreads[i], NULL, SquareThreadFunc, &threadIndexes[i]);

    return;
}

void findSimpleCount()
{
    int* threadIndexes = new int[simpleThreadCount];
    for (int i = 0; i < simpleThreadCount; i++)
        threadIndexes[i] = i;

    naturalElements = floor((double)N / simpleThreadCount);
    for (int i = 0; i < simpleThreadCount; i++)
        pthread_create(&simpleThreads[i], NULL, SimpleThreadFunc, &threadIndexes[i]);

    return;
}

int main()
{
    setlocale(LC_ALL, "rus");
    std::cout << "Введите N: ";
    std::cin >> N;
    std::cout << "Введите количество потоков для подсчёта количества квадратов: ";
    std::cin >> squareThreadCount;
    std::cout << "Введите количество потоков для подсчёта количества простых чисел: ";
    std::cin >> simpleThreadCount;

    simpleThreads = new pthread_t[simpleThreadCount];
    squareThreads = new pthread_t[squareThreadCount];
    pthread_mutex_init(&mutex, NULL);

    findSquareCount();
    findSimpleCount();

    for (int i = 0; i < simpleThreadCount; i++)
        pthread_join(simpleThreads[i], NULL);
    for (int i = 0; i < squareThreadCount; i++)
        pthread_join(squareThreads[i], NULL);

    std::cout << "Количество квадратов: " << squareCount << std::endl;
    std::cout << "Количество простых чисел: " << simpleCount << std::endl;
    pthread_mutex_destroy(&mutex);

    return 0;
}