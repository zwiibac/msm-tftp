#include <iostream>

#include <gtest/gtest.h>

#include <zwiibac/tftp/machine/transition_table.h>

namespace {

using namespace zwiibac::tftp;

// make sure that things at least get compiled

TEST(MachineCtorTest, ReadRequestTransitionTable) 
{
    // arrange
    // act
    ReadRequestTransitionTable sut{};
    // assert
}

} // end anonymous namespace

