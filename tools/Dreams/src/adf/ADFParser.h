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
* @file ADFParser.h
*/

#ifndef _ADFPARSER_H
#define _ADFPARSER_H

// General includes.
#include <iostream>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/aux/AEVector.h"
#include "dDB/DRALTag.h"
#include "ADFParserInterface.h"
#include "ADFMap.h"
#include "expressions/Expression.h"
#include "DreamsDefs.h"

// Qt includes.
#include <qdom.h>

// Class forwarding.
class DralDB;
class LogMgr;
class DreamsDB;
class Expression;
class TabSpec;
class LayoutFloorplan;
class LayoutFloorplanTable;
class LayoutFloorplanRow;
class LayoutFloorplanColumn;
class LayoutFloorplanRectangle;
class WatchWindow;


/* @typdef TagIdColorVector
 * @brief
 * Dynamic array that holds the number of visual elements (Resource rows and Floorplan
 * rectangles that references a given trakId)
 */
class TagIdColorVector : public AEVector<UINT32, 1024, 256>
{
    protected:
        /*
         * Allocates the segment number segment.
         *
         * @return void.
         */
        inline void
        allocateSegment(UINT32 segment)
        {
            segvector[segment] = new AEVectorNode<UINT32>(m2SegmentSize,segment);
            Q_ASSERT(segvector[segment] != NULL);
            ++numSegments;

            // Marks all the entries with the first color.
            for(UINT32 i = 0; i < m2SegmentSize; i++)
            {
                segvector[segment]->array[i] = 1;
            }
        }
} ;

/*
 * This class holds DRAL Graph and configuration information.
 *
 * @author Guillem Sole
 * @date started at 2005-01-14
 * @version 0.1
 */
class ADFParser : public ADFParserInterface
{
    public:
        ADFParser(DreamsDB * _dreamsdb, LogMgr * logmgr, QDomDocument * ref);
        ~ADFParser();

        bool parseADF();

    private:
        bool processDomNode(QDomNode node);
        TAG_ID getTagIdColor(TRACK_ID trackId);
        bool somethingDefined() const;

        bool processTagDescriptorStatement(QDomNode node);

        bool processShadeTableStatement(QDomNode node);
        bool processShadeSubTags(QDomNode node, ADFMapEntrySet & entries, bool & hasZero, bool & has100);

        bool processSetEdgeBandwidthStatement(QDomNode node);

        bool processItemWindowStatement(QDomNode node);
        bool processItemWindowSubTags(QDomNode node);
        bool processTabStatement(QDomNode node);
        bool processTabSubTags(QDomNode node, TabSpec * tab);
        bool processShowStatement(QDomNode node, TabSpec * tab);

        bool processRuleStatement(QDomNode node, ExpressionList & rules, UINT16 expression_type);
        bool processRuleEnvStatement(QDomNode node, ExpressionList & lRules, UINT16 expression_type);
        bool checkRules(ExpressionList & expr_list, UINT16 type) const;

        bool processResourceStatement(QDomNode node);
        bool processResourceDomNode(QDomNode node);
        bool processResourceGroupStatement(QDomNode node);
        bool processResourceEdgeStatement(QDomNode node);
        bool processResourceNodeStatement(QDomNode node);
        bool processResourcePhaseStatement(QDomNode node);
        bool processResourceEnterNodeStatement(QDomNode node);
        bool processResourceExitNodeStatement(QDomNode node);
        bool processResourceBlankStatement(QDomNode node);

        bool processFloorplanStatement(QDomNode node);
        bool processFloorplanTableStatement(LayoutFloorplanTable * table, QDomNode node, INT32 level);
        bool processFloorplanTRStatement(LayoutFloorplanRow * row, QDomNode node, INT32 level);
        bool processFloorplanTDStatement(LayoutFloorplanColumn * column, QDomNode node, INT32 level);
        bool processFloorplanRectangleStatement(LayoutFloorplanRectangle * rectangle, QDomNode node, INT32 level);
        bool processFloorplanRectangleNode(LayoutFloorplanRectangle * rectangle, QString sRectangleNode, QDomElement elem);
        bool processFloorplanRectangleEdge(LayoutFloorplanRectangle * rectangle, QString sRectangleEdge, QDomElement elem);
        bool processFloorplanLayoutStatement(LayoutFloorplanRectangle * rectangle, QDomNode node);

        bool processDreamsWFStatement(QDomNode node);
        bool processDreamsWFMoveItemStatement(QDomNode node);
        bool processDreamsWFEnterNodeStatement(QDomNode node);
        bool processDreamsWFExitNodeStatement(QDomNode node);

        bool processDreamsWWStatement(QDomNode node);
        bool processDreamsWWNode(WatchWindow * window, QString sWatchWindowNode, QDomElement elem);
        bool processDreamsWWEdge(WatchWindow * window, QString sWatchWindowEdge, QDomElement elem);
        bool processDreamsWWShowStatement(QDomNode node, WatchWindow * window);
        bool processDreamsWWPointerStatement(QDomNode node, WatchWindow * window);

    private:
        DralDB *   draldb;    ///< Pointer to the dral database of the trace.
        LogMgr *   myLogMgr;  ///< Pointer to the log manager.
        DreamsDB * dreamsdb;  ///< Pointer to the trace.

        bool itemWindowDefined; ///< Is ItemWindow already defined.
        bool dreamsRSefined;  ///< Is Resource already defined.
        bool dreamsFPefined;  ///< Is Floorplan already defined.
        bool dreamsWWDefined; ///< Is WatchWindows already defined.
        bool dreamsWFDefined; ///< Is WaterFall already defined.

        ExpressionList createdRules; ///< List of created rules that will be erased later.

        /*
         * AEVector that holds the number of visualization elements that are
         * defined for a given trackId.  This number is used to created the name
         * of the tag that will hold the color/shape/letter tag in the DralDB.
         * Once computed the name of the tag, the tagId will be obtained directly
         * from the DralDB
         */
        TagIdColorVector tagIdColorVector;
        TAG_ID assignedTagIdColorVector[256];
        TAG_ID maxColorTagId;
} ;

#endif
