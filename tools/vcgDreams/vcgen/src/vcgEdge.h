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


#ifndef __vcgEdge
#define __vcgEdge

// ---------------------------
// -- some attribute types:
// ---------------------------
#include "vcgArrowStyleAttr.h"
#include "vcgColorAttr.h"
#include "vcgLineStyleAttr.h"
// ---------------------------

#include "vcgComponent.h"
#include "vcgNode.h"

/**
  * Normal Edge component.
  * vcgen framework component
  * @version 0.3
  * @date started at 2002-02-26 
  * @author Federico Ardanaz
  */
class vcgEdge : public vcgComponent
{
	public:
		/**
		  * Constructor based on node objects.
		  * @param src Source Node
		  * @param dst Destination Node
		  * @param label [Optional] edge label
		  */
		vcgEdge(vcgNode *src, vcgNode *dst, char* label=NULL)
		{ 
			initProperties();	
			this->myLayer = EDGE_LAYER;
			sourcename_prop_set = true;
			targetname_prop_set = true;
			sourcename_prop = strdup(src->getTitle());
			targetname_prop = strdup(dst->getTitle());
			if (label!=NULL)
				this->setLabel(label);
		}

		/**
		  * Constructor based on node names (strings).
		  * @param srcTitle Source Node
		  * @param dstTitle Destination Node
		  * @param label [Optional] edge label
		  */
		vcgEdge(char *srcTitle, char *dstTitle, char* label=NULL)
		{ 
			initProperties();	
			this->myLayer = EDGE_LAYER;
			sourcename_prop_set = true;
			targetname_prop_set = true;
			sourcename_prop = strdup(srcTitle);
			targetname_prop = strdup(dstTitle);
			if (label!=NULL)
				this->setLabel(label);
		}

		/**
		  * Destructor
		  */
		virtual ~vcgEdge(void)
		{
			if (sourcename_prop!=NULL)	free(sourcename_prop);
			if (targetname_prop!=NULL)	free(targetname_prop);
			if (label_prop!=NULL)		free(label_prop);
		}

		/**
		  * vcgComponent interface methods.
		  */
		virtual void toVCG(FILE *f,int tabLevel);

		// -----------------------------------------------------------
		// public methods to set node properties
		// -----------------------------------------------------------
		inline void setLabel (char* value)
		{if(label_prop!=NULL) free(label_prop); label_prop=strdup(value);label_prop_set=true;}

		inline void setTextColor (vcgColor value)
		{textcolor_prop=value;textcolor_prop_set=true;}

		inline void setTextColor (int value)
		{setTextColor((vcgColor)value);}

		inline void setColor (vcgColor value)
		{color_prop=value;color_prop_set=true;}

		inline void setColor (int value)
		{setColor((vcgColor)value);}
		
		inline void setThickness (int value)
		{thickness_prop=value;thickness_prop_set=true;}

		inline void setClass (int value)
		{class_prop=value;class_prop_set=true;}

		inline void setPriority (int value)
		{priority_prop=value;priority_prop_set=true;}

		inline void setArrowColor (vcgColor value)
		{arrowcolor_prop=value;arrowcolor_prop_set=true;}

		inline void setArrowColor (int value)
		{setArrowColor((vcgColor)value);}

		inline void setBackArrowColor (vcgColor value)
		{backarrowcolor_prop=value;backarrowcolor_prop_set=true;}

		inline void setBackArrowColor (int value)
		{setBackArrowColor((vcgColor)value);}

		inline void setArrowSize (int value)
		{arrowsize_prop=value;arrowsize_prop_set=true;}

		inline void setBackArrowSize (int value)
		{backarrowsize_prop=value;backarrowsize_prop_set=true;}

		inline void setArrowStyle (vcgArrowStyle value)
		{arrowstyle_prop=value;arrowstyle_prop_set=true;}

		inline void setBackArrowStyle (vcgArrowStyle value)
		{backarrowstyle_prop=value;backarrowstyle_prop_set=true;}

		inline void setLineStyle (vcgLineStyle value)
		{linestyle_prop=value;linestyle_prop_set=true;}

		inline void setAnchor (int value)
		{anchor_prop=value;anchor_prop_set=true;}

		inline void setHorizontalOrder (int value)
		{horizontalorder_prop=value;horizontalorder_prop_set=true;}

		inline void setNear (bool value)
		{nearEdge=value;}



		// -----------------------------------------------------------
		// public methods to get node properties
		// -----------------------------------------------------------
		inline char* getLabel ()
		{return label_prop;}

		inline vcgColor getTextColor ()
		{return textcolor_prop;}

		inline vcgColor getColor ()
		{return color_prop;}

		inline int getThickness ()
		{return thickness_prop;}

		inline int getClass ()
		{return class_prop;}

		inline int getPriority ()
		{return priority_prop;}

		inline vcgColor getArrowColor ()
		{return arrowcolor_prop;}

		inline vcgColor getBackArrowColor ()
		{return backarrowcolor_prop;}

		inline int getArrowSize ()
		{return arrowsize_prop;}

		inline int getBackArrowSize ()
		{return backarrowsize_prop;}

		inline vcgArrowStyle getArrowStyle ()
		{return arrowstyle_prop;}

		inline vcgArrowStyle getBackArrowStyle ()
		{return backarrowstyle_prop;}

		inline vcgLineStyle getLineStyle ()
		{return linestyle_prop;}

		inline int getAnchor ()
		{return anchor_prop;}

		inline int getHorizontalOrder ()
		{return horizontalorder_prop;}


		// -----------------------------------------------------------
		// public methods to know whether a property is set or not
		// -----------------------------------------------------------
		inline bool isLabelSet()
		{return label_prop_set;}

		inline bool isTextColorSet()
		{return textcolor_prop_set;}

		inline bool isColorSet()
		{return color_prop_set;}

		inline bool isThicknessSet()
		{return thickness_prop_set;}

		inline bool isClassSet()
		{return class_prop_set;}

		inline bool isPrioritySet()
		{return priority_prop_set;}

		inline bool isArrowColorSet()
		{return arrowcolor_prop_set;}

		inline bool isBackArrowColorSet()
		{return backarrowcolor_prop_set;}

		inline bool isArrowSizeSet()
		{return arrowsize_prop_set;}

		inline bool isBackArrowSizeSet()
		{return backarrowsize_prop_set;}

		inline bool isArrowStyleSet()
		{return arrowstyle_prop_set;}

		inline bool isBackArrowStyleSet()
		{return backarrowstyle_prop_set;}

		inline bool isLineStyleSet()
		{return linestyle_prop_set;}

		inline bool isAnchorSet()
		{return anchor_prop_set;}

		inline bool isHorizontalOrderSet()
		{return horizontalorder_prop_set;}


	protected:
		// -----------------------------------------------------------
		// prop. initialization & dump
		// -----------------------------------------------------------
		virtual void initProperties(void);
		virtual void dumpEdgeProperties(FILE *f);
		virtual void dumpEdgeProperties(FILE *f,char*);
		
		// -----------------------------------------------------------
		// property-related fields (value)
		// -----------------------------------------------------------
		char*			sourcename_prop;
		char*			targetname_prop;
		char*			label_prop;
		vcgColor		textcolor_prop;
		vcgColor		color_prop;
		int				thickness_prop;
		int				class_prop;
		int				priority_prop;
		vcgColor		arrowcolor_prop;
		vcgColor		backarrowcolor_prop;
		int				arrowsize_prop;
		int				backarrowsize_prop;
		vcgArrowStyle	arrowstyle_prop;
		vcgArrowStyle	backarrowstyle_prop;
		vcgLineStyle	linestyle_prop;
		int				anchor_prop;
		int				horizontalorder_prop;

        bool            nearEdge;

		// -----------------------------------------------------------
		// property-related fields (assigned flag)
		// -----------------------------------------------------------
		bool sourcename_prop_set;
		bool targetname_prop_set;
		bool label_prop_set;
		bool textcolor_prop_set;
		bool color_prop_set;
		bool thickness_prop_set;
		bool class_prop_set;
		bool priority_prop_set;
		bool arrowcolor_prop_set;
		bool backarrowcolor_prop_set;
		bool arrowsize_prop_set;
		bool backarrowsize_prop_set;
		bool arrowstyle_prop_set;
		bool backarrowstyle_prop_set;
		bool linestyle_prop_set;
		bool anchor_prop_set;
		bool horizontalorder_prop_set;
};

#endif
