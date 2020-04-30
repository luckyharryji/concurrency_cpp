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


using `lock_guard` instead of `mutex.lock()` since it ensures the unlock was called during de-construction/exception.

Todo: function return pointer/reference? which means the mutex will be invalid since it pass the access to the client?

Need to mark **all** the pieces of code that access the data structure as mutually exclusive: developer's responsibility
-> Don’t pass pointers and references to protected data outside the scope of the lock, whether by returning them from a function, storing them in externally visible memory, or passing them as arguments to user-supplied functions. `?? what about database rows?`


## spotting race condition with interface

example: c++ `stack`. The memeory allocation from heap can fail and throw exception, so if `pop()` function remove top element from stack and then return the value to the caller, the data could be dropped since memeory allocation exception happens after `remove data from stack` operation. So cpp choose to have both `top()` and `pop()` function to do this action.

few options to handle this case:

- PASS IN A REFERENCE
- REQUIRE A NO-THROW COPY CONSTRUCTOR OR MOVE CONSTRUCTOR
- RETURN A POINTER TO THE POPPED ITEM (?? how to gurarntee the access from different threads?)

a combination of option 1 and 2/3 is normal


## handling deadlock

use `std::lock()` to lock multiple mutex at the same time, pass mutex ownership to lock_guard with `std::adopt_lock`

`std::lock()` provides lock all or nothing for these 2 mutex instance.


```cpp
    void swap(X& lhs, X& rhs) {
        // check it's different instance because attempting to acquire a lock on a std::mutex when you already hold it is undefined behavior
        if (&lhs==&rhs) {
            return;
        }
        std::lock(lhs.m,rhs.m);
        std::lock_guard<std::mutex> lock_a(lhs.m,std::adopt_lock);
        std::lock_guard<std::mutex> lock_b(rhs.m,std::adopt_lock);
        swap(lhs.data,rhs.data);
    }
```


general guideline:
don’t wait for another thread if there’s a chance it’s waiting for you

avoid calling user supplied code while handling lock

ACQUIRE LOCKS IN A FIXED ORDER between threads:
think about the double linked list case when multiple thread doing traversal / deleting nodes

design a hierarchical mutex to represent this fixed order
