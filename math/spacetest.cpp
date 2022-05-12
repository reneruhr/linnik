#include <array>
#include <iostream>

template <class T>
class A
{
    public:
    std::array<T,2> data_;
    constexpr friend auto operator==(const A<T>& a, const A<T>& b)
    {
        return  ( a.data_ == b.data_ );
    }
};

int main()
{
    using B = A<int>;

    auto a = B{0,0};
    auto b = B{1,1};

    std::cout << std::boolalpha;
    std::cout << (a.data_ <b.data_ );
    std::cout << (a == b);

}
