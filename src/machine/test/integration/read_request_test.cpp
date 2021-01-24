#include <chrono>
#include <functional>
#include <tuple>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "base_fixture.h"

namespace 
{

using namespace zwiibac::tftp;

using zwiibac::tftp::testing::BaseFixture;

using ::testing::_;
using ::testing::InSequence;

class ReadRequestFixture : public BaseFixture, public ::testing::WithParamInterface<std::tuple<uint16_t, size_t>>
{
    public:
    enum Param 
    {
        BlockCount = 0,
        FinalBlockSize = 1
    };
protected:
    std::string file_data;
    Tid tid;

    const std::string local_host = "127.0.0.1";
    
    virtual void SetUp() override 
    {
        number_of_blocks = Get<Param::BlockCount>();
        final_block_size = Get<Param::FinalBlockSize>();

        remote_end_point = Endpoint(boost::asio::ip::address_v4::from_string(local_host), tid.CreateTid());
        
        sut.local_address_ = boost::asio::ip::address_v4::from_string(local_host);

        SetUpFileData();
        SetUpListenerSocket();

        sut.DelegateFileNameMockToFake();
        sut.DelegateFileStreamMockToFake();
        sut.DelegateTimerMockToFake();
    }

    template<size_t Index>
    typename std::tuple_element<Index, ParamType>::type Get() { return std::get<Index>(GetParam()); }    
};

INSTANTIATE_TEST_SUITE_P(ReadRequestTest, ReadRequestFixture, ::testing::Values(
    std::make_tuple(1, 0),
    std::make_tuple(1, 14),
    std::make_tuple(1, 511),
    std::make_tuple(2, 0),
    std::make_tuple(2, 1),
    std::make_tuple(2, 63),
    std::make_tuple(31, 511),
    std::make_tuple(32, 0),
    std::make_tuple(32, 1)
));


TEST_P(ReadRequestFixture, Regular) 
{
    // arrange    
    {
        InSequence seq;

        ExpectReceiveRequest(OpCode::ReadRequest,{"/tmp/hello.txt", "octet"});

        for (uint16_t block=1;block<=number_of_blocks;++block) 
        {
            ExpectSendDataBlock(block);
            ExpectReceiveAck(block);            
        }
    }
    // act
    sut.start();
    // assert
}

TEST_P(ReadRequestFixture, OneTimeOutOnEveryReceiveAck) 
{
    // arrange    
    {
        InSequence seq;

        ExpectReceiveRequest(OpCode::ReadRequest,{"/tmp/hello.txt", "octet"});

        for (uint16_t block=1;block<=number_of_blocks;++block) 
        {
            ExpectSendDataBlock(block);
            ExpectReceiveTimeOut();
            ExpectSendDataBlock(block);
            ExpectReceiveAck(block);
        }
    }
    // act
    sut.start();
    // assert
}

TEST_P(ReadRequestFixture, ThreeTimeOutsOnLastReceiveAck) 
{
    // arrange    
    {
        InSequence seq;

        ExpectReceiveRequest(OpCode::ReadRequest,{"/tmp/hello.txt", "octet"});

        for (uint16_t block=1;block<=number_of_blocks;++block) 
        {
            ExpectSendDataBlock(block);
            if (block != number_of_blocks)
                ExpectReceiveAck(block);
            else
                ExpectReceiveTimeOut();
        }

        for (int resend=0;resend<3;++resend) 
        {
            ExpectSendDataBlock(number_of_blocks);
            ExpectReceiveTimeOut();
        }
        EXPECT_CALL(sut.socket_, async_send_to(_,_, _)).Times(0);
    }
    // act
    sut.start();
    // assert
}

} // end anonymous namespace