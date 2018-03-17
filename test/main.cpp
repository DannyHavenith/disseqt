/*
 * main.cpp
 *
 * Implementation of the main function for the unit test executable.
 *
 */

#include <gtest/gtest.h>


int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
