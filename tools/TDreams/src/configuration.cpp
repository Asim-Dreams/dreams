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

#include "configuration.h"
#include <iostream>
#include <libxml/parser.h>

using namespace std; 

bool
CONFIGURATION_DB::parseXML(string filename)
{
    bool ok = false;

    xmlDocPtr doc;
    doc = xmlParseFile(filename.c_str());

    // Root node element
    xmlNode* root = xmlDocGetRootElement(doc);

    if  (!root || !root->name || xmlStrcmp(root->name, BAD_CAST "tdreams"))
    {
        xmlFreeDoc(doc);
        return false;
    }

    xmlNode* child_node;
    for (child_node = root->children; child_node != NULL; child_node = child_node->next)
    {
        if (child_node->type == XML_ELEMENT_NODE && !xmlStrcmp(child_node->name, BAD_CAST "parameter"))
        {
            // Parse parameter
            xmlChar* name = xmlGetProp(child_node, BAD_CAST "name");
            xmlChar* value = xmlGetProp(child_node, BAD_CAST "value");

            parameters_[reinterpret_cast<const char*>(name)]=reinterpret_cast<const char*>(value);

        }
        else if (child_node->type == XML_ELEMENT_NODE && !xmlStrcmp(child_node->name, BAD_CAST "ignore"))
        {
            // Parse ignore
            xmlChar* node = xmlGetProp(child_node, BAD_CAST "node");
            DEBUG(cout << "Ignoring node " << BAD_CAST(node) << endl); 
            ignoredNodes_.push_back(reinterpret_cast<const char*>(node));
        }
        else if (child_node->type == XML_ELEMENT_NODE && !xmlStrcmp(child_node->name, BAD_CAST "showtag"))
        {
            // Parse showtag
            xmlChar* name = xmlGetProp(child_node, BAD_CAST "name");
            DEBUG(cout << "Showing tag " << reinterpret_cast<const char*>(name) << endl); 
            shownTags_.push_back(reinterpret_cast<const char*>(name));
        }
    }
    ok = true;

    return ok;
}

string
CONFIGURATION_DB::getParameter(string name) const
{
    map<string, string>::const_iterator it = parameters_.find(name);
    
    if (it != parameters_.end())
    {
        return it->second;
    }
    else
    {
        return "";
    }
}

bool 
CONFIGURATION_DB::isIgnored(string name) const
{
    std::deque<std::string>::const_iterator it = find(ignoredNodes_.begin(), ignoredNodes_.end(), name);

    return it != ignoredNodes_.end();

}
