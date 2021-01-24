#include <gtest/gtest.h>

#include <boost/noncopyable.hpp>

#include <zwiibac/tftp/action/time_out.h>

namespace {

using namespace zwiibac::tftp;

struct TimeoutMachineMock : private boost::noncopyable
{
    size_t resend_count_ = 31;
};

struct SourceState {};
struct TargetState {};
struct Event {};

TEST(TimeoutActionTest, Do) 
{
    // arrange
    TimeoutMachineMock state_machine_mock;
    size_t expected_resend_count = state_machine_mock.resend_count_ + 1;
    SourceState source_state;
    TargetState target_state;
    Event event;
    // act
    TimeOut sut{};
    sut(event, state_machine_mock, source_state, target_state);
    // assert
    EXPECT_EQ(state_machine_mock.resend_count_, expected_resend_count);
}

} // end anonymous namespaceb