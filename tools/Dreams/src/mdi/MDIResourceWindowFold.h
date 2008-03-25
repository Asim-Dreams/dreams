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
 * @file MDIResourceWindowFold.h
 */

#ifndef _MDIRESOURCEWINDOWFOLD_H
#define _MDIRESOURCEWINDOWFOLD_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"

// Qt includes.
#include <qframe.h>
#include <qsize.h>
#include <qcolor.h>

// Class forwarding.
class DreamsDB;
class DralDB;
class ResourceDAvtView;
class LayoutResourceCache;
class AGTTipLabel;
class QPainter;
class QMouseEvent;
class QEvent;
class QPopupMenu;

/*
 * @typedef enum ResourceViewFoldEnum
 * @description
 * Enumeration of the different elements that can be painted
 * in the folding unfolding mechanism.
 */
typedef enum
{
    ResourceViewFoldUnfold,
    ResourceViewFoldFold,
    ResourceViewFoldLine,
    ResourceViewFoldLineAndEnd,
    ResourceViewFoldEnd,
    ResourceViewFoldNothing
} ResourceViewFoldEnum;

/*
 * @brief
 * Widget that contains the two dreams avt view and the folding widget.
 *
 * @description
 *
 *
 * @version 0.1
 * @date started at 2005-03-16
 * @author Guillem Sole
 */
class MDIResourceWindowFold : public QFrame
{
    Q_OBJECT
    public:
        MDIResourceWindowFold(DreamsDB * _dreamsdb, LayoutResourceCache * _cache, QWidget * parent);
        ~MDIResourceWindowFold();

        QSize sizeHint() const;
        void setTwoDAvtView(ResourceDAvtView * v);

    public slots:
        void resourceChanged(double);
        void do_showNames();
        void do_showOccupancy();
        void do_showRowNumber();
        void do_showRowType();
        void do_showRowName();
        void do_showRowFrom();
        void do_showRowTo();
        void do_showRowLane();
        void do_showRowLevel();

    protected:
        void drawContents(QPainter * p);
        void drawUnfoldItem(QPainter * p, INT32 pos, float scfy);
        void drawFoldItem(QPainter * p, INT32 pos, float scfy);
        void drawLineItem(QPainter * p, INT32 pos, float scfy);
        void drawLineAndEndItem(QPainter * p, INT32 pos, float scfy);
        void drawEndItem(QPainter * p, INT32 pos, float scfy);

        void mouseReleaseEvent(QMouseEvent * e);
        void mouseMoveEvent(QMouseEvent * e);
        void leaveEvent(QEvent * e);

        UINT32 posToRow(UINT32 pos) const;

        void cleanTip();
        void addContextMenu();

    protected:
        DreamsDB * dreamsdb;         ///< Pointer to the trace.
        DralDB * draldb;             ///< Pointer to the database.
        ResourceDAvtView * davt;     ///< Pointer to the avt.
        LayoutResourceCache * cache; ///< Cache of the mapping to LayoutResource.

        QColor bgcolor;    ///< Background color.
        QColor linecolor;  ///< Line color.
        float lastScfy;    ///< Last known scale factor.
        UINT32 lastOffset; ///< Last known offset.
        AGTTipLabel * tip; ///< Tip used to show the group.
        UINT32 lastTipRow; ///< Last row that the tip showed.
        UINT32 row_mask;   ///< What elements should be view in the row description.

        QPopupMenu * contextMenu;               ///< Right-click menu.
        QPopupMenu * contextMenuShow;           ///< Right-click sub-menu.
        QPopupMenu * contextMenuSetup;          ///< Right-click sub-menu.
        QPopupMenu * contextMenuSetupNames;     ///< Right-click sub-menu.
        QPopupMenu * contextMenuSetupOccupancy; ///< Right-click sub-menu.

        INT32 show_name_id;         ///< Menu ids.
        INT32 show_occupancy_id;    ///< Menu ids.
        INT32 setup_name_number_id; ///< Menu ids.
        INT32 setup_name_type_id;   ///< Menu ids.
        INT32 setup_name_name_id;   ///< Menu ids.
        INT32 setup_name_from_id;   ///< Menu ids.
        INT32 setup_name_to_id;     ///< Menu ids.
        INT32 setup_name_lane_id;   ///< Menu ids.
        INT32 setup_name_level_id;  ///< Menu ids.
} ;

#endif // _MDIRESOURCEWINDOWFOLD_H.
