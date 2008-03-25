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
  * @file ADFParser2.cpp
  */

#include "ADFParser2.h"

ADFParser2::ADFParser2(QDomDocument* ref) : ADFParser1(ref)
{
}

ADFParser2::~ADFParser2()
{
}

// ------------------------------------------------------------------
// Redefined methods
// ------------------------------------------------------------------

bool
ADFParser2::processDomNode(QDomNode node,INT32* dfsNum)
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
    else if (tagname==ADF_SHADETABLE)
    { return processShadeTableStatement(node,dfsNum); }
    else if (tagname==ADF_ITEMWINDOW)
    { return processItemWindowStatement(node,dfsNum); }
    else if (tagname==ADF_GROUP)
    { return processGroupStatement(node,dfsNum); }
    else if (tagname==ADF_IF)
    { return processIfStatement(node,dfsNum); }
    else if (tagname==ADF_SET)
    { return processSetStatement(node,dfsNum); }
    else if (tagname==ADF_EDGE)
    { return processEdgeStatement(node,dfsNum); }
    else if (tagname==ADF_NODE)
    { return processNodeStatement(node,dfsNum); }
    else if (tagname==ADF_ENTERNODE)
    { return processEnterNodeStatement(node,dfsNum); }
    else if (tagname==ADF_EXITNODE)
    { return processExitNodeStatement(node,dfsNum); }
    else if (tagname==ADF_BLANKROWS)
    { return processBlankStatement(node,dfsNum); }
    else
    {
        myLogMgr->addLog("Unknown XML-TAG "+tagname+" in ADF.");
        return (false);
    }
}

bool
ADFParser2::processIfStatement(QDomNode node,INT32* dfsNum, INT16 row, TabSpec* tab)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    if (!node.hasChildNodes())
    {
        myLogMgr->addLog("Invalid empty if statement in ADF.");
        return false;
    }
    
    QString desc = "";
    QTextStream dStream( &desc, IO_WriteOnly | IO_Append );
    
    if ( (row<0) && (tab==NULL) )
    {
        dStream << "Global Scope Conditional Rule: \n";
    }
    else if (row>=0)
    {
        dStream << "Row Scope Conditional Rule: \n";
    }
    else
    {
        dStream << "Tab Scope Conditional Rule: \n";
    }
        
    // dump xml description of the "if" node
    node.save(dStream,4);        
    
    // check for if attributes

    // 1) tagname (in ADF2 can be itemtag,nodetag or cycletag) but only one!
    QString itemtgnamestr  = e.attribute(ADF_ITEMTAGNAME_ATTRIB).stripWhiteSpace();
    QString nodetgnamestr  = e.attribute(ADF_NODETAGNAME_ATTRIB).stripWhiteSpace();
    QString cycletgnamestr = e.attribute(ADF_CYCLETAGNAME_ATTRIB).stripWhiteSpace();
    QString tgnamestr;

    bool nullc1 = itemtgnamestr  == QString::null;
    bool nullc2 = nodetgnamestr  == QString::null;
    bool nullc3 = cycletgnamestr == QString::null;
    int tgatcnt = 0;
    if (!nullc1) { ++tgatcnt; tgnamestr=itemtgnamestr; }
    if (!nullc2) { ++tgatcnt; tgnamestr=nodetgnamestr; }
    if (!nullc3) { ++tgatcnt; tgnamestr=cycletgnamestr; }
    if (tgatcnt==0)
    {
        myLogMgr->addLog("Invalid if statement in ADF, missing some tag attribute.");
        return false;
    }
    if (tgatcnt>1)
    {
        myLogMgr->addLog("Invalid if statement in ADF, only one tag attribute supported.");
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

    // color/letter/shape rules (pre-sets)
    SetInsideIfData siid;
    siid.hasTvc = false;
    siid.hasTvs = false;
    siid.hasTvl = false;

    siid.tvcSet.dfsNum  = *dfsNum;
    siid.tvcSet.tagId   = tagId;
    siid.tvcSet.tagValue= value;
    siid.tvcSet.isItemTag = !nullc1;
    siid.tvcSet.isNodeTag = !nullc2;
    siid.tvcSet.isCycleTag = !nullc3;
    if (!nullc2) {siid.tvcSet.nodeid = getNodeIdFromRow(row); }

    siid.tvsSet.dfsNum  = *dfsNum;
    siid.tvsSet.tagId   = tagId;
    siid.tvsSet.tagValue= value;
    siid.tvsSet.isItemTag = !nullc1;
    siid.tvsSet.isNodeTag = !nullc2;
    siid.tvsSet.isCycleTag = !nullc3;
    if (!nullc2) {siid.tvsSet.nodeid = getNodeIdFromRow(row); }

    siid.tvlSet.dfsNum  = *dfsNum;
    siid.tvlSet.tagId   = tagId;
    siid.tvlSet.tagValue= value;
    siid.tvlSet.isItemTag = !nullc1;
    siid.tvlSet.isNodeTag = !nullc2;
    siid.tvlSet.isCycleTag = !nullc3;
    if (!nullc2) {siid.tvlSet.nodeid = getNodeIdFromRow(row); }

    // ItemFont rules (pre-sets)
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

    // put the rule description:
    bool iok = cdb->addRuleDescriptor(*dfsNum,desc);
    Q_ASSERT(iok);

    *dfsNum = (*dfsNum)+1;
    return (true);
}

bool
ADFParser2::processSetStatement(QDomNode node,INT32* dfsNum, INT16 row)
{
    bool isTransp = false;
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    // this is only called on naked set statements
    // in other words a <set ... />
    // but not on <if ...> <set ../> </if>

    QString desc = "";
    QTextStream dStream( &desc, IO_WriteOnly | IO_Append );
    
    if (row<0) 
        dStream << "Global Scope Unconditional Rule: \n";
    else
        dStream << "Row Scope Unconditional Rule: \n";

    // dump xml description of the "if" node
    node.save(dStream,4);        

    // look for the valid attributes
    // 1) color
    QString colorstr = e.attribute(ADF_COLOR_ATTRIB).stripWhiteSpace();
    if (colorstr!=QString::null)
    {
        QColor coloratr = QColor(0,0,0);
        if ((row>=0) && (colorstr == ADF_COLOR_TRANSP))
        {
            isTransp=true;
        }
        else
        {
            coloratr =  QColor(colorstr);
            if (!coloratr.isValid())
            {
                QString err = "Invalid color declaration: "+colorstr+" in ADF.";
                myLogMgr->addLog(err);
                return (false);
            }
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
            rr->setColor(coloratr,*dfsNum,isTransp);
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

    // put the rule description:
    bool iok = cdb->addRuleDescriptor(*dfsNum,desc);
    Q_ASSERT(iok);

    *dfsNum = (*dfsNum)+1;
    return (true);
}

bool
ADFParser2::processSetInsideIfStatement(QDomElement eset,SetInsideIfData* siid)
{
    // look for the valid SET attributes
    // 1) color
    QString colorstr = eset.attribute(ADF_COLOR_ATTRIB).stripWhiteSpace();
    QColor coloratr = QColor(0,0,0);
    if (colorstr!=QString::null)
    {
        if (colorstr == ADF_COLOR_TRANSP)
        {
            siid->tvcSet.isTransparentColor = 1;
        }
        else
        {
            coloratr =  QColor(colorstr);
            siid->tvcSet.isTransparentColor = 0;
            if (!coloratr.isValid())
            {
                QString err = "Invalid color declaration: "+colorstr+" in ADF.";
                myLogMgr->addLog(err);
                return (false);
            }
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

bool
ADFParser2::processShowStatement (QDomNode node,INT32* dfsNum, TabSpec* tab)
{
    //printf("processShowStatement called!\n");fflush(stdout);

    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    // 1) check for tagname attrib
    QString tgnamestr = e.attribute(ADF_ITEMTAGNAME_ATTRIB).stripWhiteSpace();
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
ADFParser2::processNodeStatement(QDomNode node,INT32* dfsNum)
{
    //printf("ADFParser2::processNodeStatement called...\n");
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    // 1) name attribute
    QString namestr = e.attribute(ADF_NAME_ATTRIB).stripWhiteSpace();
    if (namestr==QString::null)
    {
        myLogMgr->addLog("Invalid node statement in ADF, missing node name.");
        return true; // just ignore and keep going
    }                      
    // check node name notation
    namestr=cdb->normalizeNodeName(namestr);

    // 2) check node name
    DralGraphNode* dralNode = cdb->dgnListByName->find(namestr);
    if (dralNode==NULL)
    {
        myLogMgr->addLog("Invalid node statement, unkown node "+namestr+". Your ADF can be deprecated.");
		return true;
    }

    // create a new row for the node declaration
    dralNode->setConfigured(true);
    INT16 myRow = cdb->nextRow;
    NodeRow* nr = new NodeRow;
    nr->node_id = dralNode->getNodeId();
    nr->row     = myRow;
    cdb->nrlist->insert((long)myRow,nr);
    bool addok = dralNode->addSimpleNodeRow(myRow);
    //printf ("ADFParser2::processNodeStatement  alloc row %d for node_id=%d\n",(int)nr->row,(int)nr->node_id);
    Q_ASSERT(addok);

    // register the node-slot on the draldb
    NodeSlot rslot;
    rslot.specDimensions=0;
    rslot.dimVec=NULL;
    INT32 trackId = draldb->trackNodeTags (dralNode->getNodeId(),rslot);
    //printf("ADFParser2::processNodeStatement trackId = %d\n",trackId);
    
    //Q_ASSERT(trackId>=0); // can be a old adf talking about non-existent nodes...
    nr->trackId = trackId;

    // update next row
    cdb->nextRow++;

    // now I need to iterate through the if/set statements
    QDomNode cnode = e.firstChild();
    bool go_on = true;
    while( !cnode.isNull() && go_on )
    {
        // check if this node has a tag
        go_on = processNodeSubTags(cnode,dfsNum,myRow);
        cnode = cnode.nextSibling();
    }
    return (go_on);
}

bool
ADFParser2::processNodeSubTags(QDomNode node,INT32* dfsNum, INT16 row)
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
    else if (tagname==ADF_SHADING)
    { return processShadingStatement(node,dfsNum,row); }
    else
    {
        myLogMgr->addLog("Invalid Node Subtag "+tagname+" in ADF.");
        return (false);
    }
}

bool
ADFParser2::processShadingStatement(QDomNode node,INT32* dfsNum, INT16 row)
{
    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    // --------------------------------------------------------------
    // mandatory attributes
    // --------------------------------------------------------------

    // 1) nodetag attribute
    QString nodetagstr  = e.attribute(ADF_NODETAGNAME_ATTRIB).stripWhiteSpace();
    if (nodetagstr==QString::null)
    {
        myLogMgr->addLog("Invalid Shading in ADF, missing nodetag attribute.");
        return (false);
    }

    // 2) shadetable attribute
    QString shadetblstr  = e.attribute(ADF_SHADETBL_ATTRIB).stripWhiteSpace();
    if (shadetblstr==QString::null)
    {
        myLogMgr->addLog("Invalid Shading in ADF, missing shadetable attribute.");
        return (false);
    }

    // look for this shadetable
    ShadeRuleTbl* rtbl = cdb->shrtblset->find(shadetblstr);
    if (rtbl==NULL)
    {
        QString err = "Invalid Shading in ADF, unknown shadetable ";
        err += shadetblstr + " (forwarded references not supported).";
        myLogMgr->addLog(err);
        return (false);
    }

    // look for the tag
    INT32 tgId = draldb->scanTagName(nodetagstr);
    if (tgId<0)
    {
        QString err = "Invalid Shading in ADF, unknown tag ";
        err += nodetagstr + ".";
        myLogMgr->addLog(err);
        return (false);
    }

    // --------------------------------------------------------------
    // optional attributes
    // --------------------------------------------------------------

    // minvalue
    QString minvaluestr  = e.attribute(ADF_MINVALUE_ATTRIB).stripWhiteSpace();
    INT32   minvalue=-1;
    if (minvaluestr!=QString::null)
    {
        bool pok;
        minvalue = minvaluestr.toInt(&pok);
        if (!pok)
        {
            QString err = "Invalid Shading in ADF, unable to parse minvalue ";
            err += minvaluestr + " as an integer value.";
            myLogMgr->addLog(err);
            return (false);
        }
        if (minvalue<0)
        {
            myLogMgr->addLog("Invalid Shading in ADF, minvalue constant must be positive.");
            return (false);
        }
    }

    // maxvalue
    QString maxvaluestr  = e.attribute(ADF_MAXVALUE_ATTRIB).stripWhiteSpace();
    INT32   maxvalue=-1;
    if (maxvaluestr!=QString::null)
    {
        bool pok;
        maxvalue = maxvaluestr.toInt(&pok);
        if (!pok)
        {
            QString err = "Invalid Shading in ADF, unable to parse maxvalue ";
            err += maxvaluestr + " as an integer value.";
            myLogMgr->addLog(err);
            return (false);
        }
        if (maxvalue<0)
        {
            myLogMgr->addLog("Invalid Shading in ADF, maxvalue constant must be positive.");
            return (false);
        }
        if ( (minvalue>=0) && (minvalue>maxvalue) )
        {
            myLogMgr->addLog("Invalid Shading in ADF, maxvalue must be greater or equal than minvalue!");
            return (false);
        }

    }

    // minvaluetag
    // maxvaluetag

    QString desc = "";
    QTextStream dStream( &desc, IO_WriteOnly | IO_Append );
    dStream << "Row Scope Shading Table Rule: \n";
    // dump xml description of the "if" node
    node.save(dStream,4);        
    // put the rule description:
    bool iok = cdb->addRuleDescriptor(*dfsNum,desc);
    Q_ASSERT(iok);

    // save all the data on the row rule object
    RowRules* rr = cdb->getRowRules(row);
    Q_ASSERT(rr!=NULL);
    rr->setShadingTbl(shadetblstr);
    rr->setShadingTag(tgId);
    rr->setShadingDfsNum(*dfsNum);

    // check min/max value stuff
    if (maxvalue>=0)
    {rr->setShadingMaxValue(maxvalue);}
    else
    {
        // get node capacity
        UINT16 nodeid = rr->getNodeId();
        DralGraphNode* node = cdb->getNode(nodeid);
        Q_ASSERT(node!=NULL);
        rr->setShadingMaxValue(node->getCapacity());
    }

    if (minvalue>=0)
    {rr->setShadingMinValue(minvalue);}
    else
    {rr->setShadingMinValue(0);}

    *dfsNum = *dfsNum + 1;
    return true;
}

bool
ADFParser2::processShadeTableStatement (QDomNode node,INT32* dfsNum)
{
    //printf ("ADFParser2::processShadeTableStatement called with dfs=%d\n",*dfsNum);

    bool hasZero=false;
    bool has100 =false;
    ShadeRuleTbl* myShrTbl=NULL;

    // node type checking
    QDomElement e = node.toElement(); // try to convert the node to an element.
    Q_ASSERT (!e.isNull());

    // look for the name attribute
    QString namestr = e.attribute(ADF_NAME_ATTRIB).stripWhiteSpace();
    if (namestr==QString::null)
    {
        myLogMgr->addLog("Invalid Shading Table in ADF, missing name attribute.");
        return (false);
    }

    // allocate tbl
    myShrTbl = new ShadeRuleTbl();
    Q_ASSERT(myShrTbl!=NULL);
    cdb->shrtblset->insert(namestr,myShrTbl);

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
}

bool
ADFParser2::processShadeSubTags (QDomNode node,INT32 dfsNum,
          ShadeRuleTbl* shrtbl, bool& hasZero, bool& has100)
{
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
}

// ------------------------------------------------------------------

UINT16
ADFParser2::getNodeIdFromRow (INT32 row)
{
    ///@todo implement this
    return 0;
}

bool
ADFParser2::processGroupStatement  (QDomNode node,INT32* dfsNum)
{
    myLogMgr->addLog("ADFParser2::processGroupStatement: sorry, feature not implemented yet.");
    return false;
}

bool
ADFParser2::processEnterNodeStatement (QDomNode node,INT32* dfsNum)
{
    myLogMgr->addLog("ADFParser2::processEnterNodeStatement: sorry, feature not implemented yet.");
    return false;
}

bool
ADFParser2::processExitNodeStatement (QDomNode node,INT32* dfsNum)
{
    myLogMgr->addLog("ADFParser2::processExitNodeStatement: sorry, feature not implemented yet.");
    return false;
}

