#include "zwiibac/tftp/protocol/header.h"
#include <bits/stdint-uintn.h>
#include <span>

#include <gtest/gtest.h>

#include <zwiibac/tftp/guard/is_old_data.h>

namespace {

using namespace zwiibac::tftp;

struct Event{};

struct SourceState {};
struct TargetState {};

struct MachineMock 
{
    using Endpoint = boost::asio::ip::udp::endpoint;

    std::array<char, 16> buffer_;
    std::span<const char> received_packet_ = {buffer_.data(), 4};

    size_t last_saved_block_ = 0;
};

class IsOldDataTest : public ::testing::Test
{
protected:
    SourceState source_state;
    TargetState target_state;
    Event event;
    MachineMock machine_mock;

    IsOldData sut;
};

TEST_F(IsOldDataTest, IsOld) 
{
    // arrange
    uint16_t data_block;
    auto received_header = HeaderProxy::FromBuffer(machine_mock.buffer_);
    received_header.SetWord(data_block);
    machine_mock.last_saved_block_ = data_block + 1;
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(IsOldDataTest, IsLast) 
{
    // arrange
    uint16_t data_block;
    auto received_header = HeaderProxy::FromBuffer(machine_mock.buffer_);
    received_header.SetWord(data_block);
    machine_mock.last_saved_block_ = data_block;
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(IsOldDataTest, IsNew) 
{
    // arrange
    uint16_t data_block;
    auto received_header = HeaderProxy::FromBuffer(machine_mock.buffer_);
    received_header.SetWord(data_block);
    machine_mock.last_saved_block_ = data_block - 1;
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

} // end anonymous namespace
