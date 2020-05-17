#include<exception>
#include<mutex>
#include<stack>
#include<thread>
#include<vector>
#include<iostream>
#include<queue>

template<typename T>
class ThreadSafeQueue {
    private:
        mutable std::mutex mutx_;
        std::queue<T> dataQueue_;
        std::condition_variable dataCond_;

    public:
        ThreadSafeQueue() {}

        void push(T newValue) {
            std::lock_guard<std::mutex> lk(mutx_);
            dataQueue_.push(std::move(newValue));
            dataCond_.notify_one();
        }

        void waitAndPop(T& value) {
            std::unique_lock<std::mutex> lk(mutx_);
            dataCond_.wait(lk, [this] {return !dataQueue_.empty();});
            value = std::move(dataQueue_.front());
            dataQueue_.pop();
        }

        std::shared_ptr<T> waitAndPop() {
            std::unique_lock<std::mutex> lk(mutx_);
            dataCond_.wait(lk, [this] {return !dataQueue_.empty();});
            auto result = std::make_shared<T>(std::move(dataQueue_.front()));
            dataQueue_.pop();
            return result;
        }

        bool tryPop(T& value) {
            std::lock_guard<std::mutex> lk(mutx_);
            if (dataQueue_.empty()) {
                return false;
            }
            value = std::move(dataQueue_.front());
            dataQueue_.pop();
            return true;
        }

        std::shared_ptr<T> tryPop() {
            std::lock_guard<std::mutex> lk(mutx_);
            if (dataQueue_.empty()) {
                return nullptr;
            }
            auto result = std::make_shared<T>(std::move(dataQueue_.front()));
            dataQueue_.pop();
            return result;
        }

        bool empty() {
            std::lock_guard<std::mutex> lk(mutx_);
            return dataQueue_.empty();
        }

};
