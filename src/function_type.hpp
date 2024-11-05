#pragma once
#include <functional>
#include <tuple>
#include <type_traits>

// 関数型から引数型を抽出するテンプレート
template <typename T>
struct function_traits;

// 通常の関数ポインタの場合
template <typename R, typename... Args>
struct function_traits<R(*)(Args...)> {
    using return_type = R;
    using argument_tuple = std::tuple<Args...>;
};

// std::functionの場合
template <typename R, typename... Args>
struct function_traits<std::function<R(Args...)>> {
    using return_type = R;
    using argument_tuple = std::tuple<Args...>;
};

// メンバ関数ポインタの場合
template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...)> {
    using return_type = R;
    using argument_tuple = std::tuple<Args...>;
};

// constメンバ関数ポインタの場合
template <typename C, typename R, typename... Args>
struct function_traits<R(C::*)(Args...) const> {
    using return_type = R;
    using argument_tuple = std::tuple<Args...>;
};

// 関数オブジェクトやラムダのための推論
template <typename F>
struct function_traits : function_traits<decltype(&F::operator())> {};

// 引数型を取り出すためのヘルパー
template <size_t N, typename F>
using function_argument_t = typename std::tuple_element<N, typename function_traits<F>::argument_tuple>::type;