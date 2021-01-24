#include <gtest/gtest.h>

#include <zwiibac/tftp/guard/logical_operators.h>
namespace {

using namespace zwiibac::tftp;

struct Event{};

struct SourceState {};
struct TargetState {};

struct Machine {};

struct TrueGuard
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    bool operator()(Evt const&,Fsm& fsm,const SourceState&,const TargetState&) const
    {
        return true;
    }
};

struct FalseGuard 
{
    template <class Evt,class Fsm,class SourceState,class TargetState>
    bool operator()(Evt const&,Fsm& fsm,const SourceState&,const TargetState&) const
    {
        return false;
    }
};

class LogicalOperatorsTest : public ::testing::Test
{
protected:
    SourceState source_state;
    TargetState target_state;
    Event event;
    Machine machine;
};

TEST_F(LogicalOperatorsTest, TrueAndTrue) 
{
    // arrange
    auto sut = And<TrueGuard,TrueGuard>{};
    // act
    auto actual_result = sut(event, machine, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(LogicalOperatorsTest, FalseAndTrue) 
{
    // arrange
    auto sut = And<FalseGuard,TrueGuard>{};
    // act
    auto actual_result = sut(event, machine, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

TEST_F(LogicalOperatorsTest, TrueAndFalse) 
{
    // arrange
    auto sut = And<TrueGuard,FalseGuard>{};
    // act
    auto actual_result = sut(event, machine, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

TEST_F(LogicalOperatorsTest, FalseAndFalse) 
{
    // arrange
    auto sut = And<FalseGuard,FalseGuard>{};
    // act
    auto actual_result = sut(event, machine, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

TEST_F(LogicalOperatorsTest, TrueOrTrue) 
{
    // arrange
    auto sut = Or<TrueGuard,TrueGuard>{};
    // act
    auto actual_result = sut(event, machine, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(LogicalOperatorsTest, FalseOrTrue) 
{
    // arrange
    auto sut = Or<FalseGuard,TrueGuard>{};
    // act
    auto actual_result = sut(event, machine, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(LogicalOperatorsTest, TrueOrFalse) 
{
    // arrange
    auto sut = Or<TrueGuard,FalseGuard>{};
    // act
    auto actual_result = sut(event, machine, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

TEST_F(LogicalOperatorsTest, FalseOrFalse) 
{
    // arrange
    auto sut = Or<FalseGuard,FalseGuard>{};
    // act
    auto actual_result = sut(event, machine, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

TEST_F(LogicalOperatorsTest, NotTrue) 
{
    // arrange
    auto sut = Not<TrueGuard>{};
    // act
    auto actual_result = sut(event, machine, source_state, target_state);
    // assert
    EXPECT_FALSE(actual_result);
}

TEST_F(LogicalOperatorsTest, NotFalse) 
{
    // arrange
    auto sut = Not<FalseGuard>{};
    // act
    auto actual_result = sut(event, machine, source_state, target_state);
    // assert
    EXPECT_TRUE(actual_result);
}

} // end anonymous namespace