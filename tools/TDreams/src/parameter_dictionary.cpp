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
#include "parameter_dictionary.h"
#include <libxml/parser.h>
#include <iostream>

using namespace std;

bool 
PARAMETER_DICTIONARY::parseLookupTag(string filename)
{
    bool ok = false;
    xmlDocPtr doc;
    doc = xmlParseFile(filename.c_str());

    if (doc == NULL)
    {
        cerr << "Unable to parse " << filename << endl;
        return false;
    }

    // Root node element
    xmlNode* root = xmlDocGetRootElement(doc);

    if  (!root || !root->name || xmlStrcmp(root->name, BAD_CAST "tdreams"))
    {
        xmlFreeDoc(doc);
        return false;
    }
    xmlChar* tag = xmlGetProp(root, BAD_CAST "lookup-tag");

    if (tag != NULL)
    {
        dict_["lookup-tag"] = reinterpret_cast<const char*>(tag);
        ok = true;
    }
    else 
    {
        ok = false;
    }

    cout << "Lookup Tag: " << dict_["lookup_tag"] << endl;

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return ok;
}

void
PARAMETER_DICTIONARY::Initialize()
{
    // Read Configuration
    if (!parseLookupTag("config.xml"))
    {
        cerr << "Configuration file (config.xml) seems to be incorrect. Aborting..." << endl;
        exit(-1);
    }
}

string
PARAMETER_DICTIONARY::GetParameter(const string param_name) const throw()
{
    map<string, string>::const_iterator it_find = dict_.find(param_name);
    
    if (it_find != dict_.end()) 
    {
        return it_find->second;
    }
    else 
    {
        throw NotFoundException();
        return "NOT_FOUND";
    }
};
