// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#ifndef CRUNCH_BENCHMARKING_RESULT_TABLE_HPP
#define CRUNCH_BENCHMARKING_RESULT_TABLE_HPP

#include "crunch/base/noncopyable.hpp"
#include "crunch/benchmarking/result_sink.hpp"
#include "crunch/benchmarking/result_table_descriptor.hpp"

#include <tuple>

namespace Crunch { namespace Benchmarking {

namespace Detail
{
    template<std::size_t Size> struct ColumnNameTuple_;
    template<> struct ColumnNameTuple_<1> { typedef std::tuple<char const*> Type; };
    template<> struct ColumnNameTuple_<2> { typedef std::tuple<char const*, char const*> Type; };
    template<> struct ColumnNameTuple_<3> { typedef std::tuple<char const*, char const*, char const*> Type; };
    template<> struct ColumnNameTuple_<4> { typedef std::tuple<char const*, char const*, char const*, char const*> Type; };
    template<> struct ColumnNameTuple_<5> { typedef std::tuple<char const*, char const*, char const*, char const*, char const*> Type; };
    template<> struct ColumnNameTuple_<6> { typedef std::tuple<char const*, char const*, char const*, char const*, char const*, char const*> Type; };
    template<> struct ColumnNameTuple_<7> { typedef std::tuple<char const*, char const*, char const*, char const*, char const*, char const*, char const*> Type; };
    template<> struct ColumnNameTuple_<8> { typedef std::tuple<char const*, char const*, char const*, char const*, char const*, char const*, char const*, char const*> Type; };
    template<> struct ColumnNameTuple_<9> { typedef std::tuple<char const*, char const*, char const*, char const*, char const*, char const*, char const*, char const*, char const*> Type; };
    template<> struct ColumnNameTuple_<10> { typedef std::tuple<char const*, char const*, char const*, char const*, char const*, char const*, char const*, char const*, char const*, char const*> Type; };
    template<typename RowType> struct ColumnNameTuple : ColumnNameTuple_<std::tuple_size<RowType>::value> {};

    template<typename T> ColumnType GetColumnType();
    template<> ColumnType GetColumnType<double>() { return ColumnType::Float64; }
    template<> ColumnType GetColumnType<int32>() { return ColumnType::Int32; }
    template<> ColumnType GetColumnType<std::string>() { return ColumnType::String; }
    template<> ColumnType GetColumnType<char const*>() { return ColumnType::String; }

    template<typename RowType, std::size_t Index = std::tuple_size<RowType>::value>
    struct ResultAdder
    {
        static void Add(IResultSink& sink, RowType const& row)
        {
            ResultAdder<RowType, Index - 1>::Add(sink, row);
            sink.Add(std::get<Index - 1>(row));
        }
    };

    template<typename RowType>
    struct ResultAdder<RowType, 0>
    {
        static void Add(IResultSink&, RowType const&) {}
    };

    template<typename RowType, std::size_t Index = std::tuple_size<RowType>::value>
    struct ColumnAdder
    {
        typedef typename ColumnNameTuple<RowType>::Type NameType;

        static void Add(ResultTableDescriptor& descriptor, NameType const& names)
        {
            ColumnAdder<RowType, Index - 1>::Add(descriptor, names);
            descriptor.AddColumn(std::get<Index - 1>(names), GetColumnType<typename std::tuple_element<Index - 1, RowType>::type>());
        }
    };

    template<typename RowType>
    struct ColumnAdder<RowType, 0>
    {
        template<typename NameType>
        static void Add(ResultTableDescriptor&, NameType const&) {}
    };
}

template<typename RowType_>
class ResultTable : NonCopyable
{
public:
    typedef RowType_ RowType;
    typedef typename Detail::ColumnNameTuple<RowType>::Type ColumnNameType;

    ResultTable(char const* name, uint32 version, ColumnNameType const& columnNames, IResultSink& sink = GetResultSink());

    ~ResultTable();

    void Add(RowType const& row);

private:
    IResultSink& mSink;
};

template<typename R>
ResultTable<R>::ResultTable(char const* name, uint32 version, ColumnNameType const& columnNames, IResultSink& sink)
    : mSink(sink)
{
    ResultTableDescriptor descriptor(name, version);
    Detail::ColumnAdder<R>::Add(descriptor, columnNames);
    mSink.BeginTable(descriptor);
}

template<typename R>
ResultTable<R>::~ResultTable()
{
    mSink.EndTable();
}

template<typename R>
void ResultTable<R>::Add(R const& row)
{
    mSink.BeginRow();
    Detail::ResultAdder<R, std::tuple_size<R>::value>::Add(mSink, row);
    mSink.EndRow();
}

}}

#endif
