# Inline Reflect

A header-only util that allows writing reflection code within the class

```c++
class MyClass
{
    using this_class = MyClass;
    float w;
    ANON_TYPE = member_reflect<&this_class::w>;

    int x;
public:
    int y;
    int z;
    ANON_TYPE = member_reflect_attribute<
            attribute_set<serialize_filed>,
            &this_class::x,
            &this_class::y,
            &this_class::z>;

    void set_x(int x) { this->x = x; }

    void set_y(int y) { this->y = y; }
    ANON_TYPE = member_reflect<&this_class::set_x, &this_class::set_y>;
    
    struct t {};
    void set_t(t tt) {}
    ANON_TYPE = member_reflect<&this_class::set_t>;

    static int static_func(int x, int y) { return x + y; }

    ANON_TYPE = static_func_reflect<MyClass, &MyClass::static_func>;

    MyClass(int x, int y, int z) : w(0), x(x), y(y), z(z) {}
};
```

output
```text
w = 0 : float     
x = 1 : int        serialize_filed
y = 2 : int        serialize_filed
z = 3 : int        serialize_filed
```


## compile time type info query
```c++
for_each_type(
        object_info<MyClass>::fields{},
        []<typename FieldInfo>(meta::type_wrapper<FieldInfo>)
        {
            using type = FieldInfo::type;
            auto name = FieldInfo::name;
            using attributes = FieldInfo::attributes;
            std::print(std::cout, "{:15s} : {:10s}", name, raw_name_of<type>());
            for_each_attribute(attributes(), [&]<typename Attr>(Attr)
            {
                std::print(std::cout, " {}", raw_name_of<Attr>());
            });
            std::println(std::cout, "");
        });
for_each_type(
        object_info<MyClass>::methods{},
        []<typename FieldInfo>(meta::type_wrapper<FieldInfo>)
        {
            using type = FieldInfo::function_type;
            auto name = FieldInfo::name;
            using attributes = FieldInfo::attributes;
            std::print(std::cout, "{:15s} : {:30s}", name, raw_name_of<type>());
            for_each_attribute(attributes(), [&]<typename Attr>(Attr)
            {
                std::print(std::cout, " {}", raw_name_of<Attr>());
            });
            std::println(std::cout, "");
        });
for_each_type(
        object_info<MyClass>::static_methods{},
        []<typename FieldInfo>(meta::type_wrapper<FieldInfo>)
        {
            using type = FieldInfo::function_type;
            auto name = FieldInfo::name;
            using attributes = FieldInfo::attributes;
            std::print(std::cout, "{:15s} : {:30s}", name, raw_name_of<type>());
            for_each_attribute(attributes(), [&]<typename Attr>(Attr)
            {
                std::print(std::cout, " {}", raw_name_of<Attr>());
            });
            std::println(std::cout, "");
        });

```


output
```text
w               : float     
x               : int        serialize_filed
y               : int        serialize_filed
z               : int        serialize_filed
set_x           : void (int)                    
set_y           : void (int)                    
set_t           : void (MyClass::t)             
static_func     : int (int, int)   
```
