# concurrency_cpp


##  basics
- ```join()``` when exception happens
    - simple: do join() inside ```catch(...)```
    - design pattern of : RAII. join() for joinable object in deconstructor.
    - detach(), if the thread-function needs to remain alive after thread object destroyed


**Argument passing:**
```cpp
void f(int i,std::string const& s);
void oops(int some_param) {
    char buffer[1024];
    sprintf(buffer, "%i", some_param);
    std::thread t(f, 3, buffer);
    t.detach();
}
```

for this function, converting `buffer` to `std::string` could finish before function `opps()` returns, lead to undefined behavior.

change to:

```cpp
void f(int i,std::string const& s);
void oops(int some_param) {
    char buffer[1024];
    sprintf(buffer, "%i", some_param);
    std::thread t(f, 3, std::string(buffer));
    t.detach();
}
```

Thread default copy value into function pointer. Generally be careful when passing reference to a thread, use `std::ref()` (https://ncona.com/2018/08/passing-by-reference-to-a-thread-in-c/)


Use `std::move()` when passing non-copyable unique_ptr or other object.


**Transferring ownership of a thread**


```cpp
void some_function();
void some_other_function();
std::thread t1(some_function);
std::thread t2=std::move(t1);
t1=std::thread(some_other_function);
std::thread t3;
t3=std::move(t2);
t1=std::move(t3); // program will crash here since it can’t just “drop” a thread by assigning a new value to the std::thread object that manages it.
```



## sharing data between threads

concept of **invariants**, double linked list example -> race condition
