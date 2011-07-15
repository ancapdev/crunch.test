// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BENCHMARKING_STREAM_RESULT_SINK_HPP
#define CRUNCH_BENCHMARKING_STREAM_RESULT_SINK_HPP

#include "crunch/base/noncopyable.hpp"
#include "crunch/benchmarking/result_sink.hpp"
#include "crunch/benchmarking/result_table_descriptor.hpp"

#include <iosfwd>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

namespace Crunch { namespace Benchmarking {

class StreamResultSink : public IResultSink, NonCopyable
{
public:
    StreamResultSink(std::ostream& stream, bool showProgress = true);

    virtual void BeginTable(ResultTableDescriptor const& descriptor);
    virtual void EndTable();

    virtual void BeginRow();
    virtual void EndRow();

    virtual void Add(double value);
    virtual void Add(int32 value);
    virtual void Add(const std::string& value);

private:
    typedef std::vector<std::pair<std::string, bool>> Row;
    typedef std::vector<Row> RowList;

    std::ostream& mStream;
    bool const mShowProgress;
    std::stringstream mTempStream;
    std::unique_ptr<ResultTableDescriptor> mCurrentDescriptor;
    RowList mRows;
};

}}

#endif
