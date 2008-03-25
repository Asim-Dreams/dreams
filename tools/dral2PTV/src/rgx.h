/*
 * Copyright (C) 2006 Intel Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */      

#ifndef __RGX_H__
#define __RGX_H__

#ifndef lint
static char *rcsid_rgx_h = "$Header$";
#endif

/************************************************************
 * 
 * NOTE: look at the bottom of this file for 
 *       example usage.
 * 
 * Here's the story: I (JimQ) stole Henry Spencer's regular
 * expression package.  This package is an implementation of
 * V8 regexp(3). This package is completely public domain 
 * and is used also in Perl (an enhanced version).  So if
 * you are familiar with the usage and power of 
 * regex matching, this is very similar.
 *
 * To encourage the use of this file, I have created a single
 * simple routine to call for pattern matching.  It 
 * performs the RE compile and RE match in one call.
 * The package actually breaks it into two calls.
 * If you want this original functionality, please
 * export the calls in this .h file.  I have resisted 
 * doing this since I do not want everybody scared
 * from using this RE package.  Proper documentation
 * for RE operation and matching should be in man page
 * or postscript form.
 *
 * The regex matching syntax is similar to egreps.  
 * Subexpression matches, deleineated by parens, 
 * are indicated by index arrays.
 *
 * re_simple_match(...), like regexec(), returns 1 for
 * success(match) and 0 for failure.  When it returns 1, 
 * the indices of the matching subexpressions are
 * given in rgx_index_(start|end)[].
 * This is a departure from Henry's code; he uses
 * char pointers to acheive the same effect.  If
 * an entry in these arrays has a -1, than the end of
 * the valid subexpression list has been reached. Of
 * course, there can be at most NSUBEXP valid entries.
 *
 * Note that with re_simple_match(...), you do not see
 * the malloced and freed regexp structure.  As such,
 * this is probably not a fast routine, but in most
 * cases that is not a problem.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif


typedef struct regexp		*rgx_regexp;

/*
 * Hard limit on the number of sub-expressions that
 * can be "grabbed".  I doubt if you need more than 10.
 */
#define NSUBEXP  10

/*
 * This is the do-it-all-in-one-call function.  For
 * more involved pattern matching, I suggest reading
 * the documentation and possibly rgx.c.
 */
extern
int 
rgx_simple_match(
	const char *regex,  /* the regular expression   */
	const char *str);   /* the string to be matched */

/*
 * This function returns the value from an
 * array that contains the starting indicies of
 * the matched subexpression.  The 0th element ALWAYS
 * gives the start of the string that matched the entire 
 * pattern.
 */
extern
int
rgx_index_start(int index);

/*
 * This function returns the value from an
 * array that contains the ending indicies of
 * the matched subexpression.  The 0th element ALWAYS
 * gives the end of the string that matched the entire 
 * pattern.
 */
extern
int
rgx_index_end(int index);

/*
 * rgx_compile
 * Compile a regular expression.
 */
extern 
rgx_regexp 
rgx_compile(
    const char		*pattern);

/*
 * rgx_match
 * Match against a precompiled regular expression.
 */
extern
int
rgx_match(
    rgx_regexp		rgx,
    const char		*string);

/*
 * rgx_free_regexp
 * Free a regular expression.
 */
extern
void
rgx_free_regexp(
    rgx_regexp		rgx);

 

/*****************************************************************************
 *****************************************************************************
 * 
 * EXAMPLE USAGE:
 * 
 * 
 *  Enter string:   ababddddefefef
 *  Enter pattern:  (ba.)(d+)([ef]+)
 *  Result:         1 (It's a match!)
 *
 * =================================
 * Array
 * Index   Beg   End   Substring
 * =================================
 *  0        1    14   babddddefefef
 *  1        1     4   bab
 *  2        4     8   dddd
 *  3        8    14   efefef
 * =================================
 * 
 * If you want to run your own examples, compile this with
 * REGEX_STANDALONE_TEST defined and link with str.[ch].
 * 
 * 
 *****************************************************************************
 *****************************************************************************/

#ifdef __cplusplus
} //extern "C"
#endif


#endif
/*
 * $Log$
 * Revision 1.1  2006/02/15 22:42:50  pscabrex
 * CSN-dreams-325
 *
 * Revision 1.1  2004/05/03 00:38:29  bmorris
 * CSN-core-917
 *
 * Revision 1.4  2003/06/03 22:48:10  msblacks
 * aliased Osiris' reserved field for instruction id with ours so that we can sync
 * by inst_id in jptv
 *
 * Revision 1.3  2003/05/08 00:57:58  msblacks
 * got rid of hash_map and switched to map so that
 * we can finally get rid of the separate STL library that was causing
 * many many headaches, especially for me on windows
 *
 * Revision 1.2  2002/02/19 18:30:21  msblacks
 * implementation of datastores and sorters complete, working, needs more validation
 * also, bug fixes for writers and readers
 *
 * Revision 1.5  1998/07/22 15:41:27  kolding
 * Exposed the internal compile/match interfaces in the regular expression
 * package.  Might be nice to go to a standard package someday, rather than these
 * hacky versions.
 * Fixed a potential bug in the strings package where data would get copied into
 * NULL strings if strings couldn't be allocated.
 *
 * Revision 1.4  1997/04/08 18:35:04  kolding
 * Updated copyright messages to 1997.
 *
 */





