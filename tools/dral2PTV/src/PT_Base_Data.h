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

#ifndef _PT_BASE_DATA_
#define _PT_BASE_DATA_

// Supported data types for PT_Statement_DataElement and PT_DataElement.
///////////////////////////////////////////////////////////////////////////////

typedef enum {
	        Pipe_Invalid_Data = 0,
                Pipe_Boolean      = 1,
                Pipe_Integer      = 2,
                Pipe_Hex_Integer  = 3,
                Pipe_String       = 4,
                Pipe_Association  = 5,
                Pipe_Enumeration  = 6,
                Pipe_ByteArray    = 7,
                Pipe_Hex_Integer64 = 8,
                Pipe_Max_Data_Types
} Base_Data_Type;

#endif


  




