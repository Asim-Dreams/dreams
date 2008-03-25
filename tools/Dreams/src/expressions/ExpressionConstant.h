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
  * @file  ExpressionConstant.h
  * @brief
  */

#ifndef _EXPRESSION_CONSTANT_H_
#define _EXPRESSION_CONSTANT_H_

// Dreams includes.
#include "DreamsDefs.h"
#include "Expression.h"
#include "CSLData.h"
#include "dDB/DralDB.h"

/**
 * Expression that evaluates constants for the rule language. As defined
 * in Expression.h we currently have 5 basi types of literals: values, fp
 * values, letters, shapes and strings.
 */
class ExpressionConstant : public Expression
{
  private:

    /**
     * Needed to convert integers to strings.
     */
    DralDB * draldb;

  public:

    /**
     * Constructors for the basic five data types
     */
    ExpressionConstant(UINT64      val) : Expression(ExpressionIntegerValue) { setValue(val);       }
    ExpressionConstant(double      val) : Expression(ExpressionFPValue)      { setFPValue(val); }
    ExpressionConstant(char        val) : Expression(ExpressionLetterValue)  { setLetterValue(val); }
    ExpressionConstant(EventShape  val) : Expression(ExpressionShapeValue)   { setShapeValue(val); }
    ExpressionConstant(DralDB * _draldb, QString val)
        : Expression(ExpressionStringValue)
    {
        draldb = _draldb;
        setStringValue(draldb->addString(val));
    }

    ExpressionConstant(DralDB * _draldb, char * val)
        : Expression(ExpressionStringValue)
    {
        draldb = _draldb;
        setStringValue(draldb->addString(QString(val)));
    }

    /** 
     * Returns the bitmask corresponding to the properties that may be
     * modified by this expression
     */
    inline UINT16 evalComputeMask();

    /**
     * Perform type checking on the Expression and its descendents
     */
    inline bool typeCheck(QString & error);

    /** 
     * Evaluation method of Shape expression. 
     */
    inline bool eval(RuleEvalEnv * env);

    /** 
     * Evaluation method of itag statement. 
     */
    inline bool eval(RuleEvalEnv * env, RuleEvalEnvDesc * desc, QString * pre, QString * post, INT32 indent);

    inline void getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags);

    /**
     * Return String for the expression
     */
    inline QString toString(QString indent);

    /**
     * Inherited from AMEMObj: return object size
     */
    inline INT64 getObjSize() const;

    /**
     * Inherited from AMEMObj: return textual explanation of size used by
     * this object
     */
    inline QString getUsageDescription() const;
};

bool 
ExpressionConstant::eval(RuleEvalEnv * env)
{
    return true;
}

bool 
ExpressionConstant::eval(RuleEvalEnv * env, RuleEvalEnvDesc * desc, QString * pre, QString * post, INT32 indent)
{
    switch(type)
    {
        case ExpressionIntegerValue:
            * pre += QString::number(getValue());
            break;

        case ExpressionStringValue:
            * pre += draldb->getString(getStringValue());
            break;

        case ExpressionFPValue:
            * pre += QString::number(getFPValue());
            break;

        case ExpressionLetterValue:
            * pre += QChar(getLetterValue());
            break;

        case ExpressionShapeValue:
            * pre += EventShapeToolkit::toString(getShapeValue());
            break;

        default:
            break;
    }
    return true;
}

UINT16
ExpressionConstant::evalComputeMask()
{
    computeMask = 0;
    return computeMask;
}

bool
ExpressionConstant::typeCheck(QString & error)
{
    return true;
}

QString
ExpressionConstant::toString(QString indent)
{
    switch ( type ) {
        case ExpressionStringValue:  return  indent + "CONST_STR("   + draldb->getString(getStringValue())            + ")";
        case ExpressionIntegerValue: return  indent + "CONST_INT("   + QString::number(getValue()) + ", 0x" + QString::number(getValue(), 16) + ")";
        case ExpressionFPValue:      return  indent + "CONST_FP("    + QString::number(getFPValue()) + ")";
        case ExpressionLetterValue:  return  indent + "CONST_CHAR("  + getLetterValue() + ")";
        case ExpressionShapeValue:   return  indent + "CONST_SHAPE(" + EventShapeToolkit::toString(getShapeValue())   + ")";
        default:                     return  indent + "CONST_ERROR(" + ((UINT32)type) + ")";
    }
}

INT64 
ExpressionConstant::getObjSize() const
{
    return sizeof(*this);
}

QString 
ExpressionConstant::getUsageDescription() const
{
    return "ExpressionConstant size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n";
}

void
ExpressionConstant::getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags)
{
}

#endif // _EXPRESSION_CONSTANT_H_
