#include <iostream>
#include <utility>
#include <memory>
#include "naive_demo/naive_demo.h"
#include "naive_desttile_demo/naive_desttile_demo.h"

using MainFuncType = int(*)(int, char**);
struct MainFuncTableEntry
{
    MainFuncType func_ptr;
    const char* func_name;
};

static constexpr const MainFuncTableEntry stc_main_func_table[] =
{
    { &naive_demo_main, "naive_demo_main" },
    { &naive_desttile_demo_main, "naive_desttile_demo_main" },
};

int select_execute_return(int argc, char** argv)
{
    std::cout << std::endl;
    int idx = 0;
    for (const auto& entry : stc_main_func_table)
    {
        std::cout << "[" << idx << "] " << entry.func_name << std::endl;
        ++idx;
    }
    std::cout << "Select index:" << std::endl;
    std::string response;
    std::getline(std::cin, response);
    bool idx_good = false;
    try
    {
        idx = std::stoi(response, nullptr);
        idx_good = (idx >= 0 && idx < std::size(stc_main_func_table));
    }
    catch(const std::exception& e)
    {
        std::cerr << "Bad response." << std::endl;
        std::cerr << e.what() << '\n';
        return 0;
    }
    if (idx_good)
    {
        return stc_main_func_table[idx].func_ptr(argc, argv);
    }
    else
    {
        std::cerr << "Bad response." << std::endl;
    }
    return 0;
}

int main(int argc, char** argv)
{
    return select_execute_return(argc, argv);
}
