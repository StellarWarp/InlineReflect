#pragma once


#include <utility>
#include <iostream>
#include <array>
#include "static_string.h"
#include "name_reflect.h"
#include "type_list.h"
#include "function_traits.h"


namespace inline_reflect::details
{
    using namespace meta;

    template<class, size_t>
    struct mapping
    {
        friend constexpr auto get(mapping);

        template<auto SetValue>
        struct set
        {
            friend constexpr auto get(mapping) { return SetValue; }
        };
    };

    template<typename T, auto N = 0, auto tag = [] {}, auto condition = requires { get(mapping<T, N>{}); }>
    static consteval auto mapping_count()
    {
#ifdef __INTELLISENSE__
		return 0;
#else
        if constexpr (!condition)
            return N;
        else
            return mapping_count<T, N + 1>();
#endif
    }

    template<auto Var>
    struct var_type_wrapper
    {
        static constexpr auto value = Var;
    };

    struct member_info_tag {};

    template<auto MemberVar,
            typename Attributes>
    struct field_info;

    template<typename ClassType,
            typename T,
            T ClassType::* MemberVar,
            typename Attributes>
    struct field_info<MemberVar, Attributes> : member_info_tag
    {
        static constexpr auto ptr = MemberVar;
        static constexpr auto name = field_name_of<MemberVar>();
        using class_type = ClassType;
        using type = T;
        using attributes = Attributes;
    };

    template<auto MemberFunc, typename Attributes>
    struct method_info;

    template<typename ClassType,
            typename FuncT,
            FuncT ClassType::* MemberFunc,
            typename Attributes>
    struct method_info<MemberFunc, Attributes> : member_info_tag
    {
        static constexpr auto ptr = MemberFunc;
        static constexpr auto name = method_name_of<MemberFunc>();
        using class_type = ClassType;
        using member_function_type = decltype(MemberFunc);
        using decay_member_function_type = function_traits<member_function_type>::decay_member_function_type;
        using function_type = FuncT;
        using decay_function_type = function_traits<member_function_type>::decay_function_type;
        using attributes = Attributes;
    };
    template<auto Func, static_string Name, typename Attributes>
    struct static_func_info;
    template<typename Ret, typename... Args,
            Ret (* StaticFunc)(Args...),
            static_string Name,
            typename Attributes>
    struct static_func_info<StaticFunc, Name, Attributes> : member_info_tag
    {
        static constexpr auto ptr = StaticFunc;
        static constexpr auto name = Name;
        using function_type = Ret(Args...);
        using attributes = Attributes;
    };

    template<typename T>
    struct member_var_tag;

    template<typename T>
    struct member_func_tag;

    template<typename T>
    struct static_func_tag;

    template<typename T>
    struct constructor_tag;

    template<typename... T>
    struct attribute_set : type_list<T...> {};

    template<typename... Attribute>
    constexpr void for_each_attribute(attribute_set<Attribute...>, auto&& functor)
    {
        (functor(Attribute{}), ...);
    }


    using no_attribute_t = attribute_set<>;

    template<auto MemberVar, typename Attributes>
    struct inline_reflect_impl;

    template<typename ClassType,
            typename T,
            T ClassType::* MemberVar,
            typename Attributes> requires std::is_member_object_pointer_v<decltype(MemberVar)>
    struct inline_reflect_impl<MemberVar, Attributes>
    {
        using info_t = field_info<MemberVar, Attributes>;
        using setter_t = mapping<
                member_var_tag<ClassType>, mapping_count<member_var_tag<ClassType>>()
        >::template set<info_t{}>;
        static constexpr auto _ = setter_t();
    };

    template<typename ClassType,
            typename FuncT,
            FuncT ClassType::* MemberFunc,
            typename Attributes
            > requires std::is_member_function_pointer_v<decltype(MemberFunc)>
    struct inline_reflect_impl<MemberFunc, Attributes>
    {
        using info_t = method_info<MemberFunc, Attributes>;
        using setter_t = mapping<
                member_func_tag<ClassType>, mapping_count<member_func_tag<ClassType>>()
        >::template set<info_t{}>;
        static constexpr auto _ = setter_t();
    };

    template<auto ...>
    struct multi_reflect_impl;

    template<auto... MemberVar>
    using member_reflect = multi_reflect_impl<inline_reflect_impl<MemberVar, no_attribute_t>{}...>;
    template<typename Attributes, auto... MemberVar>
    using member_reflect_attribute = multi_reflect_impl<inline_reflect_impl<MemberVar, Attributes>{}...>;

    template<typename T>
    void for_each_field(T&& object, auto&& functor)
    {
        using ClassType = std::decay_t<T>;
        constexpr size_t N = mapping_count<member_var_tag<ClassType>>();

        [&]<size_t...I>(std::index_sequence<I...>)
        {
            (
                    [&]<size_t Index>(std::integral_constant<size_t, Index>)
                    {
                        const auto info = get(mapping<member_var_tag<ClassType>, Index>{});
                        auto member_ptr = info.ptr;
                        using attributes = std::decay_t<decltype(info)>::attributes;
                        if constexpr (std::invocable<decltype(functor), decltype(object.*member_ptr)>)
                            functor(object.*member_ptr);
                        else
                            functor(object.*member_ptr, info);
                    }(std::integral_constant<size_t, I>{})
                    ,
                    ...);
        }(std::make_index_sequence<N>{});
    }


    template<typename ClassType, auto Func, typename Attributes>
    struct static_func_reflect_impl;

    template<typename ClassType, typename Ret, typename... Args,
            Ret(* StaticFunc)(Args...),
            typename Attributes>
    struct static_func_reflect_impl<ClassType, StaticFunc, Attributes>
    {
        using info_t = static_func_info<StaticFunc, method_name_of<StaticFunc>(), Attributes>;
        using setter_t = mapping<
                static_func_tag<ClassType>, mapping_count<static_func_tag<ClassType>>()
        >::template set<info_t{}>;
        static constexpr auto _ = setter_t();
    };

    template<typename ClassType, auto Func, typename Attributes = no_attribute_t>
    using static_func_reflect = decltype(static_func_reflect_impl<ClassType, Func, Attributes>());

    template<typename ClassType, typename... Args>
    struct inline_reflect_constructor_impl
    {
        static constexpr ClassType* (* constructor)(ClassType*, Args...) = [](ClassType* p, Args... args)
        {
            return ::new(static_cast<void*>(p)) ClassType(std::forward<Args>(args)...);
        };

        using setter_t = mapping<
                constructor_tag<ClassType>, mapping_count<constructor_tag<ClassType>>()
        >::template set<constructor>;
        static constexpr auto _ = setter_t();
    };

    template<typename ClassType, typename... Args>
    using constructor_reflect = decltype(inline_reflect_constructor_impl<ClassType, Args...>());

    template<typename T>
    struct object_info
    {
        template<typename... U>
        using type_list = type_list<U...>;
        template<typename, typename>
        struct info_access_impl;
        template<typename tag, size_t... I>
        struct info_access_impl<tag, std::index_sequence<I...>>
        {
#ifdef __INTELLISENSE__
			using type = type_list<>;
#else
			using type = type_list<decltype(get(mapping<tag, I>{}))... >;
#endif
        };
        using field_tag = member_var_tag<T>;
        using method_tag = member_func_tag<T>;
        using static_method_tag = static_func_tag<T>;
        using cons_tag = constructor_tag<T>;

        using fields = info_access_impl<field_tag, std::make_index_sequence<mapping_count<field_tag>()>>::type;
        using methods = info_access_impl<method_tag, std::make_index_sequence<mapping_count<method_tag>()>>::type;
        using static_methods = info_access_impl<static_method_tag, std::make_index_sequence<mapping_count<static_method_tag>()>>::type;
        using constructors = info_access_impl<cons_tag, std::make_index_sequence<mapping_count<cons_tag>()>>::type;
    };

    template<typename T>
    constexpr auto reflect_info()
    {
#ifdef __INTELLISENSE__
        struct info
        {
            std::tuple<> var_info;
            std::tuple<> method_info;
            std::tuple<> static_method_info;
            std::tuple<> constructor_info;
        };
        return info{};
#else
        using ClassType = std::decay_t<T>;
        constexpr size_t var_count = mapping_count<member_var_tag<T>>();
        constexpr size_t method_count = mapping_count<member_func_tag<T>>();
        constexpr size_t static_method_count = mapping_count<static_func_tag<T>>();
        constexpr size_t constructor_count = mapping_count<constructor_tag<T>>();

        constexpr auto var_info = []<size_t...I>(std::index_sequence<I...>)
        {
            return std::tuple(get(mapping<member_var_tag<ClassType>, I>{})...);
        }(std::make_index_sequence<var_count>{});
        using var_info_t = decltype(var_info);
        constexpr auto method_info = []<size_t...I>(std::index_sequence<I...>)
        {
            return std::tuple(get(mapping<member_func_tag<ClassType>, I>{})...);
        }(std::make_index_sequence<method_count>{});
        using method_info_t = decltype(method_info);
        constexpr auto static_method_info = []<size_t...I>(std::index_sequence<I...>)
        {
            return std::tuple(get(mapping<static_func_tag<ClassType>, I>{})...);
        }(std::make_index_sequence<static_method_count>{});
        using static_method_info_t = decltype(static_method_info);
        constexpr auto constructor_info = []<size_t...I>(std::index_sequence<I...>)
        {
            return std::tuple(get(mapping<constructor_tag<ClassType>, I>{})...);
        }(std::make_index_sequence<constructor_count>{});
        using constructor_info_t = decltype(constructor_info);

        struct info
        {
            var_info_t var_info;
            method_info_t method_info;
            static_method_info_t static_method_info;
            constructor_info_t constructor_info;
        };
        return info{var_info, method_info, static_method_info, constructor_info};
#endif
    }


    template<class T>
    constexpr bool has_class_property()
    {
#ifdef __INTELLISENSE__
        return true;
#else
        return mapping_count<member_var_tag<T>>() != 0;
#endif // __INTELLISENSE__
    }

}

namespace inline_reflect
{
    using details::field_info;
    using details::method_info;
    using details::static_func_info;
    using details::attribute_set;
    using details::for_each_attribute;
    using details::for_each_field;
    using details::for_each_type;
    using details::no_attribute_t;

    using details::member_reflect;
    using details::member_reflect_attribute;
    using details::static_func_reflect;
    using details::constructor_reflect;
    using details::object_info;
    using details::has_class_property;
    using details::reflect_info;

    using details::raw_name_of;
    using details::enum_name_of;
    using details::field_name_of;
    using details::method_name_of;

}


template<std::derived_from<inline_reflect::details::member_info_tag> Info>
struct std::formatter<Info> : std::formatter<const char*>
{
    auto format(const Info& info, auto& ctx) const
    {
        auto out = ctx.out();
        if constexpr (requires { typename Info::type; })
            out = std::format_to(out, "{}", inline_reflect::raw_name_of<typename Info::type>());
        else if constexpr (requires { typename Info::member_function_type; })
            out = std::format_to(out, "{}", inline_reflect::raw_name_of<typename Info::member_function_type>());
        else if constexpr (requires { typename Info::function_type; })
            out = std::format_to(out, "{}", inline_reflect::raw_name_of<typename Info::function_type>());
        out = std::format_to(out, " {}", info.name);
        if constexpr (sizeof(info.ptr) == sizeof(intptr_t))
            out = std::format_to(out, " : {}", *(void**) (&info.ptr));
        else if constexpr (sizeof(info.ptr) == sizeof(uint32_t))
            out = std::format_to(out, " : {}", *(uint32_t*) (&info.ptr));
        return out;
    }
};

template<typename... Args>
struct std::formatter<std::tuple<Args...>>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        auto it = ctx.begin();
        auto end = ctx.end();
        if (it != end && *it != '}')
        {
            it++;
        }
        return it;
    }

    auto format(const std::tuple<Args...>& t, auto& ctx) const
    {
        auto out = ctx.out();
        out = std::format_to(out, "{{");
        [&]<size_t... I>(std::index_sequence<I...>)
        {
            ((
                    [&]<size_t Index>(std::integral_constant<size_t, Index>)
                    {
                        const auto& elem = std::get<Index>(t);
                        out = std::format_to(out, "{}", elem);
                        if constexpr (Index != sizeof...(Args) - 1)
                            out = std::format_to(out, ", ");
                    }(std::integral_constant<size_t, I>{})
            ), ...);
        }(std::make_index_sequence<sizeof...(Args)>{});
        out = std::format_to(out, "}}");
        return out;
    }
};


