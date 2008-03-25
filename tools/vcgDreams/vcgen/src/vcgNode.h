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


#ifndef __vcgNode
#define __vcgNode

// ---------------------------
// -- some attribute types:
// ---------------------------
#include "vcgColorAttr.h"
#include "vcgShapeAttr.h"
#include "vcgTextModeAttr.h"
// ---------------------------

#include "vcgComponent.h"

/**
  * Normal Node component.
  * vcgen framework component
  * @version 0.3
  * @date started at 2002-02-26 
  * @author Federico Ardanaz
  */
class vcgNode : public vcgComponent
{
	public:
		/**
		  * Default Constructor 
		  */
		vcgNode(void)
		{ 
			this->myLayer = NODE_LAYER;
			initProperties();	
		}

		/**
		  * Named Constructor 
		  */
		vcgNode(char* title)
		{ 
			this->myLayer = NODE_LAYER;
			initProperties();
			this->setTitle(title);	
		}

		/**
		  * Destructor
		  */
		virtual ~vcgNode(void)
		{
			if (title_prop!=NULL) delete title_prop;
			if (label_prop!=NULL) delete label_prop;
			if (info1_prop!=NULL) delete info1_prop;
			if (info2_prop!=NULL) delete info2_prop;
			if (info3_prop!=NULL) delete info3_prop;
		}

		/**
		  *vcgComponent interface method
		  */
		void toVCG(FILE *f, int tabLevel);

		// -----------------------------------------------------------
		// public methods to set node properties
		// -----------------------------------------------------------
		
		inline void setTitle(char* value)
		{if(title_prop) free(title_prop);title_prop_set=true;title_prop=strdup(value);}

		inline void setLabel(char* value)
		{if(label_prop) free(label_prop);label_prop_set=true;label_prop=strdup(value);}

		inline void setInfo1(char* value)
		{if(info1_prop) free(info1_prop);info1_prop_set=true;info1_prop=strdup(value);}

		inline void setInfo2(char* value)
		{if(info2_prop) free(info2_prop);info2_prop_set=true;info2_prop=strdup(value);}

		inline void setInfo3(char* value)
		{if(info3_prop) free(info3_prop);info3_prop_set=true;info3_prop=strdup(value);}

		inline void setColor(vcgColor value)
		{color_prop_set=true;color_prop=value;}

		inline void setColor(int value)
		{setColor((vcgColor)value);}
		
		inline void setTextColor(vcgColor value)
		{textcolor_prop_set=true;textcolor_prop=value;}

		inline void setTextColor(int value)
		{setTextColor((vcgColor)value);}

		inline void setBorderColor(vcgColor value)
		{bordercolor_prop_set=true;bordercolor_prop=value;}

		inline void setBorderColor(int value)
		{setBorderColor((vcgColor)value);}

		inline void setWidth(int value)
		{width_prop_set=true;width_prop=value;}

		inline void setHeight(int value)
		{height_prop_set=true;height_prop=value;}

		inline void setBorderWidth(int value)
		{borderwidth_prop_set=true;borderwidth_prop=value;}

		inline void setLoc(int x, int y)
		{loc_prop_set=true;loc_prop[0]=x;loc_prop[1]=y;}

		inline void setFolding(int value)
		{folding_prop_set=true;folding_prop=value;}

		inline void setScaling(float value)
		{scaling_prop_set=true;scaling_prop=value;}

		inline void setTextMode(vcgTextMode value)
		{textmode_prop_set=true;textmode_prop=value;}

		inline void setShape(vcgShape value)
		{shape_prop_set=true;shape_prop=value;}

		inline void setLevel(int value)
		{level_prop_set=true;level_prop=value;}

		inline void setVerticalOrder(int value)
		{vertical_order_prop_set=true;vertical_order_prop=value;}

		inline void setHorizontalOrder(int value)
		{horizontal_order_prop_set=true;horizontal_order_prop=value;}


		// -----------------------------------------------------------
		// public methods to get node properties
		// -----------------------------------------------------------
		inline char* getTitle(void)
		{return title_prop; }
		
		inline char* getLabel()
		{return label_prop;}

		inline char* getInfo1()
		{return info1_prop;}

		inline char* getInfo2()
		{return info2_prop;}

		inline char* getInfo3()
		{return info3_prop;}

		inline vcgColor getColor()
		{return color_prop;}

		inline vcgColor getTextColor()
		{return textcolor_prop;}

		inline vcgColor getBorderColor()
		{return bordercolor_prop;}

		inline int getWidth()
		{return width_prop;}

		inline int getHeight()
		{return height_prop;}

		inline int getBorderWidth()
		{return borderwidth_prop;}

		inline int getLocX()
		{return loc_prop[0];}

		inline int getLocY()
		{return loc_prop[1];}

		inline int getFolding()
		{return folding_prop;}

		inline float getScaling()
		{return scaling_prop;}

		inline vcgTextMode getTextMode()
		{return textmode_prop;}

		inline vcgShape getShape()
		{return shape_prop;}

		inline int getLevel()
		{return level_prop;}

		inline int getVerticalOrder()
		{return vertical_order_prop;}

		inline int getHorizontalOrder()
		{return horizontal_order_prop;}


		// -----------------------------------------------------------
		// public methods to know whether a property is set or not
		// -----------------------------------------------------------
		inline bool isTitleSet()
		{return title_prop_set;}

		inline bool isLabelSet()
		{return label_prop_set;}

		inline bool isInfo1Set()
		{return info1_prop_set;}

		inline bool isInfo2Set()
		{return info2_prop_set;}

		inline bool isInfo3Set()
		{return info3_prop_set;}

		inline bool isColorSet()
		{return color_prop_set;}

		inline bool isTextColorSet()
		{return textcolor_prop_set;}

		inline bool isBorderColorSet()
		{return bordercolor_prop_set;}

		inline bool isWidthSet()
		{return width_prop_set;}

		inline bool isHeightSet()
		{return height_prop_set;}

		inline bool isBorderWidthSet()
		{return borderwidth_prop_set;}

		inline bool isLocSet()
		{return loc_prop_set;}

		inline bool isFoldingSet()
		{return folding_prop_set;}

		inline bool isScalingSet()
		{return scaling_prop_set;}

		inline bool isTextModeSet()
		{return textmode_prop_set;}

		inline bool isShapeSet()
		{return shape_prop_set;}

		inline bool isLevelSet()
		{return level_prop_set;}

		inline bool isVerticalOrderSet()
		{return vertical_order_prop_set;}

		inline bool isHorizontalOrderSet()
		{return horizontal_order_prop_set;}




	protected:
		// -----------------------------------------------------------
		// prop. initialization
		// -----------------------------------------------------------
		void initProperties(void);
		virtual void dumpNodeProperties(FILE *f);
		virtual void dumpNodeProperties(FILE *f,char*,char*);

		// -----------------------------------------------------------
		// property-related fields (value)
		// -----------------------------------------------------------
		char*		title_prop;
		char*		label_prop;
		char*		info1_prop;
		char*		info2_prop;
		char*		info3_prop;
		vcgColor	color_prop;
		vcgColor	textcolor_prop;
		vcgColor	bordercolor_prop;
		int			width_prop;
		int			height_prop;
		int			borderwidth_prop;
		int 		loc_prop[2];
		int			folding_prop;
		float		scaling_prop;
		vcgTextMode	textmode_prop;
		vcgShape	shape_prop;
		int			level_prop;
		int			vertical_order_prop;
		int			horizontal_order_prop;

		// -----------------------------------------------------------
		// property-related fields (assigned flag)
		// -----------------------------------------------------------
		bool title_prop_set;
		bool label_prop_set;
		bool info1_prop_set;
		bool info2_prop_set;
		bool info3_prop_set;
		bool color_prop_set;
		bool textcolor_prop_set;
		bool bordercolor_prop_set;
		bool width_prop_set;
		bool height_prop_set;
		bool borderwidth_prop_set;
		bool loc_prop_set;
		bool folding_prop_set;
		bool scaling_prop_set;
		bool textmode_prop_set;
		bool shape_prop_set;
		bool level_prop_set;
		bool vertical_order_prop_set;
		bool horizontal_order_prop_set;
};

#endif
