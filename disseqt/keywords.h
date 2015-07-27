/*
 * keywords.h
 *
 *  Created on: Apr 7, 2015
 *      Author: danny
 */

#ifndef DISSEQT_KEYWORDS_H_
#define DISSEQT_KEYWORDS_H_

/// keywords that can also appear as identifiers.
#define DISSEQT_NONSPECIFIC_KEYWORDS \
        (REGEXP)\
        (OUTER)\
        (INNER)\
        (CROSS)\
        (INDEXED)\
        (NATURAL)\
        (EXPLAIN)\
        (QUERY)\
        (OFFSET)\
        (RECURSIVE)\
        (ROLLBACK)\
        (TEMPORARY)\
        (WITHOUT)\
        (CONFLICT)\
        (CASCADE)\
        (RESTRICT)\
        (ACTION)\
        (INITIALLY)\
        (DEFERRED)\
        (IMMEDIATE)\
        (PLAN)\
        (WITH)\
        (DESC)\
        (FAIL)\
        (TEMP)\
        (LIKE)\
        (GLOB)\
        (END)\
        (KEY)\
        (ASC)\
        (ABORT)\
        (REPLACE)\
        (IGNORE)\
        (MATCH)\
        (LEFT)\
        (BY)\
        (IF)\
        (NO)\
        /**/

/// keywords that can not appear as identifiers.
#define DISSEQT_SPECIFIC_KEYWORDS \
        (ISNULL)\
        (NOTNULL)\
        (INSERT)\
        (FOREIGN)\
        (USING)\
        (VALUES)\
        (GROUP)\
        (HAVING)\
        (SELECT)\
        (WHERE)\
        (ORDER)\
        (LIMIT)\
        (DISTINCT)\
        (INTERSECT)\
        (EXCEPT)\
        (UNION)\
        (COLLATE)\
        (UPDATE)\
        (CREATE)\
        (TABLE)\
        (EXISTS)\
        (CONSTRAINT)\
        (PRIMARY)\
        (AUTOINCREMENT)\
        (UNIQUE)\
        (CHECK)\
        (DEFAULT)\
        (CURRENT_TIME)\
        (CURRENT_DATE)\
        (CURRENT_TIMESTAMP)\
        (DEFERRABLE)\
        (DELETE)\
        (REFERENCES)\
        (BETWEEN)\
        (JOIN)\
        (FROM)\
        (CAST)\
        (CASE)\
        (WHEN)\
        (THEN)\
        (ELSE)\
        (INTO)\
        (NOT)\
        (AND)\
        (SET)\
        (ALL)\
        (ON)\
        (AS)\
        (OR)\
        (IS)\
        (IN)\
        /**/


// All keywords understood by disseqts lexer, except for NULL, which needs some
// special treatment because it's a C macro as well.
// There are more terminals than just these keywords. These are handled in the lexer.
#define DISSEQT_KEYWORDS \
        DISSEQT_SPECIFIC_KEYWORDS\
        DISSEQT_NONSPECIFIC_KEYWORDS\
        /**/


#endif /* DISSEQT_KEYWORDS_H_ */
