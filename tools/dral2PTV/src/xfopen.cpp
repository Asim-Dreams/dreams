/*****************************************************************************
 *
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
 *****************************************************************************/

#ifndef lint
static char rcsid[] = "$Header$";
#endif

#include <stdio.h>
#include <string.h>

#ifdef WIN32
#include <io.h>
#endif

#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>

/*
 * HACK -- make sure we get Posix sources on RS/6000's.  What stupidity
 */
#define _POSIX_SOURCE

#include <sys/stat.h>
#ifndef WIN32
#  include <sys/param.h>
#else /* WIN32 */
#  include "fcntl.h"
#endif

#include "ps_common.h"

#include "rgx.h"
#include "xfopen.h"

#define REGEX_FNAME_COMPRESSED           "([.][zZ]|[.]gz)$"

#define GZIP_CMD     "gzip"
#define GZIP_ARGS    "-c"
#define GZIP_ENV_VAR	"PS_GZIP"

#define GUNZIP_CMD   "gzip"
#define GUNZIP_ARGS  "-cd"
#define GUNZIP_ENV_VAR	"PS_GUNZIP"


/****************************************************************************
 * xfopen()
 *   Open file $filename in fopen() mode $mode.
 *
 *   If specified $filename (logical file name) has a gzip(1) suffix, 
 *   then the file may be read or written in a gzip-compatible format.
 *   A pointer to the opened FILE is returned. NULL is returned
 *   if xfopen() is unable to open the file.
 *
 *   Special file names:
 *         mode = "r" and filename = "-" ==> stdin
 *         mode = "w" and filename = "-" ==> stdout
 *
 *         filename = "stdin"            ==> stdin
 *         filename = "stdout"           ==> stdout
 *
 *         filename = "stderr"           ==> stderr
 *
 *   Use xfclose() to close files/pipes opened using xfopen().
 *
 *
 *   BUGS/TODO:
 *   -Later we should grok the file header to determine if it is in the
 *    gzip format, etc.
 *
 *   -Gzip files and remote-host files cannot be rewound. Close, then
 *    reopen the files to "rewind" the file. Ugh.
 ****************************************************************************/

/* This works around an assert() macro-expansion bug on the RS6000. */
#ifndef NDEBUG
#   define IF_ASSERT(X) { if ( X ) { assert( PS_False ); } }
#else
#   define IF_ASSERT(X) /* empty */
#endif

FILE *
xfopen(
    char	*logical_filename,
    char	*orig_mode)
{
    char	command[MAXPATHLEN * 3];
    char	*physical_filename;
    FILE	*fp = NULL;
    char        mode[3];
    char *s;

    /*
     * Make sure things make some sense.
     * Return NULL if they don't
     */
    if (!(logical_filename && orig_mode)) {
	return (NULL);
    }

    strncpy(mode, orig_mode, 3);
    mode[2] = '\0';

    physical_filename = logical_filename;

    /* 
     * Special logical_filename/mode treatment for the standard input/output/error 
     */

    if ( ! strcmp(logical_filename, "stdin" ) ) {
	if (mode[0] != 'r') {
	    return(NULL);
	}
#ifdef WIN32
	_setmode(_fileno(stdin), _O_BINARY);
#endif
	return (stdin);
    } else if (!strcmp( logical_filename, "stdout" ) ) {
	if (mode[0] != 'w') {
	    return(NULL);
	}
#ifdef WIN32
	_setmode(_fileno(stdout), _O_BINARY);
#endif
	return (stdout);
    } else if (!strcmp( logical_filename, "stderr" ) ) {
	if (mode[0] != 'w') {
	    return(NULL);
	}
#ifdef WIN32
	_setmode(_fileno(stderr), _O_BINARY);
#endif
	return (stderr);
    } else if ( ! strcmp( logical_filename, "-" ) ) {
	if (mode[0] == 'r') {
#ifdef WIN32
	_setmode(_fileno(stdin), _O_BINARY);
#endif
		return (stdin);
	}
	if (mode[0] == 'w') {
#ifdef WIN32
	_setmode(_fileno(stdout), _O_BINARY);
#endif
		return (stdout);
	}
    }


    if (logical_filename[0] == '|') {
	/*
	 * first character is '|', open a pipe
	 * do NO error checking.  User's problem.
	 */

#ifndef WIN32
	/*
	 * Under Unix we can only deal with mode == "r" or "w"
	 * Except under Win32, where _popen can deal with rb and wb.
	 */
	mode[1] = '\0';
#endif

	logical_filename = &physical_filename[1];
	fp = (FILE *) popen(logical_filename, mode);
#ifdef __FreeBSD__
	if (fp != NULL) {
		fcntl(fileno(fp), F_SETFD, 1);
	}
#endif
    } else if (rgx_simple_match(REGEX_FNAME_COMPRESSED, logical_filename)) {
	/*
	 * Must open a compressed trace file via a pipe...
	 */

#ifndef WIN32
	/*
	 * Under Unix we can only deal with mode == "r" or "w"
	 * Except under Win32, where _popen can deal with rb and wb.
	 */
	mode[1] = '\0';
#else
	/* 
	 * Under Win32 all opens must include the binary mode.
	 */
	mode[1] = 'b';
	mode[2] = '\0';
#endif

	switch (mode[0]) {
	case 'r':
	    s = getenv(GUNZIP_ENV_VAR);
	    (void) sprintf(command, "%s %s %s",
			   (s ? s : GUNZIP_CMD),
			   GUNZIP_ARGS,
			   physical_filename);
	    break;

	case 'w':
	    s = getenv(GZIP_ENV_VAR);
	    (void) sprintf(command, "%s %s > %s",
			   (s ? s : GZIP_CMD),
			   GZIP_ARGS,
			   physical_filename);
	    break;

	default: 
	    /* Illegal or unsupported mode on a gzip\'ed file. */
	    break;
	}
	assert(strlen(command) < sizeof(command));

	/*
	 * test open the file, make sure we have appropriate access rights
	 * if we can't open it with fopen, I assume the pipe won't be able to open it
	 * either.
	 */
	fp = fopen(physical_filename, mode);
	if (fp != NULL) {
		fclose (fp);

		fp = (FILE *) popen(command, mode);
	}
#ifdef __FreeBSD__
	if (fp != NULL) {
		fcntl(fileno(fp), F_SETFD, 1);
	}
#endif
    } else {
	fp = fopen(physical_filename, mode);
	if (fp == NULL && mode[0] == 'r') {
		/*
		 * file doesn't exist.  Let's try appending a .gz to the filename
		 * and to read it again.
		 */
		strcpy(command, physical_filename);
		strcat(command, ".gz");
		assert(strlen(command) < sizeof(command));
		fp = xfopen(command, mode);
	}
    }

    return (fp);
}


/****************************************************************************/
/* xfclose()                                                                */
/*   Figures out if file pointer is to an open pipe or file, then           */
/*   closes it appropriately. Used to close files opened w/ xfopen().       */
/*   Shamefully eats the return code of fclose() or pclose().               */
/****************************************************************************/

#ifdef WIN32
/* the following definition is in sys/mode.h in the Unix world. */
#define S_ISFIFO(m)     (((m)&(_S_IFMT)) == (_S_IFIFO))
#endif

int
xfclose( 
    FILE	*ifp)
{
    int		fd;
    int		retval;
    struct stat	buf;

    if (!ifp) {
	return (EOF);
    }

    /* Get file descriptor for this stream. */
    fd = fileno( ifp );

    if ( fstat(fd, &buf) ) {
        /* fstat failure! - TMP: should issue warning */
	return (EOF);
    }

    if ( S_ISFIFO( buf.st_mode ) ) {
	/* Is a pipe/FIFO */
	retval = pclose( ifp );
    } else {
	/*
	 * A file closable using fclose(); not necessarily a "regular" 
	 * file (e.g. stdout, which is considered a character special
	 * under SYS VR4).
	 */
	retval = fclose( ifp );
    }
    return (retval);
}

/*
 * $Log$
 * Revision 1.1  2006/02/15 22:42:50  pscabrex
 * CSN-dreams-325
 *
 * Revision 1.1  2004/05/03 00:38:29  bmorris
 * CSN-core-917
 *
 * Revision 1.3  2003/05/08 00:57:59  msblacks
 * got rid of hash_map and switched to map so that
 * we can finally get rid of the separate STL library that was causing
 * many many headaches, especially for me on windows
 *
 * Revision 1.2  2002/03/22 20:55:44  msblacks
 * I added XFOpen from pslib b/c using zlib was way too slow
 *
 * Revision 1.1  2002/03/22 20:52:41  msblacks
 * I added XFOpen from pslib b/c using zlib was way too slow
 *
 * Revision 1.12  2000/01/07 23:28:05  kolding
 * Removed an extra printing of GZIP_CMD into popen calls to gzip.
 * How this got in, I'm not sure.  How it also stayed undetected this
 * long is way beyond me.
 *
 * Revision 1.11  1999/10/12 22:42:58  kolding
 * Added ability to specify which versions of gzip/gunzip to use with
 * the environment variables PS_GZIP and PS_GUNZIP respectively.
 *
 * Revision 1.10  1999/03/16 01:13:17  wscott
 * Don't do fcntl on filehandle if popen failed and returned NULL.
 *
 * Revision 1.9  1999/03/12 18:39:10  wscott
 * Fix another case where the broken FreeBSD 2.2 popen is causing problems.
 *
 * Revision 1.8  1999/02/25 20:06:36  wscott
 * Add two assert to catch buffer overflow
 *
 * Revision 1.7  1999/01/27 22:25:38  wscott
 * On FreeBSD set the close-on-exec flag on all file handles returned by
 * xfopen().  This is a workaround for an OS bug that was fixed in FreeBSD
 * version 3.0.
 *
 * Revision 1.6  1997/10/01 21:30:09  kolding
 * Changed xfclose() to return EOF when given an invalid file rather than
 * aborting.  Brings it into better compliance with how fclose() and
 * pclose() work.
 *
 * Revision 1.5  1997/09/25 23:31:07  kolding
 * Added support to open stdin/stdout/stderr in binary mode when used in
 * xfopen.
 *
 * Revision 1.4  1997/07/30 19:22:41  kolding
 * Fixed some "bugs" in xfopen().
 * Now you can try to open any file/pipe in binary mode.  For stdin,
 * stdout, stderr, binary mode is ignored (would fail an assert before).
 * For pipes, you can open them in binary mode.  Will ignore the binary
 * portion on Unix hosts, keep binary mode on NT (all pipes are Binary
 * on Unix).
 *
 * Now errors just return NULL instead of fatalling via an assert.  The asserts
 * were unclear, and caused the program to die in ways which wouldn't be clear
 * to a user.  This change brings the behavior in line with stdio, which just
 * returns NULL when invalid modes are passed in.
 *
 * Revision 1.3  1997/07/11 15:02:15  wscott
 * It is not always ok to modify the 'mode' argument in place so not we
 * make a copy.
 *
 * Revision 1.2  1997/07/10 23:52:28  wscott
 * Changes to support Win32.
 *
 * Under unix we always use 'r' or 'w'.
 * Under Win32 we always force 'rb' or 'wb'.
 *
 * Also use 'gunzip file |' instead of 'gunzip < file |' because
 * Win32 Netbatch can't handle it.  (I think they are hopelessly broken
 * but what do I know.)
 *
 * Revision 1.1  1997/06/24 19:05:09  wscott
 * Initial revision
 *
 * Revision 1.9  1997/04/17 19:09:25  wscott
 * Remove editorial comment about IBM that was in poor taste.
 *
 * Revision 1.8  1997/04/08 18:35:12  kolding
 * Updated copyright messages to 1997.
 *
 * Revision 1.7  1996/11/28 00:32:11  kolding
 * Added support for building under Win32 environment.
 *
 * Revision 1.6  1996/09/11 18:56:26  kolding
 * Added new functionality to xfopen().  Will automatically attempt to read
 * compressed files if the uncompressed version doesn't exist.  Will
 * interpret filenames that start with '|' as a pipe and attempt to open
 * a pipe to/from the command.  Note, this differs from perl syntax since it
 * *MUST* be the first character.
 *
 * Revision 1.5  1996/08/20 01:07:21  kolding
 * Changed prototype of xfclose to match the definition of fclose/pclose
 * and return an integer.
 *
 * Revision 1.4  1996/08/14 18:13:45  kolding
 * Removed --best option to gzip on compressing files
 *
 * Revision 1.3  1996/07/19 21:55:26  kolding
 * added check to see if file could be opened before calling the pipe.
 * Allows better error checking.
 *
 * Revision 1.2  1996/02/20 13:11:41  berko
 * Remove mode.h.
 *
 * Revision 1.1  1996/02/20 02:09:20  kolding
 * Initial revision
 *
 */

