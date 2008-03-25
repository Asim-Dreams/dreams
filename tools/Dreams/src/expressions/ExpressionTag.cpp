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

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "ExpressionTag.h"

ExpressionTag::ExpressionTag(ExpressionOperatorType _op, DralDB *_dralDB, QString itagName) : 
      Expression(ExpressionNoValue), op(_op), draldb(_dralDB), myTagName(itagName)
{ 
    //
    // Translate the tag String name into a TagId using the DDB servies.
    // Note that the tag must exist. Otherwise, we flag an error.
    //
    tagId = draldb->getTagId(itagName);
    if(tagId == TAG_ID_INVALID)
    {
        hasError = true;
        myError = "Unknown tagId " + itagName;
        return;
    }
    else
    {
        hasError = false;
    }

    //
    // Also, convert from TagType to ExpressionType
    //
    type = ExpressionToolkit::toExpressionType(draldb->getTagValueType(tagId));

    EXPDEBUG(1) { cerr << "ExpressionTag::ExpressionTag " << myTagName.ascii() << "op=" << ExpressionToolkit::toString(op) << " tagId=" << tagId << " type=" << (UINT32)type << endl; }
}

ExpressionTag::~ExpressionTag()
{
}

//
// TypeCheck: this expression can not fail type checking because
//   a) the tag name has been mapped to a tagId (or else, we've ASSERTED)
//   b) The only valid types for a Tag are STR,INT,FP //TODO: Watch out for SetOfValues!!!!
//
bool
ExpressionTag::typeCheck(QString & error)
{
    if(hasError)
    {
        error = myError;
        return false;
    }

    Q_ASSERT(type == ExpressionIntegerValue || type == ExpressionStringValue || type == ExpressionFPValue );
    return true;
}

void
ExpressionTag::getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags)
{
    switch(op)
    {
        case OperatorITag:
            itemTags->insert(tagId);
            break;

        case OperatorSTag:
            slotTags->insert(tagId);
            break;

        case OperatorCTag:
            cycleTags->insert(tagId);
            break;

        case OperatorNTag:
            nodeTags->insert(tagId);
            break;

        default:
            Q_ASSERT(0);
            break;
    }
}

bool 
ExpressionTag::eval(RuleEvalEnv * env)
{
   TagValueEntry tags;

   EXPDEBUG(1) { cerr << "ExpressionTag::eval(trackId=" << env->trackId << ",cycle=" << env->cycle << ") on " << toString("") << endl; }
   EXPDEBUG(1) { cerr << "ExpressionTag::eval: cslResult             = " << env->cslResult.toString() ; }
   //
   // Based on operator, choose the rigth set of pointers
   //
   switch ( op ) {
       case OperatorITag:
           tags = env->itemTags;
           break;

       case OperatorSTag:
           tags = env->slotTags;
           break;

       case OperatorCTag:
           tags = env->cycleTags;
           break;

       case OperatorNTag:
           tags = env->nodeTags;
           break;

       default:
           Q_ASSERT(0);
           break;
   }

   //
   // Check for valid value first. If not, return immediately
   //
   // HACK: This is an ugly hack to support if(!item_inside) expression. The problem
   // is that if no item is inside a false is returned, hence the not unary expression
   // don't evaluate and also returns false, so the whole expression evaluates to
   // false when from a user point of view should be true. So when the requested tagId
   // is 0, the value returned is true instead of false.
   if( tags.valids[tagId] == false )
   {
       if(tagId == draldb->getSlotItemTagId())
       {
           setValue(0);
           return true;
       }
       return false;
   }

   switch ( type ) 
   {
    case ExpressionIntegerValue: setValue(tags.values[tagId]); break;
    case ExpressionFPValue:      setFPValue(tags.values[tagId]); break;
    case ExpressionStringValue:  setStringValue(tags.values[tagId]); break;
    default:                     Q_ASSERT(0); break;
   }
   return true;
}

bool 
ExpressionTag::eval(RuleEvalEnv * env, RuleEvalEnvDesc * desc, QString * pre, QString * post, INT32 indent)
{
    switch ( op ) {
        case OperatorITag:
            * pre += "itag(\"" + draldb->getTagName(tagId) + "\")";
            break;

        case OperatorSTag:
            * pre += "stag(\"" + draldb->getTagName(tagId) + "\")";
            break;

        case OperatorCTag:
            * pre += "ctag(\"" + draldb->getTagName(tagId) + "\")";
            break;

        case OperatorNTag:
            * pre += "ntag(\"" + draldb->getTagName(tagId) + "\")";
            break;

        default:
            Q_ASSERT(0);
            break;
    }

    return eval(env);
}

QString
ExpressionTag::toString(QString indent)
{
 QString res;

 res  =  indent + ExpressionToolkit::toString(op) + "(\"" + myTagName + "\")";
 return res;
}

UINT16
ExpressionTag::evalComputeMask()
{
    computeMask = 0;
    return computeMask;
}

INT64 
ExpressionTag::getObjSize() const
{
    INT64 result = sizeof(*this);
    return result;
}

QString 
ExpressionTag::getUsageDescription() const
{
    QString result = "ExpressionTag size: " + QString::number(getObjSize()) + " bytes\n\n";
    return result;
}



