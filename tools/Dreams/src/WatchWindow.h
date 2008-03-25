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
 * @file  WatchWindow.h
 */

#ifndef _WATCHWINDOW_H_
#define _WATCHWINDOW_H_

// Qt includes.
#include <qnamespace.h>
#include <qptrlist.h>
#include <qprogressdialog.h>
#include <qtable.h>

// Dreams includes.
#include "dDB/DRALTag.h"
#include "dDB/ClockDomainMgr.h"
#include "dDB/DralDB.h"
#include "expressions/Expression.h"
#include "CSLCache.h"
#include "expressions/RuleEngine.h"

/**
 * Class that represents a WatchColumn state element in the 
 * WatchWindow view. This class holds all the information needed by a 
 * watch window column defined inside a watch window view. 
 * It holds the tagId that must be shown in a column and the type of
 * element to show the information, an item or a slot
 */
class WatchColumn 
{
  private:

    QString sPointerName;
    bool bPointer;

    bool bSlotTag;
    INT32 tagId;
    UINT32 width;
    Qt::AlignmentFlags align;

  public:

    WatchColumn(bool _bSlotTag, INT32 _tagId, bool _bPointer = false) 
      : bPointer(_bPointer), bSlotTag(_bSlotTag), tagId(_tagId), width(0), 
        align(static_cast<Qt::AlignmentFlags>(Qt::AlignVCenter | Qt::AlignLeft)) { }

    void setPointerName(QString &sName) { sPointerName = sName; }
    QString &getPointerName(void)       { return sPointerName; }
    bool getIsPointer(void) const       { return bPointer; }

    bool getIsSlotTag(void) const { return bSlotTag; }
    TAG_ID getTagId(void) const    { return tagId; }

    void setWidth(UINT32 w)      { width = w; }
    UINT32 getWidth(void) const  { return width; }

    bool setAlign(QString &sAlign) 
    { 
        bool bOk = true;
        if      (sAlign == "left")   align = static_cast<Qt::AlignmentFlags>(Qt::AlignVCenter | Qt::AlignLeft);
        else if (sAlign == "right")  align = static_cast<Qt::AlignmentFlags>(Qt::AlignVCenter | Qt::AlignRight);
        else if (sAlign == "center") align = Qt::AlignCenter;
        else                         bOk = false;
        return bOk;
    }
    Qt::AlignmentFlags getAlign(void) { return align; }
};

/**
 * Class that represents a WatchWindow state element in the 
 * view. This class holds all the information needed by a 
 * watch window node view. It holds the collection of attributes
 * that are shown in the view and acts as frontend between the
 * view and the DralDB
 */
class WatchWindow 
{
  public:

    enum Type
    {
        Uninitialized,
        Edge,
        Node
    } ;

  private:

    DralDB *draldb;
    WatchColumn *orderByColunmn;
    QPtrList<WatchColumn> qlColumn; ///< Pointers to the window columns

    UINT32 nTags;                   ///< Number of tags defined in the draldb
    TagValueEntry itemTags;         ///< Tags of the item inside the actual slot.
    UINT16  dralId;
    Type    watchType;

    UINT16 clockId;                 ///< Clock id of the watch window.
    QString sCaption;
    UINT32 nSameCaption;            ///< Counter used to track how many watch windows have been defined on the same 
                                    ///  node. The caption will be used to differ between all the defined windows. 
                                    ///  The first of them will hold the number of windows created. This number will 
                                    ///  be concatenated to the next window with the same name.

    UINT16 dimensions;
    UINT32 *capacities;
    UINT32 *accCapacities;
    UINT32 totalCapacity;

    TRACK_ID nodeTrackId;
    TRACK_ID trackIdBase;

    ExpressionList qlistRules;      ///< Pointers to Rules for this environment
    CSLCache cslCache;              ///< Cache of color/shape/letter combinations for curent node (Row/Group)
    TAG_ID tagIdColor;              ///< Tag Identifyer that represents the tag that holds the color for the current
                                    ///  view element

    UINT32 nPointerColumn;          ///< Number of pointer defined

    set<TAG_ID> item_tags;  ///< List of item tags that are used in the rules of this rectangle.
    set<TAG_ID> slot_tags;  ///< List of slot tags that are used in the rules of this rectangle.
    set<TAG_ID> node_tags;  ///< List of node tags that are used in the rules of this rectangle.
    set<TAG_ID> cycle_tags; ///< List of cycle tags that are used in the rules of this rectangle.

  public:

    /**
     * Item that implements the 
     */
    class WatchWindowTableItem : public QTableItem
    {
      private:
        bool bTransp;
        QColor fillColor;
        QColor letterColor;
        Qt::AlignmentFlags align;

      public:
        // We always create the element as not editable
        WatchWindowTableItem(QTable * table, const QString & text) : QTableItem(table, Never, text), bTransp(true), align(Qt::AlignAuto) { }

        void setAlignment(Qt::AlignmentFlags a) { align = a; }
        int alignment () const
        {
            if(align != Qt::AlignAuto) return align;
            return QTableItem::alignment();
        }

        void setTransparency(bool t) { bTransp = t; }
        void setFillColor(QColor f) { fillColor = f; }
        void setLetterColor(QColor l) { letterColor = l; }

        void paint(QPainter * p, const QColorGroup &cg, const QRect &cr, bool selected)
        {
            QColorGroup _cg(cg);

            if(!bTransp) _cg.setColor(QColorGroup::Base, fillColor);
            _cg.setColor(QColorGroup::Text, letterColor);

            QTableItem::paint(p, _cg, cr, selected);
        }
    };

    WatchWindow(DralDB *db);
    ~WatchWindow(); 

    void setNode(UINT16 dralId);
    void setEdge(UINT16 dralId);
    void setCapacity(UINT16 dimensions, UINT32 *capacities);
    bool setOrderBy(TAG_ID tagId);
    inline void setTagIdColor(INT32 tagId);

    void add(WatchColumn *column) { qlColumn.append(column); }
    void addPointer(WatchColumn *column) { nPointerColumn++; qlColumn.append(column); }

    void addRules(ExpressionList &rules);

    void initRuleEvalStructs(void);
    void freeRuleEvalStructs(void);
    void computeCSL(QProgressDialog * progress, QLabel * label, UINT32 &currentCell);

    void initDraw(QTable * table);
    void draw(QTable * table, CYCLE cycle);

    void drawCell(QTable *table, INT32 row, INT32 col, QPixmap &pixmap, QString &value, Qt::AlignmentFlags align, bool bTransp, QColor fillColor, QColor letterColor);
    void drawCell(QTable *table, INT32 row, INT32 col, QString &value, Qt::AlignmentFlags align, bool bTransp, QColor fillColor, QColor letterColor);

    inline UINT32 getCapacity(void);
    inline UINT32 getNumCols(void);
    inline INT32 getTrackIdForCell(UINT32 cellId);

    inline QString &getCaption(void);
    inline void setCaption(QString caption);
    inline UINT32 incNumSameCaption(void);

    inline UINT16 getDralId() const;
    inline Type getWatchType() const;
    inline CLOCK_ID getClockId() const;

    set<TAG_ID> getSlotTags() const;

    // TODO 
    // Add support for memory occupancy checking
    //INT64  getObjSize() const;
    //QString getUsageDescription() const;
};

void 
WatchWindow::setTagIdColor(INT32 tagId)
{
    tagIdColor = tagId;
}

UINT32 
WatchWindow::getCapacity(void)
{
    return totalCapacity;
}

UINT32 
WatchWindow::getNumCols(void)
{
    return qlColumn.count();
}

INT32 
WatchWindow::getTrackIdForCell(UINT32 cellId)
{
    return(trackIdBase + cellId);
}

QString &
WatchWindow::getCaption(void)
{
    return sCaption;
}

void 
WatchWindow::setCaption(QString caption)
{
    sCaption = caption;
}

/*
 * Updates the counter and returns the new value
 */
UINT32
WatchWindow::incNumSameCaption(void)
{
    nSameCaption++;
    return nSameCaption;
}

/*
 * Returns the dral id of the watched dral element.
 */
UINT16
WatchWindow::getDralId() const
{
    return dralId;
}

/*
 * Returns the type of the watched dral element.
 */
WatchWindow::Type
WatchWindow::getWatchType() const
{
    return watchType;
}

CLOCK_ID
WatchWindow::getClockId() const
{
    return clockId;
}

#endif /* _WATCHWINDOW_H_ */
