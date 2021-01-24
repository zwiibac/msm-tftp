#include <optional>
#include <tuple>
#include <string_view>
#include <string>

#include <gtest/gtest.h>

#include <zwiibac/tftp/protocol/mode.h>

namespace {

using namespace zwiibac::tftp;

using ModeTestData = std::tuple<std::string_view, std::optional<Mode> >;

class GetModeFixture : public testing::TestWithParam<ModeTestData> 
{
public:
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
    typename std::tuple_element<Index, ParamType>::type Get() { return std::get<Index>(GetParam()); }
    
    std::string_view input;
    std::optional<Mode> expected_result;
};

TEST_P(GetModeFixture, Do) 
{
    // arrange
    // act
    auto actual_result = GetMode(input);
    // assert
    EXPECT_EQ(actual_result, expected_result)
        << actual_result.value_or(static_cast<Mode>(0)) 
        << "!=" 
        << expected_result.value_or(static_cast<Mode>(0));
}

INSTANTIATE_TEST_SUITE_P(GetModeTest, GetModeFixture, testing::Values(
    ModeTestData{"mail", Mode::Mail},
    ModeTestData{"mAIl", Mode::Mail},
    ModeTestData{"majl", std::nullopt},
    ModeTestData{"mails", std::nullopt},
    ModeTestData{"ail", std::nullopt},
    ModeTestData{"mai", std::nullopt},
    ModeTestData{"", std::nullopt},
    ModeTestData{"octet", Mode::Octet},
    ModeTestData{"netascii", Mode::NetAscii},
    ModeTestData{"NEtasCiI", Mode::NetAscii}
));

} // end anonymous namespace
