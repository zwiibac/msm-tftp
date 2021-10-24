#include <array>
#include <span>

#include <boost/asio.hpp>

#include <gtest/gtest.h>

#include <zwiibac/tftp/guard/has_ack_received.h>
#include <zwiibac/tftp/protocol/header.h>

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
    size_t last_sent_block_;
};

class HasAckReceivedTest : public ::testing::Test
{
protected:
    SourceState source_state;
    TargetState target_state;
    Event event;
    MachineMock machine_mock;
    HeaderProxy header_proxy;

    virtual void SetUp() override {}

public:
    HasAckReceivedTest()
        : source_state()
        , target_state()
        , event()
        , machine_mock()
        , header_proxy(HeaderProxy::FromBuffer(machine_mock.buffer_))
    {}
};

TEST_F(HasAckReceivedTest, Do) 
{
    // arrange
    header_proxy.SetOpCd(OpCode::Acknowledgment);
    header_proxy.SetWord(41);
    machine_mock.last_sent_block_ = 41;
    machine_mock.received_packet_ = {machine_mock.buffer_.data(), 4};
    auto sut = HasAckReceived{};
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(HasAckReceivedTest, PacketTooLarge) 
{
    // arrange
    header_proxy.SetOpCd(OpCode::Acknowledgment);
    header_proxy.SetWord(41);
    machine_mock.last_sent_block_ = 41;
    machine_mock.received_packet_ = {machine_mock.buffer_.data(), 5};
    auto sut = HasAckReceived{};
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

TEST_F(HasAckReceivedTest, PacketTooSmall) 
{
    // arrange
    header_proxy.SetOpCd(OpCode::Acknowledgment);
    header_proxy.SetWord(41);
    machine_mock.last_sent_block_ = 41;
    machine_mock.received_packet_ = {machine_mock.buffer_.data(), 3};
    auto sut = HasAckReceived{};
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

TEST_F(HasAckReceivedTest, WrongOp) 
{
    // arrange
    header_proxy.SetOpCd(OpCode::Data);
    header_proxy.SetWord(41);
    machine_mock.last_sent_block_ = 41;
    machine_mock.received_packet_ = {machine_mock.buffer_.data(), 4};
    auto sut = HasAckReceived{};
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

TEST_F(HasAckReceivedTest, AckOfNotYetSentPackage) 
{
    // arrange
    header_proxy.SetOpCd(OpCode::Acknowledgment);
    header_proxy.SetWord(42);
    machine_mock.last_sent_block_ = 41;
    machine_mock.received_packet_ = {machine_mock.buffer_.data(), 4};
    auto sut = HasAckReceived{};
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

} // end anonymous namespace