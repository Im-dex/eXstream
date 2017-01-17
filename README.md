# eXstream
C++ 14 library that provides functional-style stream operations under collections.

### Example
```c++
struct Person
{
    std::string name;
    std::set<int> marks;
};

std::vector<Person> persons = { ... };

stream_of(persons)
    .flat_map([](auto person) { return std::ref(person.marks); })
    .filter  ([](auto mark)   { return mark > 0; })
    .distinct()
    .to<std::set<int>>();
    
stream_of(persons)
    .map   ([](auto person) { return std::ref(person.name); })
    .filter([](auto name)   { return name != "Mike"s; })
    .to<std::list<std::string>>();
```
