// ==================================================
// Copyright (C) 2004-2006 Intel Corporation
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 

/**
  * @file DumpMode.h
  */

#ifndef _DUMP_MODE_H
#define _DUMP_MODE_H

#include "asim/DralDB.h"

// 2Dreams
#include "DreamsDB.h"
#include "ADFParserBuilder.h"
#include "TwoDAvtView.h"
#include "ADFMgr.h"
#include "2Dreams.h"

class DumpMode
{
    public:
        DumpMode();
        ~DumpMode();

        void start(DreamsLineParams params);

    private:
        void print_usage();

        DralDB * draldb;             ///< DralDB instance.
        DreamsDB * dreamsdb;         ///< DreamsDB instance.
        ADFMgr * adfmgr;             ///< ADFMgr instance.
        ADFParserInterface * parser; ///< ADF parser instance.
        PreferenceMgr * prefMgr;     ///< Preference manager instance.
        ZDBase * zdb;                ///< ZDBase instance.
        LiveDB * livedb;             ///< LiveDB instance.
} ;

#endif
