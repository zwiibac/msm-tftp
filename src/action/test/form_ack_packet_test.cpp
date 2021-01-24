#include "zwiibac/tftp/protocol/opcode.h"
#include <array>
#include <span>

#include <boost/asio.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <zwiibac/tftp/protocol/header.h>
#include <zwiibac/tftp/action/form_ack_packet.h>

namespace {

struct StateMachine 
{
    std::array<char, 1024> receive_buffer_;
    std::array<char, 1024> send_buffer_;

    std::span<const char> received_packet_ = {receive_buffer_.data(), 23};
    boost::asio::const_buffer send_packet_;
    size_t last_saved_block_;
};

struct Event{};

struct SourceState{};

struct TargetState{};

using namespace zwiibac::tftp;

TEST(FormAckTest, Do) 
{
    // arrange
    Event event;
    SourceState source_state;
    TargetState target_state;
    StateMachine machine;
    FormAckPacket sut;

    uint16_t expected_block = 439;

    auto received_header = HeaderProxy::FromBuffer(machine.receive_buffer_);
    received_header.SetWord(expected_block);
    machine.last_saved_block_ = expected_block;

    // act
    sut(event, machine, source_state, target_state);

    // assert
    auto send_header = HeaderView::FromBuffer(machine.send_packet_);
    EXPECT_EQ(send_header.Word(), expected_block);
    EXPECT_EQ(send_header.OpCd(), OpCode::Acknowledgment);

}

}