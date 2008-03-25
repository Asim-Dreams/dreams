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
  * @file  ExpressionBinaryString.cpp
  * @brief
  */

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "ExpressionBinaryString.h"
#include "qregexp.h"

ExpressionBinaryString::ExpressionBinaryString(DralDB * _draldb, ExpressionOperatorType _op, Expression *_left, Expression *_right) : Expression(ExpressionIntegerValue), op(_op), left(_left), right(_right)
{
    Q_ASSERT(_left);
    Q_ASSERT(_right);
    draldb = _draldb;
}

ExpressionBinaryString::~ExpressionBinaryString()
{
    delete left;
    delete right;
}

bool
ExpressionBinaryString::typeCheck(QString & error)
{
    bool ok = left->typeCheck(error) && right->typeCheck(error);

    if(ok)
    {
        // Checks if the expressions compared are of the same type.
        if(left->getType() != right->getType())
        {
            ok = false;
            error = QString("The operands of an comparison expression are not from the same type.");
        }
    }
    return ok;
}

void
ExpressionBinaryString::getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags)
{
    left->getUsedTags(slotTags, itemTags, nodeTags, cycleTags);
    right->getUsedTags(slotTags, itemTags, nodeTags, cycleTags);
}

UINT16
ExpressionBinaryString::evalComputeMask()
{
    computeMask = left->evalComputeMask() | right->evalComputeMask();
    return computeMask;
}

bool
ExpressionBinaryString::eval(RuleEvalEnv *env)
{
    bool ok;

    EXPDEBUG(1) { cerr << "ExpressionBinaryString::eval(trackId=" << env->trackId << ",cycle=" << env->cycle << ") on " << toString("") << endl; }
    EXPDEBUG(1) { cerr << "ExpressionBinaryString::eval: cslResult             = " << env->cslResult.toString() ; }

    //
    // Evaluate LHS. If "undefined", return immediately
    //
    if ( left->eval(env) == false ) return false;

    //
    // Evaluate RHS. If "undefined", return immediately
    //
    if ( right->eval(env) == false ) return false;

    //
    // Now perform Operation
    //
    switch ( op ) {
      case OperatorEqual:         setValue((left->getStringValue() == right->getStringValue())); break;
      case OperatorNotEqual:      setValue((left->getStringValue() != right->getStringValue())); break;
      case OperatorRegExpMatch:  
                              {
                                  QRegExp rx( draldb->getString(right->getStringValue()) );
                                  setValue((rx.search(draldb->getString(left->getStringValue())) != -1)); 
                              }
                              break;
      default:                      Q_ASSERT(0); break;
    }
    EXPDEBUG(1) { cerr << "ExpressionBinaryString::eval: String : (" << draldb->getString(left->getStringValue()) << " " << ExpressionToolkit::toString(op) << " " << draldb->getString(right->getStringValue()) << ") = " << getValue() << endl; }
     
    return true;
}

bool
ExpressionBinaryString::eval(RuleEvalEnv *env, RuleEvalEnvDesc * desc, QString * pre, QString * post, INT32 indent)
{
    bool ok;
    bool ret = true;

    EXPDEBUG(1) { cerr << "ExpressionBinaryString::eval(trackId=" << env->trackId << ",cycle=" << env->cycle << ") on " << toString("") << endl; }
    EXPDEBUG(1) { cerr << "ExpressionBinaryString::eval: cslResult             = " << env->cslResult.toString() ; }

    QString temp1;
    QString temp2;
    QString strOp;

    //
    // Evaluate LHS. If "undefined", return immediately
    //
    if(left->eval(env, desc, &temp1, NULL, 0) == false)
    {
        ret = false;
    }

    //
    // Evaluate RHS. If "undefined", return immediately
    //
    if(right->eval(env, desc, &temp2, NULL, 0) == false)
    {
        ret = false;
    }

    //
    // Now perform Operation
    //
    switch(op)
    {
      case OperatorEqual:
          setValue((left->getStringValue() == right->getStringValue()));
          strOp = " == ";
          break;

      case OperatorNotEqual:
          setValue((left->getStringValue() != right->getStringValue()));
          strOp = " != ";
          break;

      case OperatorRegExpMatch:
          {
          QRegExp rx( draldb->getString(right->getStringValue()) );
          setValue((rx.search(draldb->getString(left->getStringValue())) != -1)); 
          strOp = " =~ ";
          }
          break;

      default:
          Q_ASSERT(0);
          break;
    }

    * pre += temp1 + strOp + temp2; 

    EXPDEBUG(1) { cerr << "ExpressionBinaryString::eval: String : (" << draldb->getString(left->getStringValue()) << " " << ExpressionToolkit::toString(op) << " " << draldb->getString(right->getStringValue()) << ") = " << getValue() << endl; }
     
    return ret;
}

QString
ExpressionBinaryString::toString(QString indent)
{
    return (indent + left->toString("") + " " + ExpressionToolkit::toString(op) + " " + right->toString(""));
}

INT64 
ExpressionBinaryString::getObjSize() const
{
    INT64 result = sizeof(*this);
    result += left->getObjSize();
    result += right->getObjSize();
    return result;
}

QString 
ExpressionBinaryString::getUsageDescription() const
{
    QString result = "ExpressionBinaryString size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n\n";
    result += "\n\nDetailed memory usage for each component:\n";
    result += left->getUsageDescription();
    result += right->getUsageDescription();

    return result;
}

