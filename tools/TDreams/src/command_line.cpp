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
#include "command_line.h"
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

namespace {
    char* const PROMPT="lifetime> ";
}

COMMAND_LINE_PARSER::COMMAND_LINE_PARSER()
{
    using_history();
}

COMMAND_LINE_PARSER::~COMMAND_LINE_PARSER()
{
    clear_history();
}

string
COMMAND_LINE_PARSER::readLine()
{
    string line = "";
    char* line_char = readline(PROMPT);
    line = line_char;
    if (line != "")
    {
        add_history(line_char);
    }
    free(line_char);
    
    return line;
}

