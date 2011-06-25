#ifndef CRUNCH_BASE_STREAM_AS_BINARY_HPP
#define CRUNCH_BASE_STREAM_AS_BINARY_HPP

#include <ostream>

namespace Crunch {

template<typename T>
struct StreamAsBinaryHelper
{
    StreamAsBinaryHelper(T value, int delimitPer = 4) : value(value), delimitPer(delimitPer) {}

    T value;
    int delimitPer;
};

template<typename T>
std::ostream& operator << (std::ostream& os, StreamAsBinaryHelper<T> value)
{
    for (int i = 0; i < (sizeof(T) * 8); ++i)
    {
        if (i != 0 && i % value.delimitPer == 0)
            os << " ";

        os  << ((value.value & (1 << i)) == 0 ? '0' : '1');
    }

    return os;
}

template<typename T>
StreamAsBinaryHelper<T> StreamAsBinary(T value, int delimitPer = 4)
{
    return StreamAsBinaryHelper<T>(value, delimitPer);
}

}

#endif
