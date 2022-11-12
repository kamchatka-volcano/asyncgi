#pragma once
#include <asyncgi/request.h>
#include <asyncgi/response.h>
#include <functional>
#include <type_traits>
#include <tuple>

namespace asyncgi::detail{

template<typename T>
struct type_wrapper{
    using type = T;
};
template<typename TWrapper>
using unwrap_type = typename TWrapper::type;

template<typename... T>
using type_list = std::tuple<type_wrapper<T>...>;

template<std::size_t index, typename TList>
using type_list_element = unwrap_type<std::remove_reference_t<decltype(std::get<index>(TList{}))>>;

template<typename TList>
constexpr auto type_list_size = std::tuple_size_v<TList>;

template<std::size_t index, typename TList>
constexpr auto typeListElement(){
    auto list = TList{};
    return type_wrapper<unwrap_type<std::remove_reference_t<decltype(std::get<index>(list))>>>{};
}
template<typename TList, std::size_t... I>
constexpr auto makeTypeListElementsTuple(std::index_sequence<I...>) -> std::tuple<type_list_element<I, TList>...>;

template<typename TList, std::size_t Size = std::tuple_size_v<TList>>
using type_list_elements_tuple = decltype(makeTypeListElementsTuple<TList>(std::make_index_sequence<Size>()));

template <typename... T>
constexpr auto makeDecayTuple (std::tuple<T...> const &)
   -> std::tuple<std::decay_t<T>...>;

template <typename T>
using decay_tuple = decltype(makeDecayTuple(std::declval<T>()));

template <typename T>
struct callable_signature;

template <typename R, typename... Args>
struct callable_signature<std::function<R(Args...)>> {
    using return_type = R;
    using args = type_list<Args...>;
};

template<typename TCallable>
using callable_args = typename callable_signature<decltype(std::function{std::declval<TCallable>()})>::args;

template<typename T, typename = void>
struct is_response : std::false_type{};

template<typename T>
struct is_response<T, std::void_t<typename T::response_route_context>> : std::true_type{};

template<typename T>
constexpr auto is_response_v = is_response<T>::value;

template<typename TRequestProcessorArgs>
constexpr void checkRequestProcessorSignature()
{
    using args = TRequestProcessorArgs;
    constexpr auto argsSize = type_list_size<args>;
    static_assert(argsSize == 2);
    static_assert(std::is_same_v<const asyncgi::Request&, typename decltype(typeListElement<argsSize - 2, args>())::type>);
    static_assert(is_response_v<std::decay_t<typename decltype(typeListElement<argsSize - 1, args>())::type>>);
}

}