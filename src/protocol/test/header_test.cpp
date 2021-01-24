#include <array>

#include <gtest/gtest.h>

#include <zwiibac/tftp/protocol/header.h>

namespace {

using namespace zwiibac::tftp;

class HeaderTest : public testing::Test 
{
protected:
    virtual void SetUp() override 
    {
        header->opcode = 0x0400;
        header->word = 0x3100;
    }

    std::array<char, 10> buffer;
    detail::Header* header = reinterpret_cast<detail::Header*>(buffer.data());
    OpCode expected_op_code = OpCode::Acknowledgment;
    uint16_t expected_word = 49;
};

class ShortHeaderTest : public testing::Test 
{
protected:
    virtual void SetUp() override 
    {
        header->opcode = 0x0400;
    }

    std::array<char, 10> buffer;
    detail::ShortHeader* header = reinterpret_cast<detail::ShortHeader*>(buffer.data());
    OpCode expected_op_code = OpCode::Acknowledgment;
    int16_t expected_word = 49;    
};


TEST_F(HeaderTest, ViewFromHeader) 
{
    // arrange     
    // act
    auto sut = detail::HeaderView::FromHeader(header);
    // assert
    EXPECT_EQ(sut.OpCd(), expected_op_code);
    EXPECT_EQ(sut.Word(), expected_word);
    EXPECT_EQ(detail::HeaderView::Size(), sizeof(detail::Header));
}

TEST_F(HeaderTest, ViewFromBuffer) 
{
    // arrange     
    // act
    auto sut = HeaderView::FromBuffer(buffer);
    // assert
    EXPECT_EQ(sut.OpCd(), expected_op_code);
    EXPECT_EQ(sut.Word(), expected_word);
}

TEST_F(HeaderTest, ProxyFromHeader) 
{
    // arrange     
    // act
    auto sut = detail::HeaderProxy::FromHeader(header);
    // assert
    EXPECT_EQ(sut.OpCd(), expected_op_code);
    EXPECT_EQ(sut.Word(), expected_word);
    EXPECT_EQ(detail::HeaderProxy::Size(), sizeof(detail::Header));
}

TEST_F(HeaderTest, ProxyFromBuffer) 
{
    // arrange     
    // act
    auto sut = HeaderProxy::FromBuffer(buffer);
    // assert
    EXPECT_EQ(sut.OpCd(), expected_op_code);
    EXPECT_EQ(sut.Word(), expected_word);
}

TEST_F(ShortHeaderTest, ViewFromHeader) 
{
    // arrange     
    // act
    auto sut = detail::ShortHeaderView::FromHeader(header);
    // assert
    EXPECT_EQ(sut.OpCd(), expected_op_code);
}

TEST_F(ShortHeaderTest, ViewFromBuffer) 
{
    // arrange     
    // act
    auto sut = ShortHeaderView::FromBuffer(buffer);
    // assert
    EXPECT_EQ(sut.OpCd(), expected_op_code);
}

TEST_F(ShortHeaderTest, ProxyFromHeader) 
{
    // arrange     
    // act
    auto sut = detail::ShortHeaderProxy::FromHeader(header);
    // assert
    EXPECT_EQ(sut.OpCd(), expected_op_code);
}

TEST_F(ShortHeaderTest, ProxyFromBuffer) 
{
    // arrange     
    // act
    auto sut = ShortHeaderProxy::FromBuffer(buffer);
    // assert
    EXPECT_EQ(sut.OpCd(), expected_op_code);
}

} // end anonymous namespace