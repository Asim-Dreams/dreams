/*
 * Copyright (C) 2003-2006 Intel Corporation
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
 */

/**
  * @file ADFParser1.cpp
  */

#include "ADFParser1.h"

ADFParser1::ADFParser1(QDomDocument* ref) : ADFParserInterface(ref)
{
    myLogMgr = LogMgr::getInstance();
    cdb = ConfigDB::getInstance();
    myZDB = ZDBase::getInstance();
    draldb = DralDB::getInstance();
}

ADFParser1::~ADFParser1()
{
}

bool
ADFParser1::parseADF()
{
    Q_ASSERT(myADF!=NULL);

    //clearRules();

    INT32 dfsNum = 0;
    QDomElement docElem = myADF->documentElement();
    QDomNode n = docElem.firstChild();
    bool ok = true;
    while( !n.isNull() && ok )
    {
      ok = processDomNode(n,&dfsNum);
      n = n.nextSibling();
    }
    return (ok);
}

bool
ADFParser1::processDomNode(QDomNode node,INT32* dfsNum)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    if (e.isNull()) return (true);    // comments or similar stuff

    //cout << e.tagName() << endl;

    QString tagname = e.tagName();

    // check for known tags

    if      (tagname==ADF_2DREAMS)
    { return true; }
    else if (tagname==ADF_TAGDESC)
    { return processTagDescriptorStatement(node,dfsNum); }
    else if (tagname==ADF_IF)
    { return processIfStatement(node,dfsNum); }
    else if (tagname==ADF_SET)
    { return processSetStatement(node,dfsNum); }
    else if (tagname==ADF_SHADETABLE)
    { return processShadeTableStatement(node,dfsNum); }
    else if (tagname==ADF_EDGE)
    { return processEdgeStatement(node,dfsNum); }
    else if (tagname==ADF_NODE)
    { return processNodeStatement(node,dfsNum); }
    else if (tagname==ADF_BLANKROWS)
    { return processBlankStatement(node,dfsNum); }
    else if (tagname==ADF_ITEMWINDOW)
    { return processItemWindowStatement(node,dfsNum); }
    else
    {
        myLogMgr->addLog("Unknown TAG "+tagname+" in ADF.");
        return (false);
    }
}

bool
ADFParser1::processSetStatement(QDomNode node,INT32* dfsNum, INT16 row)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    // this is only called on naked set statements
    // in other words a <set ... />
    // but not on <if ...> <set ../> </if>

    // look for the valid attributes
    // 1) color
    QString colorstr = e.attribute(ADF_COLOR_ATTRIB).stripWhiteSpace();
    if (colorstr!=QString::null)
    {
        QColor coloratr =  QColor(colorstr);
        if (!coloratr.isValid())
        {
            QString err = "Invalid color declaration: "+colorstr+" in ADF(1).";
            myLogMgr->addLog(err);
            return (false);
        }

        if (row==-1)
        {
            cdb->defaultColor = coloratr;
            cdb->defaultColorDFSNum = *dfsNum;
            /*cout << "root level color definition: " << colorstr.latin1() <<
            " dfs:" << defaultColorDFSNum << endl;*/
        }
        else
        {
            RowRules* rr = cdb->getRowRules(row);
            if (rr==NULL) return false;
            rr->setColor(coloratr,*dfsNum,false);
            /*cout << "edge color definition: " << colorstr << " dfs:" << (*dfsNum) <<
            " edge:" << edgeID << endl;*/
        }
    }

    // 2) shape
    QString shapestr = e.attribute(ADF_SHAPE_ATTRIB).stripWhiteSpace().lower();
    if (shapestr!=QString::null)
    {
        EventShape eshape;
        bool valid =  EventShapeToolkit::parseShape(shapestr,&eshape);
        if (!valid)
        {
            QString err = "Invalid shape declaration: "+shapestr+" in ADF.";
            myLogMgr->addLog(err);
            return (false);
        }

        if (row==-1)
        {
            cdb->defaultShape = eshape;
            cdb->defaultShapeDFSNum = *dfsNum;
            /*cout << "root level shape definition: " << shapestr.latin1() <<
            " dfs:" << defaultShapeDFSNum << endl;*/
        }
        else
        {
            RowRules* rr = cdb->getRowRules(row);
            if (rr==NULL) return false;
            rr->setShape(eshape,*dfsNum);
            /*cout << "edge shape definition: " << shapestr.latin1() <<
            " dfs:" << defaultShapeDFSNum << "edge:" << edgeID << endl;*/
        }
    }

    // 3) letter
    QString letterstr = e.attribute(ADF_LETTER_ATTRIB).stripWhiteSpace();
    if (letterstr!=QString::null)
    {
        bool valid =  (letterstr.length() == 1);
        if (!valid)
        {
            QString err = "Invalid letter declaration: "+letterstr+" in ADF.";
            myLogMgr->addLog(err);
            return (false);
        }
        QChar letterattr = letterstr.at(0);

        if (row==-1)
        {
            cdb->defaultLetter = letterattr.latin1();
            cdb->defaultLetterDFSNum = *dfsNum;
            /*cout << "root level letter definition: " << letterstr.latin1() <<
            " dfs:" << defaultLetterDFSNum << endl;*/
        }
        else
        {
            RowRules* rr = cdb->getRowRules(row);
            if (rr==NULL) return false;
            rr->setLetter(letterattr.latin1(),*dfsNum);
            /*cout << "edge letter definition: " << letterstr.latin1() <<
            " dfs:" << defaultLetterDFSNum << "edge:" << edgeID << endl;*/
        }
    }

    *dfsNum = (*dfsNum)+1;
    return (true);
}

bool
ADFParser1::processIfStatement(QDomNode node,INT32* dfsNum, INT16 row, TabSpec* tab)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    if (!node.hasChildNodes())
    {
        myLogMgr->addLog("Invalid empty if statement in ADF.");
        return false;
    }

    // check for if attributes
    // 1) tagname:
    QString tgnamestr = e.attribute(ADF_TAGNAME_ATTRIB).stripWhiteSpace();
    if (tgnamestr==QString::null)
    {
        myLogMgr->addLog("Invalid if statement in ADF, missing tagname attribute.");
        return false;
    }

    // 2) value:
    QString valuestr = e.attribute(ADF_VALUE_ATTRIB).stripWhiteSpace();
    if (valuestr==QString::null)
    {
        myLogMgr->addLog("Invalid if statement in ADF, missing value attribute.");
        return false;
    }

    // get the identifier for the given Tag String
    INT32 tagId = draldb->scanTagName(tgnamestr);
    if (tagId<0)
    {
        QString err = "Invalid DRAL-TAG reference: " + tgnamestr+" in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    // check the type of the used tag..
    TagValueType tgType = draldb->getTagValueType(tagId);
    if (tgType!=TagIntegerValue && tgType!=TagStringValue)
    {
        myLogMgr->addLog("Sorry, conditionals only supported with integer and string tags.");
        return false;
    }

    // try to convert valustr into an UINT64
    UINT64 value;
    if (tgType==TagIntegerValue)
    {
        bool numparsed;
        value = AGTTypes::str2uint64(valuestr.latin1(),numparsed);
        if (!numparsed)
        {
            myLogMgr->addLog("Invalid if statement in ADF, malformed value attribute.");
            return false;
        }
    }
    else
    {
        INT32 stridx=draldb->addString(valuestr);
        value = (UINT64)stridx;
    }


    // color/letter/shape rules (sets)
    SetInsideIfData siid;
    siid.hasTvc = false;
    siid.hasTvs = false;
    siid.hasTvl = false;
    siid.tvcSet.dfsNum  = *dfsNum;
    siid.tvcSet.tagId   = tagId;
    siid.tvcSet.tagValue= value;
    siid.tvsSet.dfsNum  = *dfsNum;
    siid.tvsSet.tagId   = tagId;
    siid.tvsSet.tagValue= value;
    siid.tvlSet.dfsNum  = *dfsNum;
    siid.tvlSet.tagId   = tagId;
    siid.tvlSet.tagValue= value;

    // ItemFont rules
    ItemFontInsideIfData ifiid;
    ifiid.hasTvf = false;
    ifiid.tvfSet.tagId    = tagId;
    ifiid.tvfSet.tagValue = value;
    ifiid.tvfSet.dfsNum   = *dfsNum;
    ifiid.tvfSet.fontSpec.bold     = false;
    ifiid.tvfSet.fontSpec.underline= false;
    ifiid.tvfSet.fontSpec.italic   = false;
    ifiid.tvfSet.fontSpec.color    = QColor(0,0,0);

    QDomNode currentChild = e.firstChild();
    QDomNode lChild = e.lastChild();
    bool ok=true;
    while( !currentChild.isNull() && ok)
    {
        QDomElement eset = currentChild.toElement();
        if (eset.isNull())
        {
            currentChild = currentChild.nextSibling();
            continue;
        }

        if (eset.tagName()==ADF_SET)
        {
            ok = ok && processSetInsideIfStatement(eset,&siid);
        }
        else if (eset.tagName()==ADF_ITEMFONT)
        {
            ok = ok && processItemFontInsideIfStatement(eset,&ifiid);
        }
        else
        {
            myLogMgr->addLog("ADF Error: Invalid if statement.");
            return false;
        }
        // go to next
        currentChild = currentChild.nextSibling();
    }
    if (!ok)
    {
        return (false);
    }

    applySetInsideIfRules(&siid,row);

    if (ifiid.hasTvf)
    {
        // ItemFont rules doesnt make sense inside edge statements
        if (row!=-1)
        {
            myLogMgr->addLog("ADF Error: ItemFont cannot be declared inside an <edge>.");
            return (false);
        }
        else
        {
            if (tab==NULL)
            {
                //printf("adding conditional itemFont setting for global rule list\n");fflush(stdout);
                cdb->iwfrlist->append(ifiid.tvfSet);
            }
            else
            {
                //printf("adding conditional itemFont setting for tab %s\n",tab->getName().latin1());fflush(stdout);
                tab->addFontRule(ifiid.tvfSet);
            }
        }
    }

    *dfsNum = (*dfsNum)+1;
    return (true);
}

bool
ADFParser1::processItemFontInsideIfStatement(QDomElement eset,ItemFontInsideIfData* ifiid)
{
    //printf("processItemFontInsideIfStatement called...\n");fflush(stdout);
    // look for the valid itemfont attributes

    // 1) color
    QString colorstr = eset.attribute(ADF_COLOR_ATTRIB).stripWhiteSpace();
    if (colorstr!=QString::null)
    {
        QColor coloratr = QColor(colorstr);
        if (!coloratr.isValid())
        {
            QString err = "Invalid color declaration: "+colorstr+" in ADF(1).";
            myLogMgr->addLog(err);
            return (false);
        }
        ifiid->tvfSet.fontSpec.color = coloratr;
        ifiid->hasTvf = true;
    }

    // 2) bold
    QString boldstr = eset.attribute(ADF_BOLD_ATTRIB).stripWhiteSpace().lower();
    if (boldstr!=QString::null)
    {
        if (boldstr!="true" && boldstr!="false")
        {
            QString err = "Invalid bold attribute: "+boldstr+" in ADF.";
            myLogMgr->addLog(err);
            return (false);
        }
        ifiid->tvfSet.fontSpec.bold = boldstr=="true" ? true : false;
        ifiid->hasTvf = true;
    }

    // 3) underline
    QString underlinestr = eset.attribute(ADF_UNDERLINE_ATTRIB).stripWhiteSpace().lower();
    if (underlinestr!=QString::null)
    {
        if (underlinestr!="true" && underlinestr!="false")
        {
            QString err = "Invalid underline attribute: "+underlinestr+" in ADF.";
            myLogMgr->addLog(err);
            return (false);
        }
        ifiid->tvfSet.fontSpec.underline = underlinestr=="true" ? true : false;
        ifiid->hasTvf = true;
    }

    // 4) italic
    QString italicstr = eset.attribute(ADF_ITALIC_ATTRIB).stripWhiteSpace().lower();
    if (italicstr!=QString::null)
    {
        if (italicstr!="true" && italicstr!="false")
        {
            QString err = "Invalid italic attribute: "+italicstr+" in ADF.";
            myLogMgr->addLog(err);
            return (false);
        }
        ifiid->tvfSet.fontSpec.italic = italicstr=="true" ? true : false;
        ifiid->hasTvf = true;
    }
    return (true);
}

bool
ADFParser1::processSetInsideIfStatement(QDomElement eset,SetInsideIfData* siid)
{
    // look for the valid SET attributes
    // 1) color
    QString colorstr = eset.attribute(ADF_COLOR_ATTRIB).stripWhiteSpace();
    if (colorstr!=QString::null)
    {
        QColor coloratr =  QColor(colorstr);
        if (!coloratr.isValid())
        {
            QString err = "Invalid color declaration: "+colorstr+" in ADF(1).";
            myLogMgr->addLog(err);
            return (false);
        }
        siid->tvcSet.color   = coloratr;
        siid->hasTvc=true;
    }

    // 2) shape
    QString shapestr = eset.attribute(ADF_SHAPE_ATTRIB).stripWhiteSpace().lower();
    if (shapestr!=QString::null)
    {
        EventShape eshape;
        bool valid =  EventShapeToolkit::parseShape(shapestr,&eshape);
        if (!valid)
        {
            QString err = "Invalid shape declaration: "+shapestr+" in ADF.";
            myLogMgr->addLog(err);
            return (false);
        }
        siid->tvsSet.shape   = eshape;
        siid->hasTvs=true;
    }

    // 3) letter
    QString letterstr = eset.attribute(ADF_LETTER_ATTRIB).stripWhiteSpace();
    if (letterstr!=QString::null)
    {
        bool valid = (letterstr.length() == 1);
        if (!valid)
        {
            QString err = "Invalid letter declaration: "+letterstr+" in ADF.";
            myLogMgr->addLog(err);
            return (false);
        }
        QChar letterattr = letterstr.at(0);
        siid->tvlSet.letter  = letterattr.latin1();
        siid->hasTvl=true;
    }
    return (true);
}

void
ADFParser1::applySetInsideIfRules(SetInsideIfData* siid, INT16 row)
{
    RowRules* rr = NULL;
    if (row>=0) {rr=cdb->getRowRules(row);}

    if (siid->hasTvc)
    {
        ColorRuleList* scrlist;
        if (row==-1)
        {
            scrlist = cdb->crlist;
        }
        else
        {
            if (rr==NULL) { return; }
            scrlist = rr->getCrlist();
        }
        scrlist->append(siid->tvcSet);
        /*cout << "tagged color definition: " << colorstr.latin1() <<
        " dfs:" << tvcSet.dfsNum << " tag: " << tgnamestr.latin1() <<
        " value: " << value << endl;*/
    }

    if (siid->hasTvs)
    {
        ShapeRuleList* ssrlist;
        if (row==-1)
        {
            ssrlist = cdb->srlist;
        }
        else
        {
            if (rr==NULL) {return;}
            ssrlist = rr->getSrlist();
        }
        ssrlist->append(siid->tvsSet);
        /*cout << "tagged shape definition: " << shapestr.latin1() <<
        " dfs:" << tvsSet.dfsNum << " tag: " << tgnamestr.latin1() <<
        " value: " << value << endl;*/
    }

    if (siid->hasTvl)
    {
        LetterRuleList* slrlist;
        if (row==-1)
        {
            slrlist = cdb->lrlist;
        }
        else
        {
            if (rr==NULL) return;
            slrlist = rr->getLrlist();
        }
        slrlist->append(siid->tvlSet);
        /*cout << "tagged letter definition: " << letterstr.latin1() <<
        " dfs:" << tvlSet.dfsNum << " tag: " << tgnamestr.latin1() <<
        " value: " << value << endl;*/
    }
}

bool
ADFParser1::processItemWindowStatement (QDomNode node,INT32* dfsNum)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    // now I need to iterate through the if/set statements
    QDomNode cnode = e.firstChild();
    bool go_on = true;
    (*dfsNum)=(*dfsNum)+1;

    while( !cnode.isNull() && go_on )
    {
        // check if this node has a tag
        go_on = processItemWindowSubTags(cnode,dfsNum);
        cnode = cnode.nextSibling();
    }
    return (go_on);
}

bool
ADFParser1::processItemWindowSubTags(QDomNode node,INT32* dfsNum)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    if (e.isNull()) return (true);    // comments or similar stuff

    QString tagname = e.tagName();

    // check for valid tags
    if      (tagname==ADF_ITEMFONT)
    { return processItemFontStatement(node,dfsNum,NULL); }
    else if (tagname==ADF_IF)
    { return processIfStatement(node,dfsNum,-1,NULL); }
    else if (tagname==ADF_TAB)
    { return processTabStatement(node,dfsNum); }
    else
    {
        myLogMgr->addLog("Invalid ItemWindow Subtag "+tagname+" in ADF.");
        return (false);
    }
}

bool
ADFParser1::processTabStatement(QDomNode node,INT32* dfsNum)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    // 1) name attribute
    QString namestr = e.attribute(ADF_NAME_ATTRIB).stripWhiteSpace();
    //printf("processTabStatement called on %s\n",namestr.latin1());fflush(stdout);

    if (namestr==QString::null)
    {
        myLogMgr->addLog("Invalid tab statement in ADF, missing tab name.");
        return false;
    }

    // 2) withtag attribute
    QString withtagstr = e.attribute(ADF_WITHTAG_ATTRIB).stripWhiteSpace();
    INT32 withtagid;
    if (withtagstr!=QString::null)
    {
        // get the identifier for the given Tag String
        withtagid = draldb->scanTagName(withtagstr);
        if (withtagid<0)
        {
            QString err = "Invalid DRAL-TAG reference: " + namestr+" in ADF.";
            myLogMgr->addLog(err);
            return false;
        }
    }

    // 3) select attribute
    QString selectstr = e.attribute(ADF_SELECT_ATTRIB).stripWhiteSpace();
    INT32 selectid;
    TagValueType selecttgtype = TagSetOfValues;
    if (selectstr!=QString::null)
    {
        // get the identifier for the given Tag String
        selectid = draldb->scanTagName(selectstr);
        if (selectid<0)
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

    // 4) value attribute
    QString valuestr = e.attribute(ADF_VALUE_ATTRIB).stripWhiteSpace();

    // 5) splitby attribute
    QString splitbystr = e.attribute(ADF_SPLITBY_ATTRIB).stripWhiteSpace();
    INT32 splitbyid;
    if (splitbystr!=QString::null)
    {
        // get the identifier for the given Tag String
        splitbyid = draldb->scanTagName(splitbystr);
        if (splitbyid<0)
        {
            QString err = "Invalid DRAL-TAG reference: " + splitbystr+" in ADF.";
            myLogMgr->addLog(err);
            return false;
        }
    }

    // 6) order by attribute
    QString orderbystr = e.attribute(ADF_ORDERBY_ATTRIB).stripWhiteSpace();
    INT32 orderbyid;
    if (orderbystr!=QString::null)
    {
        // get the identifier for the given Tag String
        orderbyid = draldb->scanTagName(orderbystr);
        if (orderbyid<0)
        {
            QString err = "Invalid DRAL-TAG reference: " + orderbystr+" in ADF.";
            myLogMgr->addLog(err);
            return false;
        }
    }

    // value and select MUST be togheter
    if ( (selectstr!=QString::null) || (valuestr!=QString::null) )
    {
        // both MUST be defined
        if ( (selectstr==QString::null) || (valuestr==QString::null) )
        {
            myLogMgr->addLog("Invalid tab statement in ADF, wrong <select,value> specification.");
            return false;
        }
    }

    // select and splitby are incompatible
    if ( (selectstr!=QString::null) && (splitbystr!=QString::null) )
    {
        myLogMgr->addLog("Invalid tab statement in ADF, 'select' and 'splitby' are incompatible.");
        return false;
    }

    // check select tag coherence with given value
    UINT64 value=0;
    if (valuestr!=QString::null)
    {
        // try to convert valustr into an UINT64
        if (selecttgtype==TagIntegerValue)
        {
            bool numparsed;
            value = AGTTypes::str2uint64(valuestr.latin1(),numparsed);
            if (!numparsed)
            {
                myLogMgr->addLog("Invalid tab statement in ADF, malformed value attribute.");
                return false;
            }
        }
        else if (selecttgtype==TagStringValue)
        {
            INT32 stridx=draldb->addString(valuestr);
            value = (UINT64)stridx;
        }
        else
        {
            myLogMgr->addLog("Invalid tab statement in ADF, select is implemented only for integer and string DRAL-Tags.");
            return false;
        }
    }

    // Finally create the tab object
    TabSpec* tab = new TabSpec(namestr);
    Q_ASSERT(tab!=NULL);
    tab->setSortBy(orderbystr);
    tab->setWithTag(withtagstr);
    tab->setStrValue(valuestr);
    tab->setMatchingValue(value);

    if (splitbystr != QString::null)
    {
        tab->setSelectTag(splitbystr);
        tab->setIsSplit(true);
    }
    else if (selectstr != QString::null)
    {
        tab->setSelectTag(selectstr);
        tab->setIsSplit(false);
    }
    cdb->tabspeclist->append(tab);

    // now I need to iterate through the if/set statements
    QDomNode cnode = e.firstChild();
    bool go_on = true;
    (*dfsNum)=(*dfsNum)+1;

    while( !cnode.isNull() && go_on )
    {
        // check if this node has a tag
        go_on = processTabSubTags(cnode,dfsNum,tab);
        cnode = cnode.nextSibling();
    }
    return (go_on);
}

bool
ADFParser1::processTabSubTags (QDomNode node,INT32* dfsNum, TabSpec* tab)
{
    //printf("processTabSubTags called!\n");fflush(stdout);

    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    if (e.isNull()) return (true);    // comments or similar stuff

    QString tagname = e.tagName();

    // check for valid tags
    if      (tagname==ADF_ITEMFONT)
    { return processItemFontStatement(node,dfsNum,tab); }
    else if (tagname==ADF_IF)
    { return processIfStatement(node,dfsNum,-1,tab); }
    else if (tagname==ADF_SHOW)
    { return processShowStatement(node,dfsNum,tab); }
    else
    {
        myLogMgr->addLog("Invalid Tab Subtag "+tagname+" in ADF.");
        return (false);
    }
}

bool
ADFParser1::processShowStatement (QDomNode node,INT32* dfsNum, TabSpec* tab)
{
    //printf("processShowStatement called!\n");fflush(stdout);

    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    // 1) check for tagname attrib
    QString tgnamestr = e.attribute(ADF_TAGNAME_ATTRIB).stripWhiteSpace();
    if (tgnamestr==QString::null)
    {
        myLogMgr->addLog("Invalid show statement in ADF, missing tagname attribute.");
        return false;
    }

    // get the identifier for the given Tag String
    INT32 tagId = draldb->scanTagName(tgnamestr);
    if (tagId<0)
    {
        QString err = "Invalid DRAL-TAG reference: " + tgnamestr+" in ADF.";
        myLogMgr->addLog(err);
        return false;
    }

    // check the type of the used tag..
    TagValueType tgType = draldb->getTagValueType(tagId);
    if (tgType!=TagIntegerValue && tgType!=TagStringValue)
    {
        myLogMgr->addLog("Sorry, 'show' supported only with integer and string tags.");
        return false;
    }

    // all right then...
    tab->addShowTag(tgnamestr);
    return true;
}

bool
ADFParser1::processItemFontStatement (QDomNode node,INT32* dfsNum, TabSpec* tab)
{
    //printf("processItemFontStatement called!\n");fflush(stdout);

    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    ItemWinFont myFont;
    if (tab==NULL)
    {
        cdb->defaultItemWinFontDFSNum = *dfsNum;
        myFont = cdb->defaultItemWinFont;
    }
    else
    {
        tab->setDefaultFontDFS(*dfsNum);
        myFont = tab->getDefaultFont();
    }

    // look for the valid itemfont attributes...

    // 1) color
    QString colorstr = e.attribute(ADF_COLOR_ATTRIB).stripWhiteSpace();
    if (colorstr!=QString::null)
    {
        QColor coloratr = QColor(colorstr);
        if (!coloratr.isValid())
        {
            QString err = "Invalid color declaration: "+colorstr+" in ADF(1).";
            myLogMgr->addLog(err);
            return (false);
        }
        myFont.color = coloratr;
    }

    // 2) bold
    QString boldstr = e.attribute(ADF_BOLD_ATTRIB).stripWhiteSpace().lower();
    if (boldstr!=QString::null)
    {
        if (boldstr!="true" && boldstr!="false")
        {
            QString err = "Invalid bold attribute: "+boldstr+" in ADF.";
            myLogMgr->addLog(err);
            return (false);
        }
        myFont.bold = boldstr=="true" ? true : false;
    }

    // 3) underline
    QString underlinestr = e.attribute(ADF_UNDERLINE_ATTRIB).stripWhiteSpace().lower();
    if (underlinestr!=QString::null)
    {
        if (underlinestr!="true" && underlinestr!="false")
        {
            QString err = "Invalid underline attribute: "+underlinestr+" in ADF.";
            myLogMgr->addLog(err);
            return (false);
        }
        myFont.underline = underlinestr=="true" ? true : false;
    }

    // 4) italic
    QString italicstr = e.attribute(ADF_ITALIC_ATTRIB).stripWhiteSpace().lower();
    if (italicstr!=QString::null)
    {
        if (italicstr!="true" && italicstr!="false")
        {
            QString err = "Invalid italic attribute: "+italicstr+" in ADF.";
            myLogMgr->addLog(err);
            return (false);
        }
        myFont.italic = italicstr=="true" ? true : false;
    }
    
    if (tab==NULL)
    {
        cdb->defaultItemWinFont = myFont;
    }
    else
    {
        tab->setDefaultFont(myFont);
    }

    *dfsNum = (*dfsNum)+1;
    return (true);
}

bool
ADFParser1::processShadeTableStatement (QDomNode node,INT32* dfsNum, DralGraphNode* dralNode)
{
    /*
    //printf ("ADFParser1::processShadeTableStatement called with dfs=%d\n",*dfsNum);fflush(stdout);
    bool hasZero=false;
    bool has100 =false;
    ShadeRuleTbl* myShrTbl=NULL;

    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    if (dralNode!=NULL)
    {
        myShrTbl = dralNode->getShrTbl();
        Q_ASSERT(myShrTbl!=NULL);
    }
    else
    {
        // another global scope table
        myShrTbl = new ShadeRuleTbl();
        Q_ASSERT(myShrTbl!=NULL);
        cdb->shrtblset->insert((long)*dfsNum,myShrTbl);
    }

    // now I need to iterate through the <shade> subtags
    QDomNode cnode = e.firstChild();
    bool go_on = true;
    while( !cnode.isNull() && go_on )
    {
        // check if this node has a tag
        go_on = processShadeSubTags(cnode,*dfsNum,myShrTbl,hasZero,has100);
        cnode = cnode.nextSibling();
    }

    if (go_on && !hasZero)
    {
        myLogMgr->addLog("Invalid ShadeTable Tag in ADF, missing 0% sample.");
        return (false);
    }

    if (go_on && !has100)
    {
        myLogMgr->addLog("Invalid ShadeTable Tag in ADF, missing 100% sample.");
        return (false);
    }

    bool ok = go_on && hasZero && has100;

    (*dfsNum) = (*dfsNum)+1;
    return (ok);
    */
    return true;
}

bool
ADFParser1::processShadeSubTags (QDomNode node,INT32 dfsNum,
          ShadeRuleTbl* shrtbl, bool& hasZero, bool& has100)
{
    /*
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    if (e.isNull()) { return (true); } // ignore comments and similar nodes

    // look for the valid attributes
    // 1) color
    QString colorstr = e.attribute(ADF_COLOR_ATTRIB).stripWhiteSpace();
    if (colorstr==QString::null)
    {
        myLogMgr->addLog("Invalid Shade Tag in ADF, missing color attribute.");
        return (false);
    }

    QColor coloratr =  QColor(colorstr);
    if (!coloratr.isValid())
    {
        QString err = "Invalid color declaration: "+colorstr+" in ADF.";
        myLogMgr->addLog(err);
        return (false);
    }

    // 2) occupancy attribute
    QString occstr = e.attribute(ADF_OCCUPANCY_ATTRIB).stripWhiteSpace().lower();
    if (occstr==QString::null)
    {
        myLogMgr->addLog("Invalid Shade Tag in ADF, missing occupancy attribute.");
        return (false);
    }
    bool occok = false;

    // strip the '%'
    occstr.truncate(occstr.length()-1);
    INT32 occupancy = (INT32) occstr.toInt(&occok);
    if ( (!occok) || (occupancy<0) ||(occupancy>100) )
    {
        myLogMgr->addLog("Invalid Shade Tag in ADF, malformed occupancy.");
        return (false);
    }

    // check repeated occupancy declarations
    if (cdb->hasOccupancy(shrtbl,occupancy))
    {
        myLogMgr->addLog("Invalid Shade Tag in ADF, repeated occupancy declarations.");
        return (false);
    }

    // all right, put the rule!
    //myLogMgr->addLog(">> adding shade rule color "+colorstr+" occupancy "+QString::number(occupancy));

    ShadeRule shadeRule;
    shadeRule.color  = coloratr;
    shadeRule.occupancy = occupancy;
    shrtbl->append(shadeRule);

    hasZero = hasZero || (occupancy==0);
    has100  = has100 || (occupancy==100);
    return (true);
    */
    return true;
}

bool
ADFParser1::processBlankStatement(QDomNode node,INT32* dfsNum)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    bool hasColor = false;
    // check for rows attribute (mandatory)
    QString tgrowsstr = e.attribute(ADF_ROWS_ATTRIB).stripWhiteSpace().lower();
    if (tgrowsstr==QString::null)
    {
        myLogMgr->addLog("Invalid <blank> tag in ADF, rows attribute is mandatory.");
        return (false);
    }
    // try to convert it to an integer
    bool parseok;
    INT32 tgrow = (INT32)tgrowsstr.toInt(&parseok);
    if ( (tgrow<1) || (tgrow>256) )
    {
        myLogMgr->addLog("Invalid <blank> tag in ADF, rows attribute must be in the range [1..256].");
        return (false);
    }

    // check for color attribute (optional)
    QString tgcolorstr = e.attribute(ADF_COLOR_ATTRIB).stripWhiteSpace();
    QColor coloratr;
    if (tgcolorstr!=QString::null)
    {
        coloratr =  QColor(tgcolorstr);
        if (!coloratr.isValid())
        {
            QString err = "Invalid color declaration: "+tgcolorstr+" in ADF(1).";
            myLogMgr->addLog(err);
            return (false);
        }
        hasColor=true;
   }
   for (INT32 i=0;i<tgrow;i++)
   {
       INT16 myRow = cdb->nextRow;
       cdb->addBlankRow(myRow,coloratr,hasColor);
       cdb->nextRow++;
   }

    QString desc = "";
    QTextStream dStream( &desc, IO_WriteOnly | IO_Append );
    
    dStream << "Global Scope Blank Rule: \n";
    // dump xml description of the "if" node
    node.save(dStream,4);        

    // put the rule description:
    bool iok = cdb->addRuleDescriptor(*dfsNum,desc);
    Q_ASSERT(iok);

    *dfsNum = (*dfsNum)+1;

    return (true);
}

bool
ADFParser1::processTagDescriptorStatement(QDomNode node,INT32* dfsNum)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    // look for attributes

    // 1) tagname
    QString tgnamestr = e.attribute(ADF_TAGNAME_ATTRIB).stripWhiteSpace();
    if (tgnamestr==QString::null)
    {
        myLogMgr->addLog("Invalid Tag Descriptor in ADF, no tag name.");
        return (false);
    }

    // 2) value type
    QString typestr = e.attribute(ADF_TYPE_ATTRIB).stripWhiteSpace();
    if (typestr==QString::null)
    {
        myLogMgr->addLog("Invalid Tag Descriptor in ADF, no value type.");
        return (false);
    }
    // check that value type is one of the well known ones:
    TagValueType type;
    bool tvtok = TagValueTypeToolkit::parseValueType(typestr.latin1(),&type);
    if (!tvtok)
    {
        QString err = "Invalid Tag Descriptor, unknown value type: " + typestr +" in ADF.";
        myLogMgr->addLog(err);
        return (false);
    }

    // 3) base (default 10)
    QString basestr = e.attribute(ADF_BASE_ATTRIB).stripWhiteSpace();
    INT32 base = 10;
    if (basestr!=QString::null)
    {
        bool baseparseok;
        base = basestr.toInt(&baseparseok,10);
        if ((!baseparseok) || ((base!=2) && (base!=8) && (base!=10) && (base!=16)) )
        {
            myLogMgr->addLog ("Invalid Tag Descriptor in ADF, invalid base specification.");
            return (false);
        }
    }
    INT32 tagId = draldb->newTagDescriptor(tgnamestr,type,(INT16)base);
    Q_ASSERT(tagId>=0);

    /*cout << "new tag descriptor added: " << tgnamestr.latin1() << " type: " <<
    typestr.latin1() << " base: " << base << endl;*/

    return (true);
}



bool
ADFParser1::processEdgeStatement(QDomNode node,INT32* dfsNum)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    // 1) name attribute
    QString tgnamestr = e.attribute(ADF_NAME_ATTRIB).stripWhiteSpace();
    if (tgnamestr==QString::null)
    {
        myLogMgr->addLog("Invalid edge statement in ADF, missing edge name.");
        return false;
    }

    // 2) from attribute
    QString fromstr = e.attribute(ADF_FROM_ATTRIB).stripWhiteSpace();
    if (fromstr==QString::null)
    {
        myLogMgr->addLog("Invalid edge statement in ADF, missing 'from' attribute.");
        return false;
    }
    fromstr=cdb->normalizeNodeName(fromstr);

    // 3) to attribute
    QString tostr = e.attribute(ADF_TO_ATTRIB).stripWhiteSpace();
    if (tostr==QString::null)
    {
        myLogMgr->addLog("Invalid edge statement in ADF, missing 'to' attribute.");
        return false;
    }
    tostr=cdb->normalizeNodeName(tostr);

    // EDGE checking
    DralGraphNode* fromNode = cdb->dgnListByName->find(fromstr);
    DralGraphNode* toNode   = cdb->dgnListByName->find(tostr);

    if (fromNode==NULL)
    {
        QString err = "Invalid edge statement, wrong source node: " + fromstr+" in ADF.";
        myLogMgr->addLog(err);
        //cdb->createFakeEdge(tgnamestr,fromstr,tostr);
		return true;
    }
    if (toNode==NULL)
    {
        QString err = "Invalid edge statement, wrong destination node: " + tostr+" in ADF.";
        myLogMgr->addLog(err);
        //cdb->createFakeEdge(tgnamestr,fromstr,tostr);
		return true;
    }
    DralGraphEdge* dralEdge = cdb->findEdgeByNameFromTo(tgnamestr,fromstr,tostr);
    if (dralEdge==NULL)
    {
        QString err  = "Invalid edge statement, edge not found in DRAL Graph. Source node = " +
        fromstr + ", destination node = " + tostr + ", edge name = " + tgnamestr+" in ADF.";
        myLogMgr->addLog(err);
        //dralEdge = cdb->createFakeEdge(tgnamestr,fromstr,tostr);
		return true;
    }

    // mark the edge as tracked
    dralEdge->setConfigured(true);
    // and inform the draldb about it
    bool trackok = draldb->trackMoveItem (dralEdge->getEdgeId());
    Q_ASSERT(trackok);

    // 4) draw_when attribute
    QString draw_whenstr = e.attribute(ADF_DRAW_WHEN_ATTRIB).stripWhiteSpace().lower();
    bool isDrawOnMove = true;
    if ( draw_whenstr!=QString::null )
    {
        if (draw_whenstr == ADF_DWHEN_RECEIVED)
        {
            isDrawOnMove = false;
        }
        else if (draw_whenstr != ADF_DWHEN_SENT)
        {
            QString err  = "Invalid edge statement in ADF, draw_when attribute can be 'sent' or 'received' only.";
            err += "Source node = "+fromstr+", destination node = "+tostr + ", edge name = "+tgnamestr+".";
            myLogMgr->addLog(err);
            return false;
        }
    }

    // mark this edge with current dfs
    //dralEdge->setDfs(*dfsNum);
    //(*dfsNum)=(*dfsNum)+1;

    // create a new row for the edge declaration
    INT16 myRow = cdb->nextRow;
    EdgeRow* er = new EdgeRow;
    er->edge_id = dralEdge->getEdgeId();
    cdb->erlist->insert((long)myRow,er);
    bool addok = dralEdge->addRow(myRow);
    Q_ASSERT(addok);

    // update next row
    cdb->nextRow += dralEdge->getBandwidth();

    // update draw on move info
    RowRules* rr = dralEdge->getRow(myRow);
    Q_ASSERT(rr!=NULL);
    rr->setDrawOnMove(isDrawOnMove);

    // now I need to iterate through the if/set statements
    QDomNode cnode = e.firstChild();
    bool go_on = true;
    while( !cnode.isNull() && go_on )
    {
        // check if this node has a tag
        go_on = processEdgeSubTags(cnode,dfsNum,myRow);
        cnode = cnode.nextSibling();
    }
    return (go_on);
}

bool
ADFParser1::processEdgeSubTags(QDomNode node,INT32* dfsNum, INT16 row)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    if (e.isNull()) return (true);    // comments or similar stuff

    QString tagname = e.tagName();

    // check for valid tags
    if      (tagname==ADF_SET)
    { return processSetStatement(node,dfsNum,row); }
    else if (tagname==ADF_IF)
    { return processIfStatement(node,dfsNum,row); }
    else
    {
        myLogMgr->addLog("Invalid Edge/Node Subtag "+tagname+" in ADF.");
        return (false);
    }
}

bool
ADFParser1::processNodeStatement(QDomNode node,INT32* dfsNum)
{
    /*
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    // 1) name attribute
    QString tgnamestr = e.attribute(ADF_NAME_ATTRIB).stripWhiteSpace();
    if (tgnamestr==QString::null)
    {
        myLogMgr->addLog("Invalid node statement in ADF, missing node name.");
        return false;
    }
    // check node name notation
    tgnamestr=cdb->normalizeNodeName(tgnamestr);

    // 2) check node name
    DralGraphNode* dralNode = cdb->dgnListByName->find(tgnamestr);
    if (dralNode==NULL)
    {
        myLogMgr->addLog("Invalid node statement, unkown node "+tgnamestr+". Your ADF can be deprecated.");
        dralNode = cdb->createFakeNode(tgnamestr);
    }

    // assign ordering number
    dralNode->setUsageRowPos(cdb->nextUsgRow);
    dralNode->setConfigured(true);
    cdb->nextUsgRow++;

    // mark this node with current dfs
    dralNode->setDfs(*dfsNum);
    (*dfsNum)=(*dfsNum)+1;

    // now I need to iterate through the shadetable specifications (only 1)
    QDomNodeList children = node.childNodes();
    INT32 cnt=children.count();
    if (cnt>1)
    {
        myLogMgr->addLog("Invalid Node specification in ADF (node "+tgnamestr+
                         "), only one shadetable allowed.");
        return (false);
    }

    QDomNode cnode = e.firstChild();
    bool ok = processNodeSubTags(cnode,dfsNum,dralNode);

    return (ok);
    */

    (*dfsNum)=(*dfsNum)+1;
    return true;
}

bool
ADFParser1::processNodeSubTags(QDomNode node,INT32* dfsNum, DralGraphNode* dralNode)
{
    /*
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    if (e.isNull()) return (true);    // comments or similar stuff

    QString tagname = e.tagName();

    // check for valid tags
    if      (tagname==ADF_SHADETABLE)
    { return processShadeTableStatement(node,dfsNum,dralNode); }
    else
    {
        myLogMgr->addLog("Invalid Node Subtag "+tagname+" in ADF.");
        return (false);
    }
    */
    return true;
}


