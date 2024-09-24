#include "inline_reflect/inline_reflect.h"

#include <format>
#include <iostream>

using namespace inline_reflect;

struct serialize_filed {};

template<class Ch, class Tr, typename T>
requires (!has_class_property<T>())
void auto_serialize(std::basic_ostream<Ch, Tr>& os, T& t)
{
    os << t;
}

template<class Ch, class Tr, class T>
requires (has_class_property<T>())
void auto_serialize(std::basic_ostream<Ch, Tr>& os, T& c)
{
    for_each_field(c, [&]<typename Info>(auto&& x, Info info)
    {
        using attributes = Info::attributes;
        if (!attributes::template contains<serialize_filed>)
            return;
        auto_serialize(os, x);
    });
}

#define CONCATENATE_DIRECT(a, b) a##b
#define CONCATENATE(a, b) CONCATENATE_DIRECT(a, b)
#define ANON_TYPE using CONCATENATE(_inline_reflect_, __LINE__)

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

    void set_t(t tt) const volatile& noexcept {}

    ANON_TYPE = member_reflect<&this_class::set_t>;

    static int static_func(int x, int y) noexcept { return x + y; }

    ANON_TYPE = static_func_reflect<MyClass, &MyClass::static_func>;

    MyClass(int x, int y, int z) : w(0), x(x), y(y), z(z) {}
};


#if __cplusplus < 202400
#define print(os, ...) os << std::format( __VA_ARGS__ );
#define println(os, ...) os << std::format( __VA_ARGS__ ) << std::endl;
#else
using print = std::print;
using println = std::println;
#endif

int main()
{


    MyClass c{1, 2, 3};

    for_each_field(c, []<typename Info>(auto&& x, Info info)
    {
        using type = Info::type;
        using attributes = Info::attributes;
        auto name = info.name;
        using remove_v = std::remove_volatile_t<type>;
        print(std::cout, "{} = {} : {:10s}", name, (remove_v) x, raw_name_of<type>());
        for_each_attribute(attributes(), [&]<typename Attr>(Attr)
        {
            print(std::cout, " {}", raw_name_of<Attr>());
        });
        println(std::cout, "");
    });
    std::cout << "serialize : ";
    auto_serialize(std::cout, c);
    std::cout << std::endl;

    println(std::cout, "\niterate through properties");
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
                using decay_member_function_type = FieldInfo::decay_member_function_type;
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


    auto info = reflect_info<MyClass>();
    println(std::cout, "fields {} \nmethods {} \nstatic methods {}",
            info.var_info, info.method_info, info.static_method_info);
}



