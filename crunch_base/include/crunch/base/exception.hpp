// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BASE_EXCEPTION_HPP
#define CRUNCH_BASE_EXCEPTION_HPP

#include "crunch/base/override.hpp"

#include <exception>
#include <string>

namespace Crunch {

class Exception : public std::exception
{
public:
    Exception(std::string const& message)
        : mMessage(message)
    {}

    ~Exception() noexcept {}

    char const* what() const noexcept CRUNCH_OVERRIDE
    {
        return mMessage.c_str();
    }

private:
    std::string mMessage;
};

}

#endif
