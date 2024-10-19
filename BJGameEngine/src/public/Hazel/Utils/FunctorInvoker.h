#pragma once

#include "hzpch.h"
#include "MetaProgrammingInfo.h"

struct FuctorInvoker
{
    // std::tuple 에 들어있는 인자들로, fuctor 를 호출해주는 함수
    template <typename Functor, typename... TArgs>
    static typename std::result_of<Functor(TArgs...)>::type InvokeFunctor(
        Functor &&func,
        std::tuple<TArgs...> &args)
    {
        // index_range<0, sizeof...(TArgs)>() ?
        // ex) 인자 개수 3 => index_range<0,1,2> 리턴  
        return invokeFunctorImpl(std::forward<Functor>(func), 
            args, 
            index_range<0, sizeof...(TArgs)>());
    }

    template <typename Functor>
    static typename std::result_of<Functor(void)>::type InvokeFunctor(
        Functor &&func)
    {
        return invokeFunctorImpl(std::forward<Functor>(func));
    }

private:
    template <typename Functor, typename... TArgs, size_t... Is>
    static typename std::result_of<Functor(TArgs...)>::type invokeFunctorImpl(
        Functor &&func,
        std::tuple<TArgs...> &args,
        index_list<Is...>)
    {
        // std::tuple 의 0, 1, 2 번째 요소를 추출한다. 그리고 seperate argument 로 만들어준다.
        // ex) tuple[a,b,c] => (a,b,c)
        return func(std::get<Is>(args)...);
    }
    // - universal reference
    // - typename std::result_of<Functor(void)>::type : Functor(void) 의 리턴타입
    template <typename Functor>
    static typename std::result_of<Functor(void)>::type invokeFunctorImpl(
        Functor &&func)
    {
        return func();
    }
};