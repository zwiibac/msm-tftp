#include <gmock/gmock-actions.h>
#include <sstream>
#include <optional>
#include <string>

#include <boost/asio.hpp>

#include <boost/noncopyable.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <zwiibac/tftp/test_utils/random_iterator.h>
#include <zwiibac/tftp/test_utils/buffer_assert.h>

#include <zwiibac/tftp/action/form_data_packet.h>

namespace {

using namespace zwiibac::tftp;

using namespace zwiibac::tftp::test_utils;

struct FileStreamMock : private boost::noncopyable 
{
    MOCK_METHOD(FileStreamMock&, read, (char* s, size_t n));
    MOCK_METHOD(size_t, gcount, ());
    MOCK_METHOD(void, open, (const char* filename));
    MOCK_METHOD(bool, bad, ()); 
    MOCK_METHOD(bool, is_open, ());
};

struct FileMock 
{
    FileStreamMock file_stream_;
};

struct File_t
{
    std::istringstream file_stream_;
};

template<class File>
struct StateMachine : public File
{
    using ConstBuffer = boost::asio::const_buffer;

    size_t last_sent_block_;
    size_t last_block_size_;
    size_t agreed_block_size_;
    std::optional<ErrorCode> last_error_code_;
    std::array<char, 1024> send_buffer_;

    ConstBuffer send_packet_;
};

struct Event{};

struct SourceState{};

struct TargetState{};

class FormDataPacketTest : public testing::Test 
{
protected:
    Event event;
    SourceState source_state;
    TargetState target_state;
    FormDataPacket sut;
};

TEST_F(FormDataPacketTest, FormFirstDataPacket)
{
    // arrange    
    StateMachine<File_t> machine;

    const size_t file_size = 2100;
    const size_t block_size = 512;

    RandomIterator iter;
    std::string expected_data{iter, iter + file_size};
    machine.file_stream_.str(expected_data);
    machine.agreed_block_size_ = block_size;
    machine.last_sent_block_ = 0;

    // act
    sut(event, machine, source_state, target_state);

    // assert
    EXPECT_EQ(machine.last_error_code_, std::nullopt);
    auto header = HeaderView::FromBuffer(machine.send_buffer_);
    EXPECT_EQ(header.OpCd(), OpCode::Data);
    EXPECT_EQ(header.Word(), 1);
    EXPECT_EQ(machine.last_sent_block_, 1);
    EXPECT_EQ(machine.last_block_size_, block_size);
    EXPECT_EQ(machine.send_packet_.size(), HeaderView::Size() + block_size);
    test_utils::AssertEq(expected_data, machine.send_packet_, 0, block_size);
}

TEST_F(FormDataPacketTest, FormSecondDataPacket)
{
    // arrange
    StateMachine<File_t> machine;

    const size_t file_size = 2100;
    const size_t block_size = 512;

    RandomIterator iter;
    std::string expected_data{iter, iter + file_size};
    machine.file_stream_.str(expected_data);
    machine.file_stream_.seekg(block_size);
    machine.agreed_block_size_ = block_size;
    machine.last_sent_block_ = 1;

    // act
    sut(event, machine, source_state, target_state);

    // assert
    EXPECT_EQ(machine.last_error_code_, std::nullopt);
    auto header = HeaderView::FromBuffer(machine.send_packet_);
    EXPECT_EQ(header.OpCd(), OpCode::Data);
    EXPECT_EQ(header.Word(), 2);
    EXPECT_EQ(machine.last_sent_block_, 2);
    EXPECT_EQ(machine.last_block_size_, block_size);
    EXPECT_EQ(machine.send_packet_.size(), HeaderView::Size() + block_size);
    test_utils::AssertEq(expected_data, machine.send_packet_, 1, block_size);
}

using ::testing::Invoke;
using ::testing::_;
using ::testing::Return;

TEST_F(FormDataPacketTest, FormSecondDataPacketWithIoBad)
{
    // arrange
    StateMachine<FileMock> machine_mock;

    const size_t file_size = 2100;
    const size_t block_size = 512;

    RandomIterator iter;
    std::string expected_data{iter, iter + file_size};
    machine_mock.agreed_block_size_ = block_size;
    machine_mock.last_sent_block_ = 1;

    EXPECT_CALL(machine_mock.file_stream_, read(_,_)).Times(1).WillRepeatedly(Invoke([&](char *, unsigned long)->FileStreamMock&{return machine_mock.file_stream_;}));
    EXPECT_CALL(machine_mock.file_stream_, bad()).Times(1).WillRepeatedly(Return(true));
    EXPECT_CALL(machine_mock.file_stream_, gcount()).Times(0).WillRepeatedly(Return(0));

    // act
    sut(event, machine_mock, source_state, target_state);

    // assert
    EXPECT_NE(machine_mock.last_error_code_, std::nullopt);
}

TEST_F(FormDataPacketTest, FormLastDataPacket)
{
    // arrange
    StateMachine<File_t> machine;

    const size_t file_size = 2100;
    const size_t block_size = 512;

    RandomIterator iter;
    std::string expected_data{iter, iter + file_size};
    machine.file_stream_.str(expected_data);
    machine.file_stream_.seekg((file_size/block_size)*block_size);
    machine.agreed_block_size_ = block_size;
    machine.last_sent_block_ = 4;

    // act
    sut(event, machine, source_state, target_state);

    // assert
    EXPECT_EQ(machine.last_error_code_, std::nullopt);
    auto header = HeaderView::FromBuffer(machine.send_packet_);
    EXPECT_EQ(header.OpCd(), OpCode::Data);
    EXPECT_EQ(header.Word(), 5);
    EXPECT_EQ(machine.last_sent_block_, 5);
    EXPECT_EQ(machine.last_block_size_, file_size % block_size);
    EXPECT_EQ(machine.send_packet_.size(), HeaderView::Size() + file_size % block_size);
    test_utils::AssertEq(expected_data, machine.send_packet_, 5, file_size % block_size);
}

TEST_F(FormDataPacketTest, FormLastDataPacketIsEmpty)
{
    // arrange
    StateMachine<File_t> machine;

    const size_t file_size =2048;
    const size_t block_size = 512;

    RandomIterator iter;
    std::string expected_data{iter, iter + file_size};
    machine.file_stream_.str(expected_data);
    machine.file_stream_.seekg((file_size/block_size)*block_size);
    machine.agreed_block_size_ = block_size;
    machine.last_sent_block_ = 4;

    // act
    sut(event, machine, source_state, target_state);

    // assert
    EXPECT_EQ(machine.last_error_code_, std::nullopt);
    auto header = HeaderView::FromBuffer(machine.send_packet_);
    EXPECT_EQ(header.OpCd(), OpCode::Data);
    EXPECT_EQ(header.Word(), 5);
    EXPECT_EQ(machine.last_sent_block_, 5);
    EXPECT_EQ(machine.last_block_size_, file_size % block_size);
    EXPECT_EQ(machine.send_packet_.size(), HeaderView::Size() + file_size % block_size);
    test_utils::AssertEq(expected_data, machine.send_packet_, 5, file_size % block_size);
}


} // end anonymous namespace

