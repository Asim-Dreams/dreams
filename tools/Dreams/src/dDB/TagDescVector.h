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
  * @file TagDescVector.h
  */

#ifndef _DRALDB_TAGDESCVECTOR_H
#define _DRALDB_TAGDESCVECTOR_H

// General includes.
#include <stdio.h>
#include <vector>
using namespace std;
#include <ext/hash_map>
using namespace __gnu_cxx;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"
#include "dDB/DralDBDefinitions.h"
#include "dDB/StrTable.h"
#include "dDB/DBItoa.h"
#include "dDB/aux/AMemObj.h"
#include "dDB/aux/StatObj.h"
#include "TagValueType.h"

// Qt includes.
#include <qstrlist.h>

/**
  * @typedef TagDescriptor
  * @brief
  * Contains the specification of a tag.
  */
typedef struct
{
    QString        name; ///< Name of the tag.
    INT32          indx; ///< Index of the tag.
    QString        desc; ///< Description of the tag.
    TagValueType   type; ///< Type of tag: integer, string, ... .
    TagValueBase   base; ///< Base in case of integer tag.
    TagSizeStorage size; ///< Size of the internal storage: how many bits must be used.
    bool           frwd; ///< If the values of this tag are forwarded when a search is performed.
} TagDescriptor;

/**
  * @brief
  * Vector holding tag descriptors (name, value type, etc).
  *
  * @description
  * This class holds all the information of the user defined tags.
  * The name, value and type is stored.
  *
  * @version 0.1
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class TagDescVector : public AMemObj, public StatObj
{
    public:
        // ---- AMemObj Interface methods
        virtual INT64 getObjSize() const;
        virtual QString getUsageDescription() const;
        // ---- StatObj Interface methods
        QString getStats() const;

        TagDescVector(StrTable * _strtbl);
        virtual ~TagDescVector();

        inline TAG_ID getItemIdTagId();
        inline TAG_ID getSlotItemTagId();
        inline TAG_ID getNewItemTagId();
        inline TAG_ID getDelItemTagId();

        bool newTagDescriptor(QString name, TagValueType t, TagValueBase base, TagSizeStorage size, bool forwarding, TAG_ID * tag_id);
        inline void changeTagDescriptor(TAG_ID id, TagValueType t);
        inline QString getTagName(TAG_ID id);
        inline TagValueBase getTagValueBase(TAG_ID id);
        inline TagValueType getTagValueType(TAG_ID id);
        inline TagSizeStorage getTagSize(TAG_ID id);
        inline bool getTagForwarding(TAG_ID id);

        inline TAG_ID getTagId(QString name);
        inline QString getTagDesc(QString name);

        void setTagDesc(TAG_ID id, QString desc); 

        inline UINT16 getNumTags();
        QStrList getKnownTags();

        QString getFormatedTagValue(TAG_ID id, UINT64 value);

    private:
        /*
         * Struct used to make a comparision between the characters contained in 
         * (char *) key in hash_map.
         */
        struct StrCmp
        {
            bool operator()(const char * s1, const char * s2) const
            {
                return strcmp(s1, s2) == 0;
            }
        } ;

        /*
         * Structs to implement the mapping using a stl hash map.
         */
        typedef hash_map<const char *, TagDescriptor *, hash<const char *>, StrCmp> TagDescriptorHash;
        typedef hash_map<const char *, TagDescriptor *, hash<const char *>, StrCmp>::iterator TagDescriptorHashIterator;

    private:
        vector<TagDescriptor *> tags; ///< Vector of tags used to access by index.
        TagDescriptorHash hash;       ///< Hash with the tag descriptors.
        StrTable * strtbl;            ///< Instance of the string table.
        TAG_ID itemIdTagId;           ///< Value of the itemid.
        TAG_ID slotItemTagId;         ///< Value of the tag that holds the items inside an slot.
        TAG_ID newItemTagId;          ///< Value of the tag that holds the creation of an item.
        TAG_ID delItemTagId;          ///< Value of the tag that holds the destruction of an item.
} ;

/**
 * Returns the tag of the item id.
 *
 * @return the tag.
 */
TAG_ID
TagDescVector::getItemIdTagId()
{
    return itemIdTagId;
}

/**
 * Returns the tag that holds the items inside an slot.
 *
 * @return the tag.
 */
TAG_ID
TagDescVector::getSlotItemTagId()
{
    return slotItemTagId;
}

/*
 * Returns the tag that holds the creation of items.
 *
 * @return the tag.
 */
TAG_ID
TagDescVector::getNewItemTagId()
{
    return newItemTagId;
}

/*
 * Returns the tag that holds the destruction of items.
 *
 * @return the tag.
 */
TAG_ID
TagDescVector::getDelItemTagId()
{
    return delItemTagId;
}

/*
 * Changes the type of the given tag.
 *
 * @return void.
 */
void
TagDescVector::changeTagDescriptor(TAG_ID id, TagValueType t)
{
    Q_ASSERT((UINT32) id < (UINT32) hash.size());
    tags[id]->type = t;
}

/*
 * Returns the name of tag with id id.
 *
 * @return the name.
 */
QString
TagDescVector::getTagName(TAG_ID id)
{
    Q_ASSERT((UINT32) id < (UINT32) hash.size());
    return tags[id]->name;
}

/**
 * Returns the tag type of the tag description.
 *
 * @return the tag type.
 */
TagValueType
TagDescVector::getTagValueType(TAG_ID id)
{
    Q_ASSERT((UINT32) id < (UINT32) hash.size());
    return tags[id]->type;
}

/*
 * Returns the base of the tag description with id id.
 *
 * @return the base.
 */
TagValueBase
TagDescVector::getTagValueBase(TAG_ID id)
{
    Q_ASSERT((UINT32) id < (UINT32) hash.size());
    return tags[id]->base;
}

/*
 * Returns if the tag is forwarded when a search is performed.
 *
 * @return the forwarding.
 */
TagSizeStorage
TagDescVector::getTagSize(TAG_ID id)
{
    Q_ASSERT((UINT32) id < (UINT32) hash.size());
    return tags[id]->size;
}

/*
 * Returns if the tag is forwarded when a search is performed.
 *
 * @return the forwarding.
 */
bool
TagDescVector::getTagForwarding(TAG_ID id)
{
    Q_ASSERT((UINT32) id < (UINT32) hash.size());
    return tags[id]->frwd;
}

/**
 * Looks for the tag name in the vector and returns the index where
 * is stored.
 *
 * @return the index of the tag.
 */
TAG_ID
TagDescVector::getTagId(QString name)
{
    TagDescriptorHashIterator it;

    // Tries to find the string in the hash.
    it = hash.find(name.latin1());

    // The tag was already defined.
    if(it != hash.end())
    {
        return (TAG_ID) it->second->indx;
    }

    return (TAG_ID) -1;
}

/**
 * Returns the long description of the tag.
 *
 * @return the description.
 */
QString 
TagDescVector::getTagDesc(QString name)
{
    TagDescriptorHashIterator it;

    // Tries to find the string in the hash.
    it = hash.find(name.latin1());

    // The tag was already defined.
    if(it != hash.end())
    {
        return it->second->desc;
    }
    return QString::null;
}

/**
 * Return the number of tags used.
 *
 * @return number of tags.
 */
UINT16
TagDescVector::getNumTags()
{
    return hash.size();
}

#endif
