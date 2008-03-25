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
// started at 2002-02-25 by Federico Ardanaz
//
// -------------------------------------------------------------------

#include "vcgGraph.h"

// -----------------------------------------------------------
// public method to add class names
// -----------------------------------------------------------
void vcgGraph::addClassName(int i, char* value)
{
	// build a new object (struct) to hold the data
	vcgClassNameEntry* cne = new vcgClassNameEntry(i,value);
	addComponent(cne);
}


// -----------------------------------------------------------
// public method to color definitions
// -----------------------------------------------------------
void vcgGraph::addColorEntry(int i, int r, int g, int b)
{
	// build a new object (struct) to hold the data
	vcgColorEntry* ce = new vcgColorEntry(i,r,g,b);
	addComponent(ce);
}


// -----------------------------------------------------------
// methods to dump out vcg-syntax representation
// -----------------------------------------------------------

// 
void vcgGraph::toVCG(void) { toVCG(fout,0); }

// 
void vcgGraph::toVCG(FILE *f) { toVCG(f,0); }

// 
void vcgGraph::toVCG(FILE *f, int tabLevel)
{
	this->myTabLevel = tabLevel;
	//  graph header
	if (this->myParent==NULL)
	{
		fprintf(f,"// --------------------------------------------------------------\n");
		fprintf(f,"// VCG file created with 'vcgen' framework version %s \n",VCGEN_VERSION);	
		fprintf(f,"// Intel Corporation, Federico Ardanaz\n");
		fprintf(f,"// --------------------------------------------------------------\n");
	}
	else
	{
		putTabs(f);fprintf(f,"// --------------------------------------------------------------\n");
		putTabs(f);fprintf(f,"// Sub-graph specification:\n");
		putTabs(f);fprintf(f,"// --------------------------------------------------------------\n");
	}
	
	putTabs(f);fprintf(f,"graph:\n");
	putTabs(f);fprintf(f,"{\n");
	
	// layer 0 elements must be first
	List<vcgComponent*>::Iterator it0  = (compList->getLayer(0))->begin();
	List<vcgComponent*>::Iterator eol0 = (compList->getLayer(0))->end();
	if (it0 != eol0)
	{
		fprintf(f,"\n");
		putTabs(f,1);fprintf(f,"// --------------------------------------------------------------\n");
		putTabs(f,1);fprintf(f,"// Layer 0:\n");
		putTabs(f,1);fprintf(f,"// --------------------------------------------------------------\n\n");
	}
	while ( it0 != eol0 )
	{
		vcgComponent* cmp = *it0;
		it0++;
		cmp->toVCG(f,tabLevel+1);
	}

	// graph headers
	putGraphProps(f);

	// node defaults and so on
	edgeDefaults->toVCG(f,tabLevel+1);
	foldEdgeDefaults->toVCG(f,tabLevel+1);
	nodeDefaults->toVCG(f,tabLevel+1);
	foldNodeDefaults->toVCG(f,tabLevel+1);
		
	
	// other layers
	for (int i=1;i<NUM_LAYERS;i++)
	{
		List<vcgComponent*>::Iterator it  = (compList->getLayer(i))->begin();
		List<vcgComponent*>::Iterator eol = (compList->getLayer(i))->end();

		if (it != eol)
		{
			fprintf(f,"\n");
			putTabs(f,1);fprintf(f,"// --------------------------------------------------------------\n");
			putTabs(f,1);fprintf(f,"// Layer %d:\n",i);
			putTabs(f,1);fprintf(f,"// --------------------------------------------------------------\n\n");
		}
		while ( it != eol )
		{
			vcgComponent* cmp = *it;
			it++;
			cmp->toVCG(f,tabLevel+1);
		}
	}
	
	//  close graph
	putTabs(f);fprintf(f,"}\n");

	if (this->myParent!=NULL)
	{
		putTabs(f);fprintf(f,"// --------------------------------------------------------------\n");
		putTabs(f);fprintf(f,"// End of sub-graph specification\n");
		putTabs(f);fprintf(f,"// --------------------------------------------------------------\n");
	}
	else
	{
		putTabs(f);fprintf(f,"// --------------------------------------------------------------\n");
		putTabs(f);fprintf(f,"// End of main graph specification\n");
		putTabs(f);fprintf(f,"// --------------------------------------------------------------\n");
		fflush(f);		
	}
}

void vcgGraph::putGraphProps(FILE* f)
{
	if (title_prop_set) {putTabs(f,1);fprintf(f,"title: \"%s\"\n",title_prop);};
	if (label_prop_set) {putTabs(f,1);fprintf(f,"label: \"%s\"\n",label_prop);};
	if (info1_prop_set) {putTabs(f,1);fprintf(f,"info1: \"%s\"\n",info1_prop);};
	if (info2_prop_set) {putTabs(f,1);fprintf(f,"info2: \"%s\"\n",info2_prop);};
	if (info3_prop_set) {putTabs(f,1);fprintf(f,"info3: \"%s\"\n",info3_prop);};
	if (color_prop_set) {putTabs(f,1);fprintf(f,"color: %s\n",vcgColorAttr:: toStr(color_prop));};
	if (textcolor_prop_set) {putTabs(f,1);fprintf(f,"textcolor: %s\n",vcgColorAttr:: toStr(textcolor_prop));};
	if (bordercolor_prop_set) {putTabs(f,1);fprintf(f,"bordercolor: %s\n",vcgColorAttr:: toStr(bordercolor_prop));};
	if (width_prop_set) {putTabs(f,1);fprintf(f,"width: %d\n",width_prop);};
	if (height_prop_set) {putTabs(f,1);fprintf(f,"height: %d\n",height_prop);};
	if (borderwidth_prop_set) {putTabs(f,1);fprintf(f,"borderwidth: %d\n",borderwidth_prop);};
	if (loc_prop_set) {putTabs(f,1);fprintf(f,"loc: {x: %d y: %d}\n",loc_prop[0],loc_prop[1]);};
	if (folding_prop_set) {putTabs(f,1);fprintf(f,"folding: %d\n",folding_prop);};
	if (scaling_prop_set) {putTabs(f,1);fprintf(f,"scaling: %f\n",scaling_prop);};
	if (textmode_prop_set) {putTabs(f,1);fprintf(f,"textmode: %s\n",vcgTextModeAttr:: toStr(textmode_prop));};
	if (shape_prop_set) {putTabs(f,1);fprintf(f,"shape: %s\n",vcgShapeAttr:: toStr(shape_prop));};
	if (vertical_order_prop_set) {putTabs(f,1);fprintf(f,"vertical_order: %d\n",vertical_order_prop);};
	if (horizontal_order_prop_set) {putTabs(f,1);fprintf(f,"horizontal_order: %d\n",horizontal_order_prop);};
	if (status_prop_set) {putTabs(f,1);fprintf(f,"status: %s\n",vcgStatusAttr:: toStr(status_prop));};
	if (xmax_prop_set) {putTabs(f,1);fprintf(f,"xmax: %d\n",xmax_prop);};
	if (ymax_prop_set) {putTabs(f,1);fprintf(f,"ymax: %d\n",ymax_prop);};
	if (xbase_prop_set) {putTabs(f,1);fprintf(f,"xbase: %d\n",xbase_prop);};
	if (ybase_prop_set) {putTabs(f,1);fprintf(f,"ybase: %d\n",ybase_prop);};
	if (xspace_prop_set) {putTabs(f,1);fprintf(f,"xspace: %d\n",xspace_prop);};
	if (xlspace_prop_set) {putTabs(f,1);fprintf(f,"xlspace: %d\n",xlspace_prop);};
	if (yspace_prop_set) {putTabs(f,1);fprintf(f,"yspace: %d\n",yspace_prop);};
	if (xraster_prop_set) {putTabs(f,1);fprintf(f,"xraster: %d\n",xraster_prop);};
	if (xlraster_prop_set) {putTabs(f,1);fprintf(f,"xlraster: %d\n",xlraster_prop);};
	if (yraster_prop_set) {putTabs(f,1);fprintf(f,"yraster: %d\n",yraster_prop);};
	if (invisible_prop_set) {putTabs(f,1);fprintf(f,"invisible: %d\n",invisible_prop);};
	if (hidden_prop_set) {putTabs(f,1);fprintf(f,"hidden: %d\n",hidden_prop);};
	if (infoname1_prop_set) {putTabs(f,1);fprintf(f,"infoname: 1 : %s\n",infoname1_prop);};
	if (infoname2_prop_set) {putTabs(f,2);fprintf(f,"infoname: 2 : %s\n",infoname2_prop);};
	if (infoname3_prop_set) {putTabs(f,3);fprintf(f,"infoname: 3 : %s\n",infoname3_prop);};
	if (layoutalgorithm_prop_set) {putTabs(f,1);fprintf(f,"layoutalgorithm: %s\n",vcgLayoutAlgorithmAttr:: toStr(layoutalgorithm_prop));};
	if (layout_downfactor_prop_set) {putTabs(f,1);fprintf(f,"layout_downfactor: %d\n",layout_downfactor_prop);};
	if (layout_upfactor_prop_set) {putTabs(f,1);fprintf(f,"layout_upfactor: %d\n",layout_upfactor_prop);};
	if (layout_nearfactor_prop_set) {putTabs(f,1);fprintf(f,"layout_nearfactor: %d\n",layout_nearfactor_prop);};
	if (splinefactor_prop_set) {putTabs(f,1);fprintf(f,"splinefactor: %d\n",splinefactor_prop);};
	if (late_edge_labels_prop_set) {putTabs(f,1);fprintf(f,"late_edge_labels: %s\n",vcgYesNoAttr:: toStr(late_edge_labels_prop));};
	if (display_edge_labels_prop_set) {putTabs(f,1);fprintf(f,"display_edge_labels: %s\n",vcgYesNoAttr:: toStr(display_edge_labels_prop));};
	if (dirty_edge_labels_prop_set) {putTabs(f,1);fprintf(f,"dirty_edge_labels: %s\n",vcgYesNoAttr:: toStr(dirty_edge_labels_prop));};
	if (finetuning_prop_set) {putTabs(f,1);fprintf(f,"finetuning: %s\n",vcgYesNoAttr:: toStr(finetuning_prop));};
	if (ignoresingles_prop_set) {putTabs(f,1);fprintf(f,"ignoresingles: %s\n",vcgYesNoAttr:: toStr(ignoresingles_prop));};
	if (straight_phase_prop_set) {putTabs(f,1);fprintf(f,"straight_phase: %s\n",vcgYesNoAttr:: toStr(straight_phase_prop));};
	if (priority_phase_prop_set) {putTabs(f,1);fprintf(f,"priority_phase: %s\n",vcgYesNoAttr:: toStr(priority_phase_prop));};
	if (manhattan_edges_prop_set) {putTabs(f,1);fprintf(f,"manhattan_edges: %s\n",vcgYesNoAttr:: toStr(manhattan_edges_prop));};
	if (smanhattan_edges_prop_set) {putTabs(f,1);fprintf(f,"smanhattan_edges: %s\n",vcgYesNoAttr:: toStr(smanhattan_edges_prop));};
	if (nearedges_prop_set) {putTabs(f,1);fprintf(f,"nearedges: %s\n",vcgYesNoAttr:: toStr(nearedges_prop));};
	if (orientation_prop_set) {putTabs(f,1);fprintf(f,"orientation: %s\n",vcgOrientationAttr:: toStr(orientation_prop));};
	if (node_alignment_prop_set) {putTabs(f,1);fprintf(f,"node_alignment: %s\n",vcgNodeAlignAttr:: toStr(node_alignment_prop));};
	if (port_sharing_prop_set) {putTabs(f,1);fprintf(f,"port_sharing: %s\n",vcgYesNoAttr:: toStr(port_sharing_prop));};
	if (arrowmode_prop_set) {putTabs(f,1);fprintf(f,"arrowmode: %s\n",vcgArrowModeAttr:: toStr(arrowmode_prop));};
	if (spreadlevel_prop_set) {putTabs(f,1);fprintf(f,"spreadlevel: %d\n",spreadlevel_prop);};
	if (treefactor_prop_set) {putTabs(f,1);fprintf(f,"treefactor: %f\n",treefactor_prop);};
	if (crossingphase2_prop_set) {putTabs(f,1);fprintf(f,"crossingphase2: %s\n",vcgYesNoAttr:: toStr(crossingphase2_prop));};
	if (crossingoptimization_prop_set) {putTabs(f,1);fprintf(f,"crossingoptimization: \"%s\"\n",vcgYesNoAttr:: toStr(crossingoptimization_prop));};
	if (crossingweight_prop_set) {putTabs(f,1);fprintf(f,"crossingweight: %s\n",vcgCrossWeightAttr:: toStr(crossingweight_prop));};
	if (view_prop_set) {putTabs(f,1);fprintf(f,"view: %s\n",vcgViewAttr:: toStr(view_prop));};
	if (edges_prop_set) {putTabs(f,1);fprintf(f,"edges: %s\n",vcgYesNoAttr:: toStr(edges_prop));};
	if (nodes_prop_set) {putTabs(f,1);fprintf(f,"nodes: %s\n",vcgYesNoAttr:: toStr(nodes_prop));};
	if (splines_prop_set) {putTabs(f,1);fprintf(f,"splines: %s\n",vcgYesNoAttr:: toStr(splines_prop));};
	if (bmax_prop_set) {putTabs(f,1);fprintf(f,"bmax: %d\n",bmax_prop);};
	if (cmax_prop_set) {putTabs(f,1);fprintf(f,"cmax: %d\n",cmax_prop);};
	if (cmin_prop_set) {putTabs(f,1);fprintf(f,"cmin: %d\n",cmin_prop);};
	if (pmax_prop_set) {putTabs(f,1);fprintf(f,"pmax: %d\n",pmax_prop);};
	if (pmin_prop_set) {putTabs(f,1);fprintf(f,"pmin: %d\n",pmin_prop);};
	if (rmax_prop_set) {putTabs(f,1);fprintf(f,"rmax: %d\n",rmax_prop);};
	if (rmin_prop_set) {putTabs(f,1);fprintf(f,"rmin: %d\n",rmin_prop);};
	if (smax_prop_set) {putTabs(f,1);fprintf(f,"smax: %d\n",smax_prop);};
}

// -----------------------------------------------------------
// method that initialize some property flags
// -----------------------------------------------------------
void vcgGraph::initProperties()
{
	title_prop_set					=	false;
	label_prop_set					=	false;
	info1_prop_set					=	false;
	info2_prop_set					=	false;
	info3_prop_set					=	false;
	color_prop_set					=	false;
	textcolor_prop_set				=	false;
	bordercolor_prop_set			=	false;
	width_prop_set					=	false;
	height_prop_set					=	false;
	borderwidth_prop_set			=	false;
	loc_prop_set					=	false;
	folding_prop_set				=	false;
	scaling_prop_set				=	false;
	textmode_prop_set				=	false;
	shape_prop_set					=	false;
	vertical_order_prop_set			=	false;
	horizontal_order_prop_set		=	false;
	status_prop_set					=	false;
	xmax_prop_set					=	false;
	ymax_prop_set					=	false;
	xbase_prop_set					=	false;
	ybase_prop_set					=	false;
	xspace_prop_set					=	false;
	xlspace_prop_set				=	false;
	yspace_prop_set					=	false;
	xraster_prop_set				=	false;
	xlraster_prop_set				=	false;
	yraster_prop_set				=	false;
	invisible_prop_set				=	false;
	hidden_prop_set					=	false;
	infoname1_prop_set				=	false;
	infoname2_prop_set				=	false;
	infoname3_prop_set				=	false;
	layoutalgorithm_prop_set		=	false;
	layout_downfactor_prop_set		=   false;
	layout_upfactor_prop_set		=   false;
	layout_nearfactor_prop_set		=   false;
	splinefactor_prop_set			=	false;
	late_edge_labels_prop_set		=	false;
	display_edge_labels_prop_set	=	false;
	dirty_edge_labels_prop_set		=	false;
	finetuning_prop_set				=	false;
	ignoresingles_prop_set			=	false;
	straight_phase_prop_set			=	false;
	priority_phase_prop_set			=	false;
	manhattan_edges_prop_set		=	false;
	smanhattan_edges_prop_set		=	false;
	nearedges_prop_set				=	false;
	orientation_prop_set			=	false;
	node_alignment_prop_set			=	false;
	port_sharing_prop_set			=	false;
	arrowmode_prop_set				=	false;
	spreadlevel_prop_set			=	false;
	treefactor_prop_set				=	false;
	crossingphase2_prop_set			=	false;
	crossingoptimization_prop_set	=	false;
	crossingweight_prop_set			=	false;
	view_prop_set					=	false;
	edges_prop_set					=	false;
	nodes_prop_set					=	false;
	splines_prop_set				=	false;
	bmax_prop_set					=	false;
	cmax_prop_set					=	false;
	cmin_prop_set					=	false;
	pmax_prop_set					=	false;
	pmin_prop_set					=	false;
	rmax_prop_set					=	false;
	rmin_prop_set					=	false;
	smax_prop_set					=	false;
	
	// I must ensure that all pointers are null
	title_prop=NULL;
	label_prop=NULL;
	info1_prop=NULL;
	info2_prop=NULL;
	info3_prop=NULL;
	infoname1_prop=NULL;
	infoname2_prop=NULL;
	infoname3_prop=NULL;
}

// -----------------------------------------------------------
// private method to initialize graph structures
// -----------------------------------------------------------
void vcgGraph::initGraph(void)
{
	initProperties();
	compList = new vcgComponentList();
	this->myLayer = GRAPH_LAYER;
	
	// use fake node and edges to hold default properties....
	nodeDefaults = new vcgNodePrefix("node.");
	foldNodeDefaults = new vcgNodePrefix("foldnode.");

 	edgeDefaults = new vcgEdgePrefix("edge.");
 	foldEdgeDefaults = new vcgEdgePrefix("foldedge.");
}
