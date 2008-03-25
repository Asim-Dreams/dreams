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

#ifndef __vcgGraph
#define __vcgGraph

/** The framework version (as string) */
#define VCGEN_VERSION "0.3"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ----------------------------------
// -- attribute types:
// ----------------------------------
#include "vcgArrowModeAttr.h"
#include "vcgArrowStyleAttr.h"
#include "vcgColorAttr.h"
#include "vcgCrossWeightAttr.h"
#include "vcgLayoutAlgorithmAttr.h"
#include "vcgLineStyleAttr.h"
#include "vcgNodeAlignAttr.h"
#include "vcgOrientationAttr.h"
#include "vcgShapeAttr.h"
#include "vcgStatusAttr.h"
#include "vcgTextModeAttr.h"
#include "vcgViewAttr.h"
#include "vcgYesNoAttr.h"
// ----------------------------------
#include "vcgComponent.h"
#include "vcgColorEntry.h"
#include "vcgClassNameEntry.h"
#include "vcgComponentList.h"
#include "vcgNodePrefix.h"
#include "vcgEdgePrefix.h"
// ----------------------------------

/**
  * Graph class: this is the main class used to build a vcg file.
  * vcgen framework component
  *
  * The Graph class is a kind of place holder to keep al node
  * and edge references and some global properties
  *
  * @version 0.3
  * @date started at 2002-02-25 
  * @author Federico Ardanaz
  */
class vcgGraph : public vcgComponent
{
	public:

		/**
		  * Void Constructor (standard output).
		  */
		vcgGraph (void)
		{
			this->fout = stdout; 
			initGraph();
		}

		/**
		  * Named Constructor (standard output).
		  * @param title the graph title
		  */
		vcgGraph (char* title)
		{
			this->fout = stdout; 
			initGraph();
			this->setTitle(title);
		}

		/**
		  * Constructor with file specifier.
		  * @param fvalue FILE pointer where vcg-syntax will be dumped on
		  */
		vcgGraph (FILE *fvalue)
		{ 
			this->fout = fvalue; 
			initGraph();
		}
		
		/**
		  * Constructor with file specifier & title
		  * @param fvalue FILE pointer where vcg-syntax will be dumped on
		  * @param title the graph title
		  */
		vcgGraph (FILE *fvalue, char* title)
		{ 
			this->fout = fvalue; 
			initGraph();
			this->setTitle(title);
		}

		/**
		  * Destructor.
		  */
		virtual ~vcgGraph(void)
		{
			// free components
			delete compList;
			delete edgeDefaults;
			delete foldEdgeDefaults;
			delete nodeDefaults;
			delete foldNodeDefaults;
			
			// free allocated strings
			if(title_prop!=NULL) free(title_prop);
			if(label_prop!=NULL) free(label_prop);
			if(info1_prop!=NULL) free(info1_prop);
			if(info2_prop!=NULL) free(info2_prop);
			if(info3_prop!=NULL) free(info3_prop);
			if(infoname1_prop!=NULL) free(infoname1_prop);
			if(infoname2_prop!=NULL) free(infoname2_prop);
			if(infoname3_prop!=NULL) free(infoname3_prop);
		}

		/**
		  * Public method to add class names.
		  * @param i class number
		  * @value class name
		  */  
		void addClassName(int i, char* value);
		
		/**
		  * Public method to color definitions.
		  * @param i color number [0..255]
		  * @param r red component [0.255]
		  * @param g green component [0.255]
		  * @param b blue component [0.255]
		  */
		void addColorEntry(int i, int r, int g, int b);

		/**
		 * To get a reference to the default node.
		 * The default node is used to set node properties which will
		 * be used as defualt values for real nodes.
		 */
		inline vcgNode* getNodeDefaults(void)
		{ return nodeDefaults; }
		
		/**
		 * To get a reference to the default edge.
		 * The default edge is used to set edge properties which will
		 * be used as defualt values for real edges.
		 */
		inline vcgEdge* getEdgeDefaults(void)
		{ return edgeDefaults; }

		/**
		 * To get a reference to the default fold node.
		 * The default fold node is used to set node properties which will
		 * be used as defualt values for real folded nodes.
		 */
		inline vcgNode* getFoldNodeDefaults(void)
		{ return foldNodeDefaults; }

		/**
		 * To get a reference to the default fold edge.
		 * The default fold edge is used to set edge properties which will
		 * be used as defualt values for real folded edges.
		 */
		inline vcgEdge* getFoldEdgeDefaults(void)
		{ return foldEdgeDefaults; }




		// -----------------------------------------------------------
		// public methods to set graph properties
		// -----------------------------------------------------------
		inline void setTitle(char* value)
		{if(title_prop!=NULL) free(title_prop);title_prop = strdup(value); title_prop_set=true;};

		inline void setLabel(char* value)
		{if(label_prop!=NULL) free(label_prop);label_prop = value; label_prop_set=true;};

		inline void setInfo1(char* value)
		{if(info1_prop!=NULL) free(info1_prop);info1_prop = value; info1_prop_set=true;};

		inline void setInfo2(char* value)
		{if(info2_prop!=NULL) free(info2_prop);info2_prop = value; info2_prop_set=true;};

		inline void setInfo3(char* value)
		{if(info3_prop!=NULL) free(info3_prop);info3_prop = value; info3_prop_set=true;};

		inline void setColor(vcgColor value)
		{color_prop = value; color_prop_set=true;};

		inline void setColor(int value)
		{setColor((vcgColor)value);};

		inline void setTextColor(vcgColor value)
		{textcolor_prop = value; textcolor_prop_set=true;};

		inline void setTextColor(int value)
		{setTextColor((vcgColor)value);};

		inline void setBorderColor(vcgColor value)
		{bordercolor_prop = value; bordercolor_prop_set=true;};

		inline void setBorderColor(int value)
		{setBorderColor((vcgColor)value);};

		inline void setWidth(int value)
		{width_prop = value; width_prop_set=true;};

		inline void setHeight(int value)
		{height_prop = value; height_prop_set=true;};

		inline void setBorderWidth(int value)
		{borderwidth_prop = value; borderwidth_prop_set=true;};

		inline void setFolding(int value)
		{folding_prop = value; folding_prop_set=true;};

		inline void setScaling(float value)
		{scaling_prop = value; scaling_prop_set=true;};

		inline void setTextmode(vcgTextMode value)
		{textmode_prop = value; textmode_prop_set=true;};

		inline void setShape(vcgShape value)
		{shape_prop = value; shape_prop_set=true;};

		inline void setVerticalOrder(int value)
		{vertical_order_prop = value; vertical_order_prop_set=true;};

		inline void setHorizontalOrder(int value)
		{horizontal_order_prop = value; horizontal_order_prop_set=true;};

		inline void setStatus(vcgStatus value)
		{status_prop = value; status_prop_set=true;};

		inline void setXmax(int value)
		{xmax_prop = value; xmax_prop_set=true;};

		inline void setYmax(int value)
		{ymax_prop = value; ymax_prop_set=true;};

		inline void setXbase(int value)
		{xbase_prop = value; xbase_prop_set=true;};

		inline void setYbase(int value)
		{ybase_prop = value; ybase_prop_set=true;};

		inline void setXspace(int value)
		{xspace_prop = value; xspace_prop_set=true;};

		inline void setXlspace(int value)
		{xlspace_prop = value; xlspace_prop_set=true;};

		inline void setYspace(int value)
		{yspace_prop = value; yspace_prop_set=true;};

		inline void setXraster(int value)
		{xraster_prop = value; xraster_prop_set=true;};

		inline void setXlraster(int value)
		{xlraster_prop = value; xlraster_prop_set=true;};

		inline void setYraster(int value)
		{yraster_prop = value; yraster_prop_set=true;};

		inline void setInvisible(int value)
		{invisible_prop = value; invisible_prop_set=true;};

		inline void setHidden(int value)
		{hidden_prop = value; hidden_prop_set=true;};

		inline void setInfoName1(char* value)
		{if(infoname1_prop!=NULL) free(infoname1_prop);infoname1_prop = value; infoname1_prop_set=true;};

		inline void setInfoName2(char* value)
		{if(infoname2_prop!=NULL) free(infoname2_prop);infoname2_prop = value; infoname2_prop_set=true;};

		inline void setInfoName3(char* value)
		{if(infoname3_prop!=NULL) free(infoname3_prop);infoname3_prop = value; infoname3_prop_set=true;};

		inline void setLayoutAlgorithm(vcgLayoutAlgorithm value)
		{layoutalgorithm_prop = value; layoutalgorithm_prop_set=true;};

		inline void setLayoutDownFactor(int value)
		{layout_downfactor_prop = value; layout_downfactor_prop_set=true;};

		inline void setLayoutUpFactor(int value)
		{layout_upfactor_prop = value; layout_upfactor_prop_set=true;};

		inline void setLayoutNearFactor(int value)
		{layout_nearfactor_prop = value; layout_nearfactor_prop_set=true;};

		inline void setSplineFactor(int value)
		{splinefactor_prop = value; splinefactor_prop_set=true;};

		inline void setLateEdgeLabels(vcgYesNo value)
		{late_edge_labels_prop = value; late_edge_labels_prop_set=true;};

		inline void setDisplayEdgeLabels(vcgYesNo value)
		{display_edge_labels_prop = value; display_edge_labels_prop_set=true;};

		inline void setDirtyEdgeLabels(vcgYesNo value)
		{dirty_edge_labels_prop = value; dirty_edge_labels_prop_set=true;};

		inline void setFineTuning(vcgYesNo value)
		{finetuning_prop = value; finetuning_prop_set=true;};

		inline void setIgnoreSingles(vcgYesNo value)
		{ignoresingles_prop = value; ignoresingles_prop_set=true;};

		inline void setStraightPhase(vcgYesNo value)
		{straight_phase_prop = value; straight_phase_prop_set=true;};

		inline void setPriorityPhase(vcgYesNo value)
		{priority_phase_prop = value; priority_phase_prop_set=true;};

		inline void setManhattanEdges(vcgYesNo value)
		{manhattan_edges_prop = value; manhattan_edges_prop_set=true;};

		inline void setSmanhattanEdges(vcgYesNo value)
		{smanhattan_edges_prop = value; smanhattan_edges_prop_set=true;};

		inline void setNearEdges(vcgYesNo value)
		{nearedges_prop = value; nearedges_prop_set=true;};

		inline void setOrientation(vcgOrientation value)
		{orientation_prop = value; orientation_prop_set=true;};

		inline void setNodeAlignment(vcgNodeAlign value)
		{node_alignment_prop = value; node_alignment_prop_set=true;};

		inline void setPortSharing(vcgYesNo value)
		{port_sharing_prop = value; port_sharing_prop_set=true;};

		inline void setArrowMode(vcgArrowMode value)
		{arrowmode_prop = value; arrowmode_prop_set=true;};

		inline void setTreeFactor(float value)
		{treefactor_prop = value; treefactor_prop_set=true;};

		inline void setCrossingPhase2(vcgYesNo value)
		{crossingphase2_prop = value; crossingphase2_prop_set=true;};

		inline void setCrossingOptimization(vcgYesNo value)
		{crossingoptimization_prop = value; crossingoptimization_prop_set=true;};

		inline void setCrossingWeight(vcgCrossWeight value)
		{crossingweight_prop = value; crossingweight_prop_set=true;};

		inline void setView(vcgView value)
		{view_prop = value; view_prop_set=true;};

		inline void setEdges(vcgYesNo value)
		{edges_prop = value; edges_prop_set=true;};

		inline void setNodes(vcgYesNo value)
		{nodes_prop = value; nodes_prop_set=true;};

		inline void setSplines(vcgYesNo value)
		{splines_prop = value; splines_prop_set=true;};

		inline void setBmax(int value)
		{bmax_prop = value; bmax_prop_set=true;};

		inline void setCmax(int value)
		{cmax_prop = value; cmax_prop_set=true;};

		inline void setCmin(int value)
		{cmin_prop = value; cmin_prop_set=true;};

		inline void setPmax(int value)
		{pmax_prop = value; pmax_prop_set=true;};

		inline void setPmin(int value)
		{pmin_prop = value; pmin_prop_set=true;};

		inline void setRmax(int value)
		{rmax_prop = value; rmax_prop_set=true;};

		inline void setRmin(int value)
		{rmin_prop = value; rmin_prop_set=true;};

		inline void setSmax(int value)
		{smax_prop = value; smax_prop_set=true;};
		


		// -----------------------------------------------------------
		// public methods to get graph properties
		// -----------------------------------------------------------
		inline char* getTitle()
		{return title_prop;};

		inline char* getLabel()
		{return label_prop;};

		inline char* getInfo1()
		{return info1_prop;};

		inline char* getInfo2()
		{return info2_prop;};

		inline char* getInfo3()
		{return info3_prop;};

		inline vcgColor getColor()
		{return color_prop;};

		inline vcgColor getTextColor()
		{return textcolor_prop;};

		inline vcgColor getBorderColor()
		{return bordercolor_prop;};

		inline int getWidth()
		{return width_prop;};

		inline int getHeight()
		{return height_prop;};

		inline int getBorderWidth()
		{return borderwidth_prop;};

		inline int getFolding()
		{return folding_prop;};

		inline float getScaling()
		{return scaling_prop;};

		inline vcgTextMode getTextmode()
		{return textmode_prop;};

		inline vcgShape getShape()
		{return shape_prop;};

		inline int getVerticalOrder()
		{return vertical_order_prop;};

		inline int getHorizontalOrder()
		{return horizontal_order_prop;};

		inline vcgStatus getStatus()
		{return status_prop;};

		inline int getXmax()
		{return xmax_prop;};

		inline int getYmax()
		{return ymax_prop;};

		inline int getXbase()
		{return xbase_prop;};

		inline int getYbase()
		{return ybase_prop;};

		inline int getXspace()
		{return xspace_prop;};

		inline int getXlspace()
		{return xlspace_prop;};

		inline int getYspace()
		{return yspace_prop;};

		inline int getXraster()
		{return xraster_prop;};

		inline int getXlraster()
		{return xlraster_prop;};

		inline int getYraster()
		{return yraster_prop;};

		inline int getInvisible()
		{return invisible_prop;};

		inline int getHidden()
		{return hidden_prop;};

		inline char* getInfoName1()
		{return infoname1_prop;};

		inline char* getInfoName2()
		{return infoname2_prop;};

		inline char* getInfoName3()
		{return infoname3_prop;};

		inline vcgLayoutAlgorithm getLayoutAlgorithm()
		{return layoutalgorithm_prop ;};

		inline int getLayoutDownFactor()
		{return layout_downfactor_prop ;};

		inline int getLayoutUpFactor()
		{return layout_upfactor_prop ;};

		inline int getLayoutNearFactor()
		{return layout_nearfactor_prop ;};

		inline int getSplineFactor()
		{return splinefactor_prop ;};

		inline vcgYesNo getLateEdgeLabels()
		{return late_edge_labels_prop ;};

		inline vcgYesNo getDisplayEdgeLabels()
		{return display_edge_labels_prop;};

		inline vcgYesNo getDirtyEdgeLabels()
		{return dirty_edge_labels_prop;};

		inline vcgYesNo getFineTuning()
		{return finetuning_prop ;};

		inline vcgYesNo getIgnoreSingles()
		{return ignoresingles_prop ;};

		inline vcgYesNo getStraightPhase()
		{return straight_phase_prop ;};

		inline vcgYesNo getPriorityPhase()
		{return priority_phase_prop ;};

		inline vcgYesNo getManhattanEdges()
		{return manhattan_edges_prop ;};

		inline vcgYesNo getSmanhattanEdges()
		{return smanhattan_edges_prop ;};

		inline vcgYesNo getNearEdges()
		{return nearedges_prop ;};

		inline vcgOrientation getOrientation()
		{return orientation_prop ;};

		inline vcgNodeAlign getNodeAlignment()
		{return node_alignment_prop ;};

		inline vcgYesNo getPortSharing()
		{return port_sharing_prop ;};

		inline vcgArrowMode getArrowMode()
		{return arrowmode_prop ;};

		inline float getTreeFactor()
		{return treefactor_prop ;};

		inline vcgYesNo getCrossingPhase2()
		{return crossingphase2_prop ;};

		inline vcgYesNo getCrossingOptimization()
		{return crossingoptimization_prop ;};

		inline vcgCrossWeight getCrossingWeight()
		{return crossingweight_prop ;};

		inline vcgView getView()
		{return view_prop ;};

		inline vcgYesNo getEdges()
		{return edges_prop ;};

		inline vcgYesNo getNodes()
		{return nodes_prop ;};

		inline vcgYesNo getSplines()
		{return splines_prop ;};

		inline int getBmax()
		{return bmax_prop ;};

		inline int getCmax()
		{return cmax_prop ;};

		inline int getCmin()
		{return cmin_prop ;};

		inline int getPmax()
		{return pmax_prop ;};

		inline int getPmin()
		{return pmin_prop ;};

		inline int getRmax()
		{return rmax_prop ;};

		inline int getRmin()
		{return rmin_prop ;};

		inline int getSmax()
		{return smax_prop ;};




		// -----------------------------------------------------------
		// public methods to know whether a property is set or not
		// -----------------------------------------------------------
		inline bool isTitleSet()
		{return title_prop_set;};

		inline bool isLabelSet()
		{return label_prop_set;};

		inline bool isInfo1Set()
		{return info1_prop_set;};

		inline bool isInfo2Set()
		{return info2_prop_set;};

		inline bool isInfo3Set()
		{return info3_prop_set;};

		inline bool isColorSet()
		{return color_prop_set;};

		inline bool isTextColorSet()
		{return textcolor_prop_set;};

		inline bool isBorderColorSet()
		{return bordercolor_prop_set;};

		inline bool isWidthSet()
		{return width_prop_set;};

		inline bool isHeightSet()
		{return height_prop_set;};

		inline bool isBorderWidthSet()
		{return borderwidth_prop_set;};

		inline bool isFoldingSet()
		{return folding_prop_set;};

		inline bool isScalingSet()
		{return scaling_prop_set;};

		inline bool isTextmodeSet()
		{return textmode_prop_set;};

		inline bool isShapeSet()
		{return shape_prop_set;};

		inline bool isVerticalOrderSet()
		{return vertical_order_prop_set;};

		inline bool isHorizontalOrderSet()
		{return horizontal_order_prop_set;};

		inline bool isStatusSet()
		{return status_prop_set;};

		inline bool isXmaxSet()
		{return xmax_prop_set;};

		inline bool isYmaxSet()
		{return ymax_prop_set;};

		inline bool isXbaseSet()
		{return xbase_prop_set;};

		inline bool isYbaseSet()
		{return ybase_prop_set;};

		inline bool isXspaceSet()
		{return xspace_prop_set;};

		inline bool isXlspaceSet()
		{return xlspace_prop_set;};

		inline bool isYspaceSet()
		{return yspace_prop_set;};

		inline bool isXrasterSet()
		{return xraster_prop_set;};

		inline bool isXlrasterSet()
		{return xlraster_prop_set;};

		inline bool isYrasterSet()
		{return yraster_prop_set;};

		inline bool isInvisibleSet()
		{return invisible_prop_set;};

		inline bool isHiddenSet()
		{return hidden_prop_set;};

		inline bool isInfoName1Set()
		{return infoname1_prop_set;};

		inline bool isInfoName2Set()
		{return infoname2_prop_set;};

		inline bool isInfoName3Set()
		{return infoname3_prop_set;};

		inline bool isLayoutAlgorithmSet()
		{return layoutalgorithm_prop_set;};

		inline bool isLayoutDownFactorSet()
		{return layout_downfactor_prop_set;};

		inline bool isLayoutUpFactorSet()
		{return layout_upfactor_prop_set;};

		inline bool isLayoutNearFactorSet()
		{return layout_nearfactor_prop_set;};

		inline bool isSplineFactorSet()
		{return splinefactor_prop_set;};

		inline bool isLateEdgeLabelsSet()
		{return late_edge_labels_prop_set;};

		inline bool isDisplayEdgeLabelsSet()
		{return display_edge_labels_prop_set;};

		inline bool isDirtyEdgeLabelsSet()
		{return dirty_edge_labels_prop_set;};

		inline bool isFineTuningSet()
		{return finetuning_prop_set;};

		inline bool isIgnoreSinglesSet()
		{return ignoresingles_prop_set;};

		inline bool isStraightPhaseSet()
		{return straight_phase_prop_set;};

		inline bool isPriorityPhaseSet()
		{return priority_phase_prop_set;};

		inline bool isManhattanEdgesSet()
		{return manhattan_edges_prop_set;};

		inline bool isSmanhattanEdgesSet()
		{return smanhattan_edges_prop_set;};

		inline bool isNearEdgesSet()
		{return nearedges_prop_set;};

		inline bool isOrientationSet()
		{return orientation_prop_set;};

		inline bool isNodeAlignmentSet()
		{return node_alignment_prop_set;};

		inline bool isPortSharingSet()
		{return port_sharing_prop_set;};

		inline bool isArrowModeSet()
		{return arrowmode_prop_set;};

		inline bool isTreeFactorSet()
		{return treefactor_prop_set;};

		inline bool isCrossingPhase2Set()
		{return crossingphase2_prop_set;};

		inline bool isCrossingOptimizationSet()
		{return crossingoptimization_prop_set;};

		inline bool isCrossingWeightSet()
		{return crossingweight_prop_set;};

		inline bool isViewSet()
		{return view_prop_set;};

		inline bool isEdgesSet()
		{return edges_prop_set;};

		inline bool isNodesSet()
		{return nodes_prop_set;};

		inline bool isSplinesSet()
		{return splines_prop_set;};

		inline bool isBmaxSet()
		{return bmax_prop_set;};

		inline bool isCmaxSet()
		{return cmax_prop_set;};

		inline bool isCminSet()
		{return cmin_prop_set;};

		inline bool isPmaxSet()
		{return pmax_prop_set;};

		inline bool isPminSet()
		{return pmin_prop_set;};

		inline bool isRmaxSet()
		{return rmax_prop_set;};

		inline bool isRminSet()
		{return rmin_prop_set;};

		inline bool isSmaxSet()
		{return smax_prop_set;};
		



		/**
		  * Public method to add components (nodes, edges, etc.).
		  */
		inline void addComponent(vcgComponent* c)
		{
			compList->add(c);
			c->setParent(this);
		}

		/**
		  * vcgComponent interface method.
		  * @param f FILE where to dump on
		  * @param tablevel is used to indentation
		  */
		void toVCG(FILE *f,int tabLavel);
		
		/** 
		  * toVCG without tabLevel is used for root graph
		  * @param f FILE where to dump on
		  */
		void toVCG(FILE *f);

		/** 
		  * toVCG void version (use internal FILE pointer).
		  */
		void toVCG(void);
		
	protected:
		// -----------------------------------------------------------
		// prop. initialization
		// -----------------------------------------------------------
		void initProperties(void);
		void initNodeDefaultProperties(void);
		void initEdgeDefaultProperties(void);
		void initFoldNodeDefaultProperties(void);
		void initFoldEdgeDefaultProperties(void);
		
		// -----------------------------------------------------------
		// graph headers dump out...
		// -----------------------------------------------------------
		void putGraphProps(FILE *f);

		// -----------------------------------------------------------
		// some aux internal fields
		// -----------------------------------------------------------
		FILE *fout;
		vcgComponentList* compList;
 		vcgEdge* edgeDefaults;
 		vcgEdge* foldEdgeDefaults;
		vcgNode* nodeDefaults;
		vcgNode* foldNodeDefaults;
		
		// -----------------------------------------------------------
		// property-related fields (value)
		// -----------------------------------------------------------
		char* title_prop;
		char* label_prop;
		char* info1_prop;
		char* info2_prop;
		char* info3_prop;
		vcgColor color_prop;
		vcgColor textcolor_prop;
		vcgColor bordercolor_prop;
		int width_prop;
		int height_prop;
		int borderwidth_prop;
		int loc_prop[2];
		int folding_prop;
		float scaling_prop;
		vcgTextMode textmode_prop;
		vcgShape shape_prop;
		int vertical_order_prop;
		int horizontal_order_prop;
		vcgStatus status_prop;
		int xmax_prop;
		int ymax_prop;
		int xbase_prop;
		int ybase_prop;
		int xspace_prop;
		int xlspace_prop;
		int yspace_prop;
		int xraster_prop;
		int xlraster_prop;
		int yraster_prop;
		int invisible_prop;
		int hidden_prop;
		char* infoname1_prop;
		char* infoname2_prop;
		char* infoname3_prop;
		vcgLayoutAlgorithm layoutalgorithm_prop;
		int layout_downfactor_prop;
		int layout_upfactor_prop;
		int layout_nearfactor_prop;
		int splinefactor_prop;
		vcgYesNo late_edge_labels_prop;
		vcgYesNo display_edge_labels_prop;
		vcgYesNo dirty_edge_labels_prop;
		vcgYesNo finetuning_prop;
		vcgYesNo ignoresingles_prop;
		vcgYesNo straight_phase_prop;
		vcgYesNo priority_phase_prop;
		vcgYesNo manhattan_edges_prop;
		vcgYesNo smanhattan_edges_prop;
		vcgYesNo nearedges_prop;
		vcgOrientation orientation_prop;
		vcgNodeAlign node_alignment_prop;
		vcgYesNo port_sharing_prop;
		vcgArrowMode arrowmode_prop;
		int spreadlevel_prop;
		float treefactor_prop;
		vcgYesNo crossingphase2_prop;
		vcgYesNo crossingoptimization_prop;
		vcgCrossWeight crossingweight_prop;
		vcgView view_prop;
		vcgYesNo edges_prop;
		vcgYesNo nodes_prop;
		vcgYesNo splines_prop;
		int bmax_prop;
		int cmax_prop;
		int cmin_prop;
		int pmax_prop;
		int pmin_prop;
		int rmax_prop;
		int rmin_prop;
		int smax_prop;
		
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
		bool vertical_order_prop_set;
		bool horizontal_order_prop_set;
		bool status_prop_set;
		bool xmax_prop_set;
		bool ymax_prop_set;
		bool xbase_prop_set;
		bool ybase_prop_set;
		bool xspace_prop_set;
		bool xlspace_prop_set;
		bool yspace_prop_set;
		bool xraster_prop_set;
		bool xlraster_prop_set;
		bool yraster_prop_set;
		bool invisible_prop_set;
		bool hidden_prop_set;
		bool infoname1_prop_set;
		bool infoname2_prop_set;
		bool infoname3_prop_set;
		bool layoutalgorithm_prop_set;
		bool layout_downfactor_prop_set;
		bool layout_upfactor_prop_set;
		bool layout_nearfactor_prop_set;
		bool splinefactor_prop_set;
		bool late_edge_labels_prop_set;
		bool display_edge_labels_prop_set;
		bool dirty_edge_labels_prop_set;
		bool finetuning_prop_set;
		bool ignoresingles_prop_set;
		bool straight_phase_prop_set;
		bool priority_phase_prop_set;
		bool manhattan_edges_prop_set;
		bool smanhattan_edges_prop_set;
		bool nearedges_prop_set;
		bool orientation_prop_set;
		bool node_alignment_prop_set;
		bool port_sharing_prop_set;
		bool arrowmode_prop_set;
		bool spreadlevel_prop_set;
		bool treefactor_prop_set;
		bool crossingphase2_prop_set;
		bool crossingoptimization_prop_set;
		bool crossingweight_prop_set;
		bool view_prop_set;
		bool edges_prop_set;
		bool nodes_prop_set;
		bool splines_prop_set;
		bool bmax_prop_set;
		bool cmax_prop_set;
		bool cmin_prop_set;
		bool pmax_prop_set;
		bool pmin_prop_set;
		bool rmax_prop_set;
		bool rmin_prop_set;
		bool smax_prop_set;

	private:
	
		// -----------------------------------------------------------
		// private method to initialize graph structures
		// -----------------------------------------------------------
		void initGraph(void);		

};

#endif

