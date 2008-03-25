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
  * @file  ExpressionBinaryFP.cpp
  * @brief
  */

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "ExpressionBinaryFP.h"
#include "qregexp.h"

ExpressionBinaryFP::ExpressionBinaryFP(ExpressionOperatorType _op, Expression *_left, Expression *_right) : Expression(ExpressionNoValue), op(_op), left(_left), right(_right)
{
    Q_ASSERT(_left);
    Q_ASSERT(_right);

    switch ( op ) {
      case OperatorEqual:
      case OperatorNotEqual:
      case OperatorLowerThan:
      case OperatorLowerEqual:
      case OperatorGreaterThan:
      case OperatorGreaterEqual:    type = ExpressionIntegerValue; break;
      case OperatorAdd:
      case OperatorSub:
      case OperatorMul:
      case OperatorDiv:             type = ExpressionFPValue; break;
      default:                      Q_ASSERT(0); break;
    }
}

ExpressionBinaryFP::~ExpressionBinaryFP()
{
    delete left;
    delete right;
}

bool
ExpressionBinaryFP::typeCheck(QString & error)
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
ExpressionBinaryFP::getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags)
{
    left->getUsedTags(slotTags, itemTags, nodeTags, cycleTags);
    right->getUsedTags(slotTags, itemTags, nodeTags, cycleTags);
}

UINT16
ExpressionBinaryFP::evalComputeMask()
{
    computeMask = left->evalComputeMask() | right->evalComputeMask();
    return computeMask;
}

bool
ExpressionBinaryFP::eval(RuleEvalEnv *env)
{
    bool ok;

    EXPDEBUG(1) { cerr << "ExpressionBinaryFP::eval(trackId=" << env->trackId << ",cycle=" << env->cycle << ") on " << toString("") << endl; }
    EXPDEBUG(1) { cerr << "ExpressionBinaryFP::eval: cslResult             = " << env->cslResult.toString() ; }

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
      case OperatorEqual:           setValue((left->getFPValue()  == right->getFPValue()));  break;
      case OperatorNotEqual:        setValue((left->getFPValue()  != right->getFPValue()));  break;
      case OperatorLowerThan:       setValue((left->getFPValue()  <  right->getFPValue()));  break;
      case OperatorLowerEqual:      setValue((left->getFPValue()  <= right->getFPValue()));  break;
      case OperatorGreaterThan:     setValue((left->getFPValue()  >  right->getFPValue()));  break;
      case OperatorGreaterEqual:    setValue((left->getFPValue()  >= right->getFPValue()));  break;
      case OperatorAdd:             setFPValue((left->getFPValue()   + right->getFPValue()));  break;
      case OperatorSub:             setFPValue((left->getFPValue()   - right->getFPValue()));  break;
      case OperatorMul:             setFPValue((left->getFPValue()   * right->getFPValue()));  break;
      case OperatorDiv:             setFPValue((left->getFPValue()   / right->getFPValue()));  break;
      default:                      Q_ASSERT(0); break;
    }
    EXPDEBUG(1) {
     if ( type == ExpressionIntegerValue ) cerr << "ExpressionBinaryFP::eval: Int: (" << left->getFPValue() << " " << ExpressionToolkit::toString(op) << " " << right->getFPValue() << ") = " << getValue() << endl; 
     else                                  cerr << "ExpressionBinaryFP::eval:  FP: (" << left->getFPValue() << " " << ExpressionToolkit::toString(op) << " " << right->getFPValue() << ") = " << getFPValue() << endl;
    }
     
    return true;
}

bool
ExpressionBinaryFP::eval(RuleEvalEnv * env, RuleEvalEnvDesc * desc, QString * pre, QString * post, INT32 indent)
{
    bool ok;
    bool ret = true;

    EXPDEBUG(1) { cerr << "ExpressionBinaryFP::eval(trackId=" << env->trackId << ",cycle=" << env->cycle << ") on " << toString("") << endl; }
    EXPDEBUG(1) { cerr << "ExpressionBinaryFP::eval: cslResult             = " << env->cslResult.toString() ; }

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
          setValue((left->getFPValue() == right->getFPValue()));
          strOp = " == ";
          break;

      case OperatorNotEqual:
          setValue((left->getFPValue()  != right->getFPValue()));
          strOp = " != ";
          break;

      case OperatorLowerThan:
          setValue((left->getFPValue()  <  right->getFPValue()));
          strOp = " < ";
          break;

      case OperatorLowerEqual:
          setValue((left->getFPValue()  <= right->getFPValue()));
          strOp = " <= ";
          break;

      case OperatorGreaterThan:
          setValue((left->getFPValue()  >  right->getFPValue()));
          strOp = " > ";
          break;

      case OperatorGreaterEqual:
          setValue((left->getFPValue()  >= right->getFPValue()));
          strOp = " >= ";
          break;

      case OperatorAdd:
          setFPValue((left->getFPValue()   + right->getFPValue()));
          strOp = " + ";
          break;

      case OperatorSub:
          setFPValue((left->getFPValue()   - right->getFPValue()));
          strOp = " - ";
          break;

      case OperatorMul:
          setFPValue((left->getFPValue()   * right->getFPValue()));
          strOp = " * ";
          break;

      case OperatorDiv:
          setFPValue((left->getFPValue()   / right->getFPValue()));
          strOp = " / ";
          break;

      default:
          Q_ASSERT(0);
          break;
    }

    * pre += temp1 + strOp + temp2; 

    EXPDEBUG(1) {
     if ( type == ExpressionIntegerValue ) cerr << "ExpressionBinaryFP::eval: Int: (" << left->getFPValue() << " " << ExpressionToolkit::toString(op) << " " << right->getFPValue() << ") = " << getValue() << endl; 
     else                                  cerr << "ExpressionBinaryFP::eval:  FP: (" << left->getFPValue() << " " << ExpressionToolkit::toString(op) << " " << right->getFPValue() << ") = " << getFPValue() << endl;
    }
     
    return ret;
}

QString
ExpressionBinaryFP::toString(QString indent)
{
    return (indent + "(" + left->toString("") + " " + ExpressionToolkit::toString(op) + "<" + ExpressionToolkit::toString(type) + ">" + " " + right->toString("") + ")" );
}

INT64 
ExpressionBinaryFP::getObjSize() const
{
    INT64 result = sizeof(*this);
    result += left->getObjSize();
    result += right->getObjSize();
    return result;
}

QString 
ExpressionBinaryFP::getUsageDescription() const
{
    QString result = "ExpressionBinaryFP size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n\n";
    result += "\n\nDetailed memory usage for each component:\n";
    result += left->getUsageDescription();
    result += right->getUsageDescription();

    return result;
}

