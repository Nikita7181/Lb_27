#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <thread>
#include <time.h>
#include <iomanip>

#define ARR_SIZE 20

template <typename T>
void print (std::vector<T> arr)
{
    for (int i = 0; i < arr.size(); i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;
}

template <typename T>
void fill (std::vector<T> & arr)
{
    for (int i = 0; i < ARR_SIZE; i++)
        arr.push_back (rand() % (ARR_SIZE * 10));
}



template <typename T, int thread_count>
std::vector<T> * split (std::vector<T> arr)
{
    std::vector<T> * splitted = new std::vector<T>[thread_count];
    int split_size = arr.size() / thread_count;
    int delta = arr.size() % thread_count;

    int i = 0;
    for (i = 0; i < thread_count - 1; i++)
    {
        splitted[i] = std::vector<T>(arr.begin() + split_size * i, arr.begin() +  split_size * i + split_size );
    }
    splitted[i] = std::vector<T>(arr.begin() + split_size * i, arr.begin()  + split_size * i + split_size + delta);

    return splitted;
}

template <typename T, int thread_count, bool dest>
std::vector<T> merge (std::vector<T> * splitted)
{
    std::vector<T> arr;
    int first_non_empty = 0;
    T min_val;// = splitted[first_non_empty][0];
    T new_min_val;// = splitted[first_non_empty][0];
    int min_index;// = first_non_empty;
    int new_min_index;// = first_non_empty;

    bool empty = false;

    while (!empty)
    {
        min_val = splitted[first_non_empty][0];
        new_min_val = splitted[first_non_empty][0];
        min_index = first_non_empty;
        new_min_index = first_non_empty;

        for (int i = 0; i < thread_count; i++)
        {
            if (splitted[i].size() > 0)
            {
                if (dest)
                {

                    if (splitted[i][0] < min_val)
                    {
                        new_min_val = splitted[i][0];
                        new_min_index = i;
                    }
                    if ( new_min_val < min_val)
                    {
                        min_val = new_min_val;
                        min_index = new_min_index;
                    }
                }
                else
                {
                    if (splitted[i][0] > min_val)
                    {
                        new_min_val = splitted[i][0];
                        new_min_index = i;
                    }
                    if ( new_min_val > min_val)
                    {
                        min_val = new_min_val;
                        min_index = new_min_index;
                    }
                }
            }


        }

        arr.push_back(splitted[min_index].front());
        splitted[min_index].erase(splitted[min_index].begin());


        for (int i = 0; i < thread_count; i++)
        {
            if (splitted[i].size() > 0)
            {
                empty = false;
                first_non_empty = i;
                break;
            }
            empty = true;
        }
    }

    return arr;
}

template <typename T, bool dest>
int Partition (std::vector <int> &vec, int start, int end) //принимает часть массива и относительно опорной функции переноси меньгие влево а большие вправо
{
    int value = vec[end];
    int position = start;

    for(int i = start; i < end; i++)
    {
        if (dest)
        {
            if (vec[i] <= value)
            {
                int temp = vec[i];
                vec[i] = vec[position];
                vec[position] = temp;
                position++;
            }
        }
        else
        {
            if (vec[i] >= value)
            {
                int temp = vec[i];
                vec[i] = vec[position];
                vec[position] = temp;
                position++;
            }
        }
    }
    vec[end] = vec[position];
    vec[position] = value;
    return position;
}

template <typename T, bool dest>
void quicksort(std::vector<T> &vec, int start, int end)
{
    if(start > end)
    {
        return;
    }
    int pivot = Partition<T, dest>(vec, start, end);
    quicksort<T,dest >(vec, start, pivot-1);
    quicksort<T,dest>(vec, pivot + 1, end);
}

template <typename T, int thread_count, bool dest>
void sort (std::vector<T> & arr)
{
    clock_t start = clock();

    std::vector<T> * tst = split<T, thread_count>(arr);

    std::thread thrs[thread_count];
    for (int i = 0; i < thread_count; i++)
    {
        //thrs[i] = std::thread( Insertion_Sort<T>, ref(tst[i]) );
        thrs[i] = std::thread( quicksort<T, dest>, ref(tst[i]), 0, tst[i].size()-1 );
    }


    for (auto& th : thrs)
        th.join();

    arr = merge<T,thread_count, dest> (tst);

    clock_t end = clock();
    double sec = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout << "Execution time: " << std::fixed << std::setprecision(10) << sec << " sec\n";
}


int main ()
{
    std::vector<int> arr;
    fill(arr);
    print (arr);

    //std::vector<int> * tst = split<int, 4>(arr);
    //for (int i = 0; i < 4; i++)
    //    print (tst[i]);

    sort<int, 2, false> (arr);



    print (arr);
}
