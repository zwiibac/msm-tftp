#pragma once

#include <optional>
#include <tuple>

#include <gtest/gtest.h>

namespace zwiibac {
namespace tftp {
namespace testing {

template<typename Tinput, typename Toutput>
class SimpleInputResultFixture : public ::testing::TestWithParam<std::tuple<Tinput, Toutput>> 
{
public:
    using TestData = std::tuple<Tinput, Toutput>;

    enum Param : size_t 
    {
        Input = 0,
        Result = 1
    };
protected:
    virtual void SetUp() override 
    {
        input = Get<Param::Input>();
        expected_result = Get<Param::Result>();
    }
    
    template<size_t Index>
    typename std::tuple_element_t<Index, TestData> Get() 
    { 
        return std::get<Index>(::testing::TestWithParam<TestData>::GetParam()); 
    }
    
    Tinput input;
    Toutput expected_result;
};

} // end namespace testing
} // end namespace tftp
} // end namespace zwiibac