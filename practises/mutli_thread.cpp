#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h> /* 亂數相關函數 */
#include <time.h> /* 時間相關函數 */
#include <chrono>
// #include <map>
// #include <mutex>
using namespace std;

int s_num = 10;
vector<int> bf_state;
bool lock=false;
vector<bool> waiting(s_num, false);
int collision = 0;
vector<bool> finished(s_num, false);

bool test_and_set(bool *target) {
    bool tmp = *target;
    *target = true;
    return tmp;
}

bool send(int i, int backoff_counter) {
    ++collision;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    cout <<"STA ID = " << i << ", Backoff Counter = " << backoff_counter << ", Data Sent" << endl;
    bf_state[i] = -1;
    int tmp = collision;
    collision = 0;
    if (tmp>1) {
        cout<<"Send Fail\n";
        return false;
    }
    return true;
}

void Station(int i, string s)
{
    int backoff_counter;
    do {
        if (bf_state[i] == -1) backoff_counter = rand() %7+1;
        else backoff_counter=bf_state[i];
        do {
            waiting[i] = true;
            bool key = true;
            while (waiting[i]&&key) key=test_and_set(&lock);
            waiting[i] = false;
            backoff_counter--;
            cout <<"STA ID = "<< i << ", Backoff Counter = " << backoff_counter << ", Data Queued"<< endl;
            int j = (i+1)%s_num;
            while ((j!=i)&&!waiting[j]) j=(j+1)%s_num;
            if (j==i) lock=false;
            else waiting[j] = false;
        } while (backoff_counter);
    }while (!send(i, backoff_counter)); // like await, check send is successful
    // move this to function send
    // cout <<"STA ID = " << i << ", Backoff Counter = " << backoff_counter << ", Data Sent" << endl;
    // bf_state[i] = -1;
    // --------------------------
}

int main()
{
    srand( time(NULL) ); /* 產生亂數 */
    vector<thread> threads;
    for(int i = 0; i < s_num; i++){
        bf_state.push_back(-1);
        threads.push_back(thread(Station, i, "test"));
    }
    for(int i = 0; i < threads.size(); i++){
        //cout << threads[i].get_id() << endl;
        threads[i].join();
    }
    return 0;
}