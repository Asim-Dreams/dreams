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
  * @file TrackVec.cpp
  */

// Dreams includes.
#include "dDB/trackheap/TrackVec.h"

/**
 * Creator of this class. Set defaults values.
 *
 * @return new object.
 */
TrackIDNode::TrackIDNode() : tgIdVector(2)
{
    dict = NULL;
    trt = NULL;
}

/**
 * Clears the content of the tracking vector.
 *
 * @return destroys the object.
 */
TrackIDNode::~TrackIDNode()
{
    TagIdVecNode * tag; ///< Pointer to the current tag.

    if(trt != NULL)
    {
        for(UINT16 i = 0; i < tgIdVector.getLimit(); i++)
        {
            tag = tgIdVector.constAccess(i);
            if(tag != NULL)
            {
                delete tag;
            }
        }
    }
}

/**
 * Dumps the content of the vector.
 *
 * @return void.
 */
void
TrackIDNode::dumpRegression()
{
    UINT16 tag_ren;     ///< The renamed tag value.
    TagIdVecNode * tag; ///< Pointer to the current tag.
    QString desc;       ///< Tag description.
    set<TAG_ID> tags;   ///< List of existing tags.

    if(trt != NULL)
    {
        tags = trt->getRenamedTags();
        for(set<TAG_ID>::iterator it = tags.begin(); it != tags.end(); it++)
        {
            TAG_ID tagId = * it;

            tag_ren = trt->rename(tagId);
            tag = tgIdVector.constAccess(tag_ren);
            if(tag != NULL)
            {
                desc = tagdescvec->getTagName(tagId);
                if(desc.contains(QString("__color")) == 0)
                {
                    printf("T=%s\n", desc.latin1());
                    tag->dumpTagIdVector();
                }
            }
        }
    }
}

/**
  * Compresses the vector to a dense vector.
  *
  * @return the compressed vector.
  */
ZipObject *
TrackIDNode::compressYourSelf(CYCLE cycle, bool last)
{
    TagIdVecNode * tag; ///< Pointer to the current tag.

    if(trt != NULL)
    {
        for(UINT16 i = 0; i < tgIdVector.getLimit(); i++)
        {
            tag = tgIdVector.constAccess(i);
            if(tag != NULL)
            {
                tag->compressYourSelf(cycle, last);
            }
        }
    }
    return this;
}

/*
 * Returns a list with all the known tag ids.
 *
 * @return a list with the tag ids.
 */
TagIDList *
TrackIDNode::getKnownTagIDs() const
{
    TagIDList * result; ///< Return list.
    set<TAG_ID> tags;   ///< List of existing tags.

    result = new TagIDList();

    // Gets if there's a rename table.
    if(trt != NULL)
    {
        tags = trt->getRenamedTags();
        // For all the renamed tags.
        for(set<TAG_ID>::iterator it = tags.begin(); it != tags.end(); it++)
        {
            result->append(* it);
        }
    }

    return result;
}

/*
 * Gets the object size.
 *
 * @return the object size.
 */
INT64
TrackIDNode::getObjSize() const
{
    INT64 ret;
    UINT16 i = 0;       ///< Tag iterator.
    UINT16 tag_ren;     ///< The renamed tag value.
    TagIdVecNode * tag; ///< Pointer to the current tag.
    set<TAG_ID> tags;   ///< List of existing tags.

    ret = sizeof(TrackIDNode);
    ret += tgIdVector.getObjSize() - sizeof(tgIdVector);

    if(trt != NULL)
    {
        tags = trt->getRenamedTags();
        for(set<TAG_ID>::iterator it = tags.begin(); it != tags.end(); it++)
        {
            tag_ren = trt->rename(* it);
            tag = tgIdVector.constAccess(tag_ren);
            if(tag != NULL)
            {
                ret += tag->getObjSize();
            }
        }
    }
    return ret;
}

/*
 * Returns the usage description of this object.
 *
 * @return the usage description.
 */
QString
TrackIDNode::getUsageDescription() const
{
    QString ret;        ///< Contains the description.
    INT64 size;         ///< Temporal 
    UINT16 tag_ren;     ///< The renamed tag value.
    TagIdVecNode * tag; ///< Pointer to the current tag.
    set<TAG_ID> tags;   ///< List of existing tags.

    ret += QString("\t\tThis TrackIDNode object occupies ") + QString::number(getObjSize()) + QString(" bytes.\n");
    ret += QString("\t\tsizeof(TagIdVector) ") + QString::number(sizeof(TagIdVector)) + QString(" bytes.\n");
    ret += QString("\t\tTagIdVector.getObjSizE() ") + QString::number(tgIdVector.getObjSize()) + QString(" bytes.\n");

    if(trt != NULL)
    {
        ret += QString("\t\tDescription of the different tag elements (Total size of ");

        // Adds all the tracks.
        size = 0;

        tags = trt->getRenamedTags();
        for(set<TAG_ID>::iterator it = tags.begin(); it != tags.end(); it++)
        {
            tag_ren = trt->rename(* it);
            tag = tgIdVector.constAccess(tag_ren);
            if(tag != NULL)
            {
                size += tag->getObjSize();
            }
        }

        ret += QString::number(size) + " bytes):\n";

        // Prints the descriptions.
        // Runs through all the tags.
        for(set<TAG_ID>::iterator it = tags.begin(); it != tags.end(); it++)
        {
            tag_ren = trt->rename(* it);
            tag = tgIdVector.constAccess(tag_ren);
            if(tag != NULL)
            {
                ret += QString("\t\t\t") + QString::number(* it) + QString(")\n");
                ret += tag->getUsageDescription();
            }
        }
    }
    return ret;
}
