add_cxflags("/Zc:__cplusplus", {tools = {"msvc", "cl", "clang_cl"}})

add_rules("mode.debug", "mode.release")

set_toolchains("clang-cl")

target("InlineReflect")
    set_languages("c++20")
    set_kind("binary")
    add_files("src/main.cpp")
    add_headerfiles("src/**/*.h")

target("Forwarding")
    set_languages("c++20")
    set_kind("binary")
    add_files("src/forwarding.cpp")
    add_headerfiles("src/**/*.h")

