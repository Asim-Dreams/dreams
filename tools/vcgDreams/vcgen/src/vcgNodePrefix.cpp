/*
 * Copyright (C) 2004-2006 Intel Corporation
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

// -------------------------------------------------------------------
// vcgen framework component
// version: 0.1
// started at 2002-02-28 by Federico Ardanaz
//
// Edge Prefix omponent
// -------------------------------------------------------------------

#include "vcgNodePrefix.h"

// -----------------------------------------------------------
// vcgComponent interface methods
// -----------------------------------------------------------
void vcgNodePrefix::toVCG(FILE *f,int tabLevel)
{
	this->myTabLevel = tabLevel;

	char* realPrefix = (char*) malloc(strlen(myPrefix)+tabLevel+1);
	for (int i=0;i<tabLevel;i++) realPrefix[i] = '\t';
	realPrefix[tabLevel] = '\0';
	strcat(realPrefix,myPrefix);
	
	this->dumpNodeProperties(f,realPrefix,"\n");
	
	free(realPrefix);
}
