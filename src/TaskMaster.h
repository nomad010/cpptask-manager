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

#pragma once
#include <future>
#include <map>
#include <mutex>

/** Selects a typename out of a list of typenames **/
template <typename Target, typename ...Types>
struct TaskSelector;

template <typename Target, typename First, typename ...Types>
struct TaskSelector<Target, First, Types...>
{
public:
    typedef typename TaskSelector<Target, Types...>::TaskClass TaskClass;
};

template <typename Target, typename ...Types>
struct TaskSelector<Target, Target, Types...>
{
public:
    typedef Target TaskClass;
};

template <typename Target>
struct TaskSelector<Target>
{
};

/** Performs the given task **/
template <typename Task, typename ReturnValue, typename... Args>
class TaskPerformer
{
public:
    static auto perform(Args... args) -> decltype(Task::perform(args...))
    {
        return Task::perform(args...);
    }
};

/** Will call the performer **/
template <typename Task, typename ReturnValue, typename... Args>
class TaskController
{
public:
    static std::map<std::tuple<Args...>, std::shared_future<ReturnValue>> futures_in_flight;
    static std::mutex potential_future_update;
    
    static auto perform(Args... args) -> std::shared_future<decltype(Task::perform(args...))>
    {
        if(futures_in_flight.count(std::tuple<Args...>(args...)) == 0)
        {
            std::lock_guard<std::mutex> guard(potential_future_update);
            
            if(futures_in_flight.count(std::tuple<Args...>(args...)) == 0)
            {
                TaskController<Task, ReturnValue, Args...>::futures_in_flight[std::tuple<Args...>(args...)] = std::async(std::launch::async, TaskPerformer<Task, ReturnValue, Args...>::perform, args...);
            }
        }
        return TaskController<Task, ReturnValue, Args...>::futures_in_flight[std::tuple<Args...>(args...)];
    }
};
template <typename Task, typename ReturnValue, typename... Args>
std::map<std::tuple<Args...>, std::shared_future<ReturnValue>> TaskController<Task, ReturnValue, Args...>::futures_in_flight;

template <typename Task, typename ReturnValue, typename... Args>
std::mutex TaskController<Task, ReturnValue, Args...>::potential_future_update;

/** Interface class **/
template <typename... TaskHandlers>
class TaskMaster
{
public:
    template <typename Task, typename... Args>
    static auto perform_task(Args... args) -> std::shared_future<decltype(TaskSelector<Task, TaskHandlers...>::TaskClass::perform(args...))>
    {
        typedef typename TaskSelector<Task, TaskHandlers...>::TaskClass TaskClass;
        typedef decltype(TaskSelector<Task, TaskHandlers...>::TaskClass::perform(args...)) ReturnValue;
        return TaskController<TaskClass, ReturnValue, Args...>::perform(args...);
    };
};