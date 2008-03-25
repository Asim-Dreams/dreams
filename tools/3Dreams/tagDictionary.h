/*****************************************************************************
*
* @brief Dictionary where information about tags is stored to reduce memory usage.
*
* @author Oscar Rosell
*
*  Copyright (C) 2003-2006 Intel Corporation
*  
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*  
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*  
*
*****************************************************************************/

#ifndef TAG_DICTIONARY_H
#define TAG_DICTIONARY_H

#include "cachedata.h"
#include "dictionary.h"
#include <qstring.h>
#include <assert.h>

class TagDescriptor
{
  public:
    TagDescriptor()
    {
    }
    enum TagType 
    {
        TYPE_TAG_STRING=0,
        TYPE_TAG_INTEGER,
        TYPE_TAG_SET,
        TYPE_TAG_FLOAT,
        NULL_TAG_TYPE
    };
    TagDescriptor(QString name, TagType type, UINT16 base = 10)
    {
        name_ = name;
        type_ = type;
        base_ = base;
    };
    ~TagDescriptor()
    {
    }
    QString name() const
    {
        return name_;
    }
    void setName(const QString name) 
    {
        name_ = name;
    }
    TagType type() const
    {
        return type_;
    }
    void setType(TagType type)
    {
        type_ = type;
    }
    UINT16 base() const
    {
        return base_;
    }
    void setBase(const UINT16 base)
    {
        base_ = base;
    }
    bool operator==(const TagDescriptor& other) const
    {
        if (
            name_ == other.name()
            )
        {
            return true;
        }
        return false;
    }

  private:
    QString name_;
    TagType type_;
    UINT16  base_;
};

class TagDictionary
{
  public:
    static unsigned char addEntry(TagDescriptor entry);
    static TagDescriptor getEntry(unsigned char idx);
    static unsigned char getEntryIndex(const QString& str, bool* found);
    static unsigned char getEntryIndex(const TagDescriptor& entry, bool* found);
  private:
    TagDictionary();
    static MyDictionary<TagDescriptor,unsigned char> dictionary_;
};



#endif // TAG_DICTIONARY
