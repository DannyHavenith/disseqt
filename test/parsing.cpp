/**
 * parsing.cpp
 *
 * This file containts the simplest parsing functions. More advanced parsing
 * is tested with the use of visitors in visitors.cpp
 */

#include "parser.h"
#include "disseqt_visitor.h"
#include <gtest/gtest.h>


TEST( Parsing, SimpleParse)
{
    const std::string q = "select * from a;insert into b values(1,2,3)";
    auto result = disseqt::parse( q);
    EXPECT_EQ( 2, result.size());
}

