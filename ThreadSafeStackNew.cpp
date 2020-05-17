#include<exception>
#include<mutex>
#include<stack>
#include<thread>
#include<vector>
#include<iostream>
#include<queue>

struct empty_stack_exp : std::exception {
    const char* what() const noexcept override {
        return "empty stack exception";
    }
};


template<typename T>
class ThreadSafeStack {
    private:
        mutable std::mutex mtx_;
        std::stack<T> data_;
    public:
        ThreadSafeStack() {}
        ThreadSafeStack(const ThreadSafeStack& sourceStack) {
            std::lock_guard<std::mutex> lg(sourceStack.mtx_);
            data_ = sourceStack.data_;
        }
        ~ThreadSafeStack() {};
        ThreadSafeStack& operator=(const ThreadSafeStack&) = delete;

        void push(T value) {
            std::lock_guard<std::mutex> lg(mtx_);
            data_.push(std::move(value));
        }

        std::shared_ptr<T> pop() {
            std::lock_guard<std::mutex> lg(mtx_);
            if (data_.empty()) {
                throw empty_stack_exp();
                // throw;
            }
            const auto returnValue = std::make_shared<T>(std::move(data_.top()));
            data_.pop();
            return returnValue;
        }

        void pop(T& value) {
            std::lock_guard<std::mutex> lg(mtx_);
            if (data_.empty()) {
                throw empty_stack_exp();
                // throw;
            }
            value = std::move(data_.top());
            data_.pop();
        }

        bool empty() const {
            std::lock_guard<std::mutex> lg(mtx_);
            return data_.empty();
        }
};

int main() {
    ThreadSafeStack<std::string> testStack;
    std::vector<std::thread> threadList;
    for (int i = 0; i < 100; i++) {
        threadList.push_back(std::thread(&ThreadSafeStack<std::string>::push, &testStack, std::to_string(i)));
    }

    for (auto& ele : threadList) {
        ele.join();
    }
    while (!testStack.empty()) {
        auto value = testStack.pop();
        std::cout << *value << std::endl; // shared_ptr.get() access to raw pointer
    }
    return 0;
}
