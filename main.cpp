#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

template <typename T>
void print(vector<T> &arr)
{
    for (auto i = arr.begin(); i< arr.end(); i++)
        cout << *i << " ";
    cout << endl;
}


queue< pair<int, int> > tasks;

mutex q_mutex, s_mutex;

condition_variable cv;

set<int> ss;


template <typename T>
int partition(vector<T> &arr, bool dest, int l, int r)
{
    T tmp = arr[r];
    int i = l - 1;

    for (int j = l; j <= r - 1; j++)
        if (dest)
        {
            if (arr[j] < tmp)
            {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        else
        {
            if (arr[j] > tmp)
            {
                i++;
                swap(arr[i], arr[j]);
            }
        }

    swap(arr[i + 1], arr[r]);
    i++;
    return i;
}


template <typename T>
void quick_sort(vector<T> & arr, bool dest)
{
    while (true)
    {
        unique_lock<mutex> u_lock(q_mutex);


        if ( ss.size() == arr.size() )
            return;


        if ( tasks.size() > 0 )
        {

            pair<int, int> cur_task = tasks.front();
            tasks.pop();

            int l = cur_task.first, r = cur_task.second;


            if (l < r)
            {
                int q = partition(arr, dest, l, r);


                s_mutex.lock();
                ss.insert(q);
                ss.insert(l);
                ss.insert(r);
                s_mutex.unlock();


                tasks.push( make_pair(l, q - 1) );
                tasks.push( make_pair(q + 1, r) );


                cv.notify_one();
            }

        }
        else

            cv.wait(u_lock);
    }
}




template <typename T, int threads_count, bool dest>
void sort(vector<T> & arr)
{

    tasks.push( make_pair(0, arr.size() - 1) );

    thread thrs[threads_count];
    for (int i = 0; i < threads_count; i++)
        thrs[i] = thread( quick_sort<T>, ref(arr), dest);

    for (auto& th : thrs)
        th.join();

    ss.clear();

}


class my
{
    public:
    int x,y;

    my(int x, int y) : x(x), y(y) {};
    bool operator<(my & obj)
    {
        return (x < obj.x);
    }
    bool operator>(my & obj)
    {
        return (x > obj.x);
    }
    bool operator<=(my & obj)
    {
        return (x <= obj.x);
    }
    bool operator>=(my & obj)
    {
        return (x >= obj.x);
    }
    bool operator==(my & obj)
    {
        return (x == obj.x);
    }


    my & operator=(my & nobj)
    {
        x = nobj.x;
        y = nobj.y;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const my& ob);

};

void swap (my & a, my & b)
{
    my temp(b.x, b.y);
    b = a;
    a = temp;
}

std::ostream& operator<<(std::ostream& os, const my& ob)
{
    os << "{" <<ob.x << ", " << ob.y << "}  ";
    return os;
}

int main()
{
    vector<int> arr1;
    vector<int> arr2;

    int val;
    for(int i = 0 ; i < 100; i++)
    {
        val = rand() % 10101;
        arr1.push_back(val);
        arr2.push_back(val);
    }
    print<int>(arr1);
    std::cout << "_________________________________________________" << std::endl;
    sort<int, 2, true>(arr1);
    print<int>(arr1);
    std::cout << "_________________________________________________" << std::endl;
    sort<int, 2, false>(arr2);

    print<int>(arr2);

    vector<my> arr_my;

    std::cout << "_________________________________________________" << std::endl;
    for(int i = 0; i < 10; i++)
    {
        my my_obj(rand() % 101, rand() % 7);
        arr_my.push_back(my_obj);
    }
    print<my>(arr_my);
    sort<my, 2, true>(arr_my);
    print<my>(arr_my);
}