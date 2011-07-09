// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/benchmarking/result_sink.hpp"
#include "crunch/benchmarking/stream_result_sink.hpp"

#include <iostream>

namespace Crunch { namespace Benchmarking {

IResultSink& GetResultSink()
{
    // TODO: make configurable
    static StreamResultSink sink(std::cout);
    return sink;
}

}}
