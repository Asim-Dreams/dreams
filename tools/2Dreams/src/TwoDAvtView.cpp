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
  * @file TwoDAvtView.cpp
  */

#include <qstringlist.h>
#include <qtooltip.h>
#include <qaccel.h>

#include "TwoDAvtView.h"
#include "2Dreams.h"
#include "ShowTags.h"


TwoDAvtView::TwoDAvtView(QMainWindow* parent,QMainWindow* main,int numcycles,int numevents) :
 AScrollView(parent, numcycles*cube_side, numevents*cube_side)
{
    //printf ("starting 2dview constructor...\n");
    myMDI = parent;
    mainWin=main;
    mdiLocked=true;
    
    // filter out some events
    horizontalScrollBar()->installEventFilter(this);
    verticalScrollBar()->installEventFilter(this);
    viewport()->installEventFilter(this);
    /*
    if ( parent!=NULL )
    {
        parent->installEventFilter( this );
    }
    */
    
    myDB=ZDBase::getInstance();
    myConfigDB=ConfigDB::getInstance();
    draldb=DralDB::getInstance();
    prefMgr = PreferenceMgr::getInstance();

    numCycles=numcycles;
    numEvents=numevents;
    annCtrl->setBaseSize(getBaseElementSize());
    gridSize = (int)ceil(getBaseElementSize() * 10.0);
    //annCtrl->setGridSize(gridSize);
    annCtrl->setGridSize(getBaseElementSize()); // tmp hack

    //printf ("creating vectors...\n");
    initShadeFlags();
    //printf ("vec init done!\n");

    // create tag-favorite class
    ft = FavoriteTags::getInstance();
    currentTagSelector = ft->getList().first();

    // HighLight manager
    hlmgr = new HighLightMgr(cube_side);
    connect(hlmgr,SIGNAL(highlightChanged(int)),mainWin,SLOT(highlightChanged(int)));
    
    htype = HLT_DEFAULT;

    // by default we see both highlighted and non-highlighted guys
    vHEvents  = true;
    vNHEvents = true;

    //force_dis_refresh = true;

    // I add a popup menu to ease some common operations
    addContextMenu();
    rc_pressed_x=-1;
    rc_pressed_y=-1;
    rc_released_x=-1;
    rc_released_y=-1;

    shaderBrush = QBrush(prefMgr->getShadingColor());
    bgColor = prefMgr->getBackgroundColor();

    /*
    escAccel = new QAccel( this );
    escAccel->connectItem( escAccel->insertItem(Key_Escape),this,SLOT(escKey()) );
    escapeHitted = false;
    */

    tip = new Tip2Dreams(this);

    // by default all docked windows are on
    disWinEnabled = true;
    eventWinEnabled = true;
    ancWinEnabled = true;
    createDisWindow();
    createEventWindow();
    createAncestorWindow();
    
    copyAnnItem = NULL;
    //printf ("2dview constructor done\n");
}

TwoDAvtView::~TwoDAvtView()
{

    if (disWin!=NULL)
    {
        setEnabledDisWindow(false);
    }

    if (eventWin!=NULL)
    {
        setEnabledEventWindow(false);
    }

    if (ancWin!=NULL)
    {
        setEnabledAncestorWindow(false);
    }

    if (hlmgr!=NULL) delete hlmgr;
    if (contextMenu!=NULL) delete contextMenu;
}

void 
TwoDAvtView::setMDINumber(int v) 
{
    AScrollView::setMDINumber(v);
    // propagate to the caption of the docked windows
    if (v>1)
    {
        if (disWin) disWin->setCaption(QString("Item Window ("+QString::number(v)+QString(")")));
        if (eventWin) eventWin->setCaption(QString("Tag Window ("+QString::number(v)+QString(")")));
        if (ancWin) ancWin->setCaption(QString("Relationship Window ("+QString::number(v)+QString(")")));
    }
}
void
TwoDAvtView::setRealContentsSize(INT32 cycles, INT32 events)
{
    numCycles=cycles;
    numEvents=events;
    AScrollView::setRealContentsSize((int)(cycles*cube_side), (int)(events*cube_side));
    initShadeFlags();
    updateCloseMenuState();
}

void
TwoDAvtView::reset()
{
    numCycles=0;
    numEvents=0;

    AScrollView::setRealContentsSize(0,0);

    // docked windows
    ancWin->reset();

    // clean-up highlight stuff
    hlmgr->purgeAll();
    updateCloseMenuState();

    // annotation tools
    annCtrl->reset();

    // do a forced repaint
    repaintContents(false);
    setPointerType (POINTER_SELECT);
}

// --------------------------------------------------------------------
// --------------------------------------------------------------------
void
TwoDAvtView::refreshFavoriteTagList()
{
    ft->refresh();
    eventWin->updateTagList();
}

// --------------------------------------------------------------------
// --------------------------------------------------------------------
void 
TwoDAvtView::addContextMenu()
{
    contextMenu = new QPopupMenu(this);Q_ASSERT(contextMenu!=NULL);
    rel_scan_id = contextMenu->insertItem("&Relationship Scan", this, SLOT(do_ancScan()));
    show_all_tags_id = contextMenu->insertItem("&Show All Tags", this, SLOT(do_showAllTags()));
    show_row_info_id = contextMenu->insertItem("&Show Row Info", this, SLOT(do_showRowInfo()));
    contextMenu->insertSeparator();
    // by defaul MDI is NOT rebel 
    rebel_id = contextMenu->insertItem("&Free Window", this, SLOT(do_switchRebel()));
    contextMenu->setItemChecked(rebel_id,false);
    
}

void
TwoDAvtView::do_switchRebel()
{
    mdiLocked=!mdiLocked;
    contextMenu->setItemChecked(rebel_id,!mdiLocked);
    if (mdiLocked)
    {
        myMDI->setIcon(IconFactory::getInstance()->image32);
        myMDI->setIconText("Locked");
        setFrameStyle( QFrame::Panel | QFrame::Raised );
        setLineWidth( style().pixelMetric(QStyle::PM_DefaultFrameWidth, this) );
    }        
    else
    {
        myMDI->setIcon(IconFactory::getInstance()->image33);
        myMDI->setIconText("Unlocked");
        setFrameStyle( QFrame::Box | QFrame::Raised );
        setLineWidth(3);
    }
}

void
TwoDAvtView::do_ancScan()
{
    //printf ("do_ancScan called with col=%d, row=%d\n",popup_col,popup_row);
    if (myDB->hasValidData(popup_col+myDB->getFirstEffectiveCycle(),popup_row))
    {
        if (popup_row >= myDB->getNumRows())
        {
            qApp->beep();
            return;
        }
        if (myDB->getRowType(popup_row) != SimpleEdgeRow)
        {
            qApp->beep();
            return;
        }

        TwoDreams* tdr = (TwoDreams*)mainWin;
        ItemHandler hnd;
        draldb->getMoveItem(&hnd, myDB->getRowEdgeId(popup_row),popup_col+myDB->getFirstEffectiveCycle(),myDB->getRowEdgePos(popup_row));
        ancWin->initTrackingWidth(&hnd,popup_col+myDB->getFirstEffectiveCycle(),popup_row);
        tdr->enableMenuOptions(numCycles>1);
        //tdr->ensureRScanDWinVisible();
    }
}

void
TwoDAvtView::do_showRowInfo()
{
   
	if (popup_row >= myDB->getNumRows()) { return; }
	QString report = getRowInfo(popup_row);
    QMessageBox::information (this,"Row Detailed Description",report,QMessageBox::Ok);
}

QString
TwoDAvtView::getRowInfo(INT32 selrow)
{
	INT32 nodeid;
	DralGraphNode* gnode;
	DralGraphEdge* gedge;
	RRList* rrlist;
	RowRules* rrules;
	INT32 edgeid;
	INT32 edgepos;
	QString report="";
	QString shdtbl;
    ItemHandler hnd;
    QColor evcolor;
    EventShape evshape;
    INT32 colorDFS;
    INT32 shapeDFS;
    bool isTransp;
    INT32 trackId;
    CSLEngine* csl = CSLEngine::getInstance();

	if (selrow >= myDB->getNumRows()) { return QString("Nothing to report"); }
	
	switch (myDB->getRowType(selrow))
	{
		case SimpleNodeRow:
            nodeid = myDB->getRowNodeId(selrow);
            gnode = myConfigDB->getNode(nodeid);
            if (gnode==NULL) { return QString("Unexpected inconsistency at TwoDAvtView::getRowInfo(), please report this problem!"); }
            report += "Row Number:\t "+QString::number(selrow)+" \n";
            report += "Row Type:\t Simple Node Row \n";
            report += "Node ID:\t\t "+QString::number(nodeid)+" \n";
            report += "Node Name:\t "+gnode->getName()+" \n";
            report += "Node Capacity:\t "+QString::number(gnode->getCapacity())+" \n";
            report += "\n";
            
			trackId = myConfigDB->getTrackIdForRow(selrow);
			if (trackId>=0)
            {
                bool evok = csl->getTrackEventColorShape (selrow,trackId,popup_col+myDB->getFirstEffectiveCycle(),&evcolor,&evshape,&isTransp,&colorDFS,&shapeDFS);
                if (evok)
                {
                    report += "Color Decision for current event:\n";
                    report += myConfigDB->getRuleDescriptor(colorDFS);
                    report += "\nShape Decision for current event:\n";
                    report += myConfigDB->getRuleDescriptor(shapeDFS);
                    report += "\n";
                }
                else
                {
                    report += "Shaded event with invalid or undefined value!\n";
                }
            }
            
            rrules = gnode->getRow(selrow);
            if (rrules!=NULL)
            {
                shdtbl = rrules->getShadingTbl();
                if (shdtbl!=QString::null)
                {
                    report += "Using color shading table:\t " + shdtbl + "\n";
                    report += "Shading minimum value:\t " + QString::number(rrules->getShadingMinValue())+ "\n";
                    report += "Shading maximum value:\t " + QString::number(rrules->getShadingMaxValue())+ "\n";
                    INT32 tgId = rrules->getShadingTag();
                    report += "Shading tag:\t\t " + draldb->getTagDescription(tgId) + "\n";
                }
            }
            
            rrlist = gnode->getRowList();
            if (rrlist->count()>1)
            {
                report += "Node used on rows:\t ";
                QIntDictIterator<RowRules> it( *rrlist );
                for ( ; it.current(); ++it )
                {
                    rrules = it.current();
                    report += QString::number(rrules->getRowPos());
                    report += " ";
                }
                report += "\n";
            }
            break;
	
		case SimpleEdgeRow:
            edgeid  = myDB->getRowEdgeId(selrow);
            edgepos = myDB->getRowEdgePos(selrow);
            //printf("edgeid=%d\n",(int)edgeid);
            gedge = myConfigDB->getEdge(edgeid);
            if (gedge==NULL) { return QString("Unexpected inconsistency at TwoDAvtView::getRowInfo(), please report this problem!"); }
            report += "Row Number:\t "+QString::number(selrow)+" \n";
            report += "Row Type:\t Simple Edge Row \n";
            report += "Edge ID:\t\t "+QString::number(edgeid)+" \n";
            report += "Edge Name:\t "+gedge->getName()+" \n";
            report += "Edge Source:\t "+gedge->getSourceNodeName()+" \n";
            report += "Edge Destination:\t "+gedge->getDestinationNodeName()+" \n";
            report += "Edge Bandwidth:\t "+QString::number(gedge->getBandwidth())+" \n";
            report += "Edge Latency:\t "+QString::number(gedge->getLatency())+" \n";
            report += "\n";

            // put xml associated with color and shape decision
            draldb->getMoveItem(&hnd, myDB->getRowEdgeId(selrow),popup_col+myDB->getFirstEffectiveCycle(),edgepos);
            if (hnd.isValidItemHandler())
            {
                csl->getEventColorShape (selrow,&hnd,popup_col+myDB->getFirstEffectiveCycle(),&evcolor,&evshape,&colorDFS,&shapeDFS);
                report += "Color Decision for pointed event:\n";
                report += myConfigDB->getRuleDescriptor(colorDFS);
                report += "\nShape Decision for pointed event:\n";
                report += myConfigDB->getRuleDescriptor(shapeDFS);
                report += "\n";
            }
            
            rrules = gedge->getRow(selrow-edgepos);
            if (rrules!=NULL)
            {
                if (rrules->getDrawOnMove())
                {
                    report += "Draw event when the item is sent\n";
                }
                else
                {
                    report += "Draw event when the item is received\n";
                }
            }
            break;
		
		case InputNodeRow:
            ///< @todo implement this
            break;

		case OutputNodeRow:
            ///< @todo implement this
            break;

		case SlotNodeRow:
            ///< @todo implement this
            break;

        case SimpleBlankRow:
            report += "Row Number:\t "+QString::number(selrow)+" \n";
            report += "Row Type:\t Blank Row \n";
            break;
            
		case UninitializedRow:
            default:
            break;
	}
    
    return report;
}

void
TwoDAvtView::do_showAllTags()
{
    if (myDB->hasValidData(popup_col+myDB->getFirstEffectiveCycle(),popup_row))
    {
		eventTagDialogImpl * etd;
		INT32 trackId;
		ItemHandler hnd;
		switch (myDB->getRowType(popup_row))
		{
			case SimpleNodeRow:
			trackId = myConfigDB->getTrackIdForRow(popup_row);
			if (trackId>=0)
			{
				etd = new eventTagDialogImpl(trackId,popup_col+myDB->getFirstEffectiveCycle(),this,"etd",true);
				Q_ASSERT(etd!=NULL);
				etd->exec();
			}
			break;

			case SimpleEdgeRow:
			draldb->getMoveItem(&hnd, myDB->getRowEdgeId(popup_row),popup_col+myDB->getFirstEffectiveCycle(),myDB->getRowEdgePos(popup_row));
			if (hnd.isValidItemHandler())
			{
				etd = new eventTagDialogImpl(&hnd,popup_col+myDB->getFirstEffectiveCycle(),this,"etd",true);
				Q_ASSERT(etd!=NULL);
				etd->exec();
			}
			break;
			
			default:
			return;
		}
    }
}

void 
TwoDAvtView::rightClickPressEvent(QMouseEvent* e)
{
    rc_pressed_x = e->x();
    rc_pressed_y = e->y();
}

void 
TwoDAvtView::rightClickReleaseEvent(QMouseEvent* e)
{
    rc_released_x = e->x();
    rc_released_y = e->y();

    bool c1 = abs(rc_pressed_x-rc_released_x)<3;
    bool c2 = abs(rc_pressed_y-rc_released_y)<3;
    if (c1&&c2)
    {
        int cx,cy;
        double wx,wy;
        viewportToContents (rc_released_x,rc_released_y,cx,cy);
        wx = ((double)cx/scf_x);
        wy = ((double)cy/scf_y);
        popup_col = (int)(wx/getBaseElementSize());
        popup_row = (int)(wy/getBaseElementSize());

        // check if there is something there
        bool ena = myDB->hasValidData(popup_col+myDB->getFirstEffectiveCycle(),popup_row) && (popup_row<numEvents);
        contextMenu->setItemEnabled(rel_scan_id,ena);
        contextMenu->setItemEnabled(show_all_tags_id,ena);
        contextMenu->exec(QCursor::pos());
    }
}

/*
void 
TwoDAvtView::setStatusBar(QStatusBar* value)
{

    // first invoke parent class implementation
    AScrollView::setStatusBar(value);

    // and now add 2Dreams specific stuff
    //currentOccLbl = new QLabel(value);Q_ASSERT(currentOccLbl!=NULL);
    //currentOccLbl->setText("");

    QFontMetrics fm = currentOccLbl->fontMetrics();
    QString str = "Occupancy: 99999 of 9999 (100%)";
    QRect tbr = fm.boundingRect(str);
    currentOccLbl->setFixedWidth (tbr.width()+5);
    value->addWidget(currentOccLbl,0,true);
}
*/


bool
TwoDAvtView::drawWorldCoordinatesContents(QPainter* p, int ncx, int ncy, int ncw, int nch)
{
    // compute affected area with the CUBE_SIDE...
    int x1 = ncx - (ncx%cube_side);
    int x2 = ncx+ncw+2*(ncx%cube_side);
    int y1 = ncy - (ncy%cube_side);
    int y2 = ncy+nch+2*(ncy%cube_side);

    x1 -= 10;
    x2 += 10;
    y1 -= 10;
    y2 += 10;

    if (x1<0) x1=0;
    if (y1<0) y1=0;
    if (x2>=realContentsWidth) x2=realContentsWidth-1;
    if (y2>=realContentsHeight) y2=realContentsHeight-1;

    // put the black pen and go ahead
    p->setPen( QPen(QColor(0,0,0),cube_border_size));
    p->setBrush(NoBrush);

    bool ok = true;
    if (vNHEvents)
    {
        bool ssx = (1.0/getBaseElementSize()) > scf_x;
        bool ssy = (1.0/getBaseElementSize()) > scf_y;
        
        double dtx1=(double)(x1)*scf_x;
        double dtx2=(double)(x1+(int)getBaseElementSize())*scf_x;
        bool nfsx = dtx2-dtx1<2.0;

        double dty1=(double)(y1)*scf_y;
        double dty2=(double)(y1+(int)getBaseElementSize())*scf_y;
        bool nfsy = dty2-dty1<4.0;
        
        if ( ssx && ssy )
            ok = drawSubSamplingScale(x1,x2,y1,y2,p);
        else if ( nfsx && nfsy )
            ok = drawNoFullScale(x1,x2,y1,y2,p);
        else
            ok = drawFullScale(x1,x2,y1,y2,p);

    }
    // check for highlighted guys
    drawHighlights(p,x1,x2,y1,y2,vHEvents);
    return ok;
}


bool
TwoDAvtView::drawFullScale(int x1,int x2, int y1, int y2, QPainter* painter)
{
    int i,j,mcol,mrow,cnt=0;

    int mcol2,mrow2;

    mcol = x1/cube_side;

    mcol2 = (x2+1)/cube_side;
    mrow2 = (y2+1)/cube_side;

    while (mcol<=mcol2)
    {
        ++cnt;
        emit progress_cnt_changed (cnt);

        mrow = y1/cube_side;

        // draw event rows
        while (mrow<=mrow2)
        { drawFullScaleEvent(mcol,mrow++,painter); }
        ++mcol;
    }
    return true;
}

bool
TwoDAvtView::drawNoFullScale(int x1,int x2, int y1, int y2, QPainter* painter)
{
    //printf ("drawNoFullScale called!\n");fflush(stdout);
    int i,j,mcol,mrow,mrow0,cnt=0;
    int mcol2,mrow2;

    //theStatusBar->message("Hit ESC to abort redrawing...",5000);

    mcol = x1/cube_side;
    mcol2 = (x2+1)/cube_side;
    mrow2 = (y2+1)/cube_side;
    mrow0 = y1/cube_side;
    //while ( (mcol<=mcol2) && (!escapeHitted) )
    while (mcol<=mcol2)
    {
        ++cnt;
        emit progress_cnt_changed(cnt);
        mrow = mrow0;

        // draw event rows
        while (mrow<=mrow2)
        { drawNoFullScaleEvent(mcol,mrow++,painter); }

        ++mcol;
        //if (!(mcol%512)) qApp->processEvents();
    }

    /*
    if (escapeHitted)
    {
        escapeHitted = false;
        canceledDrawRefresh();
        theStatusBar->message("Redrawing cancelled by the user!",5000);
        return false;
    }
    */

    //if (theStatusBar) theStatusBar->message("");
    return true;
}

bool
TwoDAvtView::drawSubSamplingScale(int x1,int x2, int y1, int y2, QPainter* painter)
{
    int i,j,mcol,mrow,mrow0,cnt=0;
    int mcol2,mrow2;

    double dstepx = 1.0/scf_x;
    double dstepy = 1.0/scf_y;
    int stepx = (int)ceil(dstepx);
    int stepy = (int)ceil(dstepy);
    int mstepx = stepx/cube_side;
    int mstepy = stepy/cube_side;
    
    /*
    printf ("drawSubSamplingScale called scf_x=%g, scf_y=%g, dstepx=%g, dstepy=%g, stepx=%d, stepy=%d mstepx=%d, mstepy=%d\n",
      scf_x,scf_y,dstepx,dstepy,stepx,stepy,mstepx,mstepy);fflush(stdout);
    */
    
    mcol = x1/cube_side;
    mcol2 = (x2+1)/cube_side;
    mrow2 = (y2+1)/cube_side;
    mrow0 = y1/cube_side;
    //while ( (mcol<=mcol2) && (!escapeHitted) )
    while (mcol<=mcol2)
    {
        cnt+=mstepx;
        emit progress_cnt_changed(cnt);
        mrow = mrow0;

        // draw event rows
        while (mrow<=mrow2)
        {
            drawSubSamplingScaleEvent(mcol,mrow,painter);
            mrow+=mstepy;
        }

        mcol+=mstepx;
    }

    return true;
}

void
TwoDAvtView::updateRowBar(double x,double y)
{
    if (y<0) y=0;
    if (y>=realContentsHeight) y = realContentsHeight-1;

    int col = (int)(x/getBaseElementSize());
    int row = (int)(y/getBaseElementSize());

    QString rowdesc;
    if (!myDB->hasValidData(col+myDB->getFirstEffectiveCycle(),row))
    {
        rowdesc = "";
    }
    else
    {
        rowdesc = myDB->getRowDescription(row);
    }
    emit row_msg_changed(rowdesc.leftJustify(30,' ',false));
}

void
TwoDAvtView::mouseWorldDoubleClickEvent (QMouseEvent* e, double x, double y)
{
    /** @todo This function became deprecated, remove !? */
}


void
TwoDAvtView::mouseWorldMoveEvent(QMouseEvent* e, double x, double y)
{
    int col = (int)(x/getBaseElementSize());
    int row = (int)(y/getBaseElementSize());

    if (col<0) col = 0;
    if (row<0) row = 0;

    // invoke parent implementation
    AScrollView::mouseWorldMoveEvent(e,x,y);

    // and now update the eventwindow if needed
    // look for the event on the matrix:
    if ( (eventWinEnabled) && !(e->state()&AltButton) )
    {
        if ( (row<numEvents) && (myDB->hasValidData(col+myDB->getFirstEffectiveCycle(),row)) )
        {
            RowType rtype = myDB->getRowType(row);
            ItemHandler hnd;
            INT32 trackId;
            switch (rtype)
            {
                case SimpleEdgeRow:
                draldb->getMoveItem(&hnd, myDB->getRowEdgeId(row),col+myDB->getFirstEffectiveCycle(),myDB->getRowEdgePos(row));
                if(hnd.isValidItemHandler())
                {
                    eventWin->updateEvent(&hnd,col+myDB->getFirstEffectiveCycle());
                }
                break;

                case SimpleNodeRow:
                trackId = myConfigDB->getTrackIdForRow(row);
                if (trackId>=0)
                {
                    eventWin->updateEvent(trackId,col+myDB->getFirstEffectiveCycle());
                }
                else
                { eventWin->reset();}
                break;

                default:
                Q_ASSERT(false);
            }
        }
        else
        {
            eventWin->reset();
        }
    }
}

void
TwoDAvtView::contentsSelectEvent(int col,int row, bool adding, bool shifted)
{
    bool needRep;

    if ( hasContentsOn(col,row) )
    {
        needRep = hlmgr->contentsSelectEvent(col+myDB->getFirstEffectiveCycle(),row,adding,currentTagSelector,htype);
        if (mdiLocked) emit contentsSelectedEvent((int)(col+myDB->getFirstEffectiveCycle()),row,adding,currentTagSelector,htype,true);
        
        // check shift key (synchro with Item window)
        if (!shifted && disWinEnabled)
        {
            disWin->alignWith(col+myDB->getFirstEffectiveCycle(),row);
        }
    }
    else
    {
        needRep = hlmgr->purgeLast();
        if (mdiLocked) emit purgeLastHighlight();
    }

    if (needRep)
    {
        // we can have other windows in synch...
        //((TwoDreams*)(mainWin))->repaintAllMDIContents(false);
        repaintContents(false);
    }
}


void 
TwoDAvtView::disableLastHightlight()
{
    bool needRep = hlmgr->purgeLast();
    if (needRep) repaintContents(false);
    if (mdiLocked) emit purgeLastHighlight();
}

void 
TwoDAvtView::disableAllHighlight()
{
    bool needRep = hlmgr->purgeAll();
    if (needRep) repaintContents(false);
    if (mdiLocked) emit purgeAllHighlight();
}

void 
TwoDAvtView::showHighlightCriteria()
{hlmgr->showCriteria();}

void 
TwoDAvtView::viewHEvents()
{
    vHEvents  = true;
    vNHEvents = false;
    repaintContents(false);
}

void 
TwoDAvtView::viewNHEvents()
{
    vHEvents  = false;
    vNHEvents = true;
    repaintContents(false);
}

void 
TwoDAvtView::viewAllEvents()
{
    vHEvents  = true;
    vNHEvents = true;
    repaintContents(false);
}

void
TwoDAvtView::updateCloseMenuState()
{
    //printf ("updateCloseMenuState() called with nc=%d\n",numCycles);
    TwoDreams* tdr = (TwoDreams*)mainWin;
    //tdr->enableCloseMenu(numCycles>1);
    tdr->enableMenuOptions(numCycles>1);
}

void
TwoDAvtView::drawDstRubber(int x, int y, int mx)
{
    QPainter p;
    p.begin( viewport() );
    p.setRasterOp( NotROP );
    p.setPen( QPen( color0, 1 ) );
    p.setBrush(NoBrush);
    p.setFont(QFont("Helvetica",(qApp->font()).pointSize()));

    //printf ("TwoDAvtView::drawDstRubber called with x=%d,y=%d,mx=%d,pressed_snaped_x=%d,pressed_y=%d\n",x,y,mx,pressed_snaped_x,pressed_y);

    // compute the x comp of vector
    double vx = (double)(x-pressed_snaped_x);
    double vy = 0;
    // normalize:
    double mod = sqrt(vx*vx);
    if (mod < 4.0)
    {
        p.end();
        return;
    }

    // central line:
    p.drawLine(pressed_snaped_x,y,x,y);

    // cota marks
    int vheight = visibleHeight();
    p.drawLine(pressed_snaped_x,0,pressed_snaped_x,vheight);
    p.drawLine(x,0,x,vheight);

    // put the current measure:
    QFontMetrics fm = p.fontMetrics();
    QRect tbr = fm.boundingRect(QString("Distance: ")+QString::number(abs(mx-pressed_mx)));

    vx /= mod;
    // rotate 90
    double rvy = vx;
    double rvx = -vy;

    double b = (double)(x-pressed_snaped_x);
    bool inv = (b<0.0);
    double px,py;
    px = pressed_snaped_x-(rvx*(double)tbr.height()*2.0);
    py = y-(rvy*(double)tbr.height()*2.0);
    px += (vx*mod*0.5);
    px -= (vx*(double)tbr.width()*0.5);

    if (inv)
    {
        p.translate(vx*(double)tbr.width(),vy*(double)tbr.width());
    }
    p.drawText ((int)floor(px),(int)floor(py),
    QString("Distance: ")+QString::number(abs((int)(mx - pressed_mx))));

    p.end();
}

void 
TwoDAvtView::clearShadedColumnsNoPropagate()
{
    if (cshadows==NULL) { return; }
    cshadows->clear();
    repaintContents(false);
}

void 
TwoDAvtView::clearShadedColumns()
{
    if (cshadows==NULL) { return; }
    cshadows->clear();
    repaintContents(false);
    
    // propagate is appropiated 
    if (mdiLocked) emit clearAllShadedCols(); 
}

void
TwoDAvtView::setAncestorTrackingTag(QString str)
{
    INT32 tagId = draldb->scanTagName(str);
    if (tagId<0)
    {
        qApp->beep();
        return;
    }
    if (ancWin!=NULL)
    {
        ancWin->setTrackingTag(tagId);
    }
}

void
TwoDAvtView::applyMagicWand (int mx,int my)
{
    //printf ("magic wind called on (%d,%d)\n",mx,my);fflush(stdout);
    popup_col = mx;
    popup_row = my;
    do_ancScan();
}

void
TwoDAvtView::updateCycleBar (double x)
{
    if (x<0.0)
    {
        x=0.0;
    }

    if (x>=realContentsWidth)
    {
        x = realContentsWidth-1.0;
    }

    int cycle = (int)(x/QMAX(1.0,getBaseElementSize()));
    cycle += myDB->getFirstEffectiveCycle();
    emit cycle_msg_changed(QString(" Cycle : ")+QString(AGTTypes::uint642str(cycle))+QString(" "));
}

void
TwoDAvtView::showDistance(int pressed_mx, int released_mx)
{
    pressed_mx += myDB->getFirstEffectiveCycle();
    released_mx += myDB->getFirstEffectiveCycle();

    emit status_msg_changed(QString("Cycles ")+
            QString::number((int)pressed_mx)+QString(" -> ")+
            QString::number((int)released_mx)+QString(" : ")+
            QString::number(abs(released_mx-pressed_mx)) +
            QString(" cycles "));
}

void
TwoDAvtView::keyReleaseEvent(QKeyEvent* e)
{
    // check for some keys
    switch (e->key())
    {
        case Key_Left:
             scrollBy(-cube_side,0);
             break;

        case Key_Right:
             scrollBy(cube_side,0);
             break;

        case Key_Home:
             setContentsPos(0,contentsY());
             break;

        case Key_End:
             setContentsPos(contentsWidth(),contentsY());
             break;

        case Key_Up:
             scrollBy(0,-cube_side);
             break;

        case Key_Down:
             scrollBy(0,cube_side);
             break;

        case Key_Prior:
             scrollBy(-visibleWidth(),0);
             break;

        case Key_Next:
             scrollBy(visibleWidth(),0);
             break;
    }
}

void
TwoDAvtView::updateEventWindow(ItemHandler * hnd)
{
    eventWin->updateEvent(hnd, myDB->getFirstEffectiveCycle());
}

QString 
TwoDAvtView::getTipString(bool eventOnly)
{
    int col = current_mx;
    int row = current_my;

    QString s = QString::null;
    
    switch (currentPointerType)
    {
        case POINTER_SELECT:

        if (row<numEvents)
        {
            if (myDB->hasValidData(col+myDB->getFirstEffectiveCycle(),row))
            {
                RowType rtype = myDB->getRowType(row);
                ItemHandler hnd;
                INT32 trackId;
                switch (rtype)
                {
                    case SimpleEdgeRow:
                    draldb->getMoveItem(&hnd, myDB->getRowEdgeId(row),col+myDB->getFirstEffectiveCycle(),myDB->getRowEdgePos(row));
                    if(hnd.isValidItemHandler())
                    {
                        s = ShowTags::getItemTags(&hnd,col+myDB->getFirstEffectiveCycle());
                    }
                    break;
                
                    case SimpleNodeRow:
                    trackId = myConfigDB->getTrackIdForRow(row);
                    if (trackId>=0)
                    {
                        s = ShowTags::getItemTags(trackId,col+myDB->getFirstEffectiveCycle());
                    }
                    break;
                
                    default:
                    Q_ASSERT(false);
                }
            }
            else
            {
                if (!eventOnly) s = getRowInfo(row);
            }
        }
        break;

        case POINTER_ZOOMING: 
        case POINTER_ZOOMING_NWIN:
        case POINTER_MWAND:
        s = "Cycle " + QString::number((UINT32)(current_mx+myDB->getFirstEffectiveCycle()));
        s = s + " Row " + QString::number(current_my);
        break;

        case POINTER_DISTANCE: 
        case POINTER_SHADING: 
        s = "Cycle " + QString::number((UINT32)(current_mx+myDB->getFirstEffectiveCycle()));
        break;
        
        default:
        s = QString::null;
    };
    return s;
}




Tip2Dreams::Tip2Dreams( QWidget * parent )
    : QToolTip( parent )
{
    setWakeUpDelay(1500);
}

void 
Tip2Dreams::maybeTip( const QPoint &pos )
{
    //printf ("Tip2Dreams::maybeTip called\n");
    if ( !parentWidget()->inherits( "TwoDAvtView" ) )
    {
        //printf ("Tip2Dreams::maybeTip no right parent, getting out\n");
        //hide();
        return;
    }

    TwoDAvtView *asv = (TwoDAvtView*)parentWidget();    
    /*
    if (asv->currentPointerType>=POINTER_DRAWLINE) return;
    if (asv->currentPointerType==POINTER_MWAND) return;
    if (asv->currentPointerType==POINTER_PANNING) return;
    */
    if (asv->currentPointerType!=POINTER_SELECT) return;
    
    QRect r(pos.x()-1,pos.y()-1,pos.x()+1,pos.y()+1);
    QString s = asv->getTipString(true);

    //if (s!=QString::null) printf ("Tip2Dreams::maybeTip got null string\n");
    if (s!=QString::null)
        tip( r, s );
    else
        hide();
}

void
TwoDAvtView::createDisWindow()
{
    // create the docked window

    disWin = new DisDockWin(myMDI,this);
    Q_ASSERT(disWin!=NULL);
    bool val = prefMgr->getShowDisassemblyWindow();  
    if (val)
    {
        disWin->show();
        setEnabledDisWindow(true);
    }
    else
    {
        disWin->hide();
        setEnabledDisWindow(false);
    }
    
    connect( disWin, SIGNAL(visibilityChanged(bool)), mainWin, SLOT(disWinVisibilityChanged(bool)));
    connect( disWin, SIGNAL(placeChanged(QDockWindow::Place)), mainWin, SLOT(disWinPlaceChanged(QDockWindow::Place)));
    connect( disWin, SIGNAL(orientationChanged(Orientation)), mainWin, SLOT(disWinOrientationChanged(Orientation)));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDAvtView::createEventWindow()
{
    // create the docked window
    eventWin = new EventDockWin(myMDI);
    Q_ASSERT(eventWin!=NULL);
    bool val = prefMgr->getShowEventWindow();  
    if ( val )
    {
        eventWin->show();
        setEnabledEventWindow(true);
    }
    else
    {
        eventWin->hide();
        setEnabledEventWindow(false);
    }
    
    connect( eventWin, SIGNAL(visibilityChanged(bool)),mainWin, SLOT(eventWinVisibilityChanged(bool)));
    connect( eventWin, SIGNAL(placeChanged(QDockWindow::Place)),mainWin, SLOT(eventWinPlaceChanged(QDockWindow::Place)));
    connect( eventWin, SIGNAL(orientationChanged(Orientation)), mainWin,SLOT(eventWinOrientationChanged(Orientation)));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDAvtView::createAncestorWindow()
{
    // create the Ancestor Docked Window
    ancWin = new AncestorDockWin(annCtrl,myMDI,this,hlmgr);
    Q_ASSERT(ancWin!=NULL);
    if ( prefMgr->getShowRelationshipWindow() )
    {
        ancWin->show();
        setEnabledAncestorWindow(true);
    }
    else
    {
        ancWin->hide();
        setEnabledAncestorWindow(false);
    }
    connect( ancWin, SIGNAL(visibilityChanged(bool)),mainWin, SLOT(ancWinVisibilityChanged(bool)));
    connect( ancWin, SIGNAL(placeChanged(QDockWindow::Place)),mainWin, SLOT(ancWinPlaceChanged(QDockWindow::Place)));
    connect( ancWin, SIGNAL(orientationChanged(Orientation)), mainWin,SLOT(ancWinOrientationChanged(Orientation)));
}

void 
TwoDAvtView::slotTgSelComboActivated( const QString &s )
{
    currentTagSelector = s;
    hlmgr->purgeAll();
    repaintContents(false);
}


void 
TwoDAvtView::annotationCopy()
{
    // does just a copy of the current selected guy
    if (currentAnnItem)
    {
        if (copyAnnItem) delete copyAnnItem;
        copyAnnItem = currentAnnItem->clone();
    }
    else
    {
        copyAnnItem = NULL;
    }
}

void 
TwoDAvtView::annotationPaste()
{
    if (copyAnnItem)
    {
        if (currentAnnItem)
        {
            currentAnnItem->setSelected(false);
            annCtrl->repaintAnnItem(currentAnnItem,scf_x,scf_y);
        }
        currentAnnItem = annCtrl->annotationPaste(copyAnnItem);
        copyAnnItem=currentAnnItem->clone();
    }
}

void 
TwoDAvtView::annotationRemove()
{
    if (currentAnnItem) 
    {
        annCtrl->annotationRemove(currentAnnItem);
        currentAnnItem=NULL;
        pressAnnItem=NULL;
    }
}

void 
TwoDAvtView::cloneAnnotationCtrl(AnnotationCtrl* parent)    
{ annCtrl->clone(parent); }

/*
void
TwoDAvtView::escKey()
{
    //printf ("esc hitted\n");fflush(stdout);
    escapeHitted = true;
}

bool
TwoDAvtView::eventFilter ( QObject * watched, QEvent * e )
{
    if (!redrawing) return AScrollView::eventFilter( watched, e );
    //printf ("eventFilter called while redrawing!\n");

    if ( (watched==horizontalScrollBar())||(watched==verticalScrollBar()) )
    {
        //printf (">>>scrollbars..., type=%d\n",e->type());fflush(stdout);
        bool c1 = e->type() == QEvent::MouseButtonPress;
        bool c2 = e->type() == QEvent::MouseButtonRelease;
        bool c3 = e->type() == QEvent::MouseButtonDblClick;
        bool c4 = e->type() == QEvent::MouseMove;

        return c1||c2||c3||c4;
    }
    else if (watched==viewport())
    {
        //printf (">>>viewport..., type=%d\n",e->type());fflush(stdout);
        return !((e->type()==QEvent::MouseButtonPress)||(e->type()==QEvent::MouseButtonRelease));
    }
    else if (watched==myParent)
    {
        //printf (">>>main..., type=%d\n",e->type());fflush(stdout);
        if (
            (e->type()==QEvent::AccelAvailable) ||
            (e->type()==QEvent::AccelOverride) ||
            (e->type()==QEvent::KeyPress) ||
            (e->type()==QEvent::KeyRelease)||
            (e->type()==QEvent::Destroy)||
            (e->type()==QEvent::Hide)||
            (e->type()==QEvent::Close)||
            (e->type()==QEvent::Quit)||
            (e->type()==QEvent::WindowDeactivate)
            ) return false;

        return TRUE; // eat event
    }
    else
    {
        Q_ASSERT(false);
    }
    return false;
}
*/

