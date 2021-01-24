#include "zwiibac/tftp/protocol/header.h"
#include <bits/stdint-uintn.h>
#include <span>

#include <gtest/gtest.h>

#include <zwiibac/tftp/guard/is_old_ack.h>

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

    size_t last_sent_block_ = 0;
};

class IsOldAckTest : public ::testing::Test
{
protected:
    SourceState source_state;
    TargetState target_state;
    Event event;
    MachineMock machine_mock;

    IsOldAck sut;
};

TEST_F(IsOldAckTest, IsOld) 
{
    // arrange
    uint16_t data_block;
    auto received_header = HeaderProxy::FromBuffer(machine_mock.buffer_);
    received_header.SetWord(data_block);
    machine_mock.last_sent_block_ = data_block + 1;
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(IsOldAckTest, IsCurrent) 
{
    // arrange
    uint16_t data_block;
    auto received_header = HeaderProxy::FromBuffer(machine_mock.buffer_);
    received_header.SetWord(data_block);
    machine_mock.last_sent_block_ = data_block;
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}


} // end anonymous namespace
