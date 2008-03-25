/*
 * Copyright (C) 2005-2006 Intel Corporation
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

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "avt/DAvtView.h"
#include "dockwin/tag/TagDockWin.h"
#include "highlight/HighLightMgr.h"
#include "mdi/MDIDreamsWindow.h"
#include "DisDockWin.h"
#include "TabSpec.h"
#include "DreamsDB.h"
#include "Dreams.h"
#include "TagSelector.h"

// Qt includes.
#include <qapplication.h>
#include <qprogressdialog.h>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DisTab::DisTab(DreamsDB * dreamsdb, TabSpec* tspec, DisDockWin* dockRef,QWidget*parent,const char*name,WFlags f) : QVBox(parent,name,f)
{
    dock = dockRef;
    tabSpec = tspec;
    setSpacing(0);
    headers = new HeadScrollView(this,this);
    myDis = new DisView(dreamsdb, this,this);

    text_height=1;

    strList = NULL;
    itemList = new QValueList<DisEntry>();
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
{
    sortFieldName = value;
}

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
{
    headers->setContentsPos(x,y);
}

void
DisTab::doSync(bool adding, bool moving)
{
    dock->doSync(adding, moving);
}

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

DisView::DisView(DreamsDB * _dreamsdb, QWidget * parent, DisTab * disTab)
    : QScrollView(parent, "ddisplay", WStaticContents | WResizeNoErase)
{
    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();

    setResizePolicy(Manual);
    viewport()->setBackgroundMode(FixedColor);
    viewport()->setBackgroundColor(qApp->palette().color(QPalette::Active, QColorGroup::Base));

    myTab = disTab;
    connect(this, SIGNAL(contentsMoving(int, int)), myTab, SLOT(disMoved(int, int)));
}

DisView::~DisView()
{
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
    doSync(e->state() & Qt::ControlButton, e->state() & Qt::ShiftButton);
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
    INT32 row = (INT32) floor((double) y / (double) myTab->text_height);

    // clear any previous selection
    INT32 old = myTab->selected;
    myTab->selected = row;
    if(old >= 0)
    {
        repaintContents(0, old * myTab->text_height - 2, 9999, myTab->text_height + 4, true);
    }

    repaintContents(0, row * myTab->text_height - 2, 9999, myTab->text_height + 4, true);
    ensureVisible(0, row * myTab->text_height, 0, myTab->text_height - 1);
    setFocus();

    // update event window
    QValueList<DisEntry>::iterator it = myTab->itemList->at(row);

    QMainWindow* mwin=myTab->getDisDockWin()->myMdi;
    MDIDreamsWindow * mdi = static_cast<MDIDreamsWindow *>(mwin);
    CYCLE cycle = draldb->getLastCycle();

    mdi->getTagWindow()->updateItemTag(&(*it).handler, cycle);
}

void
DisView::doSync(bool adding, bool moving)
{
    myTab->doSync(adding, moving);
}

void
DisView::keyReleaseEvent(QKeyEvent * e)
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
             doSync(e->state() & Qt::ControlButton, e->state() & Qt::ShiftButton);
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
DisView::drawContents(QPainter * p, int clipx, int clipy, int clipw, int cliph)
{
    //printf("drawContents called (%d,%d) width=%d, height=%d\n",clipx,clipy,clipw,cliph);fflush(stdout);

    TagValueBase tgbase;
    UINT64 tgvalue;
    TagValueType tgvtype;
    CYCLE tgwhen;
    CYCLE cycle = draldb->getLastCycle();

    p->fillRect(clipx, clipy, clipw, cliph, qApp->palette().color(QPalette::Active,QColorGroup::Base));

    INT32 rowSize = myTab->text_height;
    INT32 row0 = (INT32) floor((double) clipy / (double) rowSize);
    if(row0 < 0)
    {
        row0 = 0;
    }

    INT32 row1 = row0 + (INT32) ceil((double) cliph / (double) rowSize);
    INT32 row = row0;
    if(row >= (INT32) (myTab->itemList->count()))
    {
        return;
    }

    QValueList<DisEntry>::const_iterator it = myTab->itemList->at(row);

    QStringList * strlist = myTab->strList;
    QStringList::iterator strit;

    QValueList<INT32> * colLen = myTab->colLen;
    QValueList<INT32>::iterator lenit;

    while((row <= row1) && (it != myTab->itemList->end()))
    {
        ItemHandler hnd = (* it).handler;

        // loop arround each column
        lenit = colLen->begin();
        INT32 col0 = 0;
        for(strit = strlist->begin(); strit != strlist->end(); ++strit)
        {
            // get latest known value
            QString dbstr="";
            if(hnd.getTagByName(* strit, &tgvalue, &tgvtype, &tgbase, &tgwhen, cycle))
            {
                dbstr = hnd.getTagFormatedValue();
            }
            // check for font Properties
            CSLData data;

            myTab->getTabSpec()->evalItemWinFontProperties(&hnd, &data);

            QFont pfont = p->font();
            pfont.setItalic(data.getItalic());
            pfont.setBold(data.getBold());
            pfont.setUnderline(data.getUnderline());
            p->setFont(pfont);
            p->setPen(QPen(data.getLetterColor()));
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

DisDockWin::DisDockWin(DreamsDB * _dreamsdb, QMainWindow * mdi, QMainWindow * mw, DAvtView * asv)
{
    currentTab = NULL;

    prefMgr = PreferenceMgr::getInstance();
    setFocusPolicy(ClickFocus);

    dreamsdb = _dreamsdb;
    draldb = dreamsdb->getDralDB();

    myMW = mw;
    myMdi = mdi;
    myAvt = asv;

    progress = new QProgressDialog ("Creating Tabs...", "&Cancel",1, mdi,"lbl_progress", TRUE);
    Q_ASSERT(progress!=NULL);
    progress->setMinimumDuration(250);

    max_width=0;
    tabs.clear();

    setResizeEnabled( TRUE );
    setCloseMode(QDockWindow::Always);
    setMovingEnabled(true);
    setCaption("Item Window");

    mdi->addToolBar(this);
    mdi->moveDockWindow(this, prefMgr->getDisassemblyWindowDockPolicy(),
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

    // 1) check for non split by rules in DreamsDB
    TabSpecList* tabspeclist = dreamsdb->getTabSpecList();
    TabSpec* tab = tabspeclist->first();
    while (tab != NULL)
    {
        if (! tab->isSplit())
        {
            //printf("checking non split tab %s\n",tab->getName().latin1());fflush(stdout);
            DisTab* dtab = new DisTab(dreamsdb, tab, this, this);
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
            QString dbstr = hnd.getTagFormatedValue();
            if (!tab->hasKnownValue(dbstr))
            {
                tab->addKnownValue(dbstr);

                DisTab * dtab = new DisTab(dreamsdb, tab, this, this);
                Q_ASSERT(dtab != NULL);
                tabs.push_back(dtab);

                setTagList(tab->getShowList(), dtab, tab->getSortBy());
                tabWidget->addTab(dtab, tab->getName() + " - " + dbstr);
                tgvalue = hnd.getTagValue();
                dtab->setSplitByValue(tgvalue);
            }
        }
        hnd.nextItem();
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
    if(dreamsdb->getNumCycles(0).cycle <= 0)
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

    TagValueBase tgbase;
    UINT64 tgwhen;
    UINT64 tgvalue = 0;
    TagValueType  tgvtype;

    int  tabcount = (int)tabs.size();
    bool selected;
    int  auxcnt=0;
    
    //printf("starting disdock()... \n");fflush(stdout);

    UINT64 * values;  ///< Vector where the values of the tags of the actual item are stored.
    bool * valids;    ///< Vector that stores which tags are defined for the actual item.
    INT32 num_tags;   ///< Number of tags defined.

    num_tags = draldb->getNumTags();
    valids = new bool[num_tags];
    values = new UINT64[num_tags];

    // WARNING do not change loop order, this way we scan tag Heap only once and improve cache performance.
    while(hnd.isValidItemHandler())
    {
        TAG_ID tag_id;

        if (auxcnt%1024==0)
        {
            progress->setProgress(auxcnt);
            qApp->processEvents();
            if(progress->wasCancelled())
            {
                break;
            }
        }

        // Clears the valid array.
        memset(valids, 0, num_tags * sizeof(bool));

        // Gets all the values.
        while(hnd.isValidTagHandler())
        {
            // Caches the information.
            hnd.cacheTag();

            // Updates the state of the values.
            tag_id = hnd.getTagCachedId();
            valids[tag_id] = hnd.getTagCachedDefined();
            values[tag_id] = hnd.getTagCachedValue();
            hnd.nextTag();
        }

        // Loop over the tabs.
        for(INT32 tab = 0; tab < tabcount; tab++)
        {
            selected = true;

            TabSpec * tspec = tabs.at(tab)->getTabSpec();
            QString selectTag = tspec->getSelectTag();
            QString withTag = tspec->getWithTag();

            // Check withtag.
            if(withTag != QString::null)
            {
                tag_id = draldb->getTagId(withTag);
                selected = selected && valids[tag_id];
            }

            // Check select/split cases.
            if(selectTag != QString::null)
            {
                UINT64 matchingValue = 0;

                if(tspec->isSplit())
                {
                    matchingValue = tabs.at(tab)->getSplitByValue();
                }
                else
                {
                    matchingValue = tspec->getMatchingValue();
                }
                tag_id = draldb->getTagId(selectTag);

                if(valids[tag_id])
                {
                    selected = selected && (values[tag_id] == matchingValue);
                }
                else
                {
                    selected = false;
                }
            }

            if(selected)
            {
                // compute colums width & sorting criterion
                QString sortField = tabs.at(tab)->getSortFieldName();
                UINT64 sortValue = 0;

                QValueList<INT32>::Iterator lenit = (tabs.at(tab)->getColLen())->begin();
                QStringList * strlist = tabs.at(tab)->getStrList();

                for(QStringList::Iterator it = strlist->begin(); it != strlist->end(); ++it)
                {
                    QString fieldName = * it;
                    tag_id = draldb->getTagId(fieldName);
                    if(valids[tag_id])
                    {
                        // compute column width
                        QRect tbr = fm.boundingRect(draldb->getFormatedTagValue(tag_id, values[tag_id]));
                        * lenit = QMAX(* lenit, tbr.width() + DISCOLMARGIN);
                    }
                    ++lenit;
                }

                tag_id = draldb->getTagId(sortField);
                if(valids[tag_id])
                {
                    sortValue = values[tag_id];
                }
                tabs.at(tab)->getItemList()->append(DisEntry(&hnd, sortValue));
            }
        }

        // step until the next "Item Id"
        auxcnt++;
        hnd.nextItem();
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

    delete [] valids;
    delete [] values;

    //printf("disdock() sorting() completed... \n");fflush(stdout);

    QApplication::restoreOverrideCursor();
    return;
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
{
    currentTab = (DisTab *) widget;
}

/**
 * Align the first found tab with the handler given as parameter
 *
 * @param hnd Handler to synchronize to
 */
void
DisDockWin::alignWith(ItemHandler & hnd)
{
    bool somefnd = false;
    bool keepCurrentTab = false;
    bool fnd;
    int frow;
    TAG_ID tagId;
    DisTab * firstMatchTab = NULL;
    Dreams * dreams = dynamic_cast<Dreams *>(myMW);

    Q_ASSERT(dreams != NULL);

    // The alignament will be done with the highlighting tag.
    tagId = draldb->getTagId(dreams->getHighLightTagSelector()->currentText());
    if(tagId == TAG_ID_INVALID)
    {
        return;
    }

    // If the item haven't the tag defined, then no alignment is done
    if(!hnd.hasTag(tagId))
    {
        return;
    }

    UINT64 value = hnd.getTagValue();

    for(UINT32 i = 0; i < (UINT32) tabs.size(); i++)
    {
        DisTab * ctab = tabs[i];

        frow = 0;
        fnd = false;
        QValueList<DisEntry> * itemList = ctab->getItemList();
        QValueList<DisEntry>::iterator it = itemList->begin();

        while(!fnd && (it != itemList->end()))
        {
            hnd = (*it).handler;

            fnd = hnd.hasTag(tagId);
            fnd = fnd && (hnd.getTagValue() == value);
            ++frow;
            ++it;
        }

        somefnd = somefnd || fnd;

        if(fnd)
        {
            if(firstMatchTab == NULL)
            {
                firstMatchTab = ctab;
            }
            keepCurrentTab = keepCurrentTab || (ctab == currentTab);
            ctab->setSelection(frow - 1);
        }
    }

    if(!keepCurrentTab)
    {
        tabWidget->showPage(firstMatchTab);
    }
}
