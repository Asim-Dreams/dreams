// ==================================================
// Copyright (C) 2005-2006 Intel Corporation
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

/*
 * @file ADFParserInterface.h
 */

#ifndef _ADFPARSERINTEFACE_H
#define _ADFPARSERINTEFACE_H

#include "DreamsDefs.h"

// Class forwarding.
class QDomDocument;

class ADFParserInterface
{
    public:

        /**
          * Function description
          */
        ADFParserInterface(QDomDocument* ref) {myADF=ref;}
        virtual ~ADFParserInterface(){};

        /**
          * Function description
          */
        virtual bool parseADF() = 0;

    protected:
        // ref to xml dom object
        QDomDocument* myADF;
};

#endif
