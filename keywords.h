/*
 * keywords.h
 *
 *  Created on: Apr 7, 2015
 *      Author: danny
 */

#ifndef DISSEQT_KEYWORDS_H_
#define DISSEQT_KEYWORDS_H_

// keywords that can also appear as identifiers.
#define DISSEQT_NONSPECIFIC_KEYWORDS \
        (ABORT)\
        (REPLACE)\
        (IGNORE)\
        (MATCH)\
        (LEFT)\
        (NO)\
        /**/

// All keywords understood by disseqts lexer, except for NULL, which needs some
// special treatment because it's a C macro as well.
// There are more terminals than just these keywords. These are handled in the lexer.
#define DISSEQT_KEYWORDS \
        (ISNULL)\
        (NOTNULL)\
        (INSERT)\
        (REGEXP)\
        (FOREIGN)\
        (USING)\
        (NATURAL)\
        (OUTER)\
        (INNER)\
        (CROSS)\
        (INDEXED)\
        (VALUES)\
        (GROUP)\
        (HAVING)\
        (SELECT)\
        (WHERE)\
        (EXPLAIN)\
        (QUERY)\
        (ORDER)\
        (LIMIT)\
        (DISTINCT)\
        (OFFSET)\
        (RECURSIVE)\
        (INTERSECT)\
        (EXCEPT)\
        (UNION)\
        (COLLATE)\
        (ROLLBACK)\
        (UPDATE)\
        (CREATE)\
        (TABLE)\
        (TEMPORARY)\
        (EXISTS)\
        (WITHOUT)\
        (CONSTRAINT)\
        (PRIMARY)\
        (AUTOINCREMENT)\
        (UNIQUE)\
        (CHECK)\
        (DEFAULT)\
        (CURRENT_TIME)\
        (CURRENT_DATE)\
        (CURRENT_TIMESTAMP)\
        (CONFLICT)\
        (CASCADE)\
        (RESTRICT)\
        (ACTION)\
        (DEFERRABLE)\
        (INITIALLY)\
        (DEFERRED)\
        (IMMEDIATE)\
        (DELETE)\
        (REFERENCES)\
        (BETWEEN)\
        (JOIN)\
        (FROM)\
        (PLAN)\
        (WITH)\
        (DESC)\
        (FAIL)\
        (TEMP)\
        (CAST)\
        (CASE)\
        (WHEN)\
        (THEN)\
        (ELSE)\
        (INTO)\
        (LIKE)\
        (GLOB)\
        (NOT)\
        (END)\
        (KEY)\
        (AND)\
        (SET)\
        (ALL)\
        (ASC)\
        (ON)\
        (AS)\
        (BY)\
        (OR)\
        (IS)\
        (IN)\
        (IF)\
        DISSEQT_NONSPECIFIC_KEYWORDS\
        /**/


#endif /* DISSEQT_KEYWORDS_H_ */
