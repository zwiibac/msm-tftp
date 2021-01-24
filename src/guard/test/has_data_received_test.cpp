#include <array>
#include <span>

#include <gtest/gtest.h>

#include <zwiibac/tftp/protocol/header.h>
#include <zwiibac/tftp/guard/has_data_received.h>

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
    Endpoint agreed_remote_endpoint_;
    Endpoint last_remote_endpoint_;
};

class HasDataReceivedTest : public ::testing::Test
{
protected:
    SourceState source_state;
    TargetState target_state;
    Event event;
    MachineMock machine_mock;
    HeaderProxy header_proxy;

    virtual void SetUp() override 
    {
        header_proxy = HeaderProxy::FromBuffer(machine_mock.buffer_);
    }

public:
    HasDataReceivedTest()
        : source_state()
        , target_state()
        , event()
        , machine_mock()
        , header_proxy(HeaderProxy::FromBuffer(machine_mock.buffer_))
    {}
};

TEST_F(HasDataReceivedTest, PacketWithNoData) 
{
    // arrange
    header_proxy.SetOpCd(OpCode::Data);
    machine_mock.received_packet_ = {machine_mock.buffer_.data(), 4};
    auto sut = HasDataReceived{};
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(HasDataReceivedTest, Do) 
{
    // arrange
    header_proxy.SetOpCd(OpCode::Data);
    machine_mock.received_packet_ = {machine_mock.buffer_.data(), 5};
    auto sut = HasDataReceived{};
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(HasDataReceivedTest, PacketTooSmall) 
{
    // arrange
    header_proxy.SetOpCd(OpCode::Data);
    machine_mock.received_packet_ = {machine_mock.buffer_.data(), 3};
    auto sut = HasDataReceived{};
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

TEST_F(HasDataReceivedTest, WrongOp) 
{
    // arrange
    header_proxy.SetOpCd(OpCode::Acknowledgment);
    machine_mock.received_packet_ = {machine_mock.buffer_.data(), 4};
    auto sut = HasDataReceived{};
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

} // end anonymous namespace