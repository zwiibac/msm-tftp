#include <iostream>

#include <gtest/gtest.h>

#include <zwiibac/tftp/data/buffer.h>
#include <zwiibac/tftp/data/connection.h>
#include <zwiibac/tftp/data/timer.h>
#include <zwiibac/tftp/data/transfer_state.h>
#include <zwiibac/tftp/data/file.h>

namespace {

using namespace zwiibac::tftp;

// make sure that all policies at least get compiled

TEST(DataCtorTest, Buffer) 
{
    // arrange
    // act
    Buffer sut{};
    // assert
}

TEST(DataCtorTest, Connection) 
{
    // arrange
    Connection::IoContext io_context;
    // act
    Connection sut{io_context};
    //Connection sut;
    // assert
}

TEST(DataCtorTest, Timer) 
{
    // arrange
    Timer::IoContext io_context;
    // act
    Timer sut{io_context};
    //Connection sut;
    // assert
}

TEST(DataCtorTest, TransferState) 
{
    // arrange
    // act
    TransferState sut{};
    // assert
}

TEST(DataCtorTest, File) 
{
    // arrange
    // act
    File sut{};
    // assert
}

} // end anonymous namespace

