#include <span>

#include <gtest/gtest.h>

#include <zwiibac/tftp/guard/has_read_request_received.h>

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
};

class HasReadRequestReceivedTest : public ::testing::Test
{
protected:
    SourceState source_state;
    TargetState target_state;
    Event event;
    MachineMock machine_mock;
    HasReadRequestReceived sut;
};

TEST_F(HasReadRequestReceivedTest, Do) 
{
    // arrange
    auto received_header = HeaderProxy::FromBuffer(machine_mock.buffer_);
    received_header.SetOpCd(OpCode::ReadRequest);
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(HasReadRequestReceivedTest, Fail) 
{
    // arrange
    auto received_header = HeaderProxy::FromBuffer(machine_mock.buffer_);
    received_header.SetOpCd(OpCode::WriteRequest);
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

} // end namespace anonymous