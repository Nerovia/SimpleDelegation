# SimpleDelegation
SimpleDelegation is a memory cheap alternative to `<functional>` when working with Arduino. The library provides a generalized delegate type for static functions, member functions and lambda expressions, as well as a specialized property type to easily handle getter and setter delegates.

The defined types are supposed to use as little memory as possible, therefore no smart pointers or anything fancy was utilized to create them. This library is not necessarily beginner friendly, but still easy to use. Make sure you know how to release and handle heap memory, otherwise you will probably end up with memory leaks.

If memory isn't a concern or you want a more professional solution, I recommend using [functional-vlpp](https://github.com/khoih-prog/functional-vlpp.git), which inspired this project.

 ## Creating a Delegate
 To create a new delegate use the `Delegate<R, TArgs...>::create` method. Where `R` matches the return type of your function and `TArgs...` its parameters. This method will create a new instance in heap memory and return a pointer to it as `Delegate<R, TArgs>*`.
 

  ```cpp
  void staticFunction(int i, float f, bool b) { }

  Delegate<void, int, float, bool>* delegate;
  ```


  ### Static Function
  ```cpp
  int staticFunction(float arg) { return (int)arg; }

  auto* delegate = Delegate<int, float>::create(&staticFunction); 
  ```

  ### Member Function
  ```cpp
  class Example
  {
      public:
        int memberFunction(float arg) { return (int)arg; }
  };

  Example example;  

  auto* delegate = Delegate<int, float>::create(&example, &Example::memberFunction); 
  ```

  ### Lambda Expression

  ```cpp
  auto* = Delegate<int>::create([](float arg){ return (int)arg; })
  ```

 ## Invoking a Delegate
 To invoke your delegate you can either call the provided `invoke` methode directly or use the overridden `operator()` like shown below.
 ```cpp
 delegate->invoke(value);
 ```

 ```cpp
 (*delegate)(value);
 ```

 ## Disposing a Delegate
 When creating a delegate, a new instance is created in heap memory and a pointer to it is returned. Be careful when reassigning this pointer to a new delegate, because loosing the previous reference will lead to a memory leak. 

 Make sure to always dispose your delegate before it goes out of scope, by calling `delete` on it. You can also assign the delegate to a smart pointer and let that take care of it.
 ```cpp
 delete delegate;
 ```

 ## Getters, Setters, Actions
 The library provides simplified type aliases for commonly used delegates such as getters, setters and actions.

  ### Getter
  ```cpp
  int get() { return number; }
  auto* getter = Getter<int>::create(&get);
  ```

  ### Setter
  ```cpp
  void set(int value) { number = value; }
  auto* setter = Setter<int>::create(&set);
  ```

  ### Action
  ```cpp
  void do() { }
  auto* Action = Action::create(&do);
  ```
 ## Property
 To further reduce memory usage, the library features a combined getter/setter delegate for member functions, called `Property`. If both getters and setters are members of the same type, the sender reference is shared between the two delegates instead of being stored separately for both.

 ```cpp
 class Example
 {
    public:
        int get() { return number; }
        void set(int value) { number = value; }
 };

 Example example;

 auto* property = Property<int>::create(&example, &Example::get, &Example::set)
 ```

 You can also create properties from individual delegates, without any impact on memory usage. You have to create these delegates separately, but this allows you to mix static functions and lambdas for example.
 
 ```cpp
 int get() { return number; }

 auto* property = Property<int>::create(
     Getter<int>::create(&get),
     Setter<int>::create([&](int value){ number = value }));
 ```

 To invoke the getter or setter of a `Property` use the provided `get` or `set` method.

 ```cpp
 property->get();
 ``` 
  ```cpp
 property->set(value);
 ``` 

A property does not strictly require a setter and can be created without one. Calling `set` on such a property will do nothing.

```cpp
auto* property = Property<int>::create(&example, &get);
```
