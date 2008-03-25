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
#include "listener.h"
#include "selector_factory.h"
#include "commands.h"
#include "command_factory.h"
#include "command_line.h"
#include "configuration.h"
#include <libxml/parser.h>
#include <deque>
#include <asim/dralClient.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

bool fileExists(const char* const name)
{
    ifstream input;
    input.open(name);
    input.close();
    return (!input.fail());
}

vector<string>
splitLine(const string& str)
{
    vector<string> split;
    UINT32 idx = 0;
    string tmp = "";

    // Jump over initial spaces
    while (idx < str.size() && str[idx] <= 32)
        ++idx;

    while (idx < str.size())
    {
        if (str[idx] > 32)
        {
            tmp.append(1, str[idx]);
        }
        else
        {
            if (tmp.size()>0)
            {
                split.push_back(tmp);
                tmp = "";
            }
        }
        ++idx;
    }
    if (tmp!="")
    {
        split.push_back(tmp);
    }
    return split;
}

SELECTOR*
getSelector(string selector_str)
{
    return THE_SELECTOR_FACTORY::Instance().CreateObject(selector_str);
}

bool
parseSelectors(string filename)
{
    bool ok = true;

    xmlDocPtr doc;
    doc = xmlParseFile(filename.c_str());

    // Root node element
    xmlNode* root = xmlDocGetRootElement(doc);

    if  (!root || !root->name || xmlStrcmp(root->name, BAD_CAST "selector-list"))
    {
        xmlFreeDoc(doc);
        return false;
    }

    xmlNode* child_node;
    for (child_node = root->children; child_node != NULL; child_node = child_node->next)
    {
        if (child_node->type == XML_ELEMENT_NODE && !xmlStrcmp(child_node->name, BAD_CAST "selector"))
        {
            COMPOSITE_SELECTOR selector;

            if (!selector.readConfig(child_node))
            {
                xmlFreeDoc(doc);
                return false;
            }
        }
    }

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return ok;
}

void 
showUsage()
{
    cerr << "Usage: ./tdreams events_file [batch_command_file]" << endl;
}

void
showHelp()
{
    cout << "Options: \n\n";
    cout << "\t==== TEXT COMMANDS ====\n";
    cout << "\tdump SELECTOR [string] - dump information that matches selector and only stages where the name includes string\n";
    cout << "\tdumpitem item_id - dump all information of transaction that contains item identified by item_id\n";
    cout << "\tdumptran transaction_id - dump all information of transaction identified by transaction_id\n";
    cout << "\tdumptotals SELECTOR [stage_name] - dump the total lifetime of selected transactions\n";
    cout << "\tdumpworse SELECTOR #items [#stage/stage_name] - dump the #items with longer lifetime\n";
    cout << "\tdumpbest SELECTOR #items [#stage/stage_name] - dump the #items with shorter lifetime\n";
    cout << "\tdumpcloser SELECTOR ref_value #items [#stage/stage_name]- dump the #items with lifetime closer to the ref_value. If an #stage is specified count only that stage.\n"; 
    cout << "\tdumphist SELECTOR - dump stripchart of selected requests\n";
    cout << "\tdumpavg SELECTOR #stage/stage_name - dump stripchart of average time spent in #stage for selected requests\n";
//    cout << "\tdumpmean SELECTOR #stage/stage_name - dump mean time spent on the stage by selected transactions\n";
    cout << "\n";
    cout << "\t==== GRAPHIC COMMANDS ====\n";
    cout << "\tdumpsvg label_tag transaction_ids - dump an SVG of transaction identified by transaction_ids\n";
    cout << "\n";
    cout << "\t\t==== SELECTORS ====\n";
    THE_SELECTOR_FACTORY::Instance().DumpInfo("\t\t");
    cout << "\n";
    cout << "\t==== OTHER COMMANDS=====\n";
    cout << "\tset textmode [normal|simple|columns] - set output modality\n";
    cout << "\tset output filename - redirect output to file\n";
    cout << "\n";
    cout << "\tload filename - load a new events file\n";
    cout << "\n";
    cout << "\texit/quit - quit program\n";
    cout << endl;
}

deque<COMMAND*> batch_commands;

void
addCommand(COMMAND* command, LISTENER_CLASS* listener, bool batch_mode)
{
    if (!batch_mode)
    {
        command->execute(listener);
    }
    batch_commands.push_back(command);
}

bool
processInputLine(string input, LISTENER_CLASS* listener, bool batch_mode)
{
    bool input_error = true;

    // Read user requests
    vector<string> input_line;
    input_line = splitLine(input);

    if (input_line.empty())
    {
        return false;
    }
    // Parse input
    if (input_line[0] =="set")
    {
        if (input_line.size() > 1)
        {
            COMMAND* command; 
            try {
                command = THE_SET_COMMAND_FACTORY::Instance().CreateObject(input_line[1]);
            }
            catch (...) {
                // Command doesn't exist
                command = 0;
            }

            if (command)
            {
                command->readConfig(input);
                if (command->isCorrect())
                {
                    addCommand(command, listener, batch_mode);
                    input_error = false;
                }
            }
        }
    }
    else
    {
        COMMAND* command; 
        try {
            command = THE_COMMAND_FACTORY::Instance().CreateObject(input_line[0]);
        }
        catch (...) {
            // Command doesn't exist
            command = 0;
        }

        if (command)
        {
            command->readConfig(input);
            if (command->isCorrect())
            {
                addCommand(command, listener, batch_mode);
                input_error = false;
            }
        }
    }
    return input_error;
}

void
setEcho(bool active)
{
    struct termios settings;
    int result;

    result = tcgetattr(1, &settings);
    if (result < 0)
    {
        cerr << "Error setting echo\n" << endl;
        return;
    }
    settings.c_lflag &= ~ECHO;
    if (active)
        settings.c_lflag |= ECHO;
    result = tcsetattr(1, TCSANOW, &settings);
    if (result < 0)
    {
        cerr << "Error setting echo\n" << endl;
    }
}

void
setCanonical(bool active)
{
    struct termios settings;
    int result;

    result = tcgetattr(1, &settings);
    if (result < 0)
    {
        cerr << "Error setting canonical\n" << endl;
        return;
    }
    settings.c_lflag &= ~ICANON;
    if (active)
        settings.c_lflag |= ICANON;
    result = tcsetattr(1, TCSANOW, &settings);
    if (result < 0)
    {
        cerr << "Error setting canonical\n" << endl;
    }
}

namespace {
    string lookup_tag;
    string default_textmode;
    string parse_moveitems_str;
    string parse_tag_events_str;
    string enable_color_str;
    string filter_zero_length_events_str;
}

int 
main(int argc, char** argv)
{
    // Check the parameter number was correct
    if (argc<2)
    {
        // We need at least the events file
        showUsage();
        exit(-1);
    }

    const char *home = getenv("HOME");
    if (!home) {
        cerr <<  "Couldn't getenv HOME" << endl; 
        exit(-1);
    }

    string homeFileName = string(home) + "/.asim/config.xml"; 

    if (fileExists("config.xml"))
    {
        if (!THE_CONFIGURATION_DB::Instance().parseXML("config.xml"))
        {
            cerr << "Configuration file (config.xml) seems to be incorrect. Aborting..." << endl;
            exit(-1);
        }
    }
    else if (fileExists(homeFileName.c_str()))
    {
        if (!THE_CONFIGURATION_DB::Instance().parseXML(homeFileName.c_str()))
        {
            cerr << "Configuration file (" << homeFileName << ") seems to be incorrect. Aborting..." << endl;
            exit(-1);
        }
    }
     else
    {
        cerr << "A configuration file (config.xml) is required in current directory. Aborting..." << endl;
        exit(-1);
    }


    lookup_tag = THE_CONFIGURATION_DB::Instance().getParameter("lookup-tag");

    if (lookup_tag == "")
    {
        cerr << "Invalid lookup tag. Aborting..." << endl;
        exit(-1);
    }
    else
    {
        DEBUG(cerr << "Using " << lookup_tag << " as lookup tag." << endl);
    }
    

    // Parse selectors requires the default ones to be registered
    RegisterDefaultSelectors();

    homeFileName = string(home) + "/.asim/selectors.xml"; 

    if (fileExists("selectors.xml"))
    {
        if (!parseSelectors("selectors.xml"))
        {
            cerr << "Selectors file (selectors.xml) seems to be incorrect. Aborting..." << endl;
            exit(-1);
        }
    }
    else if (fileExists(homeFileName.c_str()))
    {
        if (!parseSelectors(homeFileName.c_str()))
        {
            cerr << "Selectors file (selectors.xml) seems to be incorrect. Aborting..." << endl;
            exit(-1);
        }
    }
    else
    {
        cerr << "Selectors file (selectors.xml) was not found in current directory or in ~/.asim/. Proceeding without it..." << endl;
        sleep(4);
    }

    // Create DRAL listener
    LISTENER_CLASS listener(lookup_tag);

    default_textmode = THE_CONFIGURATION_DB::Instance().getParameter("default-textmode");
    if (default_textmode != "")
    {
        COMMAND* textmode_command = THE_SET_COMMAND_FACTORY::Instance().CreateObject(string("textmode"));
        textmode_command->readConfig(string("set textmode ")+default_textmode);
        textmode_command->execute(&listener);
    }

    parse_moveitems_str = THE_CONFIGURATION_DB::Instance().getParameter("parse-moveitems");
    if (parse_moveitems_str == "true")
    {
        LISTENER_CLASS::parse_moveitems = true;
    }
    else
    {
        LISTENER_CLASS::parse_moveitems = false;
    }

    enable_color_str = THE_CONFIGURATION_DB::Instance().getParameter("enable-color");
    if (enable_color_str == "true")
    {
        LIFETIME_CLASS::enable_color = true;
    }
    else
    {
        LIFETIME_CLASS::enable_color = false;
    }

    parse_tag_events_str = THE_CONFIGURATION_DB::Instance().getParameter("parse-tag-events");
    if (parse_tag_events_str == "true")
    {
        LISTENER_CLASS::parse_tag_events = true;
    }
    else
    {
        LISTENER_CLASS::parse_tag_events = false;
    }

    filter_zero_length_events_str = THE_CONFIGURATION_DB::Instance().getParameter("filter-zero-length-events");
    if (filter_zero_length_events_str == "false")
    {
        LISTENER_CLASS::filter_zero_length_events = false;
    }
    else
    {
        LISTENER_CLASS::filter_zero_length_events = true;
    }



    string events_file = argv[1];

    int fd_trace = open(events_file.c_str(), O_RDONLY);

    if (fd_trace == -1)
    {
        cerr << "Error opening events file " << events_file << endl;
        showUsage();
        exit(-1);
    }

    DRAL_LISTENER_CONVERTER_CLASS converter(&listener);
    DRAL_CLIENT_CLASS client(fd_trace, &converter, 1024*64);

    istream* i_stream_ptr = 0;
    bool batch_mode = false;
    bool cl_mode = false;

    if (argc > 2)
    {
        // Take second param as batch command file
        i_stream_ptr = new ifstream(argv[2]);
        if (!i_stream_ptr->good())
        {
            //cerr << "Error opening batch commands file " << argv[2] << endl;
            //showUsage();
            //exit(-1);
            cl_mode = true;
        } else {
            batch_mode = true;
        }
        listener.setSilentMode(true);
    }
    istream& i_stream = (i_stream_ptr ? *i_stream_ptr : cin);

    bool exit_program = false;

    // In batch mode we want to parse the batch comands file before reading
    // the events, so that if it contains errors we can correct them.
    // That's a good thing cause reading events can take pretty long.
    if (batch_mode)
    {
        // Parse commands
        while (!exit_program)
        {
            string input; 
            getline(i_stream, input);
            bool input_error = processInputLine(input, &listener, batch_mode);

            if (input_error)
            {
                std::cerr << "Error in batch file: " << std::endl;
                std::cerr << "LINE: " << input << std::endl;
                exit(-1);
            }
            if (i_stream.eof())
            {
                // FIXME: This is as counter-intuitive as it can be
                exit_program = true;
            }
        }
    }

    if(cl_mode)
    {
        string input = argv[2];  // Should be argv[2]
        bool input_error = processInputLine(input, &listener, true);

        if (input_error)
        {
            std::cerr << "Error in command line: " << std::endl;
            std::cerr << "LINE: " << input << std::endl;
            exit(-1);
        }
        exit_program = true;
    }

    // Read DRAL events
    int result;
    while ((result = client.ProcessNextEvent(true, 1))>0)
    {}

    close(fd_trace);

    if (result == -1)
    {
        cout << "An error occurred" << endl;
    }

    if(LISTENER_CLASS::filter_zero_length_events) 
    {
        listener.filterEmptyTransactions();
    }

    if (batch_mode || cl_mode)
    {
        // Execute stored commands
        for (deque<COMMAND*>::const_iterator it = batch_commands.begin(); it != batch_commands.end(); ++it)
        {
            (*it)->execute(&listener);
        }
    }

    while (!exit_program)
    {
        string input; 

        COMMAND_LINE_PARSER& parser = THE_COMMAND_LINE_PARSER::Instance();
        input = parser.readLine();
        cout << "You wrote " << input << endl;
        bool input_error = processInputLine(input, &listener, batch_mode);

        if (input_error)
        {
            showHelp();
        }
    }
    return (0);
}

