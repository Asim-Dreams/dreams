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

/**
  * @file  Expression.cpp
  * @brief

  */

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "Expression.h"

typedef struct {
    QString str;
    UINT16  msk;
    bool    AcceptsIntegerOperands;
    bool    AcceptsStringOperands;
    bool    AcceptsFPOperands;
} OperatorTable;

static OperatorTable opTable[OperatorLast] = { 
            { "Error",        0,                     false, false, false },
            { "FColor",       CSL_FILL_COLOR_MASK,   false, false, false },
            { "BColor",       CSL_BORDER_COLOR_MASK, false, false, false },
            { "LColor",       CSL_LETTER_COLOR_MASK, false, false, false },
            { "Letter",       CSL_LETTER_MASK,       false, false, false },
            { "Shape",        CSL_SHAPE_MASK,        false, false, false },
            { "Font",         CSL_FONT_MASK,         false, false, false },
            { "Italic",       CSL_ITALIC_MASK,       false, false, false },
            { "Underline",    CSL_UNDERLINE_MASK,    false, false, false },
            { "Bold",         CSL_BOLD_MASK,         false, false, false },
            { "Equal",        0,                      true,  true,  true },
            { "NotEqual",     0,                      true,  true,  true },
            { "LowerThan",    0,                      true, false,  true },
            { "LowerEqual",   0,                      true, false,  true },
            { "GreaterThan",  0,                      true, false,  true },
            { "GreaterEqual", 0,                      true, false,  true },
            { "RegExpMatch",  0,                     false,  true, false },
            { "LogicalAnd",   0,                      true, false, false },
            { "LogicalOr",    0,                      true, false, false },
            { "LogicalNot",   0  ,                    true, false, false },
            { "BitwiseAnd",   0,                      true, false, false },
            { "BitwiseOr",    0,                      true, false, false },
            { "BitwiseXor",   0,                      true, false, false },
            { "BitwiseNot",   0,                      true, false, false },
            { "ShiftLeft",    0,                      true, false, false },
            { "ShiftRight",   0,                      true, false, false },
            { "Add",          0,                      true, false,  true },
            { "Sub",          0,                      true, false,  true },
            { "Mul",          0,                      true, false,  true },
            { "Div",          0,                      true, false,  true },
            { "Mod",          0,                      true, false, false },
            { "UnaryMinus",   0,                      true, false,  true },
            { "itag",         0,                     false,  true, false },
            { "stag",         0,                     false,  true, false },
            { "ctag",         0,                     false,  true, false },
            { "ntag",         0,                     false,  true, false },
            { "fp64",         0,                      true, false, false },
            { "uint64",       0,                     false, false,  true }
          };

QString
ExpressionToolkit::toString(ExpressionOperatorType op)
{
   Q_ASSERT(op < OperatorLast);
   return opTable[op].str;
}

UINT16
ExpressionToolkit::toMask(ExpressionOperatorType op)
{
   Q_ASSERT(op < OperatorLast);
   return opTable[op].msk;
}

bool
ExpressionToolkit::operatorMatch(ExpressionOperatorType op, ExpressionType ty)
{
   Q_ASSERT(op < OperatorLast);
   Q_ASSERT(ty < ExpressionTypeLast);

   if ( ty >= ExpressionLetterValue ) return false;

   if ( ty == ExpressionIntegerValue ) return opTable[op].AcceptsIntegerOperands;
   if ( ty == ExpressionStringValue )  return opTable[op].AcceptsStringOperands;
   if ( ty == ExpressionFPValue )      return opTable[op].AcceptsFPOperands;

   return false;
}


static QString tyTable[ExpressionTypeLast] = { 
            "NoType",
            "Integer",
            "String",
            "FP",
            "Letter",
            "Shape",
          };

QString
ExpressionToolkit::toString(ExpressionType ty)
{
   Q_ASSERT(ty < ExpressionTypeLast);
   return tyTable[ty];
}
    
/**
 * Maps a TagValueType to an ExpressionType. 
 */
ExpressionType 
ExpressionToolkit::toExpressionType(TagValueType t)
{
    switch (  t ) {
        case TAG_STRING_TYPE:  return ExpressionStringValue;
        case TAG_INTEGER_TYPE: return ExpressionIntegerValue;
        case TAG_FP_TYPE:      return ExpressionFPValue;
        default:              Q_ASSERT(0);
    }
    return ExpressionNoValue;
}

//////////////////////////////////////////////////////////////////////
////////
////////
//////// Expression List
////////
////////
//////////////////////////////////////////////////////////////////////

/**
 * Append an ExpressionList to the end of the list
 */
void 
ExpressionList::appendList(ExpressionList &elist)
{
     Expression         *expr;
     ExpressionIterator it(elist);
     while((expr = it.current()) != NULL)
     {
          QPtrList<Expression>::append(expr);
          ++it;
     }
}

/**
 * Return String representing all elements of the ExpressionList
 */
QString
ExpressionList::toString(QString indent)
{
     QString            res;
     Expression         *expr;
     ExpressionIterator it(*this);
     while((expr = it.current()) != NULL)
     {
          res += expr->toString(indent);
          ++it;
     }
     return res;
}

/** 
 * Returns the bitmask corresponding to the properties that may be
 * modified by this ExpressionList
 */
UINT16
ExpressionList::evalComputeMask()
{
     UINT16             res = 0; 
     Expression         *expr;
     ExpressionIterator it(*this);

     while((expr = it.current()) != NULL)
     {
          res = res | expr->evalComputeMask();
          ++it;
     }
     return res;
}

/**
 * Returns the list of Tags used by the current expression whenever
 * "eval" is invoked.
 */
void
ExpressionList::getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags)
{
     Expression         *expr;
     ExpressionIterator it(*this);

     while((expr = it.current()) != NULL)
     {
          expr->getUsedTags(slotTags, itemTags, nodeTags, cycleTags);
          ++it;
     }
}

/**
 * Perform type checking on all elements of the ExpressionList
 */
bool
ExpressionList::typeCheck(QString &error)
{
     bool               ok = true; 
     Expression         *expr;
     QString            localError;
     ExpressionIterator it(*this);

     while( ok && ((expr = it.current()) != NULL) )
     {
          localError = "";
          ok =  ok && expr->typeCheck(error);
          ++it;
     }
     return ok;
}

INT64
ExpressionList::getObjSize() const
{
     INT64              res = sizeof(*this); 
     Expression         *expr;
     ExpressionIterator it(*this);

     while((expr = it.current()) != NULL)
     {
          res += expr->getObjSize();
          ++it;
     }
     return res;
}

QString
ExpressionList::getUsageDescription() const
{
     QString            res;
     Expression         *expr;
     ExpressionIterator it(*this);

     res = "ExpressionList size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n";
     while((expr = it.current()) != NULL)
     {
          res += expr->getUsageDescription();
          ++it;
     }
     return res;
}
