#include "hpp/type_traits.hpp"
#include "hpp/utility.hpp"


#include <iostream>

int main()
{
    constexpr int i = 0;
    constexpr_if(i == 0)
    {
        std::cout << "case i == 0" << std::endl;
    }
    constexpr_else_if(i == 1)
    {
        std::cout << "case i == 1" << std::endl;
    }
    constexpr_else
    {
        std::cout << "case else" << std::endl;
    }
    constexpr_end_if;


    auto invokeable = [](int param)
    {
        std::cout << "invoked with " << param << std::endl;

        return param;
    };

    auto res = hpp::invoke(invokeable, 5);
    std::cout << "invoke returned " << res << std::endl;

    auto tup = std::make_tuple(6);

    auto res1 = hpp::apply(invokeable, tup);
    std::cout << "apply returned " << res1 << std::endl;


	return 0;
}
