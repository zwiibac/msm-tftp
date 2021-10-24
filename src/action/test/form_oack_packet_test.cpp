#include <array>
#include <chrono>

#include <cstddef>
#include <gtest/gtest.h>

#include "zwiibac/tftp/protocol/opcode.h"
#include <zwiibac/tftp/protocol/header.h>
#include <zwiibac/tftp/action/form_oack_packet.h>

namespace {

struct StateMachine 
{
    std::array<char, 1024> send_buffer_;
    boost::asio::const_buffer send_packet_;
    size_t last_sent_block_;
    size_t last_saved_block_;
    bool use_block_size_option_;
    bool use_time_out_option_;
    bool use_transfer_size_option_;
    std::chrono::milliseconds time_out_;
    size_t agreed_block_size_;
    std::optional<size_t> file_size_;
};

struct Event{};

struct SourceState{};

struct TargetState{};

using namespace zwiibac::tftp;

TEST(FormOackTest, Do) 
{
    // arrange
    Event event;
    SourceState source_state;
    TargetState target_state;
    StateMachine machine;
    FormOackPacket sut;

    std::chrono::milliseconds expected_time_out{4000};
    size_t expected_block_size_ = 1024;

    machine.last_sent_block_ = 0;
    machine.last_saved_block_ = 0;
    machine.time_out_ = expected_time_out;
    machine.use_time_out_option_ = true;
    machine.agreed_block_size_ = expected_block_size_;
    machine.use_block_size_option_ = true;
    machine.file_size_ = 1013;
    machine.use_transfer_size_option_ = true;

    // act
    sut(event, machine, source_state, target_state);

    // assert
    auto send_header = HeaderView::FromBuffer(machine.send_packet_);
    EXPECT_EQ(send_header.OpCd(), OpCode::OptionAcknowledgment);
    // TODO more asserts
}

}