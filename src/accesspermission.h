#pragma once

namespace asyncgi::detail{

template<typename T>
struct AccessPermission{
private:
    AccessPermission() = default;
    friend T;
};

}