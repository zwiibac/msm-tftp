#include <gtest/gtest.h>

#include <boost/noncopyable.hpp>

#include <zwiibac/tftp/action/reset_time_out.h>

namespace {

using namespace zwiibac::tftp;

struct TimeoutMachineMock : private boost::noncopyable
{
    size_t resend_count_ = 37;
};

struct SourceState {};
struct TargetState {};
struct Event {};

TEST(TimeoutActionTest, Do) 
{
    // arrange
    TimeoutMachineMock state_machine_mock;
    SourceState source_state;
    TargetState target_state;
    Event event;
    // act
    ResetTimeOut sut{};
    sut(event, state_machine_mock, source_state, target_state);
    // assert
    EXPECT_EQ(state_machine_mock.resend_count_, 0);
}

} // end anonymous namespaceb