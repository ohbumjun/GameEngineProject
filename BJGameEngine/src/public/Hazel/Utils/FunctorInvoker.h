#pragma once

#include "hzpch.h"
#include "MetaProgrammingInfo.h"

struct FuctorInvoker
{
    // std::tuple �� ����ִ� ���ڵ��, fuctor �� ȣ�����ִ� �Լ�
    template <typename Functor, typename... TArgs>
    static typename std::result_of<Functor(TArgs...)>::type InvokeFunctor(
        Functor &&func,
        std::tuple<TArgs...> &args)
    {
        // index_range<0, sizeof...(TArgs)>() ?
        // ex) ���� ���� 3 => index_range<0,1,2> ����  
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
        // std::tuple �� 0, 1, 2 ��° ��Ҹ� �����Ѵ�. �׸��� seperate argument �� ������ش�.
        // ex) tuple[a,b,c] => (a,b,c)
        return func(std::get<Is>(args)...);
    }
    // - universal reference
    // - typename std::result_of<Functor(void)>::type : Functor(void) �� ����Ÿ��
    template <typename Functor>
    static typename std::result_of<Functor(void)>::type invokeFunctorImpl(
        Functor &&func)
    {
        return func();
    }
};