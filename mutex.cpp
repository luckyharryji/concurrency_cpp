#include<vector>
#include<mutex>
#include<thread>
#include<iostream>

using namespace std;

vector<int> result;
mutex main_mutex;

void addToResult(int value) {
    lock_guard<mutex> guard(main_mutex);
    result.push_back(value);
}

bool contains(int value) {
    lock_guard<mutex> guard(main_mutex);
    return find(result.begin(), result.end(), value)
        == result.end();
}


int main() {
    vector<thread> threadList;
    for (int i = 0; i < 100; i++) {
        threadList.push_back(thread(addToResult, i));
    }
    for (int i = 0; i < 100; i++) {
        threadList[i].join();
    }
    for (const auto& num : result) {
        cout << num << endl;
    }
    return 0;
}
