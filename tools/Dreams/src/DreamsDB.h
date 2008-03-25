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
  * @file  DreamsDB.h
  */

#ifndef _DREAMSDB_H
#define _DREAMSDB_H

// Qt includes.
#include <qstrlist.h>
#include <qdom.h>

// Dreams includes.
#include "dDB/DralDB.h"
#include "DreamsDefs.h"
#include "adf/ADFDefinitions.h"
#include "TabSpec.h"
#include "ItemTab.h"
#include "ColorShapeItem.h"
#include "adf/ADFMap.h"
#include "layout/resource/LayoutResource.h"
#include "layout/floorplan/LayoutFloorplan.h"
#include "layout/waterfall/LayoutWaterfall.h"
#include "layout/watch/LayoutWatch.h"

// Asim includes.
#include "asim/AScrollView.h"

// Class forwarding.
class ColumnShadows;
class BookmarkMgr;
class ADFParserInterface;
class IOMgr;
class ADFMgr;
class QDomDocument;
class WatchWindow;
class Expression;
class ExpressionList;
class PreferenceMgr;

/**
  * @brief
  * This class holds all the information of a trace loaded in dreams.
  *
  * @descrition
  * DreamsDB is a class that represents a trace. Each trace has a set
  * of elements needed to load, compute and show all its events. The
  * dreams database is a facade to all of its elements. The elements
  * are listed below:
  * - IOMgr: needed to load the trace from the disk.
  * - ADFParser: parses the ADF of the trace.
  * - DralDB: stores all the events of the trace.
  * - Layouts: holds the information of the different layouts that are
  * active.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class DreamsDB : public QObject, public AMemObj, public StatObj
{
    Q_OBJECT

    public:
        DreamsDB(QString trace, BookmarkMgr * bookMarkMgr, LogMgr * logmgr);
        virtual ~DreamsDB();

        /***************************************************
        *********** AMemObj & StatObj Interface ************
        ***************************************************/
        INT64 getObjSize() const;
        QString getUsageDescription() const;
        QString getStats() const;

        /***************************************************
        ************ Dreams DataBase Interface *************
        ***************************************************/
        void setDralDBPreferences(PreferenceMgr * prefMgr);

        bool loadDRLHeader();
        bool parseADF(QString defaultADF, QString paramADF);
        void startLoading();
        void clearDict();

        inline DralDB * getDralDB();
        QDomDocument * getDomDocument() const;
        inline QString getTraceName() const;

        inline void addMapTable(QString name, ADFMap * map);
        inline ADFMap * getMapTable(QString name) const;

        inline bool hasLoadedData() const;

        bool saveSession(QFile* file);
        bool loadSession(QFile* file);

        inline bool getHasFatalError(bool end_sim);
        inline bool reachedEOS() const;
        inline bool nonCriticalErrors() const;
        inline CYCLE getNumCycles(UINT16 clockId) const;
        inline void  setBookMarkMgr(BookmarkMgr* mgr);
        inline void  setColumnShadows(ColumnShadows* sh);
        inline QStrList getKnownTags() const;

        void dumpRegression(bool gui_enabled);

        /***************************************************
        ************** LayoutResource Interface ************
        ***************************************************/
        inline void layoutResourceCreateLayout();
        void layoutResourceComputeData();
        inline bool layoutResourceHasLayout() const;
        inline bool layoutResourceGetOpened() const;

        inline bool layoutResourceAddRow(RowType type, TRACK_ID track_id, TAG_ID tagIdColor, DRAL_ID dralId, UINT32 rows, bool isDrawOnMove = true);
        inline bool layoutResourceNewGroup(QString name);
        inline bool layoutResourceEndGroup();

        inline void layoutResourceAddRules(ExpressionList * rules);
        inline void layoutResourceAddRules(Expression * rule);
        inline void layoutResourceAddRowRules(ExpressionList * rules, layoutResourcePhase phase);
        inline void layoutResourceAddRowRules(Expression * rule, layoutResourcePhase phase);
        inline void layoutResourceSetBlankRowColor(QColor color);
        inline bool layoutResourceSetShowPhaseHigh(bool expanded);
        inline bool layoutResourceSetShowPhaseLow(bool expanded);

        inline bool layoutResourceGetUsesDivisions() const;
        inline UINT32 layoutResourceGetNumPhyisicalRows() const;
        inline UINT32 layoutResourceGetNumGroups() const;

        inline bool layoutResourceGetValidData(UINT32 row, CYCLE cycle, LayoutResourceCache * cache) const;
        inline ColorShapeItem layoutResourceGetColorShapeLetter(UINT32 row, CYCLE col, LayoutResourceCache * cache) const;
        inline QString layoutResourceGetRowInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache) const;
        inline QString layoutResourceGetRowCSLInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache) const;
        inline bool layoutResourceGetRowItemInside(UINT32 row, CYCLE cycle, ItemHandler * hnd, LayoutResourceCache * cache) const;
        inline UINT32 layoutResourceGetRowOccupancy(UINT32 row, LayoutResourceCache * cache) const;

        inline QString layoutResourceGetRowDescription(UINT32 row, LayoutResourceCache * cache, UINT32 mask) const;
        inline TRACK_ID layoutResourceGetRowTrackId(UINT32 row, LayoutResourceCache * cache) const;
        inline RowType layoutResourceGetRowType(UINT32 row, LayoutResourceCache * cache) const;
        inline DRAL_ID layoutResourceGetRowDralId(UINT32 row, LayoutResourceCache * cache) const;
        inline CLOCK_ID layoutResourceGetRowClockId(UINT32 row, LayoutResourceCache * cache) const;
        inline UINT32 layoutResourceGetRowBaseRow(UINT32 row, LayoutResourceCache * cache) const;
        inline bool layoutResourceGetRowShowHigh(UINT32 row, LayoutResourceCache * cache) const;
        inline bool layoutResourceGetRowShowLow(UINT32 row, LayoutResourceCache * cache) const;
        inline bool layoutResourceGetRowShowExpanded(UINT32 row, LayoutResourceCache * cache) const;
        inline bool layoutResourceGetRowIsDrawOnReceive(UINT32 row, CYCLE * cycle, LayoutResourceCache * cache) const;

        inline void layoutResourceGetRowsWithDralId(DRAL_ID dralId, RowType type, const rowList ** rows) const;

        inline bool layoutResourceFindItemWindow(ITEM_ID itemId, CYCLE & firstCycle, CYCLE & lastCycle);
        inline bool layoutResourceFindItemFirstAppearence(ITEM_ID itemId, CYCLE startcycle, UINT32 & resrow, CYCLE & rescycle);
        inline bool layoutResourceFindItemLastAppearence(ITEM_ID itemId, CYCLE endcycle, UINT32 & resrow, CYCLE & rescycle);

        inline void layoutResourceGetFindData(LayoutResourceFindDataSet * entries, CYCLE startCycle, const LayoutResourceFindOptions * env) const;

        inline void layoutResourceComputeLayoutCache(LayoutResourceCache * cache);
        inline void layoutResourceComputeLayoutPlainCache();

        inline bool layoutResourceGetRowIsGroup(UINT32 row, LayoutResourceCache * cache) const;
        inline void layoutResourceSwitchRowGroup(UINT32 row, LayoutResourceCache * cache);
        inline QString layoutResourceGetRowComposedName(UINT32 row, LayoutResourceCache * cache) const;
        inline bool layoutResourceGetRowIsShown(UINT32 row, LayoutResourceCache * cache) const;
        inline UINT32 layoutResourceGetRowVirtMap(UINT32 row, LayoutResourceCache * cache) const;
        inline void layoutResourceShowRow(UINT32 row, LayoutResourceCache * cache);

        inline LayoutResourceCache * layoutResourceGetFlatCache() const;

        inline void layoutResourceDrawSubSamplingScale(INT32 x1, INT32 x2, INT32 y1, INT32 y2, double scf_x, double scf_y, QPainter * painter, LayoutResourceCache * cache, bool paint_events);
        inline void layoutResourceDrawNoFullScale(INT32 x1, INT32 x2, INT32 y1, INT32 y2, QPainter * painter, LayoutResourceCache * cache, bool paint_events);
        inline void layoutResourceDrawFullScale(INT32 x1, INT32 x2, INT32 y1, INT32 y2, QPainter * painter, AScrollView * asv, LayoutResourceCache * cache, bool paint_events);

        /***************************************************
        ************* LayoutFloorplan Interface ************
        ***************************************************/
        inline void layoutFloorplanCreateLayout();
        void layoutFloorplanComputeData();
        inline bool layoutFloorplanHasLayout() const;
        inline bool layoutFloorplanGetOpened() const;

        inline void layoutFloorplanSetRoot(LayoutFloorplanTable * root);

        inline UINT32 layoutFloorplanGetWidth() const;
        inline UINT32 layoutFloorplanGetHeight() const;
        inline UINT32 layoutFloorplanGetNumCells() const;

        inline void layoutFloorplanGetRectanglesAt(UINT32 x1, UINT32 y1, UINT32 x2, UINT32 y2, QPtrList<LayoutFloorplanNode> & listResult);
        inline QString layoutFloorplanGetCellAt(UINT32 x, UINT32 y);
        inline QString layoutFloorplanGetNodeNameAt(UINT32 x, UINT32 y);
        inline INT32 layoutFloorplanGetTrackIdForCell(UINT32 x, UINT32 y);
        inline bool layoutFloorplanHasValidData(INT32 x, INT32 y, CYCLE cycle);

        inline ItemHandler layoutFloorplanGetCellWithTagValue(TAG_ID tagId, UINT64 tagValue, CYCLE cycle, INT32 & startX, INT32 & startY);
        inline ItemHandler layoutFloorplanGetCellWithTagValue(TAG_ID tagId, QString & tagValue, bool cs, bool em, CYCLE cycle, INT32 & startX, INT32 & startY);
        inline ItemHandler layoutFloorplanGetCellWithTagValue(TAG_ID tagId, QRegExp & tagValue, CYCLE cycle, INT32 & startX, INT32 & startY);

        inline bool layoutFloorplanGetFirstCellAppearenceWithTagValue(TAG_ID tagId, UINT64 tagValue, CYCLE & cycle, INT32 & startX, INT32 & startY);

        inline void layoutFloorplanGetFindData(LayoutFloorplanFindDataSet * entries, CYCLE startCycle, const LayoutFloorplanFindOptions * env) const;

        /***************************************************
        ************* Watch Window Interface ***************
        ***************************************************/
        inline void layoutWatchCreateLayout();
        void layoutWatchComputeData();
        inline bool layoutWatchHasLayout() const;
        inline bool layoutWatchGetOpened();

        inline void layoutWatchAddWindow(WatchWindow * newWin);
        inline WatchWindow * layoutWatchGetWindow(QString caption);
        inline QDictIterator<WatchWindow> layoutWatchGetWindowIterator();

        /***************************************************
        *************** WaterFall Interface ****************
        ***************************************************/
        inline void layoutWaterfallCreateLayout();
        void layoutWaterfallComputeData();
        inline bool layoutWaterfallHasLayout() const;
        inline bool layoutWaterfallGetOpened() const;

        inline bool layoutWaterfallAddMoveItem(DralGraphEdge * edge);
        inline bool layoutWaterfallAddEnterNode(DralGraphNode * node);
        inline bool layoutWaterfallAddExitNode(DralGraphNode * node);

        inline bool layoutWaterfallAddMoveItemRules(DralGraphEdge * edge, ExpressionList & rules);
        inline bool layoutWaterfallAddEnterNodeRules(DralGraphNode * node, ExpressionList & rules);
        inline bool layoutWaterfallAddExitNodeRules(DralGraphNode * node, ExpressionList & rules);

        inline void layoutWaterfallAddDefaultRules(ExpressionList & rules, INT32 level);
        inline void layoutWaterfallAddOverrideRules(ExpressionList & rules, INT32 level);
        inline void layoutWaterfallAddRules(ExpressionList & rules, INT32 level);

        inline void layoutWaterfallComputeRangeForRow(INT32 row);
        inline CYCLE layoutWaterfallGetCurrentRowMinCycle() const;
        inline CYCLE layoutWaterfallGetCurrentRowMaxCycle() const;

        inline bool layoutWaterfallGetHasTags(UINT32 row, ItemHandler * hnd);
        inline void layoutWaterfallUpdateDBInfo();
        inline bool layoutWaterfallHasValidData(UINT32 row, INT64 ps, ItemHandler * hnd, ExpressionList ** rlist);
        inline bool layoutWaterfallGetRules(UINT32 row, ItemHandler * hnd, ExpressionList ** rlist);
        inline void layoutWaterfallResetColorShapeLetter();
        inline ColorShapeItem layoutWaterfallGetColorShapeLetter(ItemHandler * hnd, ExpressionList * list);
        inline bool layoutWaterfallGetCurrentITagValue(TAG_ID tagId, INT64 * value) const;
        inline ITEM_ID layoutWaterfallGetItemIdForRow(UINT32 row);

        inline bool layoutWaterfallGetUsesDivisions() const;
        inline UINT32 layoutWaterfallGetWidth() const;
        inline UINT32 layoutWaterfallGetHeight() const;
        inline UINT32 layoutWaterfallGetNumRows() const;
        inline void layoutWaterfallSetCurrentItemTab(ItemTab * newtab);

        /***************************************************
        *********** Item Window Data Interface *************
        ***************************************************/
        void addItemWinGlobalRule(ExpressionList & lrules);
        void addNewTab(TabSpec * tab);

        inline TabSpecList * getTabSpecList() const;
        inline ItemTabList * getItemTabList() const;

        /***************************************************
        *********** Item Window Data Interface *************
        ***************************************************/
        inline INT32 getNumMDI() const;
        inline void incNumMDI();
        inline void decNumMDI();

    private:
        bool getADF();

    public slots:
        void loadFinishSlot();
        void loadCancelSlot();

    signals:
        void loadFinishSignal();
        void loadCancelSignal();

    private:
        IOMgr * iomgr;                  ///< IOMgr.
        ADFParserInterface * adfparser; ///< ADF Parser.
        ADFMgr * adfmgr;                ///< ADF Manager.
        DralDB * draldb;                ///< The dral database.
        LayoutResource * resource;      ///< Resource layout view.
        LayoutFloorplan * floorplan;    ///< Floorplan layout view.
        LayoutWatch * watch;            ///< WatchWindow layout view.
        LayoutWaterfall * waterfall;    ///< WaterFall layout view.
        bool dataresource;              ///< True if the Resource colors are computed.
        bool datafloorplan;             ///< True if the Floorplan colors are computed.
        bool datawatch;                 ///< True if the Watch window colors are computed.
        bool datawaterfall;             ///< True if the Water Fall colors are computed.

        BookmarkMgr * bkMgr; ///< Bookmark manager.
        LogMgr * myLogMgr;   ///< Log manager.
        QString trace_file;  ///< Trace of this dreams database.
        ADFMapSet mapset;    ///< Set of maps defined in the ADF of the actual trace.

        INT32 numMDI; ///< Number of MDI assigned to this trace.

        ExpressionList tabrules;   ///< Rules of tabs.
        TabSpecList * tabspeclist;
        ItemTabList * itablist;

        ColumnShadows* cshadows;
} ;

/*
 * Returns the draldb of this trace.
 *
 * @return the draldb.
 */
DralDB *
DreamsDB::getDralDB()
{
    return draldb;
}

/*
 * Returns the name of the trace.
 *
 * @return the trace name.
 */
QString
DreamsDB::getTraceName() const
{
    return trace_file;
}

/*
 * Function used to start the layout of Resource.
 *
 * @return void.
 */
void
DreamsDB::layoutResourceCreateLayout()
{
    Q_ASSERT(resource == NULL);
    resource = new LayoutResource(draldb, myLogMgr);
}

/*
 * Returns if the resource view is defined.
 *
 * @return if the resource view is defined.
 */
bool
DreamsDB::layoutResourceHasLayout() const
{
    return (resource != NULL);
}

/*
 * Returns if the resource view has something.
 *
 * @return if the resource view has something.
 */
bool
DreamsDB::layoutResourceGetOpened() const
{
    return (resource->getNumPhysicalRows() != 0);
}

/*
 * Adds a new row in the view of the Resource.
 *
 * @return if everything has gone ok.
 */
bool
DreamsDB::layoutResourceAddRow(RowType type, TRACK_ID track_id, TAG_ID tagIdColor, DRAL_ID dralId, UINT32 rows, bool isDrawOnMove)
{
    return resource->addRow(type, track_id, tagIdColor, dralId, rows, isDrawOnMove);
}

/*
 * Adds a new group in the view of the Resource.
 *
 * @return if everything has gone ok.
 */
bool
DreamsDB::layoutResourceNewGroup(QString name)
{
    return resource->newGroup(name);
}

/*
 * Ends a group of the view of the Resource.
 *
 * @return if everything has gone ok.
 */
bool
DreamsDB::layoutResourceEndGroup()
{
    return resource->endGroup();
}

/*
 * Adds global rules to Resource.
 *
 * @return void.
 */
void
DreamsDB::layoutResourceAddRules(ExpressionList * rules)
{
    Q_ASSERT(resource != NULL);
    resource->addGlobalRules(rules);
}

/*
 * Adds global rules to Resource.
 *
 * @return void.
 */
void
DreamsDB::layoutResourceAddRules(Expression * rule)
{
    Q_ASSERT(resource != NULL);
    resource->addGlobalRules(rule);
}

/*
 * Adds local rules to Resource.
 *
 * @return if everything is ok.
 */
void
DreamsDB::layoutResourceAddRowRules(ExpressionList * rules, layoutResourcePhase phase)
{
    resource->addRowRules(rules, phase);
}

/*
 * Adds local rules to Resource.
 *
 * @return if everything is ok.
 */
void
DreamsDB::layoutResourceAddRowRules(Expression * rule, layoutResourcePhase phase)
{
    return resource->addRowRules(rule, phase);
}

/*
 * Sets the color of a blank row in the Resource view.
 *
 * @return void.
 */
void
DreamsDB::layoutResourceSetBlankRowColor(QColor color)
{
    resource->setBlankRowColor(color);
}

/*
 * The row must show the high phase.
 *
 * @return bool.
 */
bool
DreamsDB::layoutResourceSetShowPhaseHigh(bool expanded)
{
    return resource->showPhaseHigh(expanded);
}

/*
 * The row must show the low phase.
 *
 * @return bool.
 */
bool
DreamsDB::layoutResourceSetShowPhaseLow(bool expanded)
{
    return resource->showPhaseLow(expanded);
}

/*
 * Gets if the Resource view uses divisions (phase support).
 *
 * @return if divisions are used.
 */
bool
DreamsDB::layoutResourceGetUsesDivisions() const
{
    return resource->getUsesDivisions();
}

/*
 * Returns the number of rows defined in the resource.
 *
 * @return the resource rows.
 */
UINT32
DreamsDB::layoutResourceGetNumPhyisicalRows() const
{
    return resource->getNumPhysicalRows();
}

/*
 * Returns the num of groups of the resource.
 *
 * @return the num of groups.
 */
UINT32
DreamsDB::layoutResourceGetNumGroups() const
{
    return resource->getNumGroups();
}

bool 
DreamsDB::layoutResourceGetValidData(UINT32 row, CYCLE cycle, LayoutResourceCache * cache) const
{
    return resource->getValidData(row, cycle, cache);
}

ColorShapeItem 
DreamsDB::layoutResourceGetColorShapeLetter(UINT32 row, CYCLE cycle, LayoutResourceCache * cache) const
{
    return resource->getColorShapeLetter(row, cycle, cache);
}

QString
DreamsDB::layoutResourceGetRowInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache) const
{
    return resource->getRowInfo(row, cycle, cache);
}

QString
DreamsDB::layoutResourceGetRowCSLInfo(UINT32 row, CYCLE cycle, LayoutResourceCache * cache) const
{
    return resource->getRowCSLInfo(row, cycle, cache);
}

/*
 * Gets the item inside the edge or node in the given cycle.
 *
 * @return if an item is inside.
 */
bool
DreamsDB::layoutResourceGetRowItemInside(UINT32 row, CYCLE cycle, ItemHandler * hnd, LayoutResourceCache * cache) const
{
    return resource->getRowItemInside(row, cycle, hnd, cache);
}

/*
 * Gets the row occupancy.
 *
 * @return the occupancy.
 */
UINT32
DreamsDB::layoutResourceGetRowOccupancy(UINT32 row, LayoutResourceCache * cache) const
{
    return resource->getRowOccupancy(row, cache);
}

/*
 * Gets the row description.
 *
 * @return the description.
 */
QString
DreamsDB::layoutResourceGetRowDescription(UINT32 row, LayoutResourceCache * cache, UINT32 mask) const
{
    return resource->getRowDescription(row, cache, mask);
}

/*
 * Gets the row track id.
 *
 * @return the track id.
 */
TRACK_ID
DreamsDB::layoutResourceGetRowTrackId(UINT32 row, LayoutResourceCache * cache) const
{
    return resource->getRowTrackId(row, cache);
}

/*
 * Gets the row type.
 *
 * @return the type.
 */
RowType 
DreamsDB::layoutResourceGetRowType(UINT32 row, LayoutResourceCache * cache) const
{
    return resource->getRowType(row, cache);
}

/*
 * Gets the row dral id.
 *
 * @return the dral id.
 */
DRAL_ID
DreamsDB::layoutResourceGetRowDralId(UINT32 row, LayoutResourceCache * cache) const
{
    return resource->getRowDralId(row, cache);
}

/*
 * Gets the row clock id.
 *
 * @return the clock id.
 */
CLOCK_ID
DreamsDB::layoutResourceGetRowClockId(UINT32 row, LayoutResourceCache * cache) const
{
    return resource->getRowClockId(row, cache);
}

/*
 * Gets the first row of the node where the row belongs.
 *
 * @return the base row.
 */
UINT32
DreamsDB::layoutResourceGetRowBaseRow(UINT32 row, LayoutResourceCache * cache) const
{
    return resource->getRowBaseRow(row, cache);
}

/*
 * Gets if the row shows the high phase.
 *
 * @return if the phase is shown.
 */
bool
DreamsDB::layoutResourceGetRowShowHigh(UINT32 row, LayoutResourceCache * cache) const
{
    return resource->getRowShowHigh(row, cache);
}

/*
 * Gets if the row shows the low phase.
 *
 * @return if the phase is shown.
 */
bool
DreamsDB::layoutResourceGetRowShowLow(UINT32 row, LayoutResourceCache * cache) const
{
    return resource->getRowShowLow(row, cache);
}

/*
 * Gets if the row shows the phase expaned.
 *
 * @return if the phase is shown expanded.
 */
bool
DreamsDB::layoutResourceGetRowShowExpanded(UINT32 row, LayoutResourceCache * cache) const
{
    return resource->getRowShowExpanded(row, cache);
}

/*
 * Gets if the row shows the phase expaned.
 *
 * @return if the phase is shown expanded.
 */
bool
DreamsDB::layoutResourceGetRowIsDrawOnReceive(UINT32 row, CYCLE * cycle, LayoutResourceCache * cache) const
{
    return resource->getRowIsDrawOnReceive(row, cycle, cache);
}

/*
 * Gets all the rows with the row type type and that represents the
 * dral id dralId.
 *
 * @return void.
 */
void
DreamsDB::layoutResourceGetRowsWithDralId(DRAL_ID dralId, RowType type, const rowList ** rows) const
{
    resource->getRowsWithDralId(dralId, type, rows);
}

bool 
DreamsDB::layoutResourceFindItemWindow(ITEM_ID itemId, CYCLE & firstCycle, CYCLE & lastCycle)
{
    return resource->findItemWindow(itemId, firstCycle, lastCycle);
}

bool 
DreamsDB::layoutResourceFindItemFirstAppearence(ITEM_ID itemId, CYCLE startcycle, UINT32 & resrow, CYCLE & rescycle)
{
    return resource->lookForFirstAppearence(itemId, startcycle, resrow, rescycle);
}

bool 
DreamsDB::layoutResourceFindItemLastAppearence(ITEM_ID itemId, CYCLE endcycle, UINT32 & resrow, CYCLE & rescycle)
{
    return resource->lookForLastAppearence(itemId, endcycle, resrow, rescycle);
}

/*
 * Fills the data set with the information of all the rows.
 *
 * @return void.
 */
void
DreamsDB::layoutResourceGetFindData(LayoutResourceFindDataSet * entries, CYCLE startCycle, const LayoutResourceFindOptions * env) const
{
    resource->getFindData(entries, startCycle, env);
}

/*
 * Computes the layout cache.
 *
 * @return void.
 */
void
DreamsDB::layoutResourceComputeLayoutCache(LayoutResourceCache * cache)
{
    resource->computeLayoutCache(cache);
}

/*
 * Computes the layout cache.
 *
 * @return void.
 */
void
DreamsDB::layoutResourceComputeLayoutPlainCache()
{
    resource->computeLayoutPlainCache();
}

/*
 * Gets if the row is a group
 *
 * @return true if is a group.
 */
bool
DreamsDB::layoutResourceGetRowIsGroup(UINT32 row, LayoutResourceCache * cache) const
{
    return resource->getRowIsGroup(row, cache);
}

/*
 * Switches the row group fold/unfold state.
 *
 * @return void.
 */
void
DreamsDB::layoutResourceSwitchRowGroup(UINT32 row, LayoutResourceCache * cache)
{
    resource->switchRowGroup(row, cache);
}

/*
 * Gets the composed name of the group row row.
 *
 * @return the name.
 */
QString
DreamsDB::layoutResourceGetRowComposedName(UINT32 row, LayoutResourceCache * cache) const
{
    return resource->getRowComposedName(row, cache);
}

/*
 * Gets if the physical row is shown in the cache.
 *
 * @return if is shown.
 */
bool
DreamsDB::layoutResourceGetRowIsShown(UINT32 row, LayoutResourceCache * cache) const
{
    return resource->getRowIsShown(row, cache);
}

/*
 * Gets the virtual mapping of the row.
 *
 * @return virtual row.
 */
UINT32
DreamsDB::layoutResourceGetRowVirtMap(UINT32 row, LayoutResourceCache * cache) const
{
    return resource->getRowVirtMap(row, cache);
}

/*
 * Changes the folding state so the physical row row is shown.
 *
 * @return void.
 */
void
DreamsDB::layoutResourceShowRow(UINT32 row, LayoutResourceCache * cache)
{
    return resource->showRow(row, cache);
}

/*
 * Returns a cache that has no mapping between virtual and physical rows.
 *
 * @return a plain cache.
 */
LayoutResourceCache *
DreamsDB::layoutResourceGetFlatCache() const
{
    return resource->getPlainCache();
}

void
DreamsDB::addMapTable(QString name, ADFMap * map)
{
    mapset.insert(name, map);
}

ADFMap * 
DreamsDB::getMapTable(QString name) const
{
    return mapset.find(name);
}

bool
DreamsDB::hasLoadedData() const
{
    return (getNumCycles(0).cycle > 0);
}

void
DreamsDB::layoutResourceDrawSubSamplingScale(INT32 x1, INT32 x2, INT32 y1, INT32 y2, double scf_x, double scf_y, QPainter * painter, LayoutResourceCache * cache, bool paint_events)
{
    return resource->drawSubSamplingScale(x1, x2, y1, y2, scf_x, scf_y, painter, cache, paint_events);
}

void
DreamsDB::layoutResourceDrawNoFullScale(INT32 x1, INT32 x2, INT32 y1, INT32 y2, QPainter * painter, LayoutResourceCache * cache, bool paint_events)
{
    return resource->drawNoFullScale(x1, x2, y1, y2, painter, cache, paint_events);
}

void
DreamsDB::layoutResourceDrawFullScale(INT32 x1, INT32 x2, INT32 y1, INT32 y2, QPainter * painter, AScrollView * asv, LayoutResourceCache * cache, bool paint_events)
{
    return resource->drawFullScale(x1, x2, y1, y2, painter, asv, cache, paint_events);
}

void
DreamsDB::layoutFloorplanCreateLayout()
{
    Q_ASSERT(floorplan == NULL);
    floorplan = new LayoutFloorplan(draldb);
}

/*
 * Returns if the Floorplan view is defined.
 *
 * @return if the Floorplan view is defined.
 */
bool
DreamsDB::layoutFloorplanHasLayout() const
{
    return (floorplan != NULL);
}

bool
DreamsDB::layoutFloorplanGetOpened(void) const
{
    return floorplan->getOpened();
}

void
DreamsDB::layoutFloorplanSetRoot(LayoutFloorplanTable * root)
{
    floorplan->setRoot(root);
}

UINT32
DreamsDB::layoutFloorplanGetWidth(void) const
{
    return floorplan->getWidth();
}

UINT32
DreamsDB::layoutFloorplanGetHeight(void) const
{
    return floorplan->getHeight();
}

UINT32
DreamsDB::layoutFloorplanGetNumCells(void) const
{
    return floorplan->getNumCells();
}

void
DreamsDB::layoutFloorplanGetRectanglesAt(UINT32 x1, UINT32 y1, UINT32 x2, UINT32 y2, QPtrList<LayoutFloorplanNode> & listResult)
{
    floorplan->getRectanglesAt(x1, y1, x2, y2, listResult);
}

QString
DreamsDB::layoutFloorplanGetCellAt(UINT32 x, UINT32 y)
{
    return floorplan->getCellAt(x, y);
}

QString
DreamsDB::layoutFloorplanGetNodeNameAt(UINT32 x, UINT32 y)
{
    return floorplan->getNodeNameAt(x, y);
}

INT32
DreamsDB::layoutFloorplanGetTrackIdForCell(UINT32 x, UINT32 y)
{
    return floorplan->getTrackIdForCell(x, y);
}

bool
DreamsDB::layoutFloorplanHasValidData(INT32 x, INT32 y, CYCLE cycle)
{
    return floorplan->getValidData(x, y, cycle);
}

ItemHandler
DreamsDB::layoutFloorplanGetCellWithTagValue(TAG_ID tagId, UINT64 tagValue, CYCLE cycle, INT32 & startX, INT32 & startY)
{
    return floorplan->getCellWithTagValue(tagId, tagValue, cycle, startX, startY);
}

ItemHandler
DreamsDB::layoutFloorplanGetCellWithTagValue(TAG_ID tagId, QString & tagValue, bool cs, bool em, CYCLE cycle, INT32 & startX, INT32 & startY)
{
    return floorplan->getCellWithTagValue(tagId, tagValue, cs, em, cycle, startX, startY);
}

ItemHandler
DreamsDB::layoutFloorplanGetCellWithTagValue(TAG_ID tagId, QRegExp & tagValue, CYCLE cycle, INT32 & startX, INT32 & startY)
{
    return floorplan->getCellWithTagValue(tagId, tagValue, cycle, startX, startY);
}

bool
DreamsDB::layoutFloorplanGetFirstCellAppearenceWithTagValue(TAG_ID tagId, UINT64 tagValue, CYCLE & cycle, INT32 & startX, INT32 & startY)
{
    return floorplan->getFirstCellAppearenceWithTagValue(tagId, tagValue, cycle, startX, startY);
}

void
DreamsDB::layoutFloorplanGetFindData(LayoutFloorplanFindDataSet * entries, CYCLE startCycle, const LayoutFloorplanFindOptions * env) const
{
    floorplan->getFindData(entries, startCycle, env);
}

void
DreamsDB::layoutWatchCreateLayout()
{
    Q_ASSERT(watch == NULL);
    watch = new LayoutWatch();
}

/*
 * Returns if the Watch Window view is defined.
 *
 * @return if the Watch Window view is defined.
 */
bool
DreamsDB::layoutWatchHasLayout() const
{
    return (watch != NULL);
}

void 
DreamsDB::layoutWatchAddWindow(WatchWindow *newWin)
{
    watch->addWatchWindow(newWin);
}

WatchWindow *
DreamsDB::layoutWatchGetWindow(QString caption)
{
    return watch->getWatchWindow(caption);
}

QDictIterator<WatchWindow>
DreamsDB::layoutWatchGetWindowIterator(void)
{
    return watch->getWatchWindowIterator();
}

bool 
DreamsDB::layoutWatchGetOpened(void)
{
    return watch->getOpened();
}

void
DreamsDB::layoutWaterfallCreateLayout()
{
    Q_ASSERT(waterfall == NULL);
    waterfall = new LayoutWaterfall(this);
}

/*
 * Returns if the WaterFall view is defined.
 *
 * @return if the WaterFall view is defined.
 */
bool
DreamsDB::layoutWaterfallHasLayout() const
{
    return (waterfall != NULL);
}

bool
DreamsDB::layoutWaterfallAddMoveItem(DralGraphEdge * edge)
{
    return waterfall->addMoveItem(edge);
}

bool
DreamsDB::layoutWaterfallAddEnterNode(DralGraphNode * node)
{
    return waterfall->addEnterNode(node);
}

bool
DreamsDB::layoutWaterfallAddExitNode(DralGraphNode * node)
{
    return waterfall->addExitNode(node);
}

bool
DreamsDB::layoutWaterfallAddMoveItemRules(DralGraphEdge * edge, ExpressionList & rules)
{
    return waterfall->addMoveItemRules(edge, rules);
}

bool
DreamsDB::layoutWaterfallAddEnterNodeRules(DralGraphNode * node, ExpressionList & rules)
{
    return waterfall->addEnterNodeRules(node, rules);
}

bool
DreamsDB::layoutWaterfallAddExitNodeRules(DralGraphNode * node, ExpressionList & rules)
{
    return waterfall->addExitNodeRules(node, rules);
}

void
DreamsDB::layoutWaterfallAddDefaultRules(ExpressionList & rules, INT32 level)
{
    return waterfall->addDefaultRules(rules, level);
}

void
DreamsDB::layoutWaterfallAddOverrideRules(ExpressionList & rules, INT32 level)
{
    return waterfall->addOverrideRules(rules, level);
}

void
DreamsDB::layoutWaterfallAddRules(ExpressionList & rules, INT32 level)
{
    return waterfall->addRules(rules, level);
}

void
DreamsDB::layoutWaterfallComputeRangeForRow(INT32 row)
{
    waterfall->computeRangeForRow(row);
}

CYCLE
DreamsDB::layoutWaterfallGetCurrentRowMinCycle() const
{
    return waterfall->getCurrentRowMinCycle();
}

CYCLE
DreamsDB::layoutWaterfallGetCurrentRowMaxCycle() const
{
    return waterfall->getCurrentRowMaxCycle();
}

bool
DreamsDB::layoutWaterfallGetHasTags(UINT32 row, ItemHandler * hnd)
{
    return waterfall->hasTags(row, hnd);
}

void
DreamsDB::layoutWaterfallUpdateDBInfo()
{
    return waterfall->updateDBInfo();
}

bool
DreamsDB::layoutWaterfallHasValidData(UINT32 row, INT64 ps, ItemHandler * hnd, ExpressionList ** rlist)
{
    return waterfall->getValidData(row, ps, hnd, rlist);
}

bool
DreamsDB::layoutWaterfallGetRules(UINT32 row, ItemHandler * hnd, ExpressionList ** rlist)
{
    return waterfall->getRules(row, hnd, rlist);
}

void
DreamsDB::layoutWaterfallResetColorShapeLetter()
{
    return waterfall->resetColorShapeLetter();
}

ColorShapeItem
DreamsDB::layoutWaterfallGetColorShapeLetter(ItemHandler * hnd, ExpressionList * list)
{
    return waterfall->getColorShapeLetter(hnd, list);
}

bool
DreamsDB::layoutWaterfallGetCurrentITagValue(TAG_ID tagId, INT64 * value) const
{
    return waterfall->getCurrentITagValue(tagId, value);
}

ITEM_ID
DreamsDB::layoutWaterfallGetItemIdForRow(UINT32 row)
{
    return waterfall->getItemIdForRow(row);
}

bool
DreamsDB::layoutWaterfallGetUsesDivisions() const
{
    return waterfall->getUsesDivisions();
}

UINT32
DreamsDB::layoutWaterfallGetWidth() const
{
    return waterfall->getWidth();
}

UINT32
DreamsDB::layoutWaterfallGetHeight() const
{
    return waterfall->getHeight();
}

UINT32
DreamsDB::layoutWaterfallGetNumRows() const
{
    return waterfall->getNumRows();
}

bool
DreamsDB::layoutWaterfallGetOpened() const
{
    return waterfall->getOpened();
}

void
DreamsDB::layoutWaterfallSetCurrentItemTab(ItemTab * newtab)
{
    return waterfall->setCurrentItemTab(newtab);
}

TabSpecList *
DreamsDB::getTabSpecList() const
{
    return tabspeclist;
}

ItemTabList *
DreamsDB::getItemTabList() const
{
    return itablist;
}

bool
DreamsDB::getHasFatalError(bool end_sim)
{
    return draldb->getHasFatalError(end_sim);
}

bool
DreamsDB::reachedEOS() const
{
    return draldb->reachedEOS();
}

bool
DreamsDB::nonCriticalErrors() const
{
    return false;
}

CYCLE
DreamsDB::getNumCycles(UINT16 clockId) const
{
    return draldb->getNumCycles(clockId);
}

void
DreamsDB::setBookMarkMgr(BookmarkMgr* mgr)
{
    bkMgr = mgr;
}

QStrList
DreamsDB::getKnownTags() const
{
    return draldb->getKnownTags();
}

void
DreamsDB::setColumnShadows(ColumnShadows* sh)
{
    cshadows = sh;
}

INT32
DreamsDB::getNumMDI() const
{
    return numMDI;
}

void
DreamsDB::decNumMDI()
{
    numMDI--;
}

void
DreamsDB::incNumMDI()
{
    numMDI++;
}

#endif
