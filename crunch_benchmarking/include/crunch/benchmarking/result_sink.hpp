// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BENCHMARKING_RESULT_SINK_HPP
#define CRUNCH_BENCHMARKING_RESULT_SINK_HPP

#include "crunch/base/stdint.hpp"

#include <string>

namespace Crunch { namespace Benchmarking {

class ResultTableDescriptor;

struct IResultSink
{
    virtual void BeginTable(ResultTableDescriptor const& descriptor) = 0;
    virtual void EndTable() = 0;

    virtual void BeginRow() = 0;
    virtual void EndRow() = 0;

    virtual void Add(double value) = 0;
    virtual void Add(int32 value) = 0;
    virtual void Add(const std::string& value) = 0;
};

IResultSink& GetResultSink();

}}

#endif
