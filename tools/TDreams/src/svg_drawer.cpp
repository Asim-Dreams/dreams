/*****************************************************************************
 *
 * @author Oscar Rosell
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

#include "svg_drawer.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <cassert>
#include <libxml/tree.h>

using namespace std;

static const char* COLOR_MAP[8] = {"red","purple","lime","silver","olive","gray","maroon","teal"};

FLOW_DIAGRAM_DRAW::FLOW_DIAGRAM_DRAW():
    last_node_x_(0),
    first_cycle_(static_cast<unsigned int>(-1)),
    last_cycle_(0), 
    color_(0)
{
}

FLOW_DIAGRAM_DRAW::~FLOW_DIAGRAM_DRAW()
{
}

void
FLOW_DIAGRAM_DRAW::AddNode(int id, string name)
{
    NODE node;
    node.id_ = id;
    node.name_ = name;
    node.x_ = (last_node_x_ += 40);
    nodes_.push_back(node);
    node_map_[id] = nodes_.size() - 1;
}

void 
FLOW_DIAGRAM_DRAW::AddEdge(int node_start, unsigned int cycle_start, int node_end, unsigned int cycle_end, string label)
{
    EDGE edge;
    edge.node_start_ = node_start;
    edge.cycle_start_ = cycle_start;
    edge.node_end_ = node_end;
    edge.cycle_end_ = cycle_end;
    edge.label_ = label;
    edge.color_ = color_;
    edges_.push_back(edge);

    first_cycle_ = min(first_cycle_, cycle_start);
    last_cycle_ = max(last_cycle_, cycle_end);
}

void
FLOW_DIAGRAM_DRAW::Dump(ostream& o_stream)
{
#if 0
    // Document
    xmlDocPtr xml_doc = xmlNewDoc(BAD_CAST "1.0");
    // Root
    xmlNodePtr base_node = xmlNewNode(NULL, BAD_CAST "svg");
    xmlNewProp(base_node, BAD_CAST "width", BAD_CAST "800px");
    xmlNewProp(base_node, BAD_CAST "height", BAD_CAST "600px");
    // EndMarker for the arrows
    xmlNodePtr marker_node = xmlNewNode(NULL, BAD_CAST "marker");
    xmlNewProp(marker_node, BAD_CAST "id", BAD_CAST "Triangle");
    xmlNewProp(marker_node, BAD_CAST "viewBox", BAD_CAST "0 0 10 10");
    xmlNewProp(marker_node, BAD_CAST "refX", BAD_CAST "0");
    xmlNewProp(marker_node, BAD_CAST "refY", BAD_CAST "5");
    xmlNewProp(marker_node, BAD_CAST "stroke", BAD_CAST "red");
    xmlNewProp(marker_node, BAD_CAST "fill", BAD_CAST "red");
    xmlNewProp(marker_node, BAD_CAST "markerUnits", BAD_CAST "strokeWidth");
    xmlNewProp(marker_node, BAD_CAST "markerWidth", BAD_CAST "4");
    xmlNewProp(marker_node, BAD_CAST "markerHeight", BAD_CAST "3");
    xmlNewProp(marker_node, BAD_CAST "orient", BAD_CAST "auto");
    xmlNodePtr marker_path = xmlNewNode(NULL, BAD_CAST "path");
    xmlNewProp(marker_path, BAD_CAST "d", BAD_CAST "M 0 0 L 10 5 L 0 10 z");
#endif

    unsigned int vertical_size = last_cycle_ - first_cycle_;

    map<unsigned int /*cycle*/, unsigned int /*comp. cycle*/> cycleMap;
    for (vector<EDGE>::const_iterator it = edges_.begin(); it != edges_.end(); ++it)
    {
        cycleMap[it->cycle_start_] = it->cycle_start_;
        cycleMap[it->cycle_end_] = it->cycle_end_;
    }

    map<unsigned int, unsigned int>::iterator it_prev = cycleMap.begin();
//     unsigned int cycles_removed = 0;
    for (map<unsigned int, unsigned int>::iterator it = cycleMap.begin(); it != cycleMap.end(); ++it)
    {
        if (it != cycleMap.begin())
        {
            if (it->first - it_prev->first > 40)
            {
                // cycles_removed += it->second - it_prev->second + 20;
                it->second = it_prev->second + 20;
            }
            else if (it->first - it_prev->first < 5)
            {
                it->second = it_prev->second + 5;
            }
            else
            {
                it->second = it_prev->second + (it->first - it_prev->first);
            }
        }
        it_prev = it;
    }

    vertical_size = cycleMap[last_cycle_] - first_cycle_;

    o_stream    << "<svg width=\"800px\" height=\"600px\" viewBox=\"0 0 " << (140) << " " << (vertical_size + 70) << "\">\n" << endl;

    o_stream    << "\t<marker id=\"Triangle\" viewBox=\"0 0 10 10\" refX=\"0\" refY=\"5\" stroke=\"red\" fill=\"red\"" 
                << " markerUnits=\"strokeWidth\" markerWidth=\"4\" markerHeight=\"3\" orient=\"auto\">\n";
    o_stream    << "\t\t<path d=\"M 0 0 L 10 5 L 0 10 z\"/>\n";
    o_stream    << "\t</marker>" << endl;

    // SCRIPT
    o_stream    << "\t<script type=\"text/ecmascript\">\n";
    o_stream    << "\t<![CDATA[\n";
    o_stream    << "\tfunction quad_click(evt)\n";
    o_stream    << "\t{\n";
    o_stream    << "\t\tvar quad = evt.target;\n";
    o_stream    << "\t\tvar groups = document.getElementsByTagName(\"g\");\n";
    o_stream    << "\t\tvar orig_class = quad.getAttribute(\"class\");\n\n";
    o_stream    << "\t\tvar opacity_v = quad.getAttribute(\"stroke\") == \"blue\" ? 0 : 1;\n";
    o_stream    << "\t\tif (quad.getAttribute(\"stroke\") == \"blue\"){\n";
    o_stream    << "\t\t\tquad.setAttribute(\"stroke\", \"black\");\n";
    o_stream    << "\t\t}\n";
    o_stream    << "\t\telse {\n";
    o_stream    << "\t\t\tquad.setAttribute(\"stroke\", \"blue\");\n";
    o_stream    << "\t\t}\n\n";
    o_stream    << "\t\tfor (var i=0; i<groups.length; i++)\n";
    o_stream    << "\t\t{\n";
    o_stream    << "\t\t\tvar class_v = groups.item(i).getAttribute(\"class\");\n";
    o_stream    << "\t\t\tif (class_v == orig_class)\n";
    o_stream    << "\t\t\t{\n";
    o_stream    << "\t\t\t\tgroups.item(i).setAttribute(\"opacity\",opacity_v);\n";
    o_stream    << "\t\t\t}\n";
    o_stream    << "\t\t}\n";
    o_stream    << "\t}\n";
    o_stream    << "]]>\n";
    o_stream    << "</script>\n";


    // Compute max color
    int max_color = 0;
    for (vector<EDGE>::const_iterator it = edges_.begin(); it != edges_.end(); ++it)
    {
        max_color = max(max_color, it->color_);
        assert(max_color < 8);
    }

    o_stream    << "\n";
    // Buttons
    int start_y = 0;
    for (int i=0; i<=max_color; ++i)
    {
        o_stream    << "\t<rect class=\"class_" << i << "\" onclick=\"quad_click(evt)\" x=\"0\" y=\"" << start_y << "\" width=\"10\" height=\"10\" fill=\"" << COLOR_MAP[i] << "\" stroke=\"blue\"/>\n";
        start_y += 20;
    }
    o_stream    << "\n";

    o_stream << "\t<g stroke=\"green\" stroke-width=\"4\">\n";
    last_node_x_ = 0;
    for (vector<NODE>::iterator it = nodes_.begin(); it != nodes_.end(); ++it)
    {
        it->x_ = last_node_x_ + 40;
        last_node_x_ = it->x_;
        o_stream     << "\t\t<line x1=\"" << it->x_ << "\" y1=\"20\" x2=\"" 
                    << it->x_ << "\" y2=\"" << (vertical_size + 50) << "\"/>\n";
    }
    o_stream << "\t</g>\n";
    for (vector<NODE>::const_iterator it = nodes_.begin(); it != nodes_.end(); ++it)
    {
        xmlNodePtr node_name = xmlNewNode(NULL, BAD_CAST "text");
        xmlNewProp(node_name, BAD_CAST "font-size", BAD_CAST "3");
        xmlNewProp(node_name, BAD_CAST "y", BAD_CAST "18");
        xmlNewProp(node_name, BAD_CAST "text-anchor", BAD_CAST "middle");

        o_stream    << "\t<text font-size=\"3\" x=\"" << it->x_ << "\" y=\"18\" text-anchor=\"middle\">";
        o_stream    << it->name_;
        o_stream    << "</text>\n";
    }

    char letters[5];
    letters[0] = 'a';
    letters[1] = 'a';
    letters[2] = 'a';
    letters[3] = 'a';
    letters[4] = '\0';
    for (vector<EDGE>::const_iterator it = edges_.begin(); it != edges_.end(); ++it)
    {
        o_stream << "\t<g class=\"class_" << it->color_ << "\">\n";
        if (nodes_[node_map_[it->node_start_]].x_ < nodes_[node_map_[it->node_end_]].x_)
        {
            o_stream    << "\t<path id=\"path_" << letters << "\" d=\"M " 
                        << nodes_[node_map_[it->node_start_]].x_ << " "
                        << (cycleMap[it->cycle_start_] - first_cycle_ + 30) << " "
                        << nodes_[node_map_[it->node_end_]].x_ << " "
                        << (cycleMap[it->cycle_end_] - first_cycle_ + 30)
                        << "\"/>\n"; 
        }
        else
        {
            o_stream    << "\t<path id=\"path_" << letters <<  "\" d=\"M " 
                        << nodes_[node_map_[it->node_end_]].x_ << " "
                        << (cycleMap[it->cycle_end_] - first_cycle_ + 30) << " "
                        << nodes_[node_map_[it->node_start_]].x_ << " "
                        << (cycleMap[it->cycle_start_] - first_cycle_ + 30)
                        << "\"/>\n"; 
        }
        o_stream    << "\t<text font-family=\"Verdana\" font-size=\"4\" fill=\"blue\" text-anchor=\"start\""
                    << " x=\"" << nodes_[node_map_[it->node_start_]].x_ << "\" y=\"" 
                    << (cycleMap[it->cycle_start_] - first_cycle_ + 30) << "\">";
        o_stream    << it->cycle_start_;
        o_stream    << "</text>\n";
        o_stream    << "\t<line x1=\"" << nodes_[node_map_[it->node_start_]].x_ << "\" y1=\"" 
                    << (cycleMap[it->cycle_start_] - first_cycle_ + 30) 
                    << "\" x2=\"" << nodes_[node_map_[it->node_end_]].x_ << "\" y2=\"" 
                    << (cycleMap[it->cycle_end_] - first_cycle_ + 30) 
                    << "\" stroke-width=\"1\" stroke=\"" << COLOR_MAP[it->color_] << "\" marker-end=\"url(#Triangle)\"/>\n";
        o_stream    << "\t<text font-family=\"Verdana\" font-size=\"4\" fill=\"blue\" text-anchor=\"start\""
                    << " x=\"" << nodes_[node_map_[it->node_end_]].x_ << "\" y=\"" 
                    << (cycleMap[it->cycle_end_] - first_cycle_ + 30) << "\">";
        o_stream    << it->cycle_end_;
        o_stream    << "</text>\n";
        o_stream    << "\t<text font-family=\"Verdana\" font-size=\"3\" fill=\"blue\">\n";
        o_stream    << "\t\t<textPath xlink:href=\"#path_" << letters << "\" startOffset=\"20\%\">"
                    << it->label_ << "</textPath>\n"
                    << "\t</text>\n";

        letters[0] += 1;
        for (int i = 0; i < 4; ++i)
        {
            if (letters[i] >= 'z')
            {
                letters[i] = 'a';
                letters[i+1] += 1;
                break;
            }
        }
        o_stream << "\t</g>\n";
    }

    o_stream << "</svg>\n" << endl;
}

void
FLOW_DIAGRAM_DRAW::Load(string filename)
{
    
}

void
FLOW_DIAGRAM_DRAW::Add(FLOW_DIAGRAM_DRAW& other)
{
    // Merge nodes
    for (vector<NODE>::const_iterator it = other.nodes_.begin(); it != other.nodes_.end(); ++it)
    {
        if (find(nodes_.begin(), nodes_.end(), *it) == nodes_.end())
        {
            nodes_.push_back(*it);
            node_map_[it->id_] = nodes_.size() - 1;
        }
    }

    // Copy edges
    for (vector<EDGE>::const_iterator it = other.edges_.begin(); it != other.edges_.end(); ++it)
    {
        edges_.push_back(*it);
        edges_.back().color_ = color_;
    }

    first_cycle_ = min(first_cycle_, other.first_cycle_);
    last_cycle_ = max(last_cycle_, other.last_cycle_);

    ++color_;
}

