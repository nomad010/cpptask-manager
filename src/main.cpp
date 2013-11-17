/**
Copyright (c) <2013> <Julian Kenwood>

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
**/

#include <cstdio>
#include "TaskMaster.h"

using namespace std;

template <typename T>
class Sum
{
public:
    template <typename... Args>
    T static perform(T first, Args... args)
    {
        std::chrono::milliseconds dura(1000);
        std::this_thread::sleep_for(dura);
        T result = first + perform(args...);
        return result;
    }
    
    T static perform()
    {
        T result(0);
        return result;
    }
};

int main(int argc, char** argv)
{
    TaskMaster<Sum<double>> tasks;
    auto get_it = tasks.perform_task<Sum<double>>(1, 2, 3);
    std::chrono::milliseconds dura(1000);
    std::this_thread::sleep_for(dura);
    auto get_it_2 = tasks.perform_task<Sum<double>>(1, 2, 3);
    printf("%lf\n", get_it.get());
    printf("%lf\n", get_it_2.get());
    return 0;
}