#include <gtest/gtest.h>

#include <zwiibac/tftp/guard/can_retry.h>

namespace {

using namespace zwiibac::tftp;

struct Event{};

struct SourceState {};
struct TargetState {};

struct MachineMock 
{
    size_t resend_count_;
};

class RetrySendGuardTest : public ::testing::Test
{
protected:
    SourceState source_state;
    TargetState target_state;
    Event event;
    MachineMock machine_mock;
};

TEST_F(RetrySendGuardTest, CannotRetry) 
{
    // arrange
    machine_mock.resend_count_ = 3;
    auto sut = CanRetry{};
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

TEST_F(RetrySendGuardTest, CouldRetry)
{
    // arrange
    machine_mock.resend_count_ = 2;
    auto sut = CanRetry{};
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

} // end anonymous namespace