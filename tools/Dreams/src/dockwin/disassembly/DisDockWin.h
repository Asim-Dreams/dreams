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

/**
  * @file DisDockWin.h
  */

#ifndef _DISDOCKWIN_H
#define _DISDOCKWIN_H

#include "DreamsDefs.h"

// Qt library
#include <qdockwindow.h>
#include <qscrollview.h>
#include <qvaluelist.h>
#include <qvbox.h>
#include <qtabwidget.h>
#include <qvaluevector.h>
#include <qlabel.h>

// Dreams
#include "PreferenceMgr.h"
#include "dDB/itemheap/ItemHeap.h"

#define DISCOLMARGIN   6
#define DISVMARGIN     6
#define VBTNMARGIN     6
#define PATTERNSTR     "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+=-{}[]:;',./<>?"
#define MINDISCOLWIDTH 10
#define MAXDISCOLS     32

// Class forwarding.
class DAvtView;
class DisDockWin;
class DisView;
class DisTab;
class HeadScrollView;
class DreamsDB;
class DralDB;
class TabSpec;
class QProgressDialog;
class QStringList;
class QMainWindow;

extern bool dockInit;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class DisEntry
{
    public:
        DisEntry()
        {
        }

        DisEntry(ItemHandler * hnd, UINT64 v)
        {
            handler = * hnd;
            sortCriteria = v;
        }

    public:
        ItemHandler handler;
        UINT64 sortCriteria;
} ;

inline bool operator<(const DisEntry & s1,const DisEntry & s2)
{
    return (s1.sortCriteria < s2.sortCriteria);
}

inline bool operator<=(const DisEntry & s1, const DisEntry & s2)
{
    return (s1.sortCriteria <= s2.sortCriteria);
}

inline bool operator>(const DisEntry & s1, const DisEntry & s2)
{
    return (s1.sortCriteria > s2.sortCriteria);
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class DTabWidget : public QTabWidget
{
    Q_OBJECT
    public:
        DTabWidget(QWidget * parent = 0)
            : QTabWidget(parent)
        {
        }

        QSize
        sizeHint() const
        {
            PreferenceMgr * prefMgr = PreferenceMgr::getInstance();
            if(dockInit)
            {
                QSize result;
                result.setWidth(prefMgr->getDisassemblyWindowWidth());
                result.setHeight(prefMgr->getDisassemblyWindowHeight());
                return result;
            }
            else
            {
                return QTabWidget::sizeHint();
            }
        }
} ;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
class DisTab : public QVBox
{
    Q_OBJECT
    public:
        DisTab(DreamsDB * dreamsdb, TabSpec * tspec, DisDockWin * dockRef, QWidget * parent = 0, const char * name = 0, WFlags f = 0);
        ~DisTab();

        void setFieldList(QStringList *);
        void setFieldLens(QValueList<INT32> *);
        void setSortFieldName(QString);
        void setTextHeight(int);

        inline void setSplitByValue(UINT64 v) { splitByValue = v; }

        inline DisView* getDisView () { return myDis; }
        inline QValueList<INT32>* getColLen() { return colLen; }
        inline QValueList<DisEntry>* getItemList() { return itemList; }
        inline QString getSortFieldName () { return sortFieldName; }
        inline QStringList* getStrList() { return strList; }
        inline INT32 getSelected() { return selected; }
        inline TabSpec* getTabSpec() { return tabSpec; }
        inline UINT64 getSplitByValue() { return splitByValue; }
        inline DisDockWin* getDisDockWin () { return dock; }

        void reset();

        void updateFieldLens();
        void doSync(bool adding, bool moving);
        void setSelection(int row);

        friend class DisView;

    public slots:
        void disMoved (int x, int y);

    private:
        int                   text_height;
        INT32                 selected;

        DisView*              myDis;
        HeadScrollView*       headers;
        DisDockWin*           dock;

        QString               sortFieldName;
        QStringList*          strList;
        QValueList<INT32>*    colLen;
        QValueList<DisEntry>* itemList;
        QPtrList<QLabel>*     btnList;
        TabSpec*              tabSpec;

        // for splitby cases
        UINT64 splitByValue;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class HeadScrollView : public QScrollView
{
    Q_OBJECT
    public:
        HeadScrollView(DisTab* tab, QWidget*parent=0) : QScrollView (parent)
        {
            myTab=tab;
            setVScrollBarMode(QScrollView::AlwaysOff);
            setHScrollBarMode(QScrollView::AlwaysOff);
            QFontMetrics fm (font());
            QRect tbref = fm.boundingRect(PATTERNSTR);
            textHeight = tbref.height();
            setMaximumHeight(textHeight+VBTNMARGIN+4);
            //enableClipper(true);
        }

        QSize sizeHint() const
        {
            QSize ref = QScrollView::sizeHint();
            return (QSize(ref.width(),textHeight+VBTNMARGIN+4));
        }

        inline void setTextHeight(int v)
        {
            textHeight = v-DISVMARGIN;
            setMaximumHeight(textHeight+VBTNMARGIN+4);
        }

    private:
        int textHeight;
        DisTab* myTab;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class DisView : public QScrollView
{
    Q_OBJECT
    public:
        DisView(DreamsDB * _dreamsdb, QWidget * parent, DisTab * disTab);
        ~DisView();

        void setSelection(int x, int y);

    protected:
        void doSync(bool adding, bool moving);
        /**
          * This method is invoken (by QT event handler) when a given drawing area must be repainted
          */
        void
        drawContents (
                     QPainter * p,      ///< Painter where to draw on
                     int clipx,         ///< left X coordinate (QScrollView coordinates system)
                     int clipy,         ///< upper Y coordinate (QScrollView coordinates system)
                     int clipw,         ///< redrawing area width (QScrollView coordinates system)
                     int cliph          ///< redrawing area height (QScrollView coordinates system)
                     );

        void styleChange(QStyle& oldStyle);
        void contentsMouseDoubleClickEvent(QMouseEvent* event);
        void contentsMousePressEvent(QMouseEvent* e);
        void contentsMouseReleaseEvent(QMouseEvent* e);

        void keyReleaseEvent(QKeyEvent* e);

    private:
        DisTab* myTab;
        int pressed_x;
        int pressed_y;
        int released_x;
        int released_y;
        DreamsDB * dreamsdb;
        DralDB * draldb;
};

/**
  * Docked window to display disassembly tags of visible events (Item id range).
  * Put long explanation here
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class DisDockWin : public QDockWindow
{
    Q_OBJECT
    public:
        DisDockWin(DreamsDB * _dreamsdb, QMainWindow * mdi, QMainWindow * mw, DAvtView * asv);
        ~DisDockWin();

        void fontChange ( const QFont & oldFont );
        void reset();
        virtual void alignWith(INT32 col, INT32 row, CYCLE cycle) = 0;
        void setInitializing(bool v);
        void createTabs();

        virtual void doSync(bool adding, bool moving) = 0;

        friend class DisView;
        inline DAvtView* getDAvtView() { return myAvt; }

    public slots:
        void tabChanged(QWidget*);

    protected:
        void scanSplitBy(TabSpec* tab);
        void computeItemLists();
        void setTagList(QStringList* strlist, DisTab* tab,QString sorting);
        void updateNonFiltered();
        void resizeEvent ( QResizeEvent * e );
        void applyFont();
        void alignWith(ItemHandler & hnd);

        QTabWidget*      tabWidget;
        QValueVector<DisTab*> tabs;
        DisTab*          currentTab;
        INT32            max_width;

        DreamsDB * dreamsdb;
        DralDB * draldb;

        DAvtView*        myAvt;
        QMainWindow*     myMdi;
        QMainWindow*     myMW;
        QProgressDialog* progress;

        PreferenceMgr*   prefMgr;
};

#endif
