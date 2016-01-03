/*
 * parser.h
 *
 *  Declaration of the parse() function.
 *
 *  This function takes an iterator pair and
 *
 */

#ifndef DISSEQT_PARSER_H_
#define DISSEQT_PARSER_H_

#include <stdexcept>
#include "disseqt_ast_statements.h"
#include <string>

namespace disseqt
{

    class parser_error : public std::runtime_error
    {
    public:
        parser_error( const std::string &error)
        :std::runtime_error( error)
        {}
    };


    // may throw parser_error
    ast::sql_stmt_list parse( std::string::const_iterator &first, std::string::const_iterator last);
    ast::sql_stmt_list parse( const std::string &sql);
    ast::sql_stmt_list parse( std::istream &inputstream);
    bool is_valid( const std::string &sql);

}



#endif /* DISSEQT_PARSER_H_ */
