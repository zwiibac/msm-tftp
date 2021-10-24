#include <optional>
#include <string_view>
#include <string>

#include <gtest/gtest.h>

#include <zwiibac/tftp/protocol/mode.h>
#include "simple_input_result_fixture.h"

namespace {

using zwiibac::tftp::Mode;
using zwiibac::tftp::GetMode;

using GetModeFixture = zwiibac::tftp::testing::SimpleInputResultFixture<std::string_view, std::optional<Mode>>;
using ModeTestData = GetModeFixture::TestData;

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

INSTANTIATE_TEST_SUITE_P(GetModeTest, GetModeFixture, ::testing::Values(
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
