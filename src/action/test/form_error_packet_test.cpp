#include <optional>

#include <boost/asio.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <zwiibac/tftp/test_utils/random_iterator.h>
#include <zwiibac/tftp/test_utils/buffer_assert.h>

#include <zwiibac/tftp/action/form_error_packet.h>

namespace {

using namespace zwiibac::tftp;

using namespace zwiibac::tftp::test_utils;

struct StateMachine 
{
    using ConstBuffer = boost::asio::const_buffer;

    std::optional<ErrorCode> last_error_code_;
    std::array<char, 1024> send_buffer_;
    size_t last_block_size_;
    ConstBuffer send_packet_;
};

struct Event{};

struct SourceState{};

struct TargetState{};

class FormErrorPacketFixture : public ::testing::TestWithParam<std::tuple<std::optional<ErrorCode>, uint16_t>>
{
public:
    enum Param : size_t
    {
        ErrorCodeOption,
        ErrorNumber
    };
protected:
    Event event;
    StateMachine machine;
    SourceState source_state;
    TargetState target_state;
    FormErrorPacket sut;

    std::optional<ErrorCode> error_code;
    uint16_t expected_error_code;

    virtual void SetUp() override 
    {
        machine.last_error_code_ = Get<Param::ErrorCodeOption>();
        expected_error_code = Get<Param::ErrorNumber>();
    }

    template<size_t Index>
    typename std::tuple_element_t<Index, ParamType> Get() { return std::get<Index>(GetParam()); }
};

TEST_P(FormErrorPacketFixture, Do) 
{
    // arrange
    // act
    sut(event, machine, source_state, target_state);
    // assert
    auto header = HeaderProxy::FromBuffer(machine.send_buffer_);
    EXPECT_EQ(header.OpCd(), OpCode::Error);
    EXPECT_EQ(header.Word(), expected_error_code);
    EXPECT_EQ(machine.send_buffer_[HeaderProxy::Size()], '\0');
    EXPECT_EQ(machine.send_packet_.size(), 5);
}

INSTANTIATE_TEST_SUITE_P(FormErrorPacketTest, FormErrorPacketFixture, testing::Values(
    std::make_tuple(ErrorCode::AccessViolation, 2),
    std::make_tuple(ErrorCode::NotDefined, 0),
    std::make_tuple(ErrorCode::NoSuchUser, 7),
    std::make_tuple(std::nullopt, 0))
);

} // end anonymous namespace