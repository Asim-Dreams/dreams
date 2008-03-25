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
  * @file TagDescVector.cpp
  */

// Dreams includes.
#include "dDB/TagDescVector.h"
#include "dDB/PrimeList.h"

/**
 * Creator of this class.
 *
 * @return new object.
 */
TagDescVector::TagDescVector(StrTable * _strtbl)
{
    strtbl = _strtbl;

    // Inserts the information of all the internal tags.
    bool ok;
    ok = newTagDescriptor(ITEMID_STR_TAG, TAG_INTEGER_TYPE, TAG_BASE_DECIMAL, TAG_SIZE_64_BITS, false, &itemIdTagId);
    Q_ASSERT(ok && (itemIdTagId == 0));

    ok = newTagDescriptor(SLOTITEM_STR_TAG, TAG_INTEGER_TYPE, TAG_BASE_DECIMAL, TAG_SIZE_31_BITS, true, &slotItemTagId);
    Q_ASSERT(ok && (slotItemTagId == 1));

    ok = newTagDescriptor(NEWITEM_STR_TAG, TAG_INTEGER_TYPE, TAG_BASE_DECIMAL, TAG_SIZE_64_BITS, true, &newItemTagId);
    Q_ASSERT(ok && (newItemTagId == 2));

    ok = newTagDescriptor(DELITEM_STR_TAG, TAG_INTEGER_TYPE, TAG_BASE_DECIMAL, TAG_SIZE_64_BITS, true, &delItemTagId);
    Q_ASSERT(ok && (delItemTagId == 3));
}

/**
 * Deletes the object.
 *
 * @return void.
 */
TagDescVector::~TagDescVector()
{
    TagDescriptor * tag;
    char * name;

    // Erases all the allocated tag descriptors.
    while(hash.size() > 0)
    {
        tag = hash.begin()->second;
        name = (char *) hash.begin()->first;
        hash.erase(hash.begin());
        delete tag;
        free((void *) name);
    }
}

/*
 * Tries to enter a new tag description in the vector.
 *
 * @return if the tag has been added correctly.
 */
bool
TagDescVector::newTagDescriptor(QString name, TagValueType t, TagValueBase base, TagSizeStorage size, bool forwarding, TAG_ID * tag_id)
{
    TagDescriptorHashIterator it;

    // Tries to find the string in the hash.
    it = hash.find(name.latin1());

    // The tag was already defined.
    if(it != hash.end())
    {
        return false;
    }

    // If not, put it now.
    TagDescriptor * tag;

    tag = new TagDescriptor();
    tag->base = base;
    tag->desc = QString::null;
    tag->frwd = forwarding;
    tag->indx = hash.size();
    tag->name = name;
    tag->size = size;
    tag->type = t;

    char * new_str = (char *) malloc(name.length() + 1);
    strcpy(new_str, name.latin1());

    hash[new_str] = tag;
    tags.push_back(tag);
    * tag_id = tag->indx;

    return true;
}

/*
 * Returns a list of the known tags in the vector.
 *
 * @return a list with the known tags.
 */
QStrList
TagDescVector::getKnownTags()
{
    QStrList strlist(false);

    for(UINT32 i = 0; i < tags.size(); ++i)
    {
        strlist.append(tags[i]->name.latin1());
    }
    return strlist;
}

/*
 * Returns the value in a string with the format of the tag.
 *
 * @return the formatted value.
 */
QString
TagDescVector::getFormatedTagValue(TAG_ID id, UINT64 value)
{
    static char tmpstr[128];
    char * tmp;

    TagValueType tgvtype = getTagValueType(id);
    TagValueBase tgbase = getTagValueBase(id);

    if(tgvtype == TAG_STRING_TYPE)
    {
        return strtbl->getString(value);
    }

    switch(tgbase)
    {
        case TAG_BASE_BINARY:
            tmp = DBItoa::bitstring(value, 64, 64);
            return QString(tmp) + "B";
            break;

        case TAG_BASE_OCTAL:
            sprintf(tmpstr,FMTP64O,value);
            return QString(tmpstr);
            break;

        case TAG_BASE_DECIMAL:
            sprintf(tmpstr, FMT64U, value);
            return QString(tmpstr);
            break;

        case TAG_BASE_HEXA:
            sprintf(tmpstr, FMTP64X, value);
            return QString(tmpstr);
            break;
    }

    return QString("Invalid base or tag type");
}

/**
 * Sets the description of the tag.
 *
 * @return void.
 */
void    
TagDescVector::setTagDesc(TAG_ID id, QString desc)
{
    tags[id]->desc = desc;
}

/*
 * Computes the total size of the object to know the memory used.
 *
 * @return the object size.
 */
INT64
TagDescVector::getObjSize() const
{
    INT64 result = sizeof(TagDescVector);
    // 30 is an approximated value for the keys in the hash and the QStrings in the TagDescriptor.
    result += hash.size() * (sizeof(TagDescriptor *) + 30 + sizeof(TagDescriptor));
    result += tags.capacity() * sizeof(TagDescriptor *);
    return result;
}

/*
 * Returns a description of the vector.
 *
 * @return the description.
 */
QString
TagDescVector::getUsageDescription() const
{
    QString result = "\nTag Descriptor Statistics:\n";
    result += "Used Entries:\t" + QString::number(hash.size()) + "\n";
    return result;
}

/*
 * Returns the stats of the object. Only the number of entries
 * is returned.
 *
 * @return the object stats.
 */
QString
TagDescVector::getStats() const
{
   QString  result = "\tNumber of entries:\t"+QString::number(hash.size())+"\n";
   return result;
}
