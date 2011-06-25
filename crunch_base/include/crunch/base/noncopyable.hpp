// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BASE_NONCOPYABLE_HPP
#define CRUNCH_BASE_NONCOPYABLE_HPP

namespace Crunch {

class NonCopyable
{
protected:
    NonCopyable() {}

private:
    NonCopyable(NonCopyable const&);
    NonCopyable& operator = (NonCopyable const&);
};

}

#endif
