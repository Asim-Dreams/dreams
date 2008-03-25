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
#ifndef SVG_DRAWER_H
#define SVG_DRAWER_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

class FLOW_DIAGRAM_DRAW
{
    struct NODE
    {
        bool operator==(const NODE& other)
        {
            return (id_ == other.id_);
        }

        int id_;
        std::string name_;

        int x_;
    };

    struct EDGE
    {
        int node_start_;
        unsigned int cycle_start_;
        int node_end_;
        unsigned int cycle_end_;
        std::string label_;
        int color_;
    };

  public:
    FLOW_DIAGRAM_DRAW();
    ~FLOW_DIAGRAM_DRAW();

    void AddNode(int id, std::string name);
    void AddEdge(int node_start, unsigned int cycle_start, int node_end, unsigned int cycle_end, std::string label);

    void Dump(std::ostream& o_stream);
    void Load(std::string filename);
    void Add(FLOW_DIAGRAM_DRAW& other);

  private:
    std::vector<NODE> nodes_;
    std::vector<EDGE> edges_;
    std::map<int, int> node_map_;
    int last_node_x_;
    unsigned int first_cycle_;
    unsigned int last_cycle_;
    int color_;
};

#endif // SVG_DRAWER_H
