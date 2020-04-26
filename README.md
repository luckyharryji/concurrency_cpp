# concurrency_cpp


- ```join()``` when exception happens
    - simple: do join() inside ```catch(...)```
    - design pattern of : RAII. join() for joinable object in deconstructor.
    - detach(), if the thread-function needs to remain alive after thread object destroyed


- Argument passing:
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
