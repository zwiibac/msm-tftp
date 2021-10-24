#include <gtest/gtest.h>

#include "zwiibac/tftp/guard/has_more_data_to_send.h"
namespace {

using namespace zwiibac::tftp;

struct Event{};

struct SourceState {};
struct TargetState {};

struct MachineMock 
{
    size_t agreed_block_size_ = 512;
    size_t last_block_size_ = 0;
    size_t last_sent_block_ = 0;
};

class HasMoredataToSendTest : public ::testing::Test
{
protected:
    SourceState source_state;
    TargetState target_state;
    Event event;
    MachineMock machine_mock;

    HasMoreToSend sut;
};

TEST_F(HasMoredataToSendTest, NoMoreData) 
{
    // arrange
    machine_mock.last_block_size_ = 0;
    machine_mock.last_sent_block_ = 23;
    
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

TEST_F(HasMoredataToSendTest, HasSentOack) 
{
    // arrange
    machine_mock.last_block_size_ = 0;
    machine_mock.last_sent_block_ = 0;
    
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(HasMoredataToSendTest, HasSentSomeData) 
{
    // arrange
    machine_mock.last_block_size_ = machine_mock.agreed_block_size_;
    machine_mock.last_sent_block_ = 3;
    // act
    auto actual_result = sut(event, machine_mock, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}


} // end anonymous namespace
