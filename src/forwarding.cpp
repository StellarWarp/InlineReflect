#include <format>
#include <iostream>
#include "inline_reflect/inline_reflect.h"

using namespace inline_reflect;

template<typename T>
auto fn_rv_ref(T&& x, auto p_name)
{
    std::cout << std::format("{:20s} -> [{:15s}] x : {:15s} T : {:15s}",
                             p_name, "T&& x", raw_name_of<decltype(x)>(), raw_name_of<T>())
              << std::endl;

}

template<typename T>
auto fn_crv_ref(const T&& x, auto p_name)
{
    std::cout << std::format("{:20s} -> [{:15s}] x : {:15s} T : {:15s}", p_name, "const T&& x",
                             raw_name_of<decltype(x)>(), raw_name_of<T>())
              << std::endl;

}

template<typename T>
auto fn_lv_ref(T& x, auto p_name)
{
    std::cout << std::format("{:20s} -> [{:15s}] x : {:15s} T : {:15s}", p_name, "T& x",
                             raw_name_of<decltype(x)>(), raw_name_of<T>())
              << std::endl;

}

template<typename T>
auto fn_clv_ref(const T& x, auto p_name)
{
    std::cout << std::format("{:20s} -> [{:15s}] x : {:15s} T : {:15s}", p_name, "const T& x",
                             raw_name_of<decltype(x)>(), raw_name_of<T>())
              << std::endl;

}

template<typename T>
auto fn_lv(T x, auto p_name)
{
    std::cout << std::format("{:20s} -> [{:15s}] x : {:15s} T : {:15s}", p_name, "T x",
                             raw_name_of<decltype(x)>(), raw_name_of<T>())
              << std::endl;
}

template<typename T>
auto fn_clv(const T x, auto p_name)
{
    std::cout << std::format("{:20s} -> [{:15s}] x : {:15s} T : {:15s}", p_name, "const T x",
                             raw_name_of<decltype(x)>(), raw_name_of<T>())
              << std::endl;
}

#include <ranges>

int main()
{
    int lvalue;
#define rvalue 1
    int& lv_ref = lvalue;
    int&& rv_ref = rvalue;
    const int cvalue = rvalue;
    const int& clv_ref = cvalue;
    const int&& crv_ref = rvalue;

    fn_lv(lvalue, raw_name_of<decltype(lvalue)>());
    fn_lv(lv_ref, raw_name_of<decltype(lv_ref)>());
    fn_lv(rv_ref, raw_name_of<decltype(rv_ref)>() + " (lv)");
    fn_lv(static_cast<int&&>(lvalue), "int &&");
    fn_lv(rvalue, "int (rv)");
    fn_lv(cvalue, raw_name_of<decltype(cvalue)>());
    fn_lv(clv_ref, raw_name_of<decltype(clv_ref)>());
    fn_lv(crv_ref, raw_name_of<decltype(crv_ref)>() + " (lv)");
    fn_lv(static_cast<const int&&>(lvalue), "const int &&");
    std::cout << std::endl;
    fn_clv(lvalue, raw_name_of<decltype(lvalue)>());
    fn_clv(lv_ref, raw_name_of<decltype(lv_ref)>());
    fn_clv(rv_ref, raw_name_of<decltype(rv_ref)>() + " (lv)");
    fn_clv(static_cast<int&&>(lvalue), "int &&");
    fn_clv(rvalue, "int (rv)");
    fn_clv(cvalue, raw_name_of<decltype(cvalue)>());
    fn_clv(clv_ref, raw_name_of<decltype(clv_ref)>());
    fn_clv(crv_ref, raw_name_of<decltype(crv_ref)>() + " (lv)");
    fn_clv(static_cast<const int&&>(lvalue), "const int &&");
    std::cout << std::endl;
    fn_lv_ref(lvalue, raw_name_of<decltype(lvalue)>());
    fn_lv_ref(lv_ref, raw_name_of<decltype(lv_ref)>());
    fn_lv_ref(rv_ref, raw_name_of<decltype(rv_ref)>() + " (lv)");
    std::cout << std::endl;
    fn_clv_ref(lvalue, raw_name_of<decltype(lvalue)>());
    fn_clv_ref(lv_ref, raw_name_of<decltype(lv_ref)>());
    fn_clv_ref(rv_ref, raw_name_of<decltype(rv_ref)>() + " (lv)");
    fn_clv_ref(static_cast<int&&>(lvalue), "int &&");
    fn_clv_ref(rvalue, "int (rv)");
    fn_clv_ref(cvalue, raw_name_of<decltype(cvalue)>());
    fn_clv_ref(clv_ref, raw_name_of<decltype(clv_ref)>());
    fn_clv_ref(crv_ref, raw_name_of<decltype(crv_ref)>() + " (lv)");
    fn_clv_ref(static_cast<const int&&>(lvalue), "const int &&");
    std::cout << std::endl;
    fn_rv_ref(lvalue, raw_name_of<decltype(lvalue)>());
    fn_rv_ref(lv_ref, raw_name_of<decltype(lv_ref)>());
    fn_rv_ref(rv_ref, raw_name_of<decltype(rv_ref)>() + " (lv)");
    fn_rv_ref(static_cast<int&&>(lvalue), "int &&");
    fn_rv_ref(rvalue, "int (rv)");
    fn_rv_ref(cvalue, raw_name_of<decltype(cvalue)>());
    fn_rv_ref(clv_ref, raw_name_of<decltype(clv_ref)>());
    fn_rv_ref(crv_ref, raw_name_of<decltype(crv_ref)>() + " (lv)");
    fn_rv_ref(static_cast<const int&&>(lvalue), "const int &&");

    std::cout << std::endl;
    fn_crv_ref(static_cast<int&&>(lvalue), "int &&");
    fn_crv_ref(rvalue, "int (rv)");
    fn_crv_ref(static_cast<const int&&>(lvalue), "const int &&");
}