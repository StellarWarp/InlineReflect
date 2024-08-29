# Inline Reflect

A C++20 header-only util that allows writing reflection code within the class

to import the code run

```shell
git subtree add --prefix <path-to-folder> https://github.com/StellarWarp/InlineReflect.git inline_reflect --squash
```

## usage

```c++
class MyClass
{
    using this_class = MyClass;
    volatile const float w;
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
    void set_t(t tt) const volatile & noexcept{}

    ANON_TYPE = member_reflect<&this_class::set_t>;

    static int static_func(int x, int y) noexcept { return x + y; }

    ANON_TYPE = static_func_reflect<MyClass, &MyClass::static_func>;

    MyClass(int x, int y, int z) : w(0), x(x), y(y), z(z) {}
};
```

## field iteration
```c++
for_each_field(c, []<typename Info>(auto&& x, Info info)
{
    using type = Info::type;
    using attributes = Info::attributes;
    auto name = info.name;
    using remove_v = std::remove_volatile_t<type>;
    print(std::cout, "{} = {} : {:10s}", name, (remove_v)x, raw_name_of<type>());
    for_each_attribute(attributes(), [&]<typename Attr>(Attr)
    {
        print(std::cout, " {}", raw_name_of<Attr>());
    });
    println(std::cout, "");
});
```
output
```text
w = 0 : const volatile float
x = 1 : int        serialize_filed
y = 2 : int        serialize_filed
z = 3 : int        serialize_filed
```


## compile time type info visit
```c++
for_each_type(
        object_info<MyClass>::fields{},
        []<typename FieldInfo>(meta::type_wrapper<FieldInfo>)
        {
            using type = FieldInfo::type;
            auto name = FieldInfo::name;
            using attributes = FieldInfo::attributes;
            print(std::cout, "{:15s} : {:10s}", name, raw_name_of<type>());
            for_each_attribute(attributes(), [&]<typename Attr>(Attr)
            {
                print(std::cout, " {}", raw_name_of<Attr>());
            });
            println(std::cout, "");
        });
for_each_type(
        object_info<MyClass>::methods{},
        []<typename FieldInfo>(meta::type_wrapper<FieldInfo>)
        {
            using function_type = FieldInfo::function_type;
            using decay_function_type = FieldInfo::decay_function_type;
            using member_function_type = FieldInfo::member_function_type;
            using decay_member_function_type = FieldInfo::decay_member_functi
            auto name = FieldInfo::name;
            using attributes = FieldInfo::attributes;
            print(std::cout, "{:15s} : {:50s} {:30s}",
                       name, raw_name_of<function_type>(),
                       raw_name_of<decay_function_type>());
            for_each_attribute(attributes(), [&]<typename Attr>(Attr)
            {
                print(std::cout, " {}", raw_name_of<Attr>());
            });
            println(std::cout, "");
        });
for_each_type(
        object_info<MyClass>::static_methods{},
        []<typename FieldInfo>(meta::type_wrapper<FieldInfo>)
        {
            using type = FieldInfo::function_type;
            auto name = FieldInfo::name;
            using attributes = FieldInfo::attributes;
            print(std::cout, "{:15s} : {:30s}", name, raw_name_of<type>());
            for_each_attribute(attributes(), [&]<typename Attr>(Attr)
            {
                print(std::cout, " {}", raw_name_of<Attr>());
            });
            println(std::cout, "");
        });

```


output
```text
w               : const volatile float
x               : int        serialize_filed
y               : int        serialize_filed
z               : int        serialize_filed
set_x           : void (int)                                         void (int)                    
set_y           : void (int)                                         void (int)                    
set_t           : void (MyClass::t) const volatile & noexcept        void (MyClass::t)             
static_func     : int (int, int)    
```
