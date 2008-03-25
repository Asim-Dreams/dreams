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

#ifndef _PS_COMMON_H_
#define _PS_COMMON_H_

#ifndef lint
static char *rcsid_ps_common_h = "$Header$";
#endif


/*
 * OVERVIEW
 *
 * This module provides a repository for definitions and types that are
 * considered to be generally useful and common across a wide range of
 * software simulators.
 */



/*
 * INCLUDES
 */

#include <limits.h>

/*
 * WINNT cleanup
 */

#ifdef WIN32
#define	popen	_popen
#define	pclose	_pclose

#define MAXPATHLEN	(256)

#endif		/* WIN32 */


#ifdef CHECKPOINT
#include "chkpt.h"
#endif

/*
 * DEFINES
 */

#define PS_MAX_TIME     ULONG_MAX
#define PS_MAX_COUNT    ULONG_MAX


/*
 * TYPES
 */

typedef enum {
        PS_False = 0,
        PS_True  = 1
} ps_boolean_t;

typedef unsigned long   ps_time_t;
typedef unsigned long   ps_count_t;

typedef unsigned long   ps_address_t;
typedef unsigned long   ps_lin_addr_t;
typedef unsigned long   ps_phy_addr_t;



/*
 * PUBLIC INTERFACE
 */



/*
 * PRIVATE INTERFACE
 */


#endif /* _PS_COMMON_H_ */



/*
 * $Log$
 * Revision 1.1  2006/02/15 22:42:50  pscabrex
 * CSN-dreams-325
 *
 * Revision 1.1  2004/05/03 00:38:29  bmorris
 * CSN-core-917
 *
 * Revision 1.2  2003/06/03 22:48:10  msblacks
 * aliased Osiris' reserved field for instruction id with ours so that we can sync
 * by inst_id in jptv
 *
 * Revision 1.1  2002/03/22 20:52:41  msblacks
 * I added XFOpen from pslib b/c using zlib was way too slow
 *
 * Revision 1.13  1997/04/10 17:39:43  kolding
 * Added checkpointing support to this header file.
 *
 * Revision 1.12  1997/04/08 18:34:45  kolding
 * Updated copyright messages to 1997.
 *
 * Revision 1.11  1996/11/28 00:32:08  kolding
 * Added support for building under Win32 environment.
 *
 * Revision 1.10  1995/09/14 17:43:24  rmw
 * Added "#define" for 'PS_MAX_COUNT'.
 *
 * Revision 1.9  1995/09/01  17:43:35  rmw
 * Added 'PS_MAX_TIME' and an "#include" of 'limits.h' for 'ULONG_MAX'.
 *
 * Revision 1.8  1995/08/23  21:45:32  rmw
 * The 'rcs_common_h' becomes 'rcs_ps_common_h'.
 *
 * Revision 1.7  1995/08/23  18:00:47  rmw
 * Moved file to 'ps_common.h' (was 'common.h') and added 'ps_' and 'PS_' prefixes
 * to all elements to avoid problems with conflicts with other header files.
 * (In particular 'boolean_t' with '<sys>/types.h' on some systems.)
 * Removed Tab characters.
 *
 * Revision 1.6  1995/08/23  06:19:51  srodov
 * system include file sys/types.h is included for HP-UX too
 *
 * Revision 1.5  1995/08/14  12:10:12  srodov
 * SVR4 was changed to svr4
 *
 * Revision 1.4  1995/08/13  10:28:47  srodov
 * system dependent definition of 'boolean_t' on SVR4
 *
 * Revision 1.3  1995/08/02  16:00:44  srodov
 * definition of 'time_t' was removed
 *
 * Revision 1.2  1995/08/02  15:47:35  srodov
 * error.h include file was removed
 *
 * Revision 1.1  1995/07/13  15:37:15  srodov
 * Initial revision
 *
 * Revision 1.1  1995/07/13  15:36:43  srodov
 * Initial revision
 *
 */





