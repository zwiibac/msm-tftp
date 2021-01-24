#include "zwiibac/tftp/protocol/header.h"
#include <bits/stdint-uintn.h>
#include <gmock/gmock-actions.h>
#include <sstream>
#include <optional>
#include <string>
#include <span>
#include <algorithm>

#include <boost/asio.hpp>

#include <boost/noncopyable.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <zwiibac/tftp/test_utils/random_iterator.h>
#include <zwiibac/tftp/test_utils/buffer_assert.h>
#include <zwiibac/tftp/protocol/header.h>

#include <zwiibac/tftp/action/write_data.h>

namespace {

using namespace zwiibac::tftp;

using namespace zwiibac::tftp::test_utils;

struct FileStreamMock : private boost::noncopyable 
{
    // istream& read (char* s, streamsize n);
    MOCK_METHOD(FileStreamMock&, write, (const char* s, size_t n));
    MOCK_METHOD(bool, bad, ()); 
    MOCK_METHOD(bool, fail, ()); 
    MOCK_METHOD(bool, is_open, ());
};

struct FileMock 
{
    FileStreamMock file_stream_;
};

struct File_t
{
    std::ostringstream file_stream_;
};

template<class File>
struct StateMachine : public File
{
    size_t last_saved_block_;
    size_t last_block_size_;
    std::optional<ErrorCode> last_error_code_;
    std::array<char, 1024> receive_buffer_;
    std::span<const char> received_packet_;
};

struct Event{};

struct SourceState{};

struct TargetState{};

class WriteDataTest : public testing::Test 
{
protected:
    Event event;
    SourceState source_state;
    TargetState target_state;
    WriteData sut;
};

TEST_F(WriteDataTest, Do)
{
    // arrange
    StateMachine<File_t> machine;

    const size_t block_size = 512;
    const uint16_t block = 97;

    RandomIterator iter;
    std::string expected_data{iter, iter + block_size};
    machine.file_stream_.str("");

    auto received_header = HeaderProxy::FromBuffer(machine.receive_buffer_);
    received_header.SetOpCd(OpCode::Data);
    received_header.SetWord(block);

    std::copy(expected_data.cbegin(), expected_data.cend(), machine.receive_buffer_.begin() + HeaderProxy::Size());
    machine.received_packet_ = {machine.receive_buffer_.data(), expected_data.size() + HeaderProxy::Size()};

    // act
    sut(event, machine, source_state, target_state);

    // assert
    EXPECT_EQ(machine.file_stream_.str(), expected_data);
    EXPECT_EQ(machine.last_block_size_, block_size);
    EXPECT_EQ(machine.last_saved_block_, block);
}

using ::testing::Invoke;
using ::testing::_;
using ::testing::Return;


TEST_F(WriteDataTest, IoError) 
{
    // arrange
    StateMachine<FileMock> machine_mock;    

    EXPECT_CALL(machine_mock.file_stream_, write(_,_)).Times(1).WillRepeatedly(Invoke([&](const char *, unsigned long)->FileStreamMock&{return machine_mock.file_stream_;}));
    EXPECT_CALL(machine_mock.file_stream_, fail()).Times(1).WillRepeatedly(Return(true));

    // act
    sut(event, machine_mock, source_state, target_state);

    // assert
    EXPECT_NE(machine_mock.last_error_code_, std::nullopt);
}

} // end anonymous namespace

