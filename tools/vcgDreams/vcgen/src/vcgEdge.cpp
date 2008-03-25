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
// started at 2002-02-26 by Federico Ardanaz
//
// Normal Edge component
// -------------------------------------------------------------------

#include "vcgEdge.h"

// -----------------------------------------------------------
// vcgComponent interface methods
// -----------------------------------------------------------
void vcgEdge::toVCG(FILE *f,int tabLevel)
{
	this->myTabLevel = tabLevel;
	putTabs(f);
    if (!nearEdge)
        fprintf(f,"edge:\n");
    else
        fprintf(f,"nearedge:\n");
        
	putTabs(f);fprintf(f,"{\n");
	this->dumpEdgeProperties(f);
	putTabs(f);fprintf(f,"}\n\n");
}

void vcgEdge::dumpEdgeProperties(FILE *f)
{dumpEdgeProperties(f,"");}

void vcgEdge::dumpEdgeProperties(FILE *f, char* prefix)
{	
	if (sourcename_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"sourcename: \"%s\"\n",sourcename_prop);}
	if (targetname_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"targetname: \"%s\"\n",targetname_prop);}
	if (label_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"label: \"%s\"\n",label_prop);}
	if (textcolor_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"textcolor: %s\n",vcgColorAttr::toStr(textcolor_prop));}
	if (color_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"color: %s\n",vcgColorAttr::toStr(color_prop));}
	if (thickness_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"thickness: %d\n",thickness_prop);}
	if (class_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"class: %d\n",class_prop);}
	if (priority_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"priority: %d\n",priority_prop);}
	if (arrowcolor_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"arrowcolor: %s\n",vcgColorAttr::toStr(arrowcolor_prop));}
	if (backarrowcolor_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"backarrowcolor: %s\n",vcgColorAttr::toStr(backarrowcolor_prop));}
	if (arrowsize_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"arrowsize: %d\n",arrowsize_prop);}
	if (backarrowsize_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"backarrowsize: %d\n",backarrowsize_prop);}
	if (arrowstyle_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"arrowstyle: %s\n",vcgArrowStyleAttr::toStr(arrowstyle_prop));}
	if (backarrowstyle_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"backarrowstyle: %s\n",vcgArrowStyleAttr::toStr(backarrowstyle_prop));}
	if (linestyle_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"linestyle: %s\n",vcgLineStyleAttr::toStr(linestyle_prop));}
	if (anchor_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"anchor: %d\n",anchor_prop);}
	if (horizontalorder_prop_set) {putTabs(f,1);fprintf(f,"%s",prefix);fprintf(f,"horizontalorder: %d\n",horizontalorder_prop);}
}


// -----------------------------------------------------------
// prop. init
// -----------------------------------------------------------
void vcgEdge::initProperties(void)
{
	sourcename_prop_set		=	false;
	targetname_prop_set		=	false;
	label_prop_set			=	false;
	textcolor_prop_set		=	false;
	color_prop_set			=	false;
	thickness_prop_set		=	false;
	class_prop_set			=	false;
	priority_prop_set		=	false;
	arrowcolor_prop_set		=	false;
	backarrowcolor_prop_set	=	false;
	arrowsize_prop_set		=	false;
	backarrowsize_prop_set	=	false;
	arrowstyle_prop_set		=	false;
	backarrowstyle_prop_set	=	false;
	linestyle_prop_set		=	false;
	anchor_prop_set			=	false;
	horizontalorder_prop_set=	false;
	
	// ensure pointers are NULL
	sourcename_prop=NULL;
	targetname_prop=NULL;
	label_prop=NULL;

    nearEdge=false;
}
