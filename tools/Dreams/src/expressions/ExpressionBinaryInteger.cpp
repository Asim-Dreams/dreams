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
  * @file  ExpressionBinaryInteger.cpp
  * @brief
  */

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "ExpressionBinaryInteger.h"
#include "qregexp.h"

ExpressionBinaryInteger::ExpressionBinaryInteger(ExpressionOperatorType _op, Expression *_left, Expression *_right) : Expression(ExpressionIntegerValue), op(_op), left(_left), right(_right)
{
    Q_ASSERT(_left);
    Q_ASSERT(_right);
}

ExpressionBinaryInteger::~ExpressionBinaryInteger()
{
    delete left;
    delete right;
}

bool
ExpressionBinaryInteger::typeCheck(QString & error)
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
ExpressionBinaryInteger::getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags)
{
    left->getUsedTags(slotTags, itemTags, nodeTags, cycleTags);
    right->getUsedTags(slotTags, itemTags, nodeTags, cycleTags);
}

UINT16
ExpressionBinaryInteger::evalComputeMask()
{
    computeMask = left->evalComputeMask() | right->evalComputeMask();
    return computeMask;
}

bool
ExpressionBinaryInteger::eval(RuleEvalEnv *env)
{
    bool ok;

    EXPDEBUG(1) { cerr << "ExpressionBinaryInteger::eval(trackId=" << env->trackId << ",cycle=" << env->cycle << ") on " << toString("") << endl; }
    EXPDEBUG(1) { cerr << "ExpressionBinaryInteger::eval: cslResult             = " << env->cslResult.toString() ; }

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
      case OperatorEqual:           setValue((left->getValue()  == right->getValue()));  break;
      case OperatorNotEqual:        setValue((left->getValue()  != right->getValue()));  break;
      case OperatorLowerThan:       setValue((left->getValue()   < right->getValue()));  break;
      case OperatorLowerEqual:      setValue((left->getValue()  <= right->getValue()));  break;
      case OperatorGreaterThan:     setValue((left->getValue()   > right->getValue()));  break;
      case OperatorGreaterEqual:    setValue((left->getValue()  >= right->getValue()));  break;
      case OperatorLogicalAnd:      setValue((left->getValue()  && right->getValue()));  break;
      case OperatorLogicalOr:       setValue((left->getValue()  || right->getValue()));  break;
      case OperatorBitwiseAnd:      setValue((left->getValue()   & right->getValue()));  break;
      case OperatorBitwiseOr:       setValue((left->getValue()   | right->getValue()));  break;
      case OperatorBitwiseXor:      setValue((left->getValue()   ^ right->getValue()));  break;
      case OperatorShiftLeft:       setValue((left->getValue()  >> right->getValue()));  break;
      case OperatorShiftRight:      setValue((left->getValue()  << right->getValue()));  break;
      case OperatorAdd:             setValue((left->getValue()   + right->getValue()));  break;
      case OperatorSub:             setValue((left->getValue()   - right->getValue()));  break;
      case OperatorMul:             setValue((left->getValue()   * right->getValue()));  break;
      case OperatorDiv:             setValue((left->getValue()   / right->getValue()));  break;
      case OperatorMod:             setValue((left->getValue()   % right->getValue()));  break;
      default:                      Q_ASSERT(0); break;
    }
    EXPDEBUG(1) { cerr << "ExpressionBinaryInteger::eval: Integer: (" << left->getValue() << " " << ExpressionToolkit::toString(op) << " " << right->getValue() << ") = " << getValue() << endl; }
    EXPDEBUG(1) { cerr << "ExpressionBinaryInteger::eval: Hex    : (0x" << hex << left->getValue() << " " << ExpressionToolkit::toString(op) << " 0x" << right->getValue() << ") = " << getValue() << dec << endl; }
     
    return true;
}

bool
ExpressionBinaryInteger::eval(RuleEvalEnv * env, RuleEvalEnvDesc * desc, QString * pre, QString * post, INT32 indent)
{
    bool ok;
    bool ret = true;

    EXPDEBUG(1) { cerr << "ExpressionBinaryInteger::eval(trackId=" << env->trackId << ",cycle=" << env->cycle << ") on " << toString("") << endl; }
    EXPDEBUG(1) { cerr << "ExpressionBinaryInteger::eval: cslResult             = " << env->cslResult.toString() ; }

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
    switch(op) {
      case OperatorEqual:
          setValue((left->getValue()  == right->getValue()));
          strOp = " == ";
          break;

      case OperatorNotEqual:
          setValue((left->getValue()  != right->getValue()));
          strOp = " != ";
          break;

      case OperatorLowerThan:
          setValue((left->getValue()   < right->getValue()));
          strOp = " < ";
          break;

      case OperatorLowerEqual:
          setValue((left->getValue()  <= right->getValue()));
          strOp = " <= ";
          break;

      case OperatorGreaterThan:
          setValue((left->getValue()   > right->getValue()));
          strOp = " > ";
          break;

      case OperatorGreaterEqual:
          setValue((left->getValue()  >= right->getValue()));
          strOp = " >= ";
          break;

      case OperatorLogicalAnd:
          setValue((left->getValue()  && right->getValue()));
          strOp = " && ";
          break;

      case OperatorLogicalOr:
          setValue((left->getValue()  || right->getValue()));
          strOp = " || ";
          break;

      case OperatorBitwiseAnd:
          setValue((left->getValue()   & right->getValue()));
          strOp = " & ";
          break;

      case OperatorBitwiseOr:
          setValue((left->getValue()   | right->getValue()));
          strOp = " | ";
          break;

      case OperatorBitwiseXor:
          setValue((left->getValue()   ^ right->getValue()));
          strOp = " ^ ";
          break;

      case OperatorShiftLeft:
          setValue((left->getValue()  >> right->getValue()));
          strOp = " >> ";
          break;

      case OperatorShiftRight:
          setValue((left->getValue()  << right->getValue()));
          strOp = " << ";
          break;

      case OperatorAdd:
          setValue((left->getValue()   + right->getValue()));
          strOp = " + ";
          break;

      case OperatorSub:
          setValue((left->getValue()   - right->getValue()));
          strOp = " - ";
          break;

      case OperatorMul:
          setValue((left->getValue()   * right->getValue()));
          strOp = " * ";
          break;

      case OperatorDiv:
          setValue((left->getValue()   / right->getValue()));
          strOp = " / ";
          break;

      case OperatorMod:
          setValue((left->getValue()   % right->getValue()));
          strOp = " % ";
          break;

      default:
          Q_ASSERT(0);
          break;
    }

    * pre += temp1 + strOp + temp2; 

    EXPDEBUG(1) { cerr << "ExpressionBinaryInteger::eval: Integer: (" << left->getValue() << " " << ExpressionToolkit::toString(op) << " " << right->getValue() << ") = " << getValue() << endl; }
    EXPDEBUG(1) { cerr << "ExpressionBinaryInteger::eval: Hex    : (0x" << hex << left->getValue() << " " << ExpressionToolkit::toString(op) << " 0x" << right->getValue() << ") = " << getValue() << dec << endl; }
     
    return ret;
}

QString
ExpressionBinaryInteger::toString(QString indent)
{
    return (indent + "(" + left->toString("") + " " + ExpressionToolkit::toString(op) + "<" + ExpressionToolkit::toString(type) + ">" + " " + right->toString("") + ")" );
}

INT64 
ExpressionBinaryInteger::getObjSize() const
{
    INT64 result = sizeof(*this);
    result += left->getObjSize();
    result += right->getObjSize();
    return result;
}

QString 
ExpressionBinaryInteger::getUsageDescription() const
{
    QString result = "ExpressionBinaryInteger size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n\n";
    result += "\n\nDetailed memory usage for each component:\n";
    result += left->getUsageDescription();
    result += right->getUsageDescription();

    return result;
}

