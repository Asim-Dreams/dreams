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
// version: 0.2
// started at 2002-02-26 by Federico Ardanaz
//
// Normal Node component
// -------------------------------------------------------------------

#include "vcgNode.h"

// -----------------------------------------------------------
// vcgComponent interface methods
// -----------------------------------------------------------
void vcgNode::toVCG(FILE *f,int tabLevel)
{
	this->myTabLevel = tabLevel;
	putTabs(f);fprintf(f,"node: { ");
	this->dumpNodeProperties(f);
	//  close node
	fprintf(f," }\n");
	
}

void vcgNode::dumpNodeProperties(FILE *f)
{dumpNodeProperties(f,"","");}

void vcgNode::dumpNodeProperties(FILE *f, char* prefix, char* sufix)
{	
	if (title_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"title: \"%s\" ",title_prop);fprintf(f,"%s",sufix);}
	if (label_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"label: \"%s\" ",label_prop);fprintf(f,"%s",sufix);}
	if (info1_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"info1: \"%s\" ",info1_prop);fprintf(f,"%s",sufix);}
	if (info2_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"info2: \"%s\" ",info2_prop);fprintf(f,"%s",sufix);}
	if (info3_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"info3: \"%s\" ",info3_prop);fprintf(f,"%s",sufix);}
	if (color_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"color: %s ",vcgColorAttr::toStr(color_prop));fprintf(f,"%s",sufix);}
	if (textcolor_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"textcolor: %s ",vcgColorAttr::toStr(textcolor_prop));fprintf(f,"%s",sufix);}
	if (bordercolor_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"bordercolor: %s ",vcgColorAttr::toStr(bordercolor_prop));fprintf(f,"%s",sufix);}
	if (width_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"width: %d ",width_prop);fprintf(f,"%s",sufix);}
	if (height_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"height: %d ",height_prop);fprintf(f,"%s",sufix);}
	if (borderwidth_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"borderwidth: %d ",borderwidth_prop);fprintf(f,"%s",sufix);}
	if (loc_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"loc: {x: %d y: %d} ",loc_prop[0],loc_prop[1]);}fprintf(f,"%s",sufix);;
	if (folding_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"folding: %d ",folding_prop);fprintf(f,"%s",sufix);}
	if (scaling_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"scaling: %f ",scaling_prop);fprintf(f,"%s",sufix);}
	if (textmode_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"textmode: %s ",vcgTextModeAttr::toStr(textmode_prop));fprintf(f,"%s",sufix);}
	if (shape_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"shape: %s ",vcgShapeAttr::toStr(shape_prop));fprintf(f,"%s",sufix);}
	if (level_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"level: %d ",level_prop);fprintf(f,"%s",sufix);}
	if (vertical_order_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"vertical_order: %d ",vertical_order_prop);fprintf(f,"%s",sufix);}
	if (horizontal_order_prop_set) { fprintf(f,"%s",prefix);fprintf(f,"horizontal_order: %d ",horizontal_order_prop);fprintf(f,"%s",sufix);}
}

// -----------------------------------------------------------
// prop. init
// -----------------------------------------------------------
void vcgNode::initProperties(void)
{
	title_prop_set			=	false;
	label_prop_set			=	false;
	info1_prop_set			=	false;
	info2_prop_set			=	false;
	info3_prop_set			=	false;
	color_prop_set			=	false;
	textcolor_prop_set		=	false;
	bordercolor_prop_set	=	false;
	width_prop_set			=	false;
	height_prop_set			=	false;
	borderwidth_prop_set	=	false;
	loc_prop_set			=	false;
	folding_prop_set		=	false;
	scaling_prop_set		=	false;
	textmode_prop_set		=	false;
	shape_prop_set			=	false;
	level_prop_set			=	false;
	vertical_order_prop_set	=	false;
	horizontal_order_prop_set=	false;
	
	// ensure pointers are null
	title_prop=NULL;
	label_prop=NULL;
	info1_prop=NULL;
	info2_prop=NULL;
	info3_prop=NULL;
}
