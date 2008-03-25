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
  * @file DisTab.cpp
  */

// 2Dreams
#include "DisDockWin.h"
#include "TwoDAvtView.h"
#include "HighLightMgr.h"
#include "CSLEngine.h"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DisTab::DisTab(TabSpec* tspec, DisDockWin* dockRef,QWidget*parent,const char*name,WFlags f) : QVBox(parent,name,f)
{
    dock = dockRef;
    tabSpec = tspec;
    setSpacing(0);
    headers = new HeadScrollView(this,this);
    myDis = new DisView(this,this);

    text_height=1;

    strList = NULL;
    itemList = new QValueList<disEntry>();
    btnList = new QPtrList<QLabel>();
    colLen = NULL;

    sortFieldName = QString::null;
    selected=-1;

    // for some reason if we add/remove widgets to headers 'a posteriori' they don't get displayed!?
    int col=0;
    for (int i=0;i<MAXDISCOLS;i++)
    {
        QLabel* pb = new QLabel("",headers->viewport());
        pb->setAlignment(AlignCenter);
        pb->setFrameStyle( QFrame::Panel | QFrame::Raised );
        btnList->append(pb);
        headers->addChild(pb,col,0);
        col += pb->width();
        col += 1;
    }
}

DisTab::~DisTab()
{
    if (strList!=NULL) { delete strList; }
    if (colLen!=NULL) { delete colLen; }

    delete itemList;
    delete btnList;
}

void
DisTab::setFieldList (QStringList* list)
{
    strList = new QStringList(*list);
    int col=0;
    int cnt=0;
    QLabel* pb;
    for (QStringList::Iterator it = list->begin(); it !=list->end(); it++)
    {
        if ((int)(btnList->count()) > cnt)
        {
            pb = btnList->at(cnt);
            pb->setText(*it);
            col += pb->width();
            col += 1;
            ++cnt;
        }
    }

    if ((int)(btnList->count()) > cnt)
    {
        pb = btnList->at(cnt);
        pb->setText("");
        col += pb->width();
        headers->resizeContents(col+pb->width()+99,pb->height());
    }
}

void
DisTab::setFieldLens (QValueList<INT32>* lens)
{
    colLen = lens;
    Q_ASSERT(colLen->count() == strList->count() );
    updateFieldLens();
}

void
DisTab::updateFieldLens()
{
    //printf("updateFieldLens called\n");fflush(stdout);
    if (strList==NULL)
    {
        return;
    }
        
    QFontMetrics fm (font());
    QRect tbref = fm.boundingRect(PATTERNSTR);

    int pos=0;
    int col=0;
    QStringList::Iterator strit = strList->begin();
    for (QValueList<INT32>::Iterator it = colLen->begin();it != colLen->end(); it++)
    {
        QRect tbr = fm.boundingRect(*strit);
        int width = QMAX(tbr.width()+DISCOLMARGIN,*it);
        btnList->at(pos)->resize(width,tbref.height()+VBTNMARGIN);
        (*it) = btnList->at(pos)->width();
        headers->moveChild(btnList->at(pos),col,0);
        col += btnList->at(pos)->width();

        ++pos;
        ++strit;
    }
    btnList->at(pos)->resize(999,tbref.height()+VBTNMARGIN);
    headers->moveChild(btnList->at(pos),col,0);
    col+=btnList->at(pos)->width();

    for (int i=pos+1;i<MAXDISCOLS;i++)
    {
        headers->moveChild(btnList->at(i),col,0);
        col += btnList->at(i)->width();
    }

    headers->resizeContents(myDis->contentsWidth()+99,btnList->at(pos)->height());
}

void
DisTab::setSortFieldName(QString value)
{ sortFieldName = value; }

void
DisTab::setTextHeight(int value)
{
    //printf ("DisTab::setTextHeight called\n");fflush(stdout);

    text_height = value;
    headers->setTextHeight(value);
    headers->resize(headers->contentsWidth(),value+VBTNMARGIN);
    headers->resizeContents(headers->contentsWidth(),value+VBTNMARGIN);
    int col=0;
    for (int i=0;i<MAXDISCOLS;i++)
    {
        QLabel* pb = btnList->at(i);
        pb->resize(pb->width(),value+VBTNMARGIN),
        headers->addChild(pb,col,0);
        col += pb->width();
        col += 1;
    }
    headers->updateGeometry();
}

void
DisTab::disMoved (int x, int y)
{ headers->setContentsPos(x,y); }

void
DisTab::doSync()
{ dock->doSync(); }

void
DisTab::setSelection(int row)
{
    myDis->setSelection(0,row*text_height);
}


void
DisTab::reset()
{
    itemList->clear();
    updateFieldLens();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DisView::DisView (QWidget* parent,DisTab* disTab) :
         QScrollView(parent,"ddisplay",WStaticContents|WResizeNoErase)
{
    setResizePolicy(Manual);
    viewport()->setBackgroundMode(FixedColor);
    viewport()->setBackgroundColor(qApp->palette().color(QPalette::Active,QColorGroup::Base));

    myTab = disTab;
    connect(this,SIGNAL(contentsMoving(int,int)),myTab,SLOT(disMoved(int,int)));
}

void
DisView::styleChange(QStyle& oldStyle)
{
    /*
    printf ("after style change contents size %d,%d\n",contentsWidth(),contentsHeight());fflush(stdout);
    printf ("global size %d,%d\n",width(),height());fflush(stdout);
    */
}

void
DisView::contentsMouseDoubleClickEvent(QMouseEvent* e)
{
    setSelection(e->x(),e->y());
    doSync();
}

void
DisView::contentsMousePressEvent(QMouseEvent* e)
{
    pressed_x = e->x();
    pressed_y = e->y();
}

void
DisView::contentsMouseReleaseEvent(QMouseEvent* e)
{
    released_x = e->x();
    released_y = e->y();
    bool c1 = (abs(released_x-pressed_x)<3);
    bool c2 = (abs(released_y-pressed_y)<3);

    if (c1 && c2)
    {
        setSelection(released_x,released_y);
    }
}

void
DisView::setSelection(int x, int y)
{
    INT32 row = (INT32)floor((double)y/(double)(myTab->text_height));

    // clear any previous selection
    INT32 old = myTab->selected;
    myTab->selected = row;
    if (old>=0)
    {
        repaintContents(0,old*myTab->text_height-2,9999,myTab->text_height+4,true);
    }
    repaintContents(0,row*myTab->text_height-2,9999,myTab->text_height+4,true);
    ensureVisible(0,row*myTab->text_height,0,myTab->text_height - 1);
    setFocus();

    // update event window
    QValueList<disEntry>::iterator it = myTab->itemList->at(row);
    ((myTab->getDisDockWin())->getTwoDAvtView())->updateEventWindow(&(*it).handler);
}

void
DisView::doSync()
{ myTab->doSync(); }

void
DisView::keyReleaseEvent(QKeyEvent* e)
{
    INT32 lcol;
    INT32 bottomCol = (int)(myTab->getItemList()->count()-1);
    if (bottomCol<0)
    {
        bottomCol=0;
    }

    // check for some keys
    switch (e->key())
    {
        case Key_Return:
        case Key_Enter:
        case Key_Space:
             doSync();
             break;

        case Key_Left:
             scrollBy(-10,0);
             break;

        case Key_Right:
             scrollBy(10,0);
             break;

        case Key_Home:
             setSelection(0,0);
             break;

        case Key_End:
             setSelection(0,bottomCol * myTab->text_height);
             break;

        case Key_Up:
             setSelection(0,QMAX(myTab->selected - 1,0) * myTab->text_height );
             break;

        case Key_Down:
             setSelection(0,QMIN(myTab->selected + 1,bottomCol) * myTab->text_height);
             break;

        case Key_Prior:
             lcol = (INT32)floor((double)visibleHeight()/(double)myTab->text_height);
             lcol = QMAX(lcol,0);
             scrollBy(0,-lcol*myTab->text_height);
             setSelection(0,contentsY()+visibleHeight()-myTab->text_height);
             break;

        case Key_Next:
             lcol = (INT32)floor((double)visibleHeight()/(double)myTab->text_height);
             lcol = QMIN(lcol,bottomCol);
             scrollBy(0,lcol*myTab->text_height);
             setSelection(0,contentsY());
             break;
    }
}

void
DisView::drawContents (QPainter * p,int clipx,int clipy,int clipw,int cliph)
{
    //printf("drawContents called (%d,%d) width=%d, height=%d\n",clipx,clipy,clipw,cliph);fflush(stdout);

    INT16 tgbase;
    UINT64 tgvalue;
    UINT64 tgwhen;
    TagValueType  tgvtype;

    p->fillRect(clipx,clipy,clipw,cliph,qApp->palette().color(QPalette::Active,QColorGroup::Base));

    INT32 rowSize = myTab->text_height;
    INT32 row0 = (INT32) floor((double)clipy/(double)rowSize);
    if (row0<0)
    {
        row0=0;
    }
    INT32 row1 = row0+(INT32)ceil((double)cliph/(double)rowSize);
    INT32 row=row0;
    if (row >= (INT32)(myTab->itemList->count()) )
    {
        return;
    }

    QValueList<disEntry>::const_iterator it = myTab->itemList->at(row);

    QStringList* strlist = myTab->strList;
    QStringList::iterator strit;

    QValueList<INT32>* colLen = myTab->colLen;
    QValueList<INT32>::iterator lenit;

    ZDBase*   myDB = ZDBase::getInstance();
    CSLEngine* csle = CSLEngine::getInstance();

    while ( (row<=row1) && (it!=myTab->itemList->end()) )
    {
        ItemHandler hnd = (*it).handler;

        // loop arround each column
        lenit = colLen->begin();
        INT32 col0 = 0;
        for ( strit = strlist->begin(); strit != strlist->end(); ++strit )
        {
            // get latest known value
            QString dbstr="";
            INT32 ridx = hnd.getTagByName(*strit,&tgvalue,&tgvtype,&tgbase,&tgwhen);
            if(ridx>=0)
            {
                dbstr = hnd.getFormatedTagValue();
            }
            // check for font Properties
            ItemWinFont prop;
            csle->getItemWinFontProperties(myTab->getTabSpec(),&hnd,prop);
            QFont pfont = p->font();
            pfont.setItalic(prop.italic);
            pfont.setBold(prop.bold);
            pfont.setUnderline(prop.underline);
            p->setFont(pfont);
            p->setPen(QPen(prop.color));
            p->setBrush(qApp->palette().color(QPalette::Active,QColorGroup::Base));

            // check if is a selected row
            if (row==myTab->selected)
            {
                QColor penColor = qApp->palette().color(QPalette::Active,QColorGroup::HighlightedText);
                QColor brushColor = qApp->palette().color(QPalette::Active,QColorGroup::Highlight);
                p->setPen(QPen(penColor));
                p->fillRect(col0,row*rowSize,(*lenit)+DISCOLMARGIN,rowSize,brushColor);
            }
            p->drawText(col0+(DISCOLMARGIN/2),row*rowSize,*lenit,9999,0,dbstr);
            col0+=(*lenit);
            ++lenit;
        }
        ++row;
        ++it;
    }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DisDockWin::DisDockWin( QMainWindow* mw, AScrollView* asv)
{
    myAvt = (TwoDAvtView*)asv;
    currentTab = NULL;

    prefMgr = PreferenceMgr::getInstance();
    setFocusPolicy(ClickFocus);

    myDB = ZDBase::getInstance();
    myConfigDB = ConfigDB::getInstance();
    draldb = DralDB::getInstance();

    myMW = mw;

    progress = new QProgressDialog ("Creating Tabs...", "&Cancel",1, mw,"lbl_progress", TRUE);
    Q_ASSERT(progress!=NULL);
    progress->setMinimumDuration(250);

    max_width=0;
    tabs.clear();

    setResizeEnabled( TRUE );
    setCloseMode(QDockWindow::Always);
    setMovingEnabled(true);
    setCaption("Item Window");

    mw->addToolBar( this);
    mw->moveDockWindow ( this, prefMgr->getDisassemblyWindowDockPolicy(),
    prefMgr->getDisassemblyWindowNl(), prefMgr->getDisassemblyWindowIdx(),
    prefMgr->getDisassemblyWindowOffset());

    tabWidget = new DTabWidget(this);
    Q_ASSERT(tabWidget!=NULL);

    connect (tabWidget,SIGNAL(currentChanged(QWidget*)),this,SLOT(tabChanged(QWidget*)));

    setWidget(tabWidget);
    show();
}

DisDockWin::~DisDockWin()
{}

void
DisDockWin::createTabs()
{
    //printf ("createTabs() called\n");fflush(stdout);

    QApplication::setOverrideCursor( Qt::WaitCursor );

    // 1) check for non split by rules in ConfigDB
    TabSpecList* tabspeclist = myConfigDB->getTabSpecList();
    TabSpec* tab = tabspeclist->first();
    while (tab != NULL)
    {
        if (! tab->isSplit())
        {
            //printf("checking non split tab %s\n",tab->getName().latin1());fflush(stdout);
            DisTab* dtab = new DisTab(tab,this,this);
            Q_ASSERT(dtab!=NULL);
            tabs.push_back(dtab);
            setTagList(tab->getShowList(),dtab, tab->getSortBy());
            tabWidget->addTab(dtab,tab->getName());
        }
        else
        {
            scanSplitBy(tab);
        }
        tab = tabspeclist->next();
    }

    if (tabspeclist->count() > 0)
    {
        applyFont();
        tabWidget->setCurrentPage (0);
        // force first refresh
        computeItemLists();
        tabWidget->repaint();
    }

    QApplication::restoreOverrideCursor();
}

void
DisDockWin::scanSplitBy(TabSpec* tab)
{
    //printf ("checking SPLIT tab %s\n",tab->getName().latin1());fflush(stdout);
    UINT64 tgvalue;

    QString tagName = tab->getSelectTag();
    INT32 currentIdx=0;

    ItemHandler hnd;
    draldb->getFirstItem(&hnd);
    while ( hnd.isValidItemHandler() )
    {
        if (hnd.hasTag(tagName))
        {
            QString dbstr = hnd.getFormatedTagValue();
            if (!tab->hasKnownValue(dbstr))
            {
                tab->addKnownValue(dbstr);
                DisTab* dtab = new DisTab(tab,this,this);
                Q_ASSERT(dtab!=NULL);
                tabs.push_back(dtab);
                setTagList(tab->getShowList(),dtab, tab->getSortBy());
                tabWidget->addTab(dtab,tab->getName()+" - "+dbstr);
                bool vok = hnd.getCurrentTagValue(&tgvalue);
                dtab->setSplitByValue(tgvalue);
            }
        }
        hnd.skipToNextItem();
    }
    //printf ("checking SPLIT tab %s done!\n",tab->getName().latin1());fflush(stdout);
}

void
DisDockWin::applyFont()
{
    QFont neof = QFont("Courier",(qApp->font()).pointSize());
    QFontMetrics fm(neof);
    QRect tbr = fm.boundingRect(PATTERNSTR);
    int text_height = tbr.height()+DISVMARGIN;

    // propagate font for each tab
    for (int i=0;i<(int)(tabs.size());i++)
    {
        tabs[i]->getDisView()->setFont(neof);
        tabs[i]->setTextHeight(text_height);
    }
}

void
DisDockWin::fontChange ( const QFont & oldFont )
{
    applyFont();

    // we need to recompute all the column widths...
    for (int i=0;i<(int)(tabs.size());i++)
    { tabs[i]->reset(); }

    computeItemLists();
    if (currentTab!=NULL)
    {
        currentTab->getDisView()->repaintContents(true);
    }
}

void
DisDockWin::setTagList(QStringList* strlist,DisTab* tab, QString sorting)
{
    //printf ("setTagList called over tab=%d\n",tab);fflush(stdout);

    QString sortFieldName;
    if (sorting!=QString::null)
    {
        sortFieldName = sorting;
    }
    else
    {
        if (strlist->count()>0)
        {
            sortFieldName = *(strlist->at(0)); // first column by default
        }
    }

    tab->setFieldList(strlist);
    tab->setSortFieldName(sortFieldName);

    QValueList<INT32>* lens = new QValueList<INT32>();

    // min width for any column
    for ( QStringList::Iterator it = strlist->begin(); it != strlist->end(); ++it )
    {
        lens->append(MINDISCOLWIDTH);
    }
    tab->setFieldLens(lens);
}


void
DisDockWin::computeItemLists()
{
    //printf(">>computeItemLists called\n");fflush(stdout);

    // empty db?
    if (myDB->getNumCycles()<=0)
    {
        //printf("Empty db!, giving up\n");fflush(stdout);
        return;
    }
    if (tabs.size()<1)
    {
        return;
    }

    QFontMetrics fm(tabs.at(0)->getDisView()->font());
    QRect tbref = fm.boundingRect(PATTERNSTR);
    int text_height = tbref.height()+DISVMARGIN;

    ItemHandler hnd;
    draldb->getFirstItem(&hnd);

    // put clock cursor...
    QApplication::setOverrideCursor( Qt::WaitCursor );

    int numSteps = draldb->getItemHeapNumItems();
    progress->setTotalSteps(numSteps);

    int doneSteps=0;

    INT16 tgbase;
    UINT64 tgwhen;
    UINT64 tgvalue;
    TagValueType  tgvtype;

    int  tabcount = (int)tabs.size();
    bool selected;
    int  auxcnt=0;
    
    //printf("starting disdock()... \n");fflush(stdout);

    // WARNING do not change loop order, this way we scan tag Heap only once and improve cache performance.
    while ( hnd.isValidItemHandler() )
    {
        if (auxcnt%1024==0)
        {
            progress->setProgress(auxcnt);
            qApp->processEvents();
            if ( progress->wasCancelled() ) break;
        }

        // loop over the tabs
        for (int tab=0;tab<tabcount;tab++)
        {
            selected=true;

            TabSpec* tspec = tabs.at(tab)->getTabSpec();
            QString selectTag = tspec->getSelectTag();

            // check withtag
            if (tspec->getWithTag() != QString::null)
            {
                bool hasTag = hnd.hasTag(tspec->getWithTag());
                selected = selected && hasTag;
            }

            // check select/split cases
            if (selectTag != QString::null)
            {
                UINT64 matchingValue=0;
                if (tspec->isSplit())
                {
                    matchingValue = tabs.at(tab)->getSplitByValue();
                }
                else
                {
                    matchingValue = tspec->getMatchingValue();
                }
                bool htag = hnd.hasTag(selectTag);
                if (htag) { hnd.getCurrentTagValue(&tgvalue); }
                selected = selected && (htag) && (matchingValue == tgvalue);
            }

            if (selected)
            {
                // compute colums width & sorting criterion
                QString sortField = tabs.at(tab)->getSortFieldName();
                UINT64 sortValue=0;

                QValueList<INT32>::Iterator lenit = (tabs.at(tab)->getColLen())->begin();
                QStringList* strlist = tabs.at(tab)->getStrList();
                for (QStringList::Iterator it = strlist->begin(); it != strlist->end(); ++it)
                {
                    QString fieldName = *it;
                    if (hnd.hasTag(fieldName))
                    {
                        // compute column width
                        QRect tbr = fm.boundingRect(hnd.getFormatedTagValue());
                        *lenit = QMAX(*lenit,tbr.width()+DISCOLMARGIN);
                    }
                    ++lenit;
                }

                if (hnd.hasTag(sortField))
                {
                    bool gok = hnd.getCurrentTagValue(&sortValue);
                    Q_ASSERT(gok);
                }
                tabs.at(tab)->getItemList()->append(disEntry(&hnd,sortValue));
            }
        }

        // step until the next "Item Id"
        auxcnt++;
        hnd.skipToNextItem();
    }
    //printf("disdock() selection completed... \n");fflush(stdout);

    progress->setProgress(numSteps);
    max_width = 0;
    for (int k=0;k<tabcount;k++)
    {
        DisTab* ctab = tabs[k];
        qHeapSort(*(ctab->getItemList()));
        int tab_width = 0;
        QValueList<INT32>* colList = ctab->getColLen();
        QValueList<INT32>::Iterator lenit = colList->begin();
        while (lenit != colList->end())
        {
            tab_width += *lenit;
            ++lenit;
        }
        max_width = QMAX(max_width,tab_width);

        ctab->getDisView()->resizeContents(tab_width+DISCOLMARGIN,
              text_height*ctab->getItemList()->count()+DISCOLMARGIN);

        ctab->updateFieldLens();
    }

    //printf("disdock() sorting() completed... \n");fflush(stdout);

    QApplication::restoreOverrideCursor();
    return;
}

void
DisDockWin::doSync()
{
    bool searchOk=true;

    QValueList<disEntry>* itemList = currentTab->getItemList();
    INT32 selRow = currentTab->getSelected();
    if (selRow >= (INT32)itemList->count())
    {
        return;
    }

    INT32 col = -1;
    INT32 row = -1;

    ItemHandler hnd (((*itemList)[selRow]).handler);
    Q_ASSERT(hnd.isValidItemHandler());
    INT32 itemId = hnd.getItemId();
    searchOk = myDB->lookForFirstAppearence(itemId,myDB->getFirstEffectiveCycle(),col,row);

    if(!searchOk)
    {
        QMessageBox::critical (this,"Scanning Error",
        "Unable to find any Event related with this ItemID",
        QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
    }
    else
    {
        myAvt->goToColumn(col-myDB->getFirstEffectiveCycle());
        myAvt->goToRow(row);
        myAvt->shadeColumn(col-myDB->getFirstEffectiveCycle(),true);
        
        (myAvt->getHighLightMgr())->contentsSelectEvent(col,row,true,myAvt->getCurrentTagSelector(),HLT_DEFAULT,true);
        myAvt->repaintContents(false);

        // synch other tabs
        if (prefMgr->getKeepSynchTabs())
        {
            alignWith(col,row);
        }
    }
}

void
DisDockWin::reset()
{
    //printf("dis reset called\n");fflush(stdout);fflush(stderr);

    // put clock cursor...
    QApplication::setOverrideCursor( Qt::WaitCursor );

    for (int i=0;i<(int)(tabs.size());i++)
    {
        delete tabs[i];
    }
    tabs.clear();

    QApplication::restoreOverrideCursor();
}

void
DisDockWin::alignWith(INT32 col,INT32 row)
{
    if (myDB->getRowType(row) != SimpleEdgeRow) { return; }

    // 1) get Item Id at this col/row
    ItemHandler hnd;
    draldb->getMoveItem(&hnd, myDB->getRowEdgeId(row),col,myDB->getRowEdgePos(row));
    if (!hnd.isValidItemHandler())
    {
        //printf("neg. heap index in col=%d,row=%d\n",col,row);
        return;
    }
    bool somefnd = false;
    bool keepCurrentTab=false;

    bool fnd;
    int frow;
    DisTab* firstMatchTab=NULL;
    for (int i=0;i<(int)(tabs.size());i++)
    {
        DisTab* ctab=tabs[i];
        frow = 0;
        fnd = false;
        QValueList<disEntry>* itemList = ctab->getItemList();
        QValueList<disEntry>::iterator it = itemList->begin();
        while (!fnd && (it != itemList->end()) )
        {
            fnd = ((*it).handler == hnd);
            ++frow;
            ++it;
        }
        somefnd = somefnd || fnd;
        if (fnd)
        {
            if (firstMatchTab==NULL)
            {
                firstMatchTab=ctab;
            }
            keepCurrentTab = keepCurrentTab || (ctab==currentTab);
            ctab->setSelection(frow-1);
        }
    }
    if (!keepCurrentTab)
    {
        tabWidget->showPage(firstMatchTab);
    }
}

void
DisDockWin::resizeEvent ( QResizeEvent * e )
{
    if (!dockInit)
    {
        prefMgr->setDisassemblyWindowWidth(width());
        prefMgr->setDisassemblyWindowHeight(height());
    }
}

void
DisDockWin::tabChanged(QWidget* widget)
{ currentTab = (DisTab*)widget; }


/*
void 
DisDockWin::setCurrentAVT(TwoDAvtView* c)
{
    myAvt=c;
}
*/
