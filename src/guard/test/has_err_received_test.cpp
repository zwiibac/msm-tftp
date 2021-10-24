#include <array>
#include <bits/stdint-uintn.h>
#include <span>

#include <boost/asio.hpp>

#include <gtest/gtest.h>

#include <zwiibac/tftp/protocol/header.h>
#include <zwiibac/tftp/protocol/error_code.h>
#include <zwiibac/tftp/guard/has_err_received.h>

namespace {

using namespace zwiibac::tftp;

struct Event{};

struct SourceState {};
struct TargetState {};

struct MachineMock 
{
    using Endpoint = boost::asio::ip::udp::endpoint;

    std::span<const char> received_packet_;
    std::array<char, 16> buffer_;
};

class HasErrReceivedTest : public ::testing::Test
{
protected:
    SourceState source_state;
    TargetState target_state;
    Event event;
    MachineMock machine_mock;
    HeaderProxy header_proxy;

    virtual void SetUp() override {}

public:
    HasErrReceivedTest()
        : source_state()
        , target_state()
        , event()
        , machine_mock()
        , header_proxy(HeaderProxy::FromBuffer(machine_mock.buffer_))
    {}
};

TEST_F(HasErrReceivedTest, Do) 
{
    // arrange
    header_proxy.SetOpCd(OpCode::Error);
    header_proxy.SetWord((uint16_t)ErrorCode::FileNotFound);
    machine_mock.buffer_[HeaderProxy::Size()] = '\0';
    machine_mock.received_packet_ = {machine_mock.buffer_.data(), 5};
    HasErrReceived sut;
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(HasErrReceivedTest, TooSmall) 
{
    // arrange
    header_proxy.SetOpCd(OpCode::Error);
    header_proxy.SetWord((uint16_t)ErrorCode::FileNotFound);
    machine_mock.received_packet_ = {machine_mock.buffer_.data(), 4};
    HasErrReceived sut;
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

TEST_F(HasErrReceivedTest, HasAckOpCode) 
{
    // arrange
    header_proxy.SetOpCd(OpCode::Acknowledgment);
    header_proxy.SetWord((uint16_t)ErrorCode::FileNotFound);
    machine_mock.buffer_[HeaderProxy::Size()] = '\0';
    machine_mock.received_packet_ = {machine_mock.buffer_.data(), 5};
    HasErrReceived sut;
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

} // end anonymous namespace