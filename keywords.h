/*
 * keywords.h
 *
 *  Created on: Apr 7, 2015
 *      Author: danny
 */

#ifndef DISSEQT_KEYWORDS_H_
#define DISSEQT_KEYWORDS_H_


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
        (ABORT)\
        (REPLACE)\
        (IGNORE)\
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
        (MATCH)\
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
        (LEFT)\
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
        (NO)\
        (IF)\
        /**/


#endif /* DISSEQT_KEYWORDS_H_ */
