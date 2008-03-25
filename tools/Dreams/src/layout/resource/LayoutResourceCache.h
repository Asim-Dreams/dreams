// ==================================================
// Copyright (C) 2006 Intel Corporation
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
  * @file LayoutResourceCache.h
  */

#ifndef _LAYOUTRESOURCECACHE_H
#define _LAYOUTRESOURCECACHE_H

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "mdi/MDIResourceWindowFold.h"

// Class forwarding.
class DreamsDB;
class LayoutResourceNode;
class MDIResourceWindowFold;

/*
 * @brief
 * This is a entry of the resource mapping.
 *
 * @description
 * To speed up the access to the information of the rows, a mapping
 * between the physical rows in the layout and the virtual rows in
 * the screen is used. This class represents a row in the screen, which
 * accesses to the information of a physical row in the layout. The
 * information needed is the node where it accesses and the row within
 * this node.
 *
 * @author Guillem Sole
 * @date started at 2004-12-29
 * @version 0.1
 */
class LayoutResourceMapEntry
{
  public:
    LayoutResourceMapEntry();
    ~LayoutResourceMapEntry();

    inline void setNode(LayoutResourceNode * _node);
    inline void setDisp(UINT32 _disp);

    inline LayoutResourceNode * getNode() const;
    inline UINT32 getDisp() const;

  private:
    LayoutResourceNode * node; ///< Node where the requests are done.
    UINT32 disp;               ///< Displacement within the node needed to perform the requests.
} ;

/*
 * @brief
 * This class is a cache that uses the previous classes.
 *
 * @description
 * As we want that each resource window has its own folding/unfolding
 * state, each one must have a different cache. This structure will
 * then be used by the resource to know which is the physical row
 * mapped to the virtual row in all the requests.
 *
 * @author Guillem Sole
 * @date started at 2004-07-23
 * @version 0.1
 */
class LayoutResourceCache
{
    public:
        LayoutResourceCache(UINT32 _numRows, UINT32 numGroups);
        virtual ~LayoutResourceCache();

        inline UINT32 getNumRows() const;

        inline void setFoldWidget(MDIResourceWindowFold * ptr);
        inline MDIResourceWindowFold * getFoldWidget() const;

        inline LayoutResourceMapEntry * transVirToPhy(UINT32 pos) const;
        inline UINT32 transPhyToVir(UINT32 pos) const;

        friend class LayoutResource;
        friend class LayoutResourceNode;
        friend class LayoutResourceRow;
        friend class LayoutResourceGroup;
        friend class MDIResourceWindowFold;

    private:
        LayoutResourceMapEntry * virToPhy; ///< Cache from virtual to physical.
        UINT32 * phyToVir;                 ///< Cache from physical to virtual.
        ResourceViewFoldEnum * foldCache;  ///< Cache of the folding state.
        bool * groupExpansion;             ///< State of the different groups: expanded or not.
        UINT32 numRows;                    ///< Physical rows.
        UINT32 numVirtualRows;             ///< Virtual row counting.
        MDIResourceWindowFold * myFold;    ///< Fold associated to the cache.
} ;

/*
 * Return the num of rows of the cache.
 *
 * @return the num of rows.
 */
UINT32
LayoutResourceCache::getNumRows() const
{
    return numVirtualRows;
}

/*
 * Associates the cache to the fold widget.
 *
 * @return void.
 */
void
LayoutResourceCache::setFoldWidget(MDIResourceWindowFold * ptr)
{
    myFold = ptr;
}

/*
 * Returns the fold widget associated to this cache.
 *
 * @return the fold widget.
 */
MDIResourceWindowFold *
LayoutResourceCache::getFoldWidget() const
{
    return myFold;
}

/*
 * Returns the map entry associated to the virtual position
 *
 * @return the map entry.
 */
LayoutResourceMapEntry *
LayoutResourceCache::transVirToPhy(UINT32 pos) const
{
    Q_ASSERT(pos < numRows);
    return &virToPhy[pos];
}

/*
 * Returns the virtual position associated to the physical position.
 *
 * @return the position.
 */
UINT32
LayoutResourceCache::transPhyToVir(UINT32 pos) const
{
    Q_ASSERT(pos < numVirtualRows);
    return phyToVir[pos];
}

/*
 * Sets the node of the map entry.
 *
 * @return void.
 */
void
LayoutResourceMapEntry::setNode(LayoutResourceNode * _node)
{
    node = _node;
}

/*
 * Sets the displacement of the map entry.
 *
 * @return void.
 */
void
LayoutResourceMapEntry::setDisp(UINT32 _disp)
{
    disp = _disp;
}

/*
 * Gets the node of the map entry.
 *
 * @return the node.
 */
LayoutResourceNode *
LayoutResourceMapEntry::getNode() const
{
    return node;
}

/*
 * Gets the displacement of the map entry.
 *
 * @return the displacement.
 */
UINT32
LayoutResourceMapEntry::getDisp() const
{
    return disp;
}

#endif // _LAYOUTRESOURCECACHE_H
