#include <gtest/gtest.h>

#include <boost/asio.hpp>

#include <zwiibac/tftp/guard/from_agreed_endpoint.h>

namespace {

using namespace zwiibac::tftp;

struct Event{};

struct SourceState {};
struct TargetState {};

struct MachineMock 
{
    using Endpoint = boost::asio::ip::udp::endpoint;

    Endpoint agreed_remote_endpoint_;
    Endpoint last_remote_endpoint_;
};

class FromAgreedEndpointTest : public ::testing::Test
{
    protected:
    SourceState source_state;
    TargetState target_state;
    Event event;
    MachineMock machine_mock;

    FromAgreedEndpoint sut;
};

TEST_F(FromAgreedEndpointTest, Success) 
{
    // arrange
    machine_mock.agreed_remote_endpoint_ = MachineMock::Endpoint(
        boost::asio::ip::address_v4::from_string("13.0.0.27"), 1037);
    machine_mock.last_remote_endpoint_ = MachineMock::Endpoint(
        boost::asio::ip::address_v4::from_string("13.0.0.27"), 1037);
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(FromAgreedEndpointTest, WrongPort) 
{
    // arrange
    machine_mock.agreed_remote_endpoint_ = MachineMock::Endpoint(
        boost::asio::ip::address_v4::from_string("13.0.0.27"), 1037);
    machine_mock.last_remote_endpoint_ = MachineMock::Endpoint(
        boost::asio::ip::address_v4::from_string("13.0.0.27"), 1036);
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

TEST_F(FromAgreedEndpointTest, WrongAddress) 
{
    // arrange
    machine_mock.agreed_remote_endpoint_ = MachineMock::Endpoint(
        boost::asio::ip::address_v4::from_string("14.0.0.27"), 1037);
    machine_mock.last_remote_endpoint_ = MachineMock::Endpoint(
        boost::asio::ip::address_v4::from_string("13.0.0.27"), 1037);
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}


} // end anonymous namespace