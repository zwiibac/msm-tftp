#include "base_fixture.h"

namespace zwiibac {
namespace tftp {
namespace testing {

std::pair<char*, size_t> GetBufferData(const boost::asio::mutable_buffer& buffer) 
{
    return {static_cast<char*>(buffer.data()), buffer.size()};
}

std::pair<const char*, size_t> GetBufferData(const boost::asio::const_buffer& buffer) 
{
    return {static_cast<const char*>(buffer.data()), buffer.size()};
}


void BaseFixture::SetUpFileData() 
{
    auto file_size = (std::max(number_of_blocks - 1, 0) * agreed_block_size) + final_block_size;
    test_utils::RandomIterator file_data_begin;
    file_data = std::string(file_data_begin, file_data_begin + file_size);
    sut.file_stream_.stream_.str(file_data);
    sut.file_size_fake_ = file_size;
}

void BaseFixture::SetUpListenerSocket() 
{
    sut.listener_socket_ = socket_mock;
}

using ::testing::_;
using ::testing::Invoke;

void BaseFixture::ExpectSendDataBlock(uint16_t block) 
{
    EXPECT_CALL(this->sut.socket_, async_send_to(_,remote_end_point, _)).WillOnce(Invoke(
            [block=block, this]
            (const SendBuffer& buffer, const Endpoint&, std::function<void(const SystemErrorCode&, size_t)> handler)
            {
                auto [data, size] = GetBufferData(buffer);
                auto expected_block_size = block<number_of_blocks ? sut.agreed_block_size_ : final_block_size;
                EXPECT_EQ(size, expected_block_size + HeaderView::Size());
                handler(SystemErrorCode(), size);
            }
        ));
}

void BaseFixture::ExpectSendOack(std::initializer_list<const std::string> values) 
{
    EXPECT_CALL(this->sut.socket_, async_send_to(_,remote_end_point, _)).WillOnce(Invoke(
            [values=values, this]
            (const SendBuffer& buffer, const Endpoint&, std::function<void(const SystemErrorCode&, size_t)> handler)
            {
                auto [data, size] = GetBufferData(buffer);
                ExpectOack(data, data + size, values);
                handler(SystemErrorCode(), size);
            }
        ));
}

void BaseFixture::ExpectSendError(ErrorCode error) 
{
    EXPECT_CALL(sut.socket_, async_send_to(_,remote_end_point, _)).WillOnce(Invoke(
            [error=error, this](const SendBuffer& buffer, const Endpoint&, std::function<void(const SystemErrorCode&, size_t)> handler)
            {
                auto [data, size] = GetBufferData(buffer);
                auto expected_error_message_size = 1;
                EXPECT_EQ(size, expected_error_message_size + HeaderView::Size());

                auto header = HeaderView::FromRange(data, data+size);
                EXPECT_EQ(header.OpCd(), OpCode::Error);
                EXPECT_EQ(header.Word(), static_cast<uint16_t>(error));
                
                handler(SystemErrorCode(), size);
            }
        ));
}

void BaseFixture::ExpectReceiveAck(uint16_t block) 
{
    EXPECT_CALL(sut.socket_, async_receive_from(_,_,_)).WillOnce(Invoke(
            [block=block, this](const ReceiveBuffer& buffer, Endpoint& end_point, std::function<void(const SystemErrorCode&, size_t)> handler) 
            {
                end_point = remote_end_point;
                auto [data, size] = GetBufferData(buffer);
                auto bytes_transfered = SetupAck(data, data + size, block);
                handler(SystemErrorCode(), bytes_transfered);          
            }
        ));
}

void BaseFixture::ExpectReceiveTimeOut() 
{
    EXPECT_CALL(sut.socket_, async_receive_from(_,_,_)).WillOnce(Invoke(
            [](const ReceiveBuffer& buffer, Endpoint& end_point, std::function<void(const SystemErrorCode&, size_t)> handler) 
            {
                handler(boost::asio::error::operation_aborted, 0);      
            }
        ));
}

void BaseFixture::ExpectReceiveRequest(OpCode op, std::initializer_list<const std::string> values) 
{
    EXPECT_CALL(*socket_mock, async_receive_from(_,_,_)).WillOnce(Invoke(
        [op=op, values=std::move(values), this]
        (const ReceiveBuffer& buffer, Endpoint& end_point, std::function<void(const SystemErrorCode&, size_t)> handler) 
        {
            end_point = remote_end_point;
            auto [data, size] = GetBufferData(buffer);
            auto bytes_transfered = SetupRequest(data, data + size, op, values);
            handler(SystemErrorCode(), bytes_transfered);          
        }
    ));
}

void BaseFixture::ExpectWhenReset(const std::function<void(const Server&)>& expectation) 
{
    EXPECT_CALL(sut, Reset()).WillOnce(Invoke(
        [expectation=expectation, this]
        ()
        {
            expectation(sut);
        }
    ));
}


} // end namespace testing
} // end namespace tftp
} // end namespace zwiibac
