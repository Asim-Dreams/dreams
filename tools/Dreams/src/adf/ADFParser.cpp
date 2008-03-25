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
 * @file ADFParser.cpp
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "ADFParser.h"
#include "ADFDefinitions.h"
#include "DreamsDB.h"
#include "ADFMap.h"
#include "ADFMapColorInterpolate.h"
#include "expressions/Expressions.h"
#include "adf/parserCtrl.h"
#include "layout/floorplan/LayoutFloorplan.h"
#include "layout/floorplan/LayoutFloorplanColumn.h"
#include "layout/floorplan/LayoutFloorplanNode.h"
#include "layout/floorplan/LayoutFloorplanRectangle.h"
#include "layout/floorplan/LayoutFloorplanRow.h"
#include "layout/floorplan/LayoutFloorplanTable.h"
#include "WatchWindow.h"

// Defines the type of expression being parsed.
#define EXPRESSION_ITEM   (UINT16) 0
#define EXPRESSION_NORMAL (UINT16) 1

/*
 * Creator of the class. Just set some variables.
 *
 * @return the new object.
 */
ADFParser::ADFParser(DreamsDB * _dreamsdb, LogMgr * logmgr, QDomDocument * ref)
    : ADFParserInterface(ref)
{
    myLogMgr = logmgr;
    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();

    itemWindowDefined = false;
    dreamsRSefined = false;
    dreamsFPefined = false;
    dreamsWWDefined = false;
    dreamsWFDefined = false;

    maxColorTagId = 1;
}

/*
 * Destructor of the class.
 *
 * @return destroys the object.
 */
ADFParser::~ADFParser()
{
    ExpressionIterator it_rules(createdRules);
    Expression * rule;

    // Deletes all the rules.
    while((rule = it_rules.current()) != NULL)
    {
        ++it_rules;
        delete rule;
    }
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Main Methods /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*
 * This function parses all the elements of the ADF.
 *
 * @return if the parse is ok.
 */
bool
ADFParser::parseADF()
{
    Q_ASSERT(myADF != NULL);
    QDomElement docElem = myADF->documentElement();
    QDomNode n = docElem.firstChild();
    bool ok = true;

    // Parses all the childs of the document element.
    while(!n.isNull() && ok)
    {
        ok = processDomNode(n);
        n = n.nextSibling();
    }

    // If we have defined a waterfall view, we must have also defined an item
    // window.
    //
    if(!itemWindowDefined && dreamsWFDefined)
    {
        myLogMgr->addLog("ADF Error: waterfall tag defined without defining an itemwindow.");
        ok = false;
    }

    return ok;
}

/*
 * Parses a main ADF sentence.
 *
 * @param node the node containing the XML content to parse.
 *
 * @return if the parse is ok.
 */
bool
ADFParser::processDomNode(QDomNode node)
{
    // Node type checking.
    // Try to convert the node to an element.
    QDomElement e = node.toElement(); 
    if(e.isNull())
    {
        // Comments or similar stuff.
        return true;
    }

    QString tagname = e.tagName();

    // check for known tags
    //cerr << "ADFParser::processDomNode: parsing tagName = " << tagname << endl;
    if(tagname == ADF_DREAMS)
    {
        return true;
    }
    else if(tagname == ADF_TAGDESC)
    {
        if(somethingDefined())
        {
            myLogMgr->addLog("ADF Error: defining a new tag descriptor after defining a layout.");
            myLogMgr->addLog("\tPlease, set all the tags at the begining of the ADF.");
            return false;
        }
        return processTagDescriptorStatement(node);
    }
    else if(tagname == ADF_SHADETABLE)
    {
        if(somethingDefined())
        {
            myLogMgr->addLog("ADF Error: defining a new shade table  after defining a layout.");
            myLogMgr->addLog("\tPlease, set all the shade tables at the begining of the ADF.");
            return false;
        }
        return processShadeTableStatement(node);
    }
    else if(tagname == ADF_SETEDGEBW)
    {
        return processSetEdgeBandwidthStatement(node);
    }
    else if(tagname == ADF_ITEMWINDOW)
    {
        if(somethingDefined())
        {
            myLogMgr->addLog("ADF Error: defining a new item window after defining a layout.");
            myLogMgr->addLog("\tPlease, set all the item windows at the begining of the ADF.");
            return false;
        }
        else if(itemWindowDefined)
        {
            myLogMgr->addLog("ADF Error: itemwindow tag defined twice.");
            return false;
        }

        itemWindowDefined = true;
        return processItemWindowStatement(node);
    }
    else if(tagname == ADF_DREAMS2)
    {
        if(dreamsRSefined)
        {
            myLogMgr->addLog("ADF Error: dreams2 tag defined twice.");
            return false;
        }
        else
        {
            dreamsRSefined = true;
            return processResourceStatement(node);
        }
    }
    else if(tagname == ADF_DREAMS3)
    {
        if(dreamsFPefined)
        {
            myLogMgr->addLog("ADF Error: dreams3 tag defined twice.");
            return false;
        }
        else
        {
            dreamsFPefined = true;
            return processFloorplanStatement(node);
        }
    }
    else if(tagname == ADF_WATCHWINDOW)
    {
        dreamsWWDefined = true;
        return processDreamsWWStatement(node);
    }
    else if(tagname == ADF_WATERFALL)
    {
        if(dreamsWFDefined)
        {
            myLogMgr->addLog("ADF Error: waterfall tag defined twice.");
            return false;
        }

        dreamsWFDefined = true;
        return processDreamsWFStatement(node);
    }
    else
    {
        myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " in ADF.");
        return false;
    }
}

/*
 * Returns a new tag id for the color.
 *
 * @param trackId is the track id of the node.
 *
 * @return the tag id for the track.
 */
TAG_ID
ADFParser::getTagIdColor(TRACK_ID trackId)
{
    // We ask for the number of elements that show this trackId We blindly
    // create a new tag even if the new row is exacly the same as the previous
    UINT32 nViewElems = tagIdColorVector[trackId]++;
    Q_ASSERT(nViewElems < 256);

    // Faster checking if we already have the translation.
    if(nViewElems < maxColorTagId)
    {
        return assignedTagIdColorVector[nViewElems];
    }
    // We still don't have the translation, so we create a new tag.
    else
    {
        QString tagColorName = QString("__color") + QString::number(nViewElems);
        TAG_ID tagIdColor;
        bool ok;

        ok = draldb->newTagDescriptor(tagColorName, TAG_INTEGER_TYPE, TAG_BASE_DECIMAL, TAG_SIZE_8_BITS, true, &tagIdColor);
        Q_ASSERT(ok);

        // Adds the new color tag id to the assigned tags.
        assignedTagIdColorVector[nViewElems] = (UINT32) tagIdColor;
        maxColorTagId++;
        return tagIdColor;
    }
}

/*
 * Returns if a layout has already been defined.
 *
 * @return if something is defined.
 */
bool
ADFParser::somethingDefined() const
{
    return (dreamsRSefined || dreamsFPefined || dreamsWWDefined || dreamsWFDefined);
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Tag Methods //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*
 * This function parses a tag descriptor statement.
 *
 * @param node contains the parsing information.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processTagDescriptorStatement(QDomNode node)
{
    QDomElement e;

    // Try to convert the node to an element.
    e = node.toElement(); 
    Q_ASSERT(!e.isNull());

    // Look for attributes.
    // 1) tagname:
    QString tgnamestr = e.attribute(ADF_TAGNAME_ATTRIB).stripWhiteSpace();
    if(tgnamestr == QString::null)
    {
        myLogMgr->addLog("Invalid Tag Descriptor in ADF, no tag name.");
        return false;
    }

    // 2) value type:
    QString typestr = e.attribute(ADF_TYPE_ATTRIB).stripWhiteSpace();
    if(typestr == QString::null)
    {
        myLogMgr->addLog("Invalid Tag Descriptor in ADF, no value type.");
        return false;
    }

    // Check that value type is one of the well known ones.
    TagValueType type;
    bool tvtok = TagValueToolkit::parseValueType(typestr.latin1(), &type);

    if(!tvtok)
    {
        QString err = "Invalid Tag Descriptor, unknown value type: " + typestr + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    // 3) base (default 10):
    QString basestr = e.attribute(ADF_BASE_ATTRIB).stripWhiteSpace();
    TagValueBase base = TAG_BASE_DECIMAL;

    if(basestr != QString::null)
    {
        if(!TagValueToolkit::parseValueBase(basestr, &base)
)        {
            myLogMgr->addLog ("Invalid Tag Descriptor in ADF, invalid base specification.");
            return false;
        }
    }

    // 4) flush
    bool flush = false;
    QString flushstr = e.attribute(ADF_AUTOFLUSH_ATTRIB).stripWhiteSpace();
    if(flushstr != QString::null)
    {
        if(flushstr == "true")
        {
            flush = true;
        }
        else if(flushstr != "false")
        {
            myLogMgr->addLog("Invalid Tag Descriptor in ADF, invalid autoflush specification.");
            return false;
        }
    }

    TAG_ID id;

    // Finally adds the new tag description.
    if(!draldb->newTagDescriptor(tgnamestr, type, base, TAG_SIZE_64_BITS, !flush, &id))
    {
        myLogMgr->addLog("Tag " + tgnamestr + " is repeated in the ADF.");
        return false;
    }

    // 5) description
    QString descstr = e.attribute(ADF_DESCRIPTION_ATTRIB).stripWhiteSpace();
    if(descstr != QString::null)
    {
        draldb->setTagDesc(id, descstr);
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// Shade Table Methods //////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*
 * This function parses a shade table statement.
 *
 * @param node contains the parsing information.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processShadeTableStatement(QDomNode node)
{
    ADFMapColorInterpolate * shade;
    QDomElement e;

    // Try to convert the node to an element.
    e = node.toElement();
    Q_ASSERT(!e.isNull());

    // Look for the name attribute.
    QString namestr = e.attribute(ADF_NAME_ATTRIB).stripWhiteSpace();
    if(namestr == QString::null)
    {
        myLogMgr->addLog("Invalid Shading Table in ADF, missing name attribute.");
        return false;
    }

    shade = new ADFMapColorInterpolate(namestr);

    QDomNode cnode = e.firstChild();
    bool go_on;
    bool hasZero;
    bool has100;
    INT32 last_value;

    last_value = 101;
    go_on = true;
    hasZero = false;
    has100 = false;

    ADFMapEntrySet entries;

    // Iterate through the <shade> subtags.
    while(!cnode.isNull() && go_on)
    {
        go_on = processShadeSubTags(cnode, entries, hasZero, has100);
        cnode = cnode.nextSibling();
    }

    if(!go_on)
    {
        delete shade;
        return false;
    }

    if(!hasZero)
    {
        myLogMgr->addLog("Invalid ShadeTable Tag in ADF, missing 0% sample.");
        delete shade;
        return false;
    }

    if(!has100)
    {
        myLogMgr->addLog("Invalid ShadeTable Tag in ADF, missing 100% sample.");
        delete shade;
        return false;
    }

    shade->createMap(&entries);

    dreamsdb->addMapTable(namestr, shade);

    return true;
}

/*
 * This function parses a shade sub tag statement.
 *
 * @param node contains the parsing information.
 * @param entries is a pointer to the entries of the map.
 * @param hasZero is a reference that indicates if the zero value has been given.
 * @param has100 is a reference that indicates if the hundred value has been given.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processShadeSubTags(QDomNode node, ADFMapEntrySet & entries, bool & hasZero, bool & has100)
{
    QDomElement e;

    // Try to convert the node to an element.
    e = node.toElement();

    // Comments or similar stuff.
    if(e.isNull())
    {
        return true;
    }

    // Look for the valid attributes.
    // 1) color:
    QString colorstr = e.attribute(ADF_COLOR_ATTRIB).stripWhiteSpace();
    if(colorstr == QString::null)
    {
        myLogMgr->addLog("Invalid Shade Tag in ADF, missing color attribute.");
        return false;
    }

    QColor coloratr = QColor(colorstr);
    if(!coloratr.isValid())
    {
        QString err = "Invalid color declaration: " + colorstr + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    // 2) occupancy:
    QString occstr = e.attribute(ADF_OCCUPANCY_ATTRIB).stripWhiteSpace().lower();
    if(occstr == QString::null)
    {
        myLogMgr->addLog("Invalid Shade Tag in ADF, missing occupancy attribute.");
        return false;
    }
    bool occok = false;

    // Strip the '%'.
    occstr.truncate(occstr.length() - 1);
    INT32 value = (INT32) occstr.toInt(&occok);
    if(!occok || (value < 0) || (value > 100))
    {
        myLogMgr->addLog("Invalid Shade Tag in ADF, malformed occupancy.");
        return false;
    }

    ADFMapEntry entry;

    entry.key = value;
    entry.value = static_cast<UINT64>(coloratr.rgb());

    // Tries to insert the new entry. A false means that the value is repeated.
    if(!entries.insert(entry).second)
    {
        myLogMgr->addLog("Invalid Shade Tag in ADF, repeated occupancy declarations.");
        return false;
    }

    // Updates the state.
    hasZero = hasZero || (value == 0);
    has100 = has100 || (value == 100);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////// Item Tab Methods ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*
 * This function parses a set edge bandiwdht.
 *
 * @param node contains the parsing information.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processSetEdgeBandwidthStatement(QDomNode node)
{
    QDomElement e;

    // Try to convert the node to an element.
    e = node.toElement();
    Q_ASSERT(!e.isNull());

    // 1) name attribute:
    QString tgnamestr = e.attribute(ADF_NAME_ATTRIB).stripWhiteSpace();
    if(tgnamestr == QString::null)
    {
        myLogMgr->addLog("Invalid set edge bandwidth statement in ADF, missing edge name.");
        return false;
    }

    QString fromstr = e.attribute(ADF_FROM_ATTRIB).stripWhiteSpace();
    QString tostr = e.attribute(ADF_TO_ATTRIB).stripWhiteSpace();

    DralGraphEdge * dbedge = NULL;

    // 2) from attribute:
    if(fromstr == QString::null)
    {
        myLogMgr->addLog("Invalid set edge bandwidth statement in ADF, missing 'from' attribute.");
        return false;
    }

    fromstr = draldb->normalizeNodeName(fromstr);

    // 3) to attribute:
    if(tostr == QString::null)
    {
        myLogMgr->addLog("Invalid set edge bandwidth statement in ADF, missing 'to' attribute.");
        return false;
    }

    tostr = draldb->normalizeNodeName(tostr);

    // EDGE checking.
    DralGraphNode * fromNode = draldb->getNode(fromstr);
    DralGraphNode * toNode = draldb->getNode(tostr);

    if(fromNode == NULL)
    {
        QString err = "Invalid set edge bandwidth statement, wrong source node: " + fromstr + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    if(toNode == NULL)
    {
        QString err = "Invalid set edge bandwidth statement, wrong destination node: " + tostr + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    dbedge = draldb->findEdgeByNameFromTo(tgnamestr, fromstr, tostr);
    if(dbedge == NULL)
    {
        QString err = "Invalid set edge bandwidth statement, edge not found in DRAL Graph. Source node = " + fromstr + ", destination node = " + tostr + ", edge name = " + tgnamestr + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    QString bwstr = e.attribute(ADF_BW_ATTRIB).stripWhiteSpace();
    if(bwstr == QString::null)
    {
        QString err = "Invalid set edge bandwidth statement, bandwidth not specified (bw = \"bandwidth\"";
        myLogMgr->addLog(err);
        return false;
    }

    if(!dbedge->setBandwidth(atoi(bwstr.ascii())))
    {
        QString err = "Invalid set edge bandwidth statement, bandwidth already set for edge" + tgnamestr;
        myLogMgr->addLog(err);
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////// Item Tab Methods ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*
 * This function parses an item window statement.
 *
 * @param node contains the parsing information.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processItemWindowStatement(QDomNode node)
{
    QDomElement e;

    // Try to convert the node to an element.
    e = node.toElement();
    Q_ASSERT(!e.isNull());

    // Now iterate through the if/set statements.
    QDomNode cnode = e.firstChild();
    bool go_on = true;

    while(!cnode.isNull() && go_on)
    {
        // Check if this node has a tag.
        go_on = processItemWindowSubTags(cnode);
        cnode = cnode.nextSibling();
    }
    //cerr << "ADFParser::processItemWindowStatement finished" << endl;
    return go_on;
}

/*
 * This function parses an item window statement.
 *
 * @param node contains the parsing information.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processItemWindowSubTags(QDomNode node)
{
    QDomElement e;

    // Try to convert the node to an element.
    e = node.toElement(); 
    if(e.isNull())
    {
        return true;
    }

    // Comments or similar stuff.
    if(e.isNull())
    {
        return true;
    }

    QString tagname = e.tagName();

    // check for valid tags
    if(tagname == ADF_RULE)
    {
        ExpressionList expr_list;

        if(processRuleStatement(node, expr_list, EXPRESSION_ITEM))
        {
            // Add all the rules to all the tabs.
            dreamsdb->addItemWinGlobalRule(expr_list);
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(tagname == ADF_TAB)
    {
        return processTabStatement(node);
    }
    else
    {
        myLogMgr->addLog("Invalid ItemWindow Subtag " + tagname + " in ADF.");
        return false;
    }
}

/*
 * This function parses a tab statement.
 *
 * @param node contains the parsing information.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processTabStatement(QDomNode node)
{
    //cerr << "ADFParser::processTabStatement called" << endl;
    QDomElement e;

    // Try to convert the node to an element.
    e = node.toElement(); 
    Q_ASSERT(!e.isNull());

    // 1) Name attribute:
    QString namestr = e.attribute(ADF_NAME_ATTRIB).stripWhiteSpace();

    if(namestr == QString::null)
    {
        myLogMgr->addLog("Invalid tab statement in ADF, missing tab name.");
        return false;
    }

    // 2) Withtag attribute:
    QString withtagstr = e.attribute(ADF_WITHTAG_ATTRIB).stripWhiteSpace();
    TAG_ID withtagid;
    if(withtagstr != QString::null)
    {
        // get the identifier for the given Tag String
        withtagid = draldb->getTagId(withtagstr);
        if(withtagid == TAG_ID_INVALID)
        {
            QString err = "Invalid DRAL-TAG reference: " + withtagstr + " in ADF.";
            myLogMgr->addLog(err);
            return false;
        }
    }

    // 3) Select attribute:
    QString selectstr = e.attribute(ADF_SELECT_ATTRIB).stripWhiteSpace();
    TAG_ID selectid;
    TagValueType selecttgtype = TAG_SOV_TYPE;

    if(selectstr != QString::null)
    {
        // get the identifier for the given Tag String
        selectid = draldb->getTagId(selectstr);
        if(selectid == TAG_ID_INVALID)
        {
            QString err = "Invalid DRAL-TAG reference: " + selectstr + " in ADF.";
            myLogMgr->addLog(err);
            return false;
        }
        else
        {
            selecttgtype = draldb->getTagValueType(selectid);
        }
    }

    // 4) Value attribute:
    QString valuestr = e.attribute(ADF_VALUE_ATTRIB).stripWhiteSpace();

    // 5) Splitby attribute:
    QString splitbystr = e.attribute(ADF_SPLITBY_ATTRIB).stripWhiteSpace();
    TAG_ID splitbyid;
    
    if(splitbystr != QString::null)
    {
        // Get the identifier for the given Tag String.
        splitbyid = draldb->getTagId(splitbystr);
        if(splitbyid == TAG_ID_INVALID)
        {
            QString err = "Invalid DRAL-TAG reference: " + splitbystr + " in ADF.";
            myLogMgr->addLog(err);
            return false;
        }
    }

    // 6) Order by attribute:
    QString orderbystr = e.attribute(ADF_ORDERBY_ATTRIB).stripWhiteSpace();
    TAG_ID orderbyid;

    if(orderbystr != QString::null)
    {
        // Get the identifier for the given Tag String.
        orderbyid = draldb->getTagId(orderbystr);
        if(orderbyid == TAG_ID_INVALID)
        {
            QString err = "Invalid DRAL-TAG reference: " + orderbystr + " in ADF.";
            myLogMgr->addLog(err);
            return false;
        }
    }

    // Value and Select MUST be together.
    if((selectstr != QString::null) || (valuestr != QString::null))
    {
        if((selectstr == QString::null) || (valuestr == QString::null))
        {
            myLogMgr->addLog("Invalid tab statement in ADF, wrong <select,value> specification.");
            return false;
        }
    }

    // select and splitby are incompatible
    if((selectstr != QString::null) && (splitbystr != QString::null))
    {
        myLogMgr->addLog("Invalid tab statement in ADF, 'select' and 'splitby' are incompatible.");
        return false;
    }

    // Check select tag coherence with given value.
    UINT64 value=0;
    if(valuestr != QString::null)
    {
        // Try to convert valustr into an UINT64.
        if(selecttgtype == TAG_INTEGER_TYPE)
        {
            bool numparsed;
            value = AGTTypes::str2uint64(valuestr.latin1(), numparsed);
            if(!numparsed)
            {
                myLogMgr->addLog("Invalid tab statement in ADF, malformed value attribute.");
                return false;
            }
        }
        else if(selecttgtype == TAG_STRING_TYPE)
        {
            INT32 stridx = draldb->addString(valuestr);
            value = (UINT64) stridx;
        }
        else
        {
            myLogMgr->addLog("Invalid tab statement in ADF, select is implemented only for integer and string DRAL-Tags.");
            return false;
        }
    }

    // Finally create the tab object.
    TabSpec * tab = new TabSpec(namestr, draldb);
    //cerr << "ADFParser::processTabStatement created tabspec for " << namestr << endl;

    tab->setSortBy(orderbystr);
    tab->setWithTag(withtagstr);
    tab->setStrValue(valuestr);
    tab->setMatchingValue(value);

    if(splitbystr != QString::null)
    {
        tab->setSelectTag(splitbystr);
        tab->setIsSplit(true);
    }
    else if(selectstr != QString::null)
    {
        tab->setSelectTag(selectstr);
        tab->setIsSplit(false);
    }
    
    //cerr << "ADFParser::processTabStatement appending tabspec to db..." << endl;
    dreamsdb->addNewTab(tab);

    QDomNode cnode = e.firstChild();
    bool go_on = true;

    // Now iterate through the if/set statements.
    while(!cnode.isNull() && go_on)
    {
        // check if this node has a tag
        go_on = processTabSubTags(cnode, tab);
        cnode = cnode.nextSibling();
    }
    //cerr << "ADFParser::processTabStatement done" << endl;
    return go_on;
}

/*
 * This function parses the tab sub tags.
 *
 * @param node contains the parsing information.
 * @param tab points to the actual tab.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processTabSubTags(QDomNode node, TabSpec * tab)
{
    QDomElement e;

    // Try to convert the node to an element.
    e = node.toElement(); 
    Q_ASSERT(!e.isNull());

    // Comments or similar stuff.
    if(e.isNull())
    {
        return true;
    }

    QString tagname = e.tagName();

    // check for valid tags
    if(tagname == ADF_RULE)
    {
        ExpressionList expr_list;

        if(processRuleStatement(node, expr_list, EXPRESSION_ITEM))
        {
            // Add all the rules to all the tabs.
            tab->addFontRules(expr_list);
            return true;
        }
        else
        {
            return false;
        }
    }
    else if(tagname == ADF_SHOW)
    {
        return processShowStatement(node, tab);
    }
    else
    {
        myLogMgr->addLog("Invalid Tab Subtag " + tagname + " in ADF.");
        return false;
    }
}

/*
 * This function parses the show statement.
 *
 * @param node contains the parsing information.
 * @param tab points to the actual tab.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processShowStatement(QDomNode node, TabSpec * tab)
{
    QDomElement e;

    // Try to convert the node to an element.
    e = node.toElement(); 
    Q_ASSERT(!e.isNull());

    // 1) Check for tagname attrib.
    QString tgnamestr = e.attribute(ADF_ITEMTAGNAME_ATTRIB).stripWhiteSpace();
    if(tgnamestr == QString::null)
    {
        myLogMgr->addLog("Invalid show statement in ADF, missing tagname attribute.");
        return false;
    }

    // Get the identifier for the given Tag String.
    TAG_ID tagId = draldb->getTagId(tgnamestr);
    if(tagId == TAG_ID_INVALID)
    {
        QString err = "Invalid DRAL-TAG reference: " + tgnamestr + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    // Check the type of the used tag.
    TagValueType tgType = draldb->getTagValueType(tagId);
    if(tgType != TAG_INTEGER_TYPE && tgType != TAG_STRING_TYPE)
    {
        myLogMgr->addLog("Sorry, 'show' supported only with integer and string tags.");
        return false;
    }

    // All right then.
    tab->addShowTag(tgnamestr);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Rules Methods /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 * Method that parses a <rule> ... </rule> statement. One rule can set many attributes,
 * that's why the 2nd param is a list instead of a single expression onto
 * which we need to append what we parse.
 */
extern void SetParserInputString(const char *);
extern void CleanParserInputString(void);
extern int yyparse(void *ddb);
extern QString errstr;
ExpressionList *parseTree;

/*
 * This function parses a rule statement.
 *
 * @param node contains the parsing information.
 * @param rules is a reference where the expressions are inserted.
 * @param expression_type is the type of rules that can be parsed.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processRuleStatement(QDomNode node, ExpressionList & rules, UINT16 expression_type)
{
    Expression * expr;
    QString error;

    //cerr << "\tADF_RULE: start " << endl;
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    // Get the characters inside the <rule> node
    QString ruleText = e.text();
    //cerr << "\tADF_RULE: text = " << ruleText << endl;

    //
    // Call the flex/bison parser to build a rule based on the 
    // input text
    //
    bool ParsingOk;

    //
    // Need to tell flex to read from our string instead of STDIN
    //
    SetParserInputString(ruleText);

    //
    // Now invoke parser. It needs both draldb and dreamsdb, so build a
    // struct with both pointers and pass it as a pointer
    //
    yyctl_t yc;
    yc.draldb = draldb;
    yc.dreamsdb = dreamsdb;
    ParsingOk = (yyparse((void *)&yc) == 0);

    //
    // Need to tell flex to clear up its input buffer (the one allocated
    // when we called SetParserInputString()
    //
    CleanParserInputString();

    if(!ParsingOk)
    {
        myLogMgr->addLog("\tADF_RULE: Parsing FAILED: error: " + errstr + "for rule:" + ruleText);
        return false;
    }

    //
    // Perform type checking on the tree resulting from parsing to ensure 
    // rules are valid
    //
    error = "";
    if(parseTree != NULL)
    {
        ParsingOk = (parseTree->typeCheck(error) == true );
        if(!ParsingOk)
        {
            myLogMgr->addLog("\tADF_RULE: Type checking FAILED: " + error + " for rule: " + ruleText);
            myLogMgr->addLog("\tADF_RULE: Parsed Rule is      :\n" + parseTree->toString("..."));
            return false;
        }

        //
        // Precompute the mask for the tree 
        //
        parseTree->evalComputeMask();

        //
        // Add the tree just parsed to current rule list
        //
        rules.appendList(* parseTree);
        createdRules.appendList(* parseTree);
    }

    if(ParsingOk)
    {
        // If ok, checks the correctness of the rules.
        return checkRules(rules, expression_type);
    }

    //cerr << "\tADF_RULE: done with rule!!!! = " << endl << parseTree->toString("") << endl;
    return false;
}

/*
 * Parses rules.
 *
 * @param node the node containing the XML content to parse.
 * @param lRules is where the rules must be added.
 *
 * @return if the parse is ok.
 */
bool 
ADFParser::processRuleEnvStatement(QDomNode node, ExpressionList & lRules, UINT16 expression_type)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    UINT32 nDimension = 0;
    bool ok = true;
    QDomNode cnode = e.firstChild();

    while(!cnode.isNull() && ok)
    {
        QDomElement celem = cnode.toElement(); // try to convert the node to an element.
        QString tagname = celem.tagName();
        if(tagname == ADF_RULE)
        { 
            ExpressionList lexpCurrent;

            ok = processRuleStatement(cnode, lexpCurrent, expression_type);
            if(ok)
            {
                lRules.appendList(lexpCurrent);
            }
        }
        else
        { 
            myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " inside rule statement.");
            ok = false;
        }

        cnode = cnode.nextSibling();
    }

    return ok;
}

/*
 * Checks that the generated rules don't contain invalid sets.
 *
 * @return if the list is ok.
 */
bool
ADFParser::checkRules(ExpressionList & expr_list, UINT16 type) const
{
    set<TAG_ID> item_tags;
    set<TAG_ID> slot_tags;
    set<TAG_ID> node_tags;
    set<TAG_ID> cycle_tags;
    ExpressionIterator it_expr(expr_list);
    Expression * expr;
    UINT16 mask;

    // Switches the type of expression being evaluated.
    switch(type)
    {
        case EXPRESSION_ITEM:
            // Only lcolor, italic, underline and bold allowed.
            // Generates the mask.
            mask = CSL_LETTER_COLOR_MASK;
            mask |= CSL_ITALIC_MASK;
            mask |= CSL_UNDERLINE_MASK;
            mask |= CSL_BOLD_MASK;
            mask = ~mask;

            // First adds the rules to the already globally defined rules.
            while((expr = it_expr.current()) != NULL)
            {
                ++it_expr;
                // Checks the correct field set.
                if(expr->getComputeMask() & mask)
                {
                    myLogMgr->addLog("ADF Error: set a field in an item tab rule that is not valid.");
                    myLogMgr->addLog("\tOnly \"" + QString(ADF_LCOLOR_ATTRIB) + "\", \"" + QString(ADF_ITALIC_ATTRIB) + "\", \"" + QString(ADF_UNDERLINE_ATTRIB) + "\" and \"" + QString(ADF_BOLD_ATTRIB) + "\" are allowed.");
                    return false;
                }

                // Checks that no slot, ...
                expr->getUsedTags(&slot_tags, &item_tags, &node_tags, &cycle_tags);
                if(!slot_tags.empty())
                {
                    myLogMgr->addLog("ADF Error: can't use slot tags in an item tab rule. Only item tags allowed.");
                    return false;
                }
                // ... node ...
                if(!node_tags.empty())
                {
                    myLogMgr->addLog("ADF Error: can't use node tags in an item tab rule. Only item tags allowed.");
                    return false;
                }
                // ... or cycle tags are used.
                if(!cycle_tags.empty())
                {
                    myLogMgr->addLog("ADF Error: can't use cycle tags in an item tab rule. Only item tags allowed.");
                    return false;
                }
            }
            break;

        case EXPRESSION_NORMAL:
            // Italic, underline and bold are not allowed.
            // Generates the mask.
            mask = CSL_ITALIC_MASK;
            mask |= CSL_UNDERLINE_MASK;
            mask |= CSL_BOLD_MASK;

            // First adds the rules to the already globally defined rules.
            while((expr = it_expr.current()) != NULL)
            {
                ++it_expr;
                if(expr->getComputeMask() & mask)
                {
                    myLogMgr->addLog("ADF Error: set a field in a rule that is not valid.");
                    myLogMgr->addLog("\t\"" + QString(ADF_ITALIC_ATTRIB) + "\", \"" + QString(ADF_UNDERLINE_ATTRIB) + "\" and \"" + QString(ADF_BOLD_ATTRIB) + "\" can't be used in none item tab rules.");
                    return false;
                }
            }
            break;

        default:
            Q_ASSERT(false);
            break;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////// Dreams Methods ////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*
 * Parses Dreams statement.
 *
 * @param node the node containing the XML content to parse.
 *
 * @return if the parse is ok.
 */
bool
ADFParser::processResourceStatement(QDomNode node)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    // Creates the base resource in the dreams database.
    dreamsdb->layoutResourceCreateLayout();

    QDomNode cnode = e.firstChild();
    bool ok = true;
    while(!cnode.isNull() && ok)
    {
        // check if this node has a tag
        ok = processResourceDomNode(cnode);
        cnode = cnode.nextSibling();
    }

    if(ok)
    {
        dreamsdb->layoutResourceComputeLayoutPlainCache();
    }
    return ok;
}

/*
 * Parses Dreams statements.
 *
 * @param node the node containing the XML content to parse.
 *
 * @return if the parse is ok.
 */
bool
ADFParser::processResourceDomNode(QDomNode node)
{
    QDomElement e;
    ExpressionList expr_list;

    // Try to convert the node to an element.
    e = node.toElement();

    // Comments or similar stuff.
    if(e.isNull())
    {
        return true;
    }

    QString tagname = e.tagName();

    // check for known tags
    if(tagname == ADF_2DREAMS)
    {
        return true;
    }
    else if(tagname == ADF_GROUP)
    {
        return processResourceGroupStatement(node);
    }
    else if(tagname == ADF_RULE)
    {
        expr_list.clear();
        bool ok = processRuleStatement(node, expr_list, EXPRESSION_NORMAL);
        if(ok)
        {
            dreamsdb->layoutResourceAddRules(&expr_list);
        }
        return ok;
    }
    else if(tagname == ADF_EDGE)
    {
        return processResourceEdgeStatement(node);
    }
    else if(tagname == ADF_NODE)
    {
        return processResourceNodeStatement(node);
    }
    else if(tagname == ADF_ENTERNODE)
    {
        return processResourceEnterNodeStatement(node);
    }
    else if(tagname == ADF_EXITNODE)
    {
        return processResourceExitNodeStatement(node);
    }
    else if(tagname == ADF_BLANKROWS)
    {
        return processResourceBlankStatement(node);
    }
    else
    {
        myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " inside dreams2 statement.");
        return false;
    }
}

/*
 * Parses a Dreams group statement.
 *
 * @param node the node containing the XML content to parse.
 *
 * @return if the parse is ok.
 */
bool
ADFParser::processResourceGroupStatement(QDomNode node)
{
    // Node type checking and tries to convert the node to an element.
    QDomElement e = node.toElement();

    QString groupname = e.attribute(ADF_NAME_ATTRIB).stripWhiteSpace();
    if(groupname == QString::null)
    {
        myLogMgr->addLog("Invalid group statement in ADF, missing group name.");
        return false;
    }

    dreamsdb->layoutResourceNewGroup(groupname);

    QDomNode cnode = e.firstChild();
    bool ok = true;
    while(!cnode.isNull() && ok)
    {
        // check if this node has a tag
        ok = processResourceDomNode(cnode);
        cnode = cnode.nextSibling();
    }

    if(ok)
    {
        if(!dreamsdb->layoutResourceEndGroup())
        {
            myLogMgr->addLog("Problems closing the group...");
            return false;
        }
    }

    return ok;
}

/*
 * This function parses an edge statement.
 *
 * @param node contains the parsing information.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processResourceEdgeStatement(QDomNode node)
{
    QDomElement e;

    // Try to convert the node to an element.
    e = node.toElement(); 
    Q_ASSERT(!e.isNull());

    // 1) name attribute:
    QString tgnamestr = e.attribute(ADF_NAME_ATTRIB).stripWhiteSpace();
    if(tgnamestr == QString::null)
    {
        myLogMgr->addLog("Invalid edge statement in ADF, missing edge name.");
        return false;
    }

    QString fromstr = e.attribute(ADF_FROM_ATTRIB).stripWhiteSpace();
    QString tostr = e.attribute(ADF_TO_ATTRIB).stripWhiteSpace();

    DralGraphEdge * dbedge = NULL;

    // We look for the edge using only the name. If the name is enought to identify
    // the edge, we don't ask for the from and to nodes.
    if((fromstr != QString::null) || (tostr != QString::null))
    {
        // 2) from attribute:
        if(fromstr == QString::null)
        {
            myLogMgr->addLog("Invalid edge statement in ADF, missing 'from' attribute.");
            return false;
        }

        fromstr = draldb->normalizeNodeName(fromstr);

        // 3) to attribute:
        if(tostr == QString::null)
        {
            myLogMgr->addLog("Invalid edge statement in ADF, missing 'to' attribute.");
            return false;
        }

        tostr = draldb->normalizeNodeName(tostr);

        // EDGE checking.
        DralGraphNode * fromNode = draldb->getNode(fromstr);
        DralGraphNode * toNode = draldb->getNode(tostr);

        if(fromNode == NULL)
        {
            QString err = "Invalid edge statement, wrong source node: " + fromstr + " in ADF.";
            myLogMgr->addLog(err);
		    return true;
        }

        if(toNode == NULL)
        {
            QString err = "Invalid edge statement, wrong destination node: " + tostr + " in ADF.";
            myLogMgr->addLog(err);
		    return true;
        }

        dbedge = draldb->findEdgeByNameFromTo(tgnamestr, fromstr, tostr);
        if(dbedge == NULL)
        {
            QString err = "Invalid edge statement, edge not found in DRAL Graph. Source node = " + fromstr + ", destination node = " + tostr + ", edge name = " + tgnamestr + " in ADF.";
            myLogMgr->addLog(err);
            return true;
        }
    }
    else 
    {
        bool bFound = draldb->findEdgeByName(tgnamestr, dbedge);
        if(bFound && (dbedge == NULL))
        {
            myLogMgr->addLog("Invalid edge statement, name " + tgnamestr + " doesn't identify the edge. Nodes from/to required");
		    return false;
        }
        else if(dbedge == NULL)
        {
            myLogMgr->addLog("Invalid edge statement, edge not found in DRAL Graph. Edge name = " + tgnamestr + " in ADF.");
            return false;
        }
    }

    // And inform the draldb about it.
    TRACK_ID trackid = draldb->trackEdgeTags(dbedge->getEdgeId());
    Q_ASSERT(trackid != TRACK_ID_INVALID);

    // 4) draw_when attribute:
    QString draw_whenstr = e.attribute(ADF_DRAW_WHEN_ATTRIB).stripWhiteSpace().lower();
    bool isDrawOnMove = true;

    if(draw_whenstr != QString::null)
    {
        if(draw_whenstr == ADF_DWHEN_RECEIVED)
        {
            isDrawOnMove = false;
        }
        else if(draw_whenstr != ADF_DWHEN_SENT)
        {
            QString err = "Invalid edge statement in ADF, draw_when attribute can be 'sent' or 'received' only.";
            err += "Source node = " + fromstr + ", destination node = " + tostr + ", edge name = " + tgnamestr + ".";
            myLogMgr->addLog(err);
            return false;
        }
    }

    // TODO: draw on move not implemented.
    dreamsdb->layoutResourceAddRow(SimpleEdgeRow, trackid, getTagIdColor(trackid), dbedge->getEdgeId(), dbedge->getBandwidth(), isDrawOnMove);

    const ClockDomainEntry * clock;
    clock = draldb->getClockDomain(dbedge->getClockId());

    if((clock->getDivisions() > 2) || (clock->getDivisions() < 1))
    {
        QString err = "You are using a clock domain that has set a number of phases different than 1 or 2.\n";
        err += "Dreams just supports clocks with 1 or 2 phases.\n";
        myLogMgr->addLog(err);
        return false;
    }

    // Expanded attribute
    QString show_str = e.attribute(ADF_SHOW).stripWhiteSpace().lower();
    bool expanded = (clock->getDivisions() == 1);
    if(show_str != QString::null)
    {
        if(show_str == ADF_PHASE_EXP)
        {
            expanded = true;
        }
        else if(show_str == ADF_PHASE_PH)
        {
            expanded = false;
        }
        else
        {
            QString err = "Invalid edge statement in ADF, show attribute can be '" + QString(ADF_PHASE_EXP) + "' only.";
            err += "Source node = " + fromstr + ", destination node = " + tostr + ", edge name = " + tgnamestr + ".";
            myLogMgr->addLog(err);
            return false;
        }
    }

    dreamsdb->layoutResourceSetShowPhaseHigh(expanded);

    QDomNode cnode = e.firstChild();
    ExpressionList expr_list;
    bool go_on = true;

    // Now iterate through the if / set statements.
    while(!cnode.isNull() && go_on)
    {
        expr_list.clear();
        QDomElement eset = cnode.toElement();
        if(!eset.isNull() && (eset.tagName() == ADF_RULE))
        { 
            go_on = processRuleStatement(cnode, expr_list, EXPRESSION_NORMAL);
            if(go_on)
            {
                // The rules are added to the high phase rule list.
                dreamsdb->layoutResourceAddRowRules(&expr_list, LAYOUTRESOURCE_PHASE_HIGH);
            }
        }
        else
        {
            myLogMgr->addLog("ADF Error: only \"" + QString(ADF_RULE) + "\" tag allowed inside an edge statement.");
            go_on = false;
        }
        cnode = cnode.nextSibling();
    }
    return go_on;
}

/*
 * This function parses a node statement.
 *
 * @param node contains the parsing information.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processResourceNodeStatement(QDomNode node)
{
    QDomElement e;

    // Try to convert the node to an element.
    e = node.toElement(); 
    Q_ASSERT(!e.isNull());

    // 1) name attribute:
    QString namestr = e.attribute(ADF_NAME_ATTRIB).stripWhiteSpace();
    if(namestr == QString::null)
    {
        myLogMgr->addLog("Invalid node statement in ADF, missing node name.");
        return true;
    }

    // Check node name notation.
    namestr = draldb->normalizeNodeName(namestr);

    // 2) check node name
    DralGraphNode * dralNode = draldb->getNode(namestr);
    if(dralNode == NULL)
    {
        myLogMgr->addLog("Invalid node statement, unkown node " + namestr + ". Your ADF may be deprecated.");
        return true;
    }

    // Register the node-slot on the draldb.
    TRACK_ID trackId = draldb->trackNodeTags(dralNode->getNodeId());

    dreamsdb->layoutResourceAddRow(SimpleNodeRow, trackId, getTagIdColor(trackId), dralNode->getNodeId(), 1);

    QDomNode cnode = e.firstChild();
    bool go_on = true;
    bool defined = false;
    bool rule_set = false;
    bool phase_set = false;

    // Iterate through the if/set statements.
    while(!cnode.isNull() && go_on)
    {
        QDomElement eset = cnode.toElement();
        if(!eset.isNull())
        {
            // Defining a global rule for the node.
            if(eset.tagName() == ADF_RULE)
            {
                // Check syntaxis.
                if(phase_set)
                {
                    myLogMgr->addLog("ADF Error: can't use a global rule and phases in the same node statement.");
                    return false;
                }

                ExpressionList expr_list;

                // Gets the rule.
                go_on = processRuleStatement(cnode, expr_list, EXPRESSION_NORMAL);

                if(go_on)
                {
                    const ClockDomainEntry * clock;
                    clock = draldb->getClockDomain(dralNode->getClockId());

                    if(clock->getDivisions() == 1)
                    {
                        // It's the first global rule, so we set the proper layout.
                        if(!rule_set)
                        {
                            dreamsdb->layoutResourceSetShowPhaseHigh(true);
                        }
                        dreamsdb->layoutResourceAddRowRules(&expr_list, LAYOUTRESOURCE_PHASE_HIGH);
                    }
                    else if(clock->getDivisions() == 2)
                    {
                        // It's the first global rule, so we set the proper layout.
                        if(!rule_set)
                        {
                            dreamsdb->layoutResourceSetShowPhaseHigh(false);
                            dreamsdb->layoutResourceSetShowPhaseLow(false);
                        }
                        dreamsdb->layoutResourceAddRowRules(&expr_list, LAYOUTRESOURCE_PHASE_HIGH);
                        dreamsdb->layoutResourceAddRowRules(&expr_list, LAYOUTRESOURCE_PHASE_LOW);
                    }
                    else
                    {
                        Q_ASSERT(false);
                    }

                    // We have set some rules.
                    rule_set = true;
                }
            }
            else if((eset.tagName() == ADF_PHASE_HIGH) || (eset.tagName() == ADF_PHASE_LOW))
            {
                // Check syntaxis.
                if(rule_set)
                {
                    myLogMgr->addLog("ADF Error: can't use a global rule and phases in the same node statement.");
                    return false;
                }

                // Process the phase statement.
                go_on = processResourcePhaseStatement(cnode);
                phase_set = true;
            }
            else
            {
                myLogMgr->addLog("ADF Error: unknown tag \"" + eset.tagName() + "\".");
                return false;
            }

            // The row has some layout defined now.
            defined = true;
        }
        cnode = cnode.nextSibling();
    }

    if(!go_on)
    {
        return false;
    }

    if(!defined)
    {
        myLogMgr->addLog("ADF Error: no phase specification. Add at least \"" + QString(ADF_PHASE_HIGH) + "\" or \"" + QString(ADF_PHASE_LOW) + "\" or \"" + QString(ADF_RULE) + "\" in the row specification.");
        return false;
    }

    return true;
}

/*
 * This function parses a node or edge high / low rule statement.
 *
 * @param node contains the parsing information.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processResourcePhaseStatement(QDomNode node)
{
    QDomElement e;
    layoutResourcePhase phase;
    bool expanded = false;

    // Try to convert the node to an element.
    e = node.toElement(); 
    Q_ASSERT(!e.isNull());

    // Gets which phase to show and sets the row to show it.
    if((e.tagName() != ADF_PHASE_HIGH) && (e.tagName() != ADF_PHASE_LOW))
    {
        myLogMgr->addLog("ADF Error: invalid tag \"" + e.tagName() + "\" inside edge or node statement.");
        myLogMgr->addLog("\tOnly \"" + QString(ADF_PHASE_HIGH) + "\" and \"" + QString(ADF_PHASE_LOW) + "\" tags are allowed inside these statements.");
        return false;
    }

    // 1) expanded attribute:
    QString expandedstr = e.attribute(ADF_PHASE_EXP).stripWhiteSpace();
    if(expandedstr == QString::null)
    {
        myLogMgr->addLog("ADF Error: invalid phase statement in ADF, missing \"" + QString(ADF_PHASE_EXP) + "\" tag.");
        return false;
    }

    // Gets the expanded value.
    if(expandedstr == "true")
    {
        expanded = true;
    }
    else if(expandedstr == "false")
    {
        expanded = false;
    }
    else
    {
        myLogMgr->addLog("ADF Error: invalid value for expanded tag, only true and false are correct.");
        return false;
    }

    // Gets which phase to show and sets the row to show it.
    if(e.tagName() == ADF_PHASE_HIGH)
    {
        phase = LAYOUTRESOURCE_PHASE_HIGH;
        if(!dreamsdb->layoutResourceSetShowPhaseHigh(expanded))
        {
            myLogMgr->addLog("ADF Error: invalid phase statements. Check that you don't have any phase repeated, or that you're trying to show two phases with one of them expanded.");
            return false;
        }
    }
    else if(e.tagName() == ADF_PHASE_LOW)
    {
        phase = LAYOUTRESOURCE_PHASE_LOW;
        if(!dreamsdb->layoutResourceSetShowPhaseLow(expanded))
        {
            myLogMgr->addLog("ADF Error: invalid phase statements. Check that you don't have any phase repeated, or that you're trying to show two phases with one of them expanded.");
            return false;
        }
    }
    else
    {
        myLogMgr->addLog("ADF Error: invalid tag " + e.tagName() + " inside edge or node statement.");
        myLogMgr->addLog("\tOnly " + QString(ADF_PHASE_HIGH) + " and " + QString(ADF_PHASE_LOW) + " tags are allowed inside these statements.");
        return false;
    }

    QDomNode cnode = e.firstChild();
    ExpressionList expr_list;
    bool go_on = true;

    // Now iterate through the if / set statements.
    while(!cnode.isNull() && go_on)
    {
        expr_list.clear();
        QDomElement eset = cnode.toElement();
        if(!eset.isNull())
        {
            if(eset.tagName() == ADF_RULE)
            {
                go_on = processRuleStatement(cnode, expr_list, EXPRESSION_NORMAL);
                if(go_on)
                {
                    dreamsdb->layoutResourceAddRowRules(&expr_list, phase);
                }
            }
            else
            {
                myLogMgr->addLog("ADF Error: unknown tag \"" + eset.tagName() + "\".");
                return false;
            }
        }
        cnode = cnode.nextSibling();
    }
    return go_on;
}

/*
 * This function parses an enter node statement.
 *
 * @param node contains the parsing information.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processResourceEnterNodeStatement(QDomNode node)
{
    myLogMgr->addLog("ADFParser::processEnterNodeStatement: sorry, feature not implemented yet.");
    return false;
}

/*
 * This function parses an exit node statement.
 *
 * @param node contains the parsing information.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processResourceExitNodeStatement(QDomNode node)
{
    myLogMgr->addLog("ADFParser::processExitNodeStatement: sorry, feature not implemented yet.");
    return false;
}

/*
 * This function parses a blank row statement.
 *
 * @param node contains the parsing information.
 *
 * @return if the node was checked correctly.
 */
bool
ADFParser::processResourceBlankStatement(QDomNode node)
{
    QDomElement e;

    // try to convert the node to an element.
    e = node.toElement();
    Q_ASSERT(!e.isNull());

    bool hasColor = false;

    // Check for rows attribute (mandatory).
    QString tgrowsstr = e.attribute(ADF_ROWS_ATTRIB).stripWhiteSpace().lower();
    if(tgrowsstr == QString::null)
    {
        myLogMgr->addLog("Invalid <blank> tag in ADF, rows attribute is mandatory.");
        return false;
    }

    // Try to convert it to an integer.
    bool parseok;
    INT32 tgrow = (INT32) tgrowsstr.toInt(&parseok);
    if((tgrow < 1) || (tgrow > 256))
    {
        myLogMgr->addLog("Invalid <blank> tag in ADF, rows attribute must be in the range [1..256].");
        return false;
    }

    // Check for color attribute (optional).
    QString tgcolorstr = e.attribute(ADF_COLOR_ATTRIB).stripWhiteSpace();
    QColor coloratr;
    if(tgcolorstr != QString::null)
    {
        coloratr = QColor(tgcolorstr);
        if(!coloratr.isValid())
        {
            QString err = "Invalid color declaration: " + tgcolorstr + " in ADF(1).";
            myLogMgr->addLog(err);
            return false;
        }
        hasColor = true;
    }

    // Creates the blank row and sets the color.
    dreamsdb->layoutResourceAddRow(SimpleBlankRow, TRACK_ID_INVALID, TAG_ID_INVALID, DRAL_ID_INVALID, tgrow);
    dreamsdb->layoutResourceSetBlankRowColor(coloratr);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Floorplan Methods ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*
 * Parses Floorplan statement.
 *
 * @param node the node containing the XML content to parse.
 *
 * @return if the parse is ok.
 */
bool
ADFParser::processFloorplanStatement(QDomNode node)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    QString tagname = e.tagName();
    //cerr << "\tADF_DREAMS3: tagName = " << tagname << endl; 

    ExpressionList defaultRules;
    ExpressionList overrideRules;
    LayoutFloorplanTable * pltRoot = NULL;

    dreamsdb->layoutFloorplanCreateLayout();

    bool ok = true;
    QDomNode cnode = e.firstChild();
    while(!cnode.isNull() && ok)
    {
        QDomElement celem = cnode.toElement(); // try to convert the node to an element.
        QString tagname = celem.tagName();
        if(!celem.isNull()) // comments in the XML
        {
            if(tagname == ADF_TABLE)
            { 
                Q_ASSERT(pltRoot == NULL);
                pltRoot = new LayoutFloorplanTable(draldb);
                dreamsdb->layoutFloorplanSetRoot(pltRoot);
                // We are in the outer level of the ADF tree, so all the inner elements
                // are in the level 1
                ok = processFloorplanTableStatement(pltRoot, cnode, 1);  
            }
            else if(tagname == ADF_DEFAULT)
            { 
                ok = processRuleEnvStatement(cnode, defaultRules, EXPRESSION_NORMAL);
            }
            else if(tagname == ADF_OVERRIDE)
            { 
                ok = processRuleEnvStatement(cnode, overrideRules, EXPRESSION_NORMAL);
            }
            else
            { 
                myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " inside dreams3 statement.");
                return false;
            }
        }
        cnode = cnode.nextSibling();
    }

    if(ok && !defaultRules.isEmpty())
    {
        pltRoot->addDefaultRules(defaultRules, 0);
    }
    if(ok && !overrideRules.isEmpty())
    {
        pltRoot->addOverrideRules(overrideRules, 0);
    }

    return ok;
}

/*
 * Parses Floorplan table statement.
 *
 * @param table is the actual scope.
 * @param node the node containing the XML content to parse.
 * @param level is the rule priority
 *
 * @return if the parse is ok.
 */
bool
ADFParser::processFloorplanTableStatement(LayoutFloorplanTable * table, QDomNode node, INT32 level)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    QString tagname = e.tagName();
    //cerr << "\tADF_TABLE: tagName = " << tagname << endl;

    // 1) border
    QString sTableBorder = e.attribute(ADF_BORDER_ATTRIB).stripWhiteSpace();
    if (sTableBorder != QString::null)
    {
        table->setBorder(sTableBorder == "y");
        //cerr << "\tADF_TABLE: border = " << sTableBorder << endl; 
    }

    // 2) bgcolor
    QString sTableBGColor = e.attribute(ADF_BGCOLOR_ATTRIB).stripWhiteSpace();
    if (sTableBGColor != QString::null)
    {
        QColor bgColor(sTableBGColor);
        if(!bgColor.isValid())
        {
            myLogMgr->addLog("Invalid rectangle background color : " + sTableBGColor + " in ADF.");
            return false;
        }
        table->setBGColor(bgColor);
        //cerr << "\tADF_TABLE: bgcolor = " << sTableBGColor << endl; 
    }

    ExpressionList defaultRules;
    ExpressionList overrideRules;

    bool ok = true;
    LayoutFloorplanRow *pltFirstRow = NULL;
    QDomNode cnode = e.firstChild();

    while(!cnode.isNull() && ok)
    {
        QDomElement celem = cnode.toElement(); // try to convert the node to an element.
        QString tagname = celem.tagName();

        if(tagname == ADF_TR)      
        { 
            LayoutFloorplanRow *pltRow = new LayoutFloorplanRow(draldb);
            table->addSon(pltRow);
            ok = processFloorplanTRStatement(pltRow, cnode, level + 1); 
        }
        else if(tagname == ADF_DEFAULT)
        { 
            ok = processRuleEnvStatement(cnode, defaultRules, EXPRESSION_NORMAL);
        }
        else if(tagname == ADF_OVERRIDE)
        { 
            ok = processRuleEnvStatement(cnode, overrideRules, EXPRESSION_NORMAL);
        }
        else                       
        { 
            myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " inside dreams 3 table statement.");
            return false;
        }

        cnode = cnode.nextSibling();
    }

    // Add the defined rules here. the RuleEngine should be the responsible of the adding
    if(ok && !defaultRules.isEmpty())
    {
        //cerr << "\tADF_TABLE: Adding default rules: level = " << level << endl;
        table->addDefaultRules(defaultRules, level + 1);
    }
    if(ok && !overrideRules.isEmpty())
    {
        table->addOverrideRules(overrideRules, level + 1);
    }

    return ok;
}

/*
 * Parses Floorplan TR statement.
 *
 * @param row is the actual scope.
 * @param node the node containing the XML content to parse.
 * @param level is the rule priority
 *
 * @return if the parse is ok.
 */
bool 
ADFParser::processFloorplanTRStatement(LayoutFloorplanRow * row, QDomNode node, INT32 level)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    QString tagname = e.tagName();
    //cerr << "\tADF_TR: tagName = " << tagname << endl; 

    ExpressionList defaultRules;
    ExpressionList overrideRules;

    bool ok = true;
    QDomNode cnode = e.firstChild();
    while(!cnode.isNull() && ok)
    {
        LayoutFloorplanColumn *plcFirstColumn = NULL;

        QDomElement celem = cnode.toElement(); // try to convert the node to an element.
        QString tagname = celem.tagName();

        if(tagname == ADF_TD)
        { 
            LayoutFloorplanColumn * plcColumn = new LayoutFloorplanColumn(draldb);
            row->addSon(plcColumn);
            ok = processFloorplanTDStatement(plcColumn, cnode, level + 1); 
        }
        else if(tagname == ADF_DEFAULT)
        { 
            ok = processRuleEnvStatement(cnode, defaultRules, EXPRESSION_NORMAL);
        }
        else if(tagname == ADF_OVERRIDE)
        { 
            ok = processRuleEnvStatement(cnode, overrideRules, EXPRESSION_NORMAL);
        }
        else
        { 
            myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " inside dreams3 TR statement.");
            return false;
        }

        cnode = cnode.nextSibling();
    }

    if(ok && !defaultRules.isEmpty())
    {
        row->addDefaultRules(defaultRules, level + 1);
    }
    if(ok && !overrideRules.isEmpty())
    {
        row->addOverrideRules(overrideRules, level + 1);
    }

    return ok;
}

/*
 * Parses Floorplan TD statement.
 *
 * @param column is the actual scope.
 * @param node the node containing the XML content to parse.
 * @param level is the rule priority
 *
 * @return if the parse is ok.
 */
bool 
ADFParser::processFloorplanTDStatement(LayoutFloorplanColumn * column, QDomNode node, INT32 level)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    QString tagname = e.tagName();
    //cerr << "\tADF_TD: tagName = " << tagname << endl; 

    // Obtain tag attributes
    // 1) name
    QString sTDName = e.attribute(ADF_NAME_ATTRIB).stripWhiteSpace();
    if (sTDName != QString::null)
    {
        column->setName(sTDName);
        //cerr << "\tADF_TD: name = " << sTDName << endl; 
    }

    // 2) margin
    QString sTDMargin = e.attribute(ADF_MARGIN_ATTRIB).stripWhiteSpace();
    if (sTDMargin != QString::null)
    {
        column->setMargin(sTDMargin == "y");
        //cerr << "\tADF_TD: margin = " << sTDMargin << endl; 
    }

    ExpressionList defaultRules;
    ExpressionList overrideRules;

    bool ok = true;
    QDomNode cnode = e.firstChild();
    while(!cnode.isNull() && ok)
    {
        QDomElement celem = cnode.toElement(); // try to convert the node to an element.
        QString tagname = celem.tagName();

        if(tagname == ADF_TABLE)
        { 
            LayoutFloorplanTable * pltTable = new LayoutFloorplanTable(draldb);
            column->addSon(pltTable);
            ok = processFloorplanTableStatement(pltTable, cnode, level + 1);  
        }
        else if(tagname == ADF_RECTANGLE)
        { 
            LayoutFloorplanRectangle * plrRectangle = new LayoutFloorplanRectangle(draldb);
            column->addSon(plrRectangle);
            ok = processFloorplanRectangleStatement(plrRectangle, cnode, level + 1) ; 
        }
        else if(tagname == ADF_DEFAULT)
        { 
            ok = processRuleEnvStatement(cnode, defaultRules, EXPRESSION_NORMAL);
        }
        else if(tagname == ADF_OVERRIDE)
        { 
            ok = processRuleEnvStatement(cnode, overrideRules, EXPRESSION_NORMAL);
        }
        else
        { 
            myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " inside dreams3 TD statement.");
            return false;
        }

        cnode = cnode.nextSibling();
    }

    if(ok && !defaultRules.isEmpty())
    {
        column->addDefaultRules(defaultRules, level + 1);
    }
    if(ok && !overrideRules.isEmpty())
    {
        column->addOverrideRules(overrideRules, level + 1);
    }

    return ok;
}

/*
 * Parses Floorplan rectangle statement.
 *
 * @param rectangle is the actual scope.
 * @param node the node containing the XML content to parse.
 * @param level is the rule priority
 *
 * @return if the parse is ok.
 */
bool 
ADFParser::processFloorplanRectangleStatement(LayoutFloorplanRectangle * rectangle, QDomNode node, INT32 level)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    QString tagname = e.tagName();
    //cerr << "\tADF_RECTANGLE: tagName = " << tagname << endl;

    // Obtain tag attributes
    //
    // We can define a rectangle to display a Node or an Edge
    // 1.1) node
    QString sRectangleNode = e.attribute(ADF_NODE_ATTRIB).stripWhiteSpace();
    if(sRectangleNode != QString::null)
    {
        if(!processFloorplanRectangleNode(rectangle, sRectangleNode, e)) 
        {
            return false;
        }
    }

    // 1.2) edge
    QString sRectangleEdge = e.attribute(ADF_EDGE_ATTRIB).stripWhiteSpace();
    if(sRectangleEdge != QString::null)
    {
        if(!processFloorplanRectangleEdge(rectangle, sRectangleEdge, e)) 
        {
            return false;
        }
    }

    if((sRectangleNode == QString::null) && (sRectangleEdge == QString::null))
    {
        myLogMgr->addLog("Invalid rectangle statement in ADF, missing edge and node name.");
        return false;
    }
    
    // 2) border
    QString sRectangleBorder = e.attribute(ADF_BORDER_ATTRIB).stripWhiteSpace();
    if (sRectangleBorder != QString::null)
    {
        rectangle->setBorder(sRectangleBorder == "y");
        //cerr << "\tADF_RECTANGLE: border = " << sRectangleBorder << endl; 
    }

    // 3) slot border
    QString sSlotBorder = e.attribute(ADF_SBORDER_ATTRIB).stripWhiteSpace();
    if (sSlotBorder != QString::null)
    {
        rectangle->setSlotBorder(sSlotBorder == "y");
        //cerr << "\tADF_RECTANGLE: sborder = " << sSlotBorder << endl; 
    }

    // 4) margin
    QString sRectangleMargin = e.attribute(ADF_MARGIN_ATTRIB).stripWhiteSpace();
    if(sRectangleMargin != QString::null)
    {
        bool bOk;
        UINT32 nMargin = sRectangleMargin.toUInt(&bOk);
        if(!bOk)
        {
            myLogMgr->addLog("Invalid rectangle margin width specifyer : " + sRectangleMargin + ". It is not a valid unsigned number.");
            return false;
        }
        rectangle->setMargin(nMargin);
        //cerr << "\tADF_RECTANGLE: margin = " << sRectangleMargin << endl; 
    }

    // 5) bgcolor
    QString sRectangleBGColor = e.attribute(ADF_BGCOLOR_ATTRIB).stripWhiteSpace();
    if (sRectangleBGColor != QString::null)
    {
        QColor bgColor(sRectangleBGColor);
        if(!bgColor.isValid())
        {
            myLogMgr->addLog("Invalid rectangle background color : " + sRectangleBGColor + " in ADF.");
            return false;
        }
        rectangle->setBGColor(bgColor);
        //cerr << "\tADF_RECTANGLE: bgcolor = " << sRectangleBGColor << endl; 
    }

    // 5) size
    QString sRectangleSize = e.attribute(ADF_SIZE_ATTRIB).stripWhiteSpace();
    if (sRectangleSize != QString::null)
    {
        int nXPos = sRectangleSize.find('x', 0, false);
        if(nXPos < 0)
        {
            myLogMgr->addLog("Invalid DRAL-TAG size: " + sRectangleSize + " in ADF.");
            return false;
        }

        bool ok = true;
        UINT32 nHeigh = sRectangleSize.left(nXPos).toUInt(&ok);
        if(!ok)
        {
            myLogMgr->addLog("Invalid DRAL-TAG size: " + sRectangleSize + " in ADF.");
            return false;
        }
        rectangle->setCellHeigh(nHeigh);

        UINT32 nWidth = sRectangleSize.right(sRectangleSize.length() - (nXPos + 1)).toUInt(&ok);
        if(!ok)
        {
            myLogMgr->addLog("Invalid DRAL-TAG size: " + sRectangleSize + " in ADF.");
            return false;
        }
        rectangle->setCellWidth(nWidth);
        //cerr << "\tADF_RECTANGLE: size = " << sRectangleSize << endl; 
    }

    // 5.1) title
    QString sRectangleTitle = e.attribute(ADF_TITLE_ATTRIB).stripWhiteSpace().lower();
    if (sRectangleTitle != QString::null)
    {
        LayoutFloorplanRectangle::TitlePos tpPos = LayoutFloorplanRectangle::NoTitle;
        if(sRectangleTitle == "north")
        {
            tpPos = LayoutFloorplanRectangle::North;
        }
        else if(sRectangleTitle == "south")
        {
            tpPos = LayoutFloorplanRectangle::South;
        }
        else if(sRectangleTitle == "west")
        {
            tpPos = LayoutFloorplanRectangle::West;
        }
        else if(sRectangleTitle == "east")
        {
            tpPos = LayoutFloorplanRectangle::East;
        }
        else 
        {
            myLogMgr->addLog("Invalid rectangle title position: " + sRectangleTitle + " in ADF.");
            return false;
        }

        rectangle->setTitle(tpPos);
        //cerr << "\tADF_RECTANGLE: title = " << sRectangleTitle << endl; 
    }

    // 5.2) titleontop
    QString sRectangleTitleOnTop = e.attribute(ADF_TITLEONTOP_ATTRIB).stripWhiteSpace().lower();
    if(sRectangleTitleOnTop != QString::null)
    {
        rectangle->setTitleOnTop(sRectangleTitleOnTop == "y");
        //cerr << "\tADF_RECTANGLE: titleontop = " << sRectangleTitleOnTop << endl; 
    }

    // 6) tagIdColor
    // Indicate to the rectangle which is its tag id that holds its color.
    // We take as representative track id the trackId of the first slot of
    // the node.
    TRACK_ID trackId = rectangle->getTrackIdForCell(0);
    TAG_ID tagIdColor = getTagIdColor(trackId);
    //cerr << "\tADF_RECTANGLE: trackId = " << trackId << " tagIdColor = " << tagIdColor << endl; 
    rectangle->setTagIdColor(tagIdColor);

    // 7) Color filters
    bool bFilterUsed = false;

    // 7.1) Time color filter
    // We obtain the number of tabs used in the temporal filter
    QString sRectangleTFilter = e.attribute(ADF_TFILTER_ATTRIB).stripWhiteSpace();
    if(sRectangleTFilter != QString::null)
    {
        bool bOk;
        UINT32 nTFilter = sRectangleTFilter.toUInt(&bOk);
        if(!bOk)
        {
            myLogMgr->addLog("Invalid rectangle temporal filter width specifyer : " + sRectangleTFilter + ". It is not a valid unsigned number.");
            return false;
        }
        
        // We obtain another tagIdColor used for store the filter
        if(!bFilterUsed)
        {
            tagIdColor = getTagIdColor(trackId);
        }
        rectangle->setTFilter(tagIdColor, nTFilter);
        bFilterUsed = true;
        //cerr << "\tADF_RECTANGLE: tfilter = " << sRectangleTFilter << endl; 
    }

    ExpressionList rules;
    bool ok = true;
    QDomNode cnode = e.firstChild();
    while(!cnode.isNull() && ok)
    {
        QDomElement celem = cnode.toElement(); // try to convert the node to an element.
        QString tagname = celem.tagName();

        if(tagname == ADF_LAYOUT)  
        { 
            ok = processFloorplanLayoutStatement(rectangle, cnode); 
        }
        else if(tagname == ADF_RULE)
        { 
            ok = processRuleStatement(cnode, rules, EXPRESSION_NORMAL);
            if(!ok)
            {
                return false;
            }
        }
        else
        { 
            myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " inside dreams3 rectangle statement.");
            return false;
        }

        cnode = cnode.nextSibling();
    }

    if(!rules.isEmpty())
    {
        rectangle->addDefaultRules(rules, level + 1);
    }

    return ok;
}

/*
 * Parses Floorplan rectangle node statement.
 *
 * @param rectangle is the actual scope.
 * @param sRectangleNode is the node name
 * @param elem contains XML information.
 *
 * @return if the parse is ok.
 */
bool 
ADFParser::processFloorplanRectangleNode(LayoutFloorplanRectangle * rectangle, QString sRectangleNode, QDomElement elem)
{
    sRectangleNode = draldb->normalizeNodeName(sRectangleNode);
    DralGraphNode * dralNode = draldb->getNode(sRectangleNode);
    if(dralNode == NULL)
    {
        myLogMgr->addLog("Invalid node statement, unkown node " + sRectangleNode + ".");
        return false;
    }

    rectangle->setNode(dralNode->getNodeId());
    //cerr << "\tADF_RECTANGLE: node = " << sRectangleNode << endl; 

    // Registers all the node slots
    //cerr << "\tADF_RECTANGLE: dimensions = " << dralNode->getDimensions() << " capacities = " << dralNode->getCapacities() << endl; 
    if(dralNode->getDimensions() == 0)
    {
        myLogMgr->addLog("Invalid rectangle statement in ADF, node " + sRectangleNode + " without dimensions defined.");
        return false;
    }

    rectangle->setCapacity(dralNode->getDimensions(), dralNode->getCapacities());

    return true;
}

/*
 * Parses Floorplan rectangle node statement.
 *
 * @param rectangle is the actual scope.
 * @param sRectangleEdge is the edge name.
 * @param elem contains XML information.
 *
 * @return if the parse is ok.
 */
bool
ADFParser::processFloorplanRectangleEdge(LayoutFloorplanRectangle * rectangle, QString sRectangleEdge, QDomElement elem)
{
    QString sFrom = elem.attribute(ADF_FROM_ATTRIB).stripWhiteSpace();
    if(sFrom == QString::null)
    {
        myLogMgr->addLog("Invalid edge statement in ADF, missing 'from' attribute.");
        return false;
    }

    QString sTo = elem.attribute(ADF_TO_ATTRIB).stripWhiteSpace();
    if(sTo == QString::null)
    {
        myLogMgr->addLog("Invalid edge statement in ADF, missing 'to' attribute.");
        return false;
    }

    sFrom = draldb->normalizeNodeName(sFrom);
    sTo = draldb->normalizeNodeName(sTo);

    // EDGE checking.
    DralGraphNode * nodeFrom = draldb->getNode(sFrom);
    DralGraphNode * nodeTo = draldb->getNode(sTo);

    if(nodeFrom == NULL)
    {
        QString err = "Invalid edge statement, wrong source node: " + sFrom + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    if(nodeTo == NULL)
    {
        QString err = "Invalid edge statement, wrong destination node: " + sTo + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    DralGraphEdge * dbEdge = draldb->findEdgeByNameFromTo(sRectangleEdge, sFrom, sTo);
    if(dbEdge == NULL)
    {
        QString err = "Invalid edge statement, edge not found in DRAL Graph. Source node = " + sFrom + ", destination node = " + sTo + ", edge name = " + sRectangleEdge + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    rectangle->setEdge(dbEdge->getEdgeId());
    //cerr << "\tADF_RECTANGLE: edge = " << sRectangleEdge << " from = " << sFrom << " to = " << sTo << endl; 

    UINT32 capacity[] = { dbEdge->getBandwidth(), dbEdge->getLatency() };
    rectangle->setCapacity(2, capacity);

    return true;
}

/*
 * Parses Floorplan rectangle layout statement.
 *
 * @param rectangle is the actual scope.
 * @param node contains XML information.
 *
 * @return if the parse is ok.
 */
bool 
ADFParser::processFloorplanLayoutStatement(LayoutFloorplanRectangle * rectangle, QDomNode node)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    UINT32 nDimension = 0;
    bool ok = true;
    QDomNode cnode = e.firstChild();

    while(!cnode.isNull() && ok)
    {
        QDomElement celem = cnode.toElement(); // try to convert the node to an element.
        QString tagname = celem.tagName();
        if(tagname == ADF_DIMENSION)
        { 
            QString sDimensionLayout = celem.attribute(ADF_VALUE_ATTRIB).stripWhiteSpace();
            if(sDimensionLayout != QString::null)
            {
                // With this definition we avoid the creation of a RegExp each time we must look for a erroneus layout
                static QRegExp qregexpLayout("\\d+\\s*x\\s*\\d+");

                if(nDimension >= rectangle->getDimensions())
                {
                    myLogMgr->addLog("Too many dimensions defined for rectangle " + rectangle->getName() + " in ADF.");
                    ok = false;
                }
                else if(sDimensionLayout == "h")
                {
                    rectangle->setLayout(nDimension, 1, rectangle->getCapacity(nDimension));
                }
                else if(sDimensionLayout == "v")
                {
                    rectangle->setLayout(nDimension, rectangle->getCapacity(nDimension), 1);
                }
                else if(sDimensionLayout.find(qregexpLayout) == 0)
                {
                    int nXPos = sDimensionLayout.find('x', 0, false);
                    if(nXPos < 0)
                    {
                        myLogMgr->addLog("Invalid DRAL-TAG geometry: " + sDimensionLayout + " in ADF.");
                        ok = false;
                    }
                    else 
                    {
                        UINT32 nRow = sDimensionLayout.left(nXPos).toUInt(&ok);
                        UINT32 nCol = sDimensionLayout.right(sDimensionLayout.length() - (nXPos + 1)).toUInt(&ok);

                        if(!ok) 
                        { 
                            myLogMgr->addLog("Invalid DRAL-TAG geometry: " + sDimensionLayout + " in ADF."); 
                        }
                        else if((nRow * nCol) != rectangle->getCapacity(nDimension))
                        {
                            myLogMgr->addLog("Invalid DRAL-TAG geometry: " + sDimensionLayout + " in ADF. Bad element element number defined for dimension " + QString::number(nDimension) + ". Defined capacity is " + QString::number(rectangle->getCapacity(nDimension))); 
                            ok = false;
                        }
                        else    
                        { 
                            rectangle->setLayout(nDimension, nRow, nCol); 
                        }
                    }
                }
            }
            else
            {
                myLogMgr->addLog("TAG " + tagname + " without value attribute in ADF."); 
                ok = false;
            }
            //cerr << "\tADF_LAYOUT: dimension = " << sDimensionLayout << endl; 
            nDimension++;
        }
        else
        { 
            myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " inside dreams3 layout statement.");
            ok = false;
        }
        cnode = cnode.nextSibling();
    }

    if(nDimension != rectangle->getDimensions())
    {
        myLogMgr->addLog("Too few dimensions defined for rectangle " + rectangle->getName() + " in ADF.");
        ok = false;
    }

    return ok;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// WaterFall Methods ///////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*
 * Parses WaterFall statement.
 *
 * @param node the node containing the XML content to parse.
 *
 * @return if the parse is ok.
 */
bool
ADFParser::processDreamsWFStatement(QDomNode node)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    QString tagname = e.tagName();
    //cerr << "\tADF_DREAMSW: tagName = " << tagname << endl; 

    ExpressionList defaultRules;
    ExpressionList overrideRules;

    dreamsdb->layoutWaterfallCreateLayout();

    bool ok = true;
    QDomNode cnode = e.firstChild();
    while(!cnode.isNull() && ok)
    {
        QDomElement celem = cnode.toElement(); // try to convert the node to an element.
        QString tagname = celem.tagName();
        
        if(!celem.isNull()) // comments in the XML
        {
            myLogMgr->addLog("WF node " + tagname + " in ADF.");

            if(tagname == ADF_MOVEITEM)
            { 
                ok = processDreamsWFMoveItemStatement(cnode);  
            }
            else if(tagname == ADF_ENTERNODE)
            { 
                ok = processDreamsWFEnterNodeStatement(cnode);  
            }
            else if(tagname == ADF_EXITNODE)
            { 
                ok = processDreamsWFExitNodeStatement(cnode);  
            }
            else if(tagname == ADF_DEFAULT)
            { 
                ok = processRuleEnvStatement(cnode, defaultRules, EXPRESSION_NORMAL);
            }
            else if(tagname == ADF_OVERRIDE)
            { 
                ok = processRuleEnvStatement(cnode, overrideRules, EXPRESSION_NORMAL);
            }
            else
            { 
                myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " inside dreamsWF statement.");
                return false;
            }
        }

        cnode = cnode.nextSibling();
    }

    return ok;
}

/*
 * Parses a move item waterfall statement.
 *
 * @param node the node containing the XML content to parse.
 *
 * @return if the parse is ok.
 */
bool
ADFParser::processDreamsWFMoveItemStatement(QDomNode node)
{
    bool bOk = true;

    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    QString tagname = e.tagName();
    //cerr << "\tADF_WF: tagName = " << tagname << endl; 

    // Obtain tag attributes
    
    // 1) edge
    QString sEdge = e.attribute(ADF_EDGE_ATTRIB).stripWhiteSpace();
    if(sEdge == QString::null)
    {
        myLogMgr->addLog("Invalid moveitem statement in ADF, missing edge attribute.");
        return false;
    }

    // 2) from
    QString sFrom = e.attribute(ADF_FROM_ATTRIB).stripWhiteSpace();
    if(sFrom == QString::null)
    {
        myLogMgr->addLog("Invalid moveitem statement in ADF, missing 'from' attribute.");
        return false;
    }

    // 3) to
    QString sTo = e.attribute(ADF_TO_ATTRIB).stripWhiteSpace();
    if(sTo == QString::null)
    {
        myLogMgr->addLog("Invalid moveitem statement in ADF, missing 'to' attribute.");
        return false;
    }

    // validate described edge
    sFrom = draldb->normalizeNodeName(sFrom);
    sTo = draldb->normalizeNodeName(sTo);

    // EDGE checking.
    DralGraphNode * nodeFrom = draldb->getNode(sFrom);
    DralGraphNode * nodeTo = draldb->getNode(sTo);

    if(nodeFrom == NULL)
    {
        QString err = "Invalid moveitem statement, wrong source node: " + sFrom + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    if(nodeTo == NULL)
    {
        QString err = "Invalid moveitem statement, wrong destination node: " + sTo + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    DralGraphEdge * dbEdge = draldb->findEdgeByNameFromTo(sEdge, sFrom, sTo);
    if(dbEdge == NULL)
    {
        QString err = "Invalid moveitem statement, edge not found in DRAL Graph. Source node = " + sFrom + ", destination node = " + sTo + ", edge name = " + sEdge + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }
    
    bool addok = dreamsdb->layoutWaterfallAddMoveItem(dbEdge);
    if (!addok)
    {
        QString err = "Invalid repeated moveitem statement. Source node = " + sFrom + ", destination node = " + sTo + ", edge name = " + sEdge + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }
    
    ExpressionList lRules;
    QDomNode cnode = e.firstChild();
    while(!cnode.isNull() && bOk)
    {
        QDomElement celem = cnode.toElement(); // try to convert the node to an element.
        QString tagname = celem.tagName();

        if(tagname == ADF_RULE)
        { 
            bOk = processRuleStatement(cnode, lRules, EXPRESSION_NORMAL);
        }
        else
        { 
            myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " inside dreams water fall move item statement."); 
            bOk = false; 
        }

        cnode = cnode.nextSibling();
    }

    if(bOk && !lRules.isEmpty())
    {
        dreamsdb->layoutWaterfallAddMoveItemRules(dbEdge,lRules);
    }

    return bOk;
}

/*
 * Parses an enter node waterfall statement.
 *
 * @param node the node containing the XML content to parse.
 *
 * @return if the parse is ok.
 */
bool
ADFParser::processDreamsWFEnterNodeStatement(QDomNode node)
{
    bool bOk = true;

    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    QString tagname = e.tagName();
    //cerr << "\tADF_WF: tagName = " << tagname << endl; 

    // Obtain tag attributes

    // node name
    QString eNode = e.attribute(ADF_NODE_ATTRIB).stripWhiteSpace();
    if(eNode == QString::null)
    {
        myLogMgr->addLog("Invalid enternode statement in ADF, missing node attribute.");
        return false;
    }

    eNode = draldb->normalizeNodeName(eNode);
    DralGraphNode* dralNode = draldb->getNode(eNode);
    if (dralNode == NULL)
    {
        myLogMgr->addLog("Invalid node statement, unkown node " + eNode + ".");
        return false;
    }
    
    bool addok = dreamsdb->layoutWaterfallAddEnterNode(dralNode);
    if (!addok)
    {
        QString err = "Invalid repeated enternode statement on " + eNode + ".";
        myLogMgr->addLog(err);
        return false;
    }
    
    ExpressionList lRules;
    QDomNode cnode = e.firstChild();
    while(!cnode.isNull() && bOk)
    {
        QDomElement celem = cnode.toElement(); // try to convert the node to an element.
        QString tagname = celem.tagName();

        if(tagname == ADF_RULE)
        { 
            bOk = processRuleStatement(cnode, lRules, EXPRESSION_NORMAL);
        }
        else
        { 
            myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " inside dreams waterfall enter node statment."); 
            bOk = false; 
        }

        cnode = cnode.nextSibling();
    }

    if(bOk && !lRules.isEmpty())
    {
        dreamsdb->layoutWaterfallAddEnterNodeRules(dralNode,lRules);
    }

    return bOk;
}

/*
 * Parses an exit node waterfall statement.
 *
 * @param node the node containing the XML content to parse.
 *
 * @return if the parse is ok.
 */
bool
ADFParser::processDreamsWFExitNodeStatement(QDomNode node)
{
    bool bOk = true;

    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    QString tagname = e.tagName();
    //cerr << "\tADF_WF: tagName = " << tagname << endl; 

    // Obtain tag attributes
   
    // node name
    QString eNode = e.attribute(ADF_NODE_ATTRIB).stripWhiteSpace();
    if(eNode == QString::null)
    {
        myLogMgr->addLog("Invalid exitnode statement in ADF, missing node attribute.");
        return false;
    }

    eNode = draldb->normalizeNodeName(eNode);
    DralGraphNode* dralNode = draldb->getNode(eNode);
    if (dralNode == NULL)
    {
        myLogMgr->addLog("Invalid exitnode statement, unkown node " + eNode + ".");
        return false;
    }
    
    bool addok = dreamsdb->layoutWaterfallAddExitNode(dralNode);
    if (!addok)
    {
        QString err = "Invalid repeated exitnode statement on " + eNode + ".";
        myLogMgr->addLog(err);
        return false;
    }
    
    ExpressionList lRules;
    QDomNode cnode = e.firstChild();
    while(!cnode.isNull() && bOk)
    {
        QDomElement celem = cnode.toElement(); // try to convert the node to an element.
        QString tagname = celem.tagName();

        if(tagname == ADF_RULE)
        { 
            bOk = processRuleStatement(cnode, lRules, EXPRESSION_NORMAL);
        }
        else
        { 
            myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " inside dreams waterfall enter node statment.");
            bOk = false;
        }

        cnode = cnode.nextSibling();
    }

    if(bOk && !lRules.isEmpty())
    {
        dreamsdb->layoutWaterfallAddExitNodeRules(dralNode,lRules);
    }

    return bOk;
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// WatchWindow Methods //////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/*
 * Parses Watch Window statement.
 *
 * @param node the node containing the XML content to parse.
 *
 * @return if the parse is ok.
 */
bool
ADFParser::processDreamsWWStatement(QDomNode node)
{
    bool bOk = true;

    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    QString tagname = e.tagName();
    //cerr << "\tADF_WATCHWINDOW: tagName = " << tagname << endl; 

    if(!dreamsdb->layoutWatchHasLayout())
        dreamsdb->layoutWatchCreateLayout();

    WatchWindow * pwwCurrent = new WatchWindow(draldb);

    // Obtain tag attributes
    // We can define a watch window to display a Node or an Edge
    // 1.1) node
    QString sWatchWindowNode = e.attribute(ADF_NODE_ATTRIB).stripWhiteSpace();
    if(sWatchWindowNode != QString::null)
    {
        bOk = processDreamsWWNode(pwwCurrent, sWatchWindowNode, e); 
    }

    // 1.2) edge
    QString sWatchWindowEdge = e.attribute(ADF_EDGE_ATTRIB).stripWhiteSpace();
    if(sWatchWindowEdge != QString::null)
    {
        bOk = processDreamsWWEdge(pwwCurrent, sWatchWindowEdge, e); 
    }

    if((sWatchWindowNode == QString::null) && (sWatchWindowEdge != QString::null))
    {
        myLogMgr->addLog("Invalid watch window statement in ADF, missing edge and node name.");
        bOk = false;
    }

    ExpressionList lRules;

    QDomNode cnode = e.firstChild();
    while(!cnode.isNull() && bOk)
    {
        QDomElement celem = cnode.toElement(); // try to convert the node to an element.
        QString tagname = celem.tagName();

        if(tagname == ADF_SHOW)
        { 
            bOk = processDreamsWWShowStatement(cnode, pwwCurrent);  
        }
        else if(tagname == ADF_POINTER)
        { 
            bOk = processDreamsWWPointerStatement(cnode, pwwCurrent);  
        }
        else if(tagname == ADF_RULE)
        { 
            bOk = processRuleStatement(cnode, lRules, EXPRESSION_NORMAL);
        }
        else
        { 
            myLogMgr->addLog("ADF Error: Unknown TAG " + tagname + " inside dreams watch window statement."); 
            bOk = false; 
        }

        cnode = cnode.nextSibling();
    }

    if(bOk && !lRules.isEmpty())
        pwwCurrent->addRules(lRules);


    // 2) orderby
    QString sOrderBy = e.attribute(ADF_ORDERBY_ATTRIB).stripWhiteSpace();
    TAG_ID orderbyTagId;
    if(sOrderBy != QString::null)
    {
        // Get the identifier for the given Tag String.
        orderbyTagId = draldb->getTagId(sOrderBy);
        if((orderbyTagId == TAG_ID_INVALID) || pwwCurrent->setOrderBy(orderbyTagId))
        {
            myLogMgr->addLog("Invalid orderby attribute in watch window reference: " + sOrderBy + " in ADF.");
            bOk = false;
        }
    }

    // 3) tagIdColor
    TRACK_ID trackId = pwwCurrent->getTrackIdForCell(0);
    TAG_ID tagIdColor = getTagIdColor(trackId);
    pwwCurrent->setTagIdColor(tagIdColor);

    dreamsdb->layoutWatchAddWindow(pwwCurrent);

    if(!bOk)
    {
        delete pwwCurrent;
    }
    return bOk;
}

/*
 * Parses a watch window node statement.
 *
 * @param window the watch window.
 * @param sWatchWindowNode the node name.
 * @param elem contains the XML content to parse.
 *
 * @return if the parse is ok.
 */
bool 
ADFParser::processDreamsWWNode(WatchWindow * window, QString sWatchWindowNode, QDomElement elem)
{
    sWatchWindowNode = draldb->normalizeNodeName(sWatchWindowNode);
    DralGraphNode* dralNode = draldb->getNode(sWatchWindowNode);
    if (dralNode == NULL)
    {
        myLogMgr->addLog("Invalid node statement, unkown node " + sWatchWindowNode + ".");
        return false;
    }

    window->setNode(dralNode->getNodeId());

    // Registers all the node slots
    if(dralNode->getDimensions() == 0)
    {
        myLogMgr->addLog("Invalid rectangle statement in ADF, node " + sWatchWindowNode + " without dimensions defined.");
        return false;
    }

    window->setCapacity(dralNode->getDimensions(), dralNode->getCapacities());

    return true;
}

/*
 * Parses a watch window node statement.
 *
 * @param window the watch window.
 * @param sWatchWindowEdge the edge name.
 * @param elem contains the XML content to parse.
 *
 * @return if the parse is ok.
 */
bool 
ADFParser::processDreamsWWEdge(WatchWindow * window, QString sWatchWindowEdge, QDomElement elem)
{
    QString sFrom = elem.attribute(ADF_FROM_ATTRIB).stripWhiteSpace();
    if(sFrom == QString::null)
    {
        myLogMgr->addLog("Invalid edge statement in ADF, missing 'from' attribute.");
        return false;
    }

    QString sTo = elem.attribute(ADF_TO_ATTRIB).stripWhiteSpace();
    if(sTo == QString::null)
    {
        myLogMgr->addLog("Invalid edge statement in ADF, missing 'to' attribute.");
        return false;
    }

    sFrom = draldb->normalizeNodeName(sFrom);
    sTo = draldb->normalizeNodeName(sTo);

    // EDGE checking.
    DralGraphNode * nodeFrom = draldb->getNode(sFrom);
    DralGraphNode * nodeTo = draldb->getNode(sTo);

    if(nodeFrom == NULL)
    {
        QString err = "Invalid edge statement, wrong source node: " + sFrom + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    if(nodeTo == NULL)
    {
        QString err = "Invalid edge statement, wrong destination node: " + sTo + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    DralGraphEdge * dbEdge = draldb->findEdgeByNameFromTo(sWatchWindowEdge, sFrom, sTo);
    if(dbEdge == NULL)
    {
        QString err = "Invalid edge statement, edge not found in DRAL Graph. Source node = " + sFrom + ", destination node = " + sTo + ", edge name = " + sWatchWindowEdge + " in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    window->setEdge(dbEdge->getEdgeId());

    UINT32 capacity[] = { dbEdge->getBandwidth(), dbEdge->getLatency() };
    window->setCapacity(2, capacity);

    return true;
}

/*
 * Parses a show statement.
 *
 * @param node the node containing the XML content to parse.
 * @param window is the watch window.
 *
 * @return if the parse is ok.
 */
bool 
ADFParser::processDreamsWWShowStatement(QDomNode node, WatchWindow * window)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    QString tagname = e.tagName();
    //cerr << "\tADF_SHOW: tagName = " << tagname << endl;

    // Obtain tag attributes
    //
    // 1) stag / itag
    bool bSTag = true;
    QString sWatchTag = e.attribute(ADF_STAGNAME_ATTRIB).stripWhiteSpace();
    if(sWatchTag == QString::null)
    {
        bSTag = false;
        sWatchTag = e.attribute(ADF_ITAGNAME_ATTRIB).stripWhiteSpace();
    }

    if(sWatchTag == QString::null)
    {
        myLogMgr->addLog("Invalid watch statement in ADF, missing itag and stag definition.");
        return false;
    }

    TAG_ID tagId = draldb->getTagId(sWatchTag);
    if(tagId == TAG_ID_INVALID)
    {
        myLogMgr->addLog("Invalid watch statement in ADF, invalid itag/stag name.");
        return false;
    }
    
    //cerr << "\tADF_SHOW: stag/itag = " << sWatchTag << " tagId = " << tagId << endl;

    // 2) width
    QString sWatchWidth = e.attribute(ADF_WIDTH_ATTRIB).stripWhiteSpace();
    UINT32 width = 0;
    if (sWatchWidth != QString::null)
    {
        bool bOk = true;
        width = sWatchWidth.toInt(&bOk, 10);
        if(!bOk)
        {
            myLogMgr->addLog ("Invalid watch definition in ADF, invalid width specification.");
            return false;
        }
    }

    // 3) align
    QString sWatchAlign = e.attribute(ADF_ALIGN_ATTRIB).stripWhiteSpace();
    if (sWatchAlign != QString::null)
    {
        if((sWatchAlign != "left") && (sWatchAlign != "right") && (sWatchAlign != "center"))
        {
            myLogMgr->addLog ("Invalid watch definition in ADF, invalid align specification.");
            return false;
        }
    }

    // We check that it hasn't any child defined
    if(!e.firstChild().isNull())
    {
        myLogMgr->addLog("Invalid watch ADF definition. <watch> shouldn't have subtags defined.");
        return false;
    }

    WatchColumn *pwcCurrent = new WatchColumn(bSTag, tagId);
    pwcCurrent->setWidth(width);
    pwcCurrent->setAlign(sWatchAlign);

    window->add(pwcCurrent);
    return true;
}

/*
 * Parses a pointer statement.
 *
 * @param node the node containing the XML content to parse.
 * @param window is the watch window.
 *
 * @return if the parse is ok.
 */
bool
ADFParser::processDreamsWWPointerStatement(QDomNode node, WatchWindow * window)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.

    QString tagname = e.tagName();
    //cerr << "\tADF_SHOW: tagName = " << tagname << endl;

    // Obtain tag attributes
    //
    // 1) ntag 
    bool bSTag = true;
    QString sPointerTag = e.attribute(ADF_NTAGNAME_ATTRIB).stripWhiteSpace();
    if(sPointerTag == QString::null)
    {
        myLogMgr->addLog("Invalid pointer statement in ADF, missing ntag definition.");
        return false;
    }

    TAG_ID tagId = draldb->getTagId(sPointerTag);
    if(tagId == TAG_ID_INVALID)
    {
        myLogMgr->addLog("Invalid pointer statement in ADF, invalid ntag name.");
        return false;
    }
    
    //cerr << "\tADF_POINTER: ntag = " << sPointerTag << " tagId = " << tagId << endl;

    // 2) name
    QString sPointerName = e.attribute(ADF_NAME_ATTRIB).stripWhiteSpace();
    if(sPointerName == QString::null)
    {
        myLogMgr->addLog("Invalid pointer statement in ADF, missing pointer name definition.");
        return false;
    }

    // We check that it hasn't any child defined
    if(!e.firstChild().isNull())
    {
        myLogMgr->addLog("Invalid watch ADF definition. <pointer> shouldn't have subtags defined.");
        return false;
    }

    // We add a pointer column to the watchwindow.
    WatchColumn *pwcCurrent = new WatchColumn(true, tagId, true);
    pwcCurrent->setPointerName(sPointerName);

    window->addPointer(pwcCurrent);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Old Methods //////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///*
// * This function parses a set statement and creates a set expression
// * that is added into the expression list. This is only called as a
// * root sentence or inside an edge statement.
// *
// * @param node contains the parsing information.
// * @param dfsNum is a pointer to the actual rule number.
// * @param rules is a pointer to a list where the set expression must be added.
// *
// * @return if the node was checked correctly.
// */
//bool
//ADFParser::processSetStatement(QDomNode node, INT32 * dfsNum, ExpressionList * rules)
//{
//    QDomElement e;
//    Expression * expr;
//    Expression * right;
//    QString error;
//    QString err;
//
//    // Try to convert the node to an element.
//    e = node.toElement();
//    Q_ASSERT(!e.isNull());
//
//    // look for the valid attributes
//    // 1) color
//    QString colorstr = e.attribute(ADF_COLOR_ATTRIB).stripWhiteSpace();
//    if(colorstr != QString::null)
//    {
//        QColor coloratr = QColor(colorstr);
//        if(!coloratr.isValid())
//        {
//            err = "Invalid color declaration: " + colorstr + " in ADF(1).";
//            myLogMgr->addLog(err);
//            return false;
//        }
//
//        // Adds the rule to the list.
//        right = new ExpressionConstant(colorstr);
//        expr = new ExpressionSet(OperatorSetFColor, right, 0);
//        expr->setDescription(desc);
//        rules->append(expr);
//
//        // Evals the compute mask and type checking.
//        expr->evalComputeMask();
//        if(!expr->typeCheck(error))
//        {
//            err = QString("Type check error in rule ") + QString::number(* dfsNum) + QString(": ");
//            myLogMgr->addLog(err + error);
//            return false;
//        }
//    }
//
//    // 2) shape
//    QString shapestr = e.attribute(ADF_SHAPE_ATTRIB).stripWhiteSpace().lower();
//    if(shapestr != QString::null)
//    {
//        EventShape eshape;
//        bool valid = EventShapeToolkit::parseShape(shapestr, &eshape);
//        if(!valid)
//        {
//            QString err = "Invalid shape declaration: " + shapestr + " in ADF.";
//            myLogMgr->addLog(err);
//            return false;
//        }
//
//        // Adds the rule to the list.
//        right = new ExpressionConstant(eshape);
//        expr = new ExpressionSet(OperatorSetShape, right, 0);
//        expr->setDescription(desc);
//        rules->append(expr);
//
//        // Evals the compute mask and type checking.
//        expr->evalComputeMask();
//        if(!expr->typeCheck(error))
//        {
//            err = QString("Type check error in rule ") + QString::number(* dfsNum) + QString(": ");
//            myLogMgr->addLog(err + error);
//            return false;
//        }
//    }
//
//    // 3) letter
//    QString letterstr = e.attribute(ADF_LETTER_ATTRIB).stripWhiteSpace();
//    if(letterstr != QString::null)
//    {
//        if(letterstr.length() != 1)
//        {
//            QString err = "Invalid letter declaration: " + letterstr + " in ADF.";
//            myLogMgr->addLog(err);
//            return false;
//        }
//
//        // Adds the rule to the list.
//        right = new ExpressionConstant(letterstr.at(0).latin1());
//        expr = new ExpressionSet(OperatorSetLetter, right, 0);
//        expr->setDescription(desc);
//        rules->append(expr);
//
//        // Evals the compute mask and type checking.
//        expr->evalComputeMask();
//        if(!expr->typeCheck(error))
//        {
//            err = QString("Type check error in rule ") + QString::number(* dfsNum) + QString(": ");
//            myLogMgr->addLog(err + error);
//            return false;
//        }
//    }
//
//    * dfsNum = (* dfsNum) + 1;
//    return true;
//}
//
///*
// * This function parses an if statement and adds an if expression in the
// * actual scope. This function is called in both the root node function
// * and inside the edge statement.
// *
// * @param node contains the parsing information.
// * @param dfsNum is a pointer to the actual rule number.
// * @param global says if the if statement is global or belongs to the actual row.
// *
// * @return if the node was checked correctly.
// */
//bool
//ADFParser::processIfStatement(QDomNode node, INT32 * dfsNum, bool global)
//{
//    QDomElement e; 
//    QString error;
//
//    // Try to convert the node to an element.
//    e = node.toElement();
//    Q_ASSERT(!e.isNull());
//
//    if(!node.hasChildNodes())
//    {
//        myLogMgr->addLog("Invalid empty if statement in ADF.");
//        return false;
//    }
//
//    desc = "";
//    QTextStream dStream( &desc, IO_WriteOnly | IO_Append );
//    if(global)
//    {
//        dStream << "Global Scope Conditional Rule: \n";
//    }
//    else
//    {
//        dStream << "Row Scope Conditional Rule: \n";
//    }
//    node.save(dStream, 4);
//
//    // Check for if attributes.
//    // 1) tagname:
//    QString tgnamestr = e.attribute(ADF_TAGNAME_ATTRIB).stripWhiteSpace();
//    if(tgnamestr == QString::null)
//    {
//        myLogMgr->addLog("Invalid if statement in ADF, missing tagname attribute.");
//        return false;
//    }
//
//    // 2) value:
//    QString valuestr = e.attribute(ADF_VALUE_ATTRIB).stripWhiteSpace();
//    if(valuestr == QString::null)
//    {
//        myLogMgr->addLog("Invalid if statement in ADF, missing value attribute.");
//        return false;
//    }
//
//    // Get the identifier for the given Tag String.
//    TAG_ID tagId = draldb->getTagId(tgnamestr);
//    if(tagId == TAG_ID_INVALID)
//    {
//        QString err = "Invalid DRAL-TAG reference: " + tgnamestr + " in ADF.";
//        myLogMgr->addLog(err);
//        return false;
//    }
//
//    // Check the type of the used tag.
//    TagValueType tgType = draldb->getTagValueType(tagId);
//    if (tgType!=TAG_INTEGER_TYPE && tgType!=TAG_STRING_TYPE)
//    {
//        myLogMgr->addLog("Sorry, conditionals only supported with integer and string tags.");
//        return false;
//    }
//
//    Expression * expr_itag;            ///< Expression that will get the item tag value.
//    Expression * expr_const;           ///< Expression that will return the constant tag value.
//    Expression * expr_equal;           ///< Expression that will implement the equality.
//    Expression * expr_if;              ///< Expression that implements the if.
//    ExpressionList * expr_true_branch; ///< List of expressions that will be executed if the if evaluates to true.
//
//    expr_itag = NULL;
//    expr_const = NULL;
//    expr_equal = NULL;
//    expr_if = NULL;
//    expr_true_branch = NULL;
//
//    // Try to convert valustr into an UINT64.
//    UINT64 value;
//    if(tgType == TAG_INTEGER_TYPE)
//    {
//        bool numparsed;
//        value = AGTTypes::str2uint64(valuestr.latin1(), numparsed);
//        if(!numparsed)
//        {
//            myLogMgr->addLog("Invalid if statement in ADF, malformed value attribute.");
//            return false;
//        }
//
//        // Creates the integer constant expression.
//        expr_const = new ExpressionConstant(value);
//    }
//    else
//    {
//        //INT32 stridx = draldb->addString(valuestr);
//        //value = (UINT64) stridx;
//
//        // Creates the string constant expression.
//        expr_const = new ExpressionConstant(valuestr);
//    }
//
//    // Creates the itag and equal expressions.
//    expr_itag = new ExpressionTag(OperatorITag, draldb, tgnamestr);
//    expr_equal = new ExpressionCompare(OperatorEqual, expr_itag, expr_const);
//    expr_true_branch = new ExpressionList;
//    expr_if = new ExpressionIf(expr_equal, expr_true_branch, NULL);
//
//    QDomNode currentChild = e.firstChild();
//    QDomNode lChild = e.lastChild();
//    bool ok = true;
//
//    // Processes all the childs of the node.
//    while(!currentChild.isNull() && ok)
//    {
//        QDomElement eset = currentChild.toElement();
//        if(eset.isNull())
//        {
//            currentChild = currentChild.nextSibling();
//            continue;
//        }
//
//        if(eset.tagName() == ADF_SET)
//        {
//            ok = ok && processSetInsideIfStatement(eset, expr_true_branch);
//        }
//        else
//        {
//            myLogMgr->addLog("ADF Error: Invalid if statement.");
//            delete expr_if;
//            return false;
//        }
//
//        // Go to next.
//        currentChild = currentChild.nextSibling();
//    }
//
//    if(!ok)
//    {
//        delete expr_if;
//        return false;
//    }
//
//    // Evals the compute mask and type checking.
//    expr_if->evalComputeMask();
//    if(!expr_if->typeCheck(error))
//    {
//        QString err = QString("Type check error in rule ") + QString::number(* dfsNum) + QString(": ");
//        myLogMgr->addLog(err + error);
//        delete expr_if;
//        return false;
//    }
//
//    if(global)
//    {
//        dreamsdb->layoutResourceAddRules(expr_if);
//    }
//    else
//    {
//        dreamsdb->layoutResourceAddRowRules(expr_if);
//    }
//
//    * dfsNum = (* dfsNum) + 1;
//    return true;
//}
//
///*
// * This function parses an item if statement.
// *
// * @param eset contains the parsing information.
// * @param rules will contain the created expressions.
// *
// * @return if the node was checked correctly.
// */
//bool
//ADFParser::processSetInsideIfStatement(QDomElement eset, ExpressionList * rules)
//{
//    Expression * expr_set;
//    Expression * right;
//
//    // Look for the valid SET attributes.
//    // 1) color:
//    QString colorstr = eset.attribute(ADF_COLOR_ATTRIB).stripWhiteSpace();
//    if(colorstr != QString::null)
//    {
//        QColor coloratr = QColor(colorstr);
//        if(!coloratr.isValid())
//        {
//            QString err = "Invalid color declaration: " + colorstr + " in ADF(1).";
//            myLogMgr->addLog(err);
//            return false;
//        }
//
//        right = new ExpressionConstant(colorstr);
//        expr_set = new ExpressionSet(OperatorSetFColor, right, 0);
//        expr_set->setDescription(desc);
//        rules->append(expr_set);
//    }
//
//    // 2) shape:
//    QString shapestr = eset.attribute(ADF_SHAPE_ATTRIB).stripWhiteSpace().lower();
//    if(shapestr != QString::null)
//    {
//        EventShape eshape;
//        bool valid = EventShapeToolkit::parseShape(shapestr, &eshape);
//        if(!valid)
//        {
//            QString err = "Invalid shape declaration: " + shapestr + " in ADF.";
//            myLogMgr->addLog(err);
//            return false;
//        }
//
//        right = new ExpressionConstant(eshape);
//        expr_set = new ExpressionSet(OperatorSetShape, right, 0);
//        expr_set->setDescription(desc);
//        rules->append(expr_set);
//    }
//
//    // 3) letter:
//    QString letterstr = eset.attribute(ADF_LETTER_ATTRIB).stripWhiteSpace();
//    if(letterstr != QString::null)
//    {
//        if(letterstr.length() == 1)
//        {
//            QString err = "Invalid letter declaration: " + letterstr + " in ADF.";
//            myLogMgr->addLog(err);
//            return false;
//        }
//
//        right = new ExpressionConstant(letterstr.at(0).latin1());
//        expr_set = new ExpressionSet(OperatorSetLetter, right, 0);
//        expr_set->setDescription(desc);
//        rules->append(expr_set);
//    }
//    return true;
//}
