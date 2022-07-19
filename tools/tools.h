#pragma once
#include <string>
#include <utility>
namespace Tools
{
auto LoadFile(std::string)  -> std::string;

// https://vittorioromeo.info/index/blog/cpp20_lambdas_compiletime_for.html

template <auto... Xs, typename F>
constexpr void for_values(F&& f)
{
    (f.template operator()<Xs>(), ...);
};

template <auto B, auto E, typename F>
constexpr void for_range(F&& f)
{
    using t = std::common_type_t<decltype(B), decltype(E)>;

    [&f]<auto... Xs>(std::integer_sequence<t, Xs...>)
    {
        for_values<(B + Xs)...>(f);
    }
    (std::make_integer_sequence<t, E - B>{});
}


}
