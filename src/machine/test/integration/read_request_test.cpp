#include <chrono>
#include <cstddef>
#include <functional>
#include <tuple>
#include <string>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "base_fixture.h"

namespace 
{

using namespace zwiibac::tftp;

using zwiibac::tftp::testing::BaseFixture;

using ::testing::_;
using ::testing::InSequence;

class ReadRequestFixtureAbs : public BaseFixture
{
protected:
    std::string file_data;
    Tid tid;

    const std::string local_host = "127.0.0.1";

    virtual void HandleParam() = 0;
    
    virtual void SetUp() override 
    {
        HandleParam();

        remote_end_point = Endpoint(boost::asio::ip::address_v4::from_string(local_host), tid.CreateTid());        
        sut.local_address_ = boost::asio::ip::address_v4::from_string(local_host);

        SetUpFileData();
        SetUpListenerSocket();

        sut.DelegateFileNameMockToFake();
        sut.DelegateFileStreamMockToFake();
        sut.DelegateTimerMockToFake();
    } 
};

class ReadRequestFixture : public ReadRequestFixtureAbs, public ::testing::WithParamInterface<std::tuple<uint16_t, size_t>>
{
public:
    enum Param 
    {
        BlockCount = 0,
        FinalBlockSize = 1,
    };

protected:
    virtual void HandleParam() 
    {
        number_of_blocks = Get<Param::BlockCount>();
        final_block_size = Get<Param::FinalBlockSize>();
        agreed_block_size = 512;
    }

    template<size_t Index>
    typename std::tuple_element_t<Index, ParamType> Get() { return std::get<Index>(GetParam()); }
};


class ReadRequestBlockSizeOptionFixture : public ReadRequestFixtureAbs, public ::testing::WithParamInterface<std::tuple<uint16_t, size_t, size_t>>
{
public:
    enum Param 
    {
        BlockCount = 0,
        FinalBlockSize = 1,
        BlockSize = 2
    };

protected:
    virtual void HandleParam() override
    {
        number_of_blocks = Get<Param::BlockCount>();
        final_block_size = Get<Param::FinalBlockSize>();
        agreed_block_size = Get<Param::BlockSize>();
    }

    template<size_t Index>
    typename std::tuple_element_t<Index, ParamType> Get() { return std::get<Index>(GetParam()); }
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

INSTANTIATE_TEST_SUITE_P(ReadRequestBlockSizeTest, ReadRequestBlockSizeOptionFixture, ::testing::Values(
    std::make_tuple(1, 0, 2048),
    std::make_tuple(1, 14, 2048),
    std::make_tuple(1, 2047, 2048),
    std::make_tuple(1, 0, 2797),
    std::make_tuple(1, 14, 2797),
    std::make_tuple(1, 2796, 2797),
    std::make_tuple(2, 0, 2048),
    std::make_tuple(2, 1, 2048),
    std::make_tuple(2, 63, 2048),
    std::make_tuple(2, 0, 2797),
    std::make_tuple(2, 1, 2797),
    std::make_tuple(2, 63, 2797),
    std::make_tuple(31, 1023, 1024),
    std::make_tuple(32, 0, 1024),
    std::make_tuple(32, 1, 1024)
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

TEST_P(ReadRequestFixture, WithTimeOutOption) 
{
    // arrange 
    {
        InSequence seq;
        std::string expected_time_out = "4";

        ExpectReceiveRequest(OpCode::ReadRequest,{"/tmp/hello.txt", "octet", "TiMeOut", expected_time_out});
        ExpectSendOack({"timeout", expected_time_out});
        ExpectReceiveAck(0);

        for (uint16_t block=1;block<=number_of_blocks;++block) 
        {
            ExpectSendDataBlock(block);
            ExpectReceiveAck(block);            
        }

        ExpectWhenReset([](const Server& sut)
            {
                using namespace std::literals::chrono_literals;
                EXPECT_EQ(sut.time_out_, 4s);
            });
    }
    // act
    sut.start();
    // assert
    
}

TEST_P(ReadRequestFixture, WithTransferSizeOption) 
{
    // arrange    
    {
        InSequence seq;
        size_t expected_file_size = (std::max(number_of_blocks - 1, 0) * agreed_block_size) + final_block_size;

        ExpectReceiveRequest(OpCode::ReadRequest,{"/tmp/hello.txt", "octet", "TsIZe", std::to_string(0)});
        ExpectSendOack({"tsize", std::to_string(expected_file_size)});
        ExpectReceiveAck(0);

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

TEST_P(ReadRequestBlockSizeOptionFixture, WithBlockSizeOption) 
{
    // arrange    
    {
        InSequence seq;

        ExpectReceiveRequest(OpCode::ReadRequest,{"/tmp/hello.txt", "octet", "bLkSIze", std::to_string(agreed_block_size)});
        ExpectSendOack({"blksize", std::to_string(agreed_block_size)});
        ExpectReceiveAck(0);

        for (uint16_t block=1;block<=number_of_blocks;++block) 
        {
            ExpectSendDataBlock(block);
            ExpectReceiveAck(block);            
        }

        ExpectWhenReset([=](const Server& sut)
            {
                EXPECT_EQ(sut.agreed_block_size_, agreed_block_size);
            });
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