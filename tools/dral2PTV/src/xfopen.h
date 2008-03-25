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

#ifndef _XFOPEN_H_
#define _XFOPEN_H_

#ifndef lint
static char	_rcsid_xfopen_h_[] = "$Header$";
#endif

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

/*
 * xfopen()
 *
 *FUNCTION:
 *  xtended fopen.
 *  this version of fopen interprets the filename given it, and attempts to
 *  open the correct type of file.  For instance, if passed a filename that
 *  ends in .gz, it will attempt to open a pipe to gzip (either reading from
 *  gzip if the file type is read, or writing to gzip if the file type is
 *  write).  The package also interprets the filenames "stdin", "stdout", and
 *  "stdout" as meaning the files stdin, stdout, and stderr, respectively,
 *  and attempts connects the appropriate streams to them.  If the filename
 *  is '-', stdin or stdout will be returned, based on whether the mode
 *  is read or write.
 *
 *  If the filename starts with '|', it is assumed that the string is a 
 *  command, and will attempt to open a pipe to that command.  No error
 *  checking is done on the command in this case.
 *
 *  NOTE: obviously, since the file returned can be a pipe, don't use xfopen
 *  for files you might desire to do a seek on.  It would be a bad thing.
 *
 *  Function is plug-compatible with fopen().
 *
 *PARAMETERS:
 *   fname	-- (in) the file name to open
 *   mode	-- (in) the mode to open it with (see fopen(3))
 *
 *RETURNS:
 *   FILE * for the file opened
 */
extern
FILE *
xfopen(
        char	*fname,
        char	*mode);

/*
 * xfclose()
 *
 *FUNCTION:
 *  close a file opened with xfopen()
 *
 *PARAMETERS:
 *  ifp		-- (in) file to close
 *
 *RETURNS:
 *  returns the same values as fclose() or pclose().
 *  See stdlib(3)
 */

extern
int
xfclose(
        FILE	*ifp);

#ifdef __cplusplus
}
#endif

#endif /* _XFOPEN_H_ */

/*
 * $Log$
 * Revision 1.1  2006/02/15 22:42:50  pscabrex
 * CSN-dreams-325
 *
 * Revision 1.1  2004/05/03 00:38:29  bmorris
 * CSN-core-917
 *
 * Revision 1.3  2003/06/03 22:48:10  msblacks
 * aliased Osiris' reserved field for instruction id with ours so that we can sync
 * by inst_id in jptv
 *
 * Revision 1.2  2003/05/08 00:58:00  msblacks
 * got rid of hash_map and switched to map so that
 * we can finally get rid of the separate STL library that was causing
 * many many headaches, especially for me on windows
 *
 * Revision 1.1  2002/03/22 20:52:41  msblacks
 * I added XFOpen from pslib b/c using zlib was way too slow
 *
 * Revision 1.8  2001/06/22 20:44:33  mgdixon
 * Added ifdef __cpp extern "C" { statements to a number of the header files. This makes it possible to actually link a C++ program with PSLIB.
 * More cleaning still needs to be done.
 *
 * Revision 1.7  1997/04/08 18:35:16  kolding
 * Updated copyright messages to 1997.
 *
 * Revision 1.6  1996/09/11 18:56:29  kolding
 * Added new functionality to xfopen().  Will automatically attempt to read
 * compressed files if the uncompressed version doesn't exist.  Will
 * interpret filenames that start with '|' as a pipe and attempt to open
 * a pipe to/from the command.  Note, this differs from perl syntax since it
 * *MUST* be the first character.
 *
 * Revision 1.5  1996/08/20 01:19:22  kolding
 * Oops.  Screwed up the Header line.
 *
 * Revision 1.4  1996/08/20 01:07:23  kolding
 * Changed prototype of xfclose to match the definition of fclose/pclose
 * and return an integer.
 *
 * Revision 1.3  1996/07/19 21:55:26  kolding
 * changed from using the macro _LIT_UTIL_H_ to _XFOPEN_H_ to keep this
 * header file from being included more than once
 *
 * Revision 1.2  1996/03/13 01:52:00  kolding
 * Added #include of stdio.h
 *
 * Revision 1.1  1996/02/20 02:09:20  kolding
 * Initial revision
 *
 */



  



