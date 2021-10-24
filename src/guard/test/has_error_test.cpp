#include <optional>

#include <gtest/gtest.h>

#include <zwiibac/tftp/protocol/error_code.h>
#include <zwiibac/tftp/guard/has_error.h>

namespace {

using namespace zwiibac::tftp;

struct Event{};

struct SourceState {};
struct TargetState {};

struct MachineMock 
{
    std::optional<ErrorCode> last_error_code_;
};

TEST(HasErrorTest, HasAnError) 
{
    // arrange
    SourceState source_state;
    TargetState target_state;
    Event event;
    MachineMock machine_mock{ ErrorCode::NoSuchUser };
    HasError sut;
    // act
    auto result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(result);
}

TEST(HasErrorTest, HasNoError) 
{
    // arrange
    SourceState source_state;
    TargetState target_state;
    Event event;
    MachineMock machine_mock{ std::nullopt };
    HasError sut;
    // act
    auto result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(result);
}




} // end anonymous namespace