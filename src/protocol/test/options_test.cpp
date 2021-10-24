#include <chrono>
#include <gtest/gtest-param-test.h>
#include <optional>
#include <string_view>
#include <string>

#include <gtest/gtest.h>

#include <zwiibac/tftp/protocol/options.h>
#include "simple_input_result_fixture.h"

namespace {

using BlockSizeFixture = zwiibac::tftp::testing::SimpleInputResultFixture<std::string_view, std::optional<size_t>>;
using BlockSizeTestData = BlockSizeFixture::TestData;

TEST_P(BlockSizeFixture, GetBlockSize) 
{
    // arrange
    zwiibac::tftp::BlockSizeDecoder sut;
    // act 
    auto actual_result = sut.GetBlockSize(input);
    // assert
    EXPECT_EQ(actual_result, expected_result);
}

INSTANTIATE_TEST_SUITE_P(GetBlockSizeTest, BlockSizeFixture, ::testing::Values(
    BlockSizeTestData("8",8),
    BlockSizeTestData("65464",65464),
    BlockSizeTestData("389", 389),
    BlockSizeTestData("7",std::nullopt),
    BlockSizeTestData("65465", std::nullopt),
    BlockSizeTestData("6size", std::nullopt),
    BlockSizeTestData("a number", std::nullopt)
));

using TimeOutFixture = zwiibac::tftp::testing::SimpleInputResultFixture<std::string_view, std::optional<std::chrono::milliseconds>>;
using TimeOutTestData = TimeOutFixture::TestData;

TEST_P(TimeOutFixture, GetTimeOut) 
{
    //arrange
    zwiibac::tftp::TimeOutDecoder sut;
    // act
    auto actual_result = sut.GetTimeOut(input);
    // assert
    EXPECT_EQ(actual_result, expected_result);
}

using namespace std::literals::chrono_literals;

INSTANTIATE_TEST_SUITE_P(GetTimeOutTest, TimeOutFixture, ::testing::Values(
    TimeOutTestData("1", 1s),
    TimeOutTestData("31", 31s),
    TimeOutTestData("255", 255s),
    TimeOutTestData("256", std::nullopt),
    TimeOutTestData("-1", std::nullopt),
    TimeOutTestData("0", std::nullopt)
));

using TransferSizeFixture = zwiibac::tftp::testing::SimpleInputResultFixture<std::string_view, std::optional<size_t>>;
using TransferSizeTestData = TransferSizeFixture::TestData;

TEST_P(TransferSizeFixture, GetTransferSize) 
{
    // arrange
    zwiibac::tftp::TransferSizeDecoder sut;
    // act 
    auto actual_result = sut.GetTransferSize(input);
    // assert
    EXPECT_EQ(actual_result, expected_result);
}

INSTANTIATE_TEST_SUITE_P(GetTransferSizeTest, TransferSizeFixture, ::testing::Values(
    TransferSizeTestData("8",8),
    TransferSizeTestData("0",0),
    TransferSizeTestData("389", 389),
    TransferSizeTestData("-1", std::nullopt),
    TransferSizeTestData("6size", std::nullopt),
    TransferSizeTestData("a number", std::nullopt)
));

} // end anonymous namespace