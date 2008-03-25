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
  * @file  ExpressionUnary.cpp
  * @brief
  */

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "ExpressionUnary.h"

ExpressionUnary::ExpressionUnary(ExpressionOperatorType _op, Expression *_left) : Expression(ExpressionNoValue), op(_op), left(_left)
{
    Q_ASSERT(_left);
    type = left->getType(); 

    if ( op == OperatorToDouble ) {
     Q_ASSERT (left->getType() == ExpressionIntegerValue);
     type = ExpressionFPValue;
    }
    else if ( op == OperatorToUINT64 ) {
     Q_ASSERT (left->getType() == ExpressionFPValue);
     type = ExpressionIntegerValue;
    }
}

ExpressionUnary::~ExpressionUnary()
{
    delete left;
}

bool
ExpressionUnary::typeCheck(QString & error)
{
    bool ok = left->typeCheck(error);
    return ok;
}

void
ExpressionUnary::getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags)
{
    left->getUsedTags(slotTags, itemTags, nodeTags, cycleTags);
}

UINT16
ExpressionUnary::evalComputeMask()
{
    computeMask = left->evalComputeMask();
    return computeMask;
}

bool
ExpressionUnary::eval(RuleEvalEnv *env)
{
    bool ok;

    EXPDEBUG(1) { cerr << "ExpressionUnary::eval(trackId=" << env->trackId << ",cycle=" << env->cycle << ") on " << toString("") << endl; }
    EXPDEBUG(1) { cerr << "ExpressionUnary::eval: cslResult             = " << env->cslResult.toString() ; }

    //
    // Evaluate LHS. If "undefined", return immediately
    //
    if ( left->eval(env) == false ) return false;

    //
    // Now perform Operation
    //
    switch ( op ) {
      case OperatorLogicalNot:        setValue(!(left->getValue()));  break;
      case OperatorBitwiseNot:        setValue(~(left->getValue()));  break;
      case OperatorUnaryMinus:        if ( type == ExpressionIntegerValue ) { setValue(- (left->getValue()));    break; }
                                      if ( type == ExpressionFPValue )      { setFPValue(- (left->getFPValue()));  break; }
                                      Q_ASSERT(0); break;
      case OperatorToDouble:          setFPValue((double) left->getValue()); break;
      case OperatorToUINT64:          setValue((UINT64) left->getFPValue()); break;
      default:                        Q_ASSERT(0); break;
    }
    EXPDEBUG(1) {
      if ( op == OperatorToDouble ) {
        cerr << "ExpressionUnary::int/fp: " << ExpressionToolkit::toString(op) << " ("        << left->getValue() << ") = " << getFPValue() << endl;
      }
      else if ( op == OperatorToUINT64 ) {
        cerr << "ExpressionUnary::int/fp: " << ExpressionToolkit::toString(op) << " ("        << left->getFPValue() << ") = " << getValue() << endl;
      }
      else if ( type == ExpressionIntegerValue ) {
        cerr << "ExpressionUnary::int: " << ExpressionToolkit::toString(op) << " ("           << left->getValue() << ") = " << getValue() << endl;
        cerr << "ExpressionUnary::hex: " << ExpressionToolkit::toString(op) << " ( 0x" << hex << left->getValue() << ") = " << getValue() << dec << endl; 
      }
      else {
        cerr << "ExpressionUnary::fp:  " << ExpressionToolkit::toString(op) << " ("           << left->getFPValue() << ") = " << getFPValue() << dec << endl; 
      }
    }
     
    return true;
}

bool
ExpressionUnary::eval(RuleEvalEnv *env, RuleEvalEnvDesc * desc, QString * pre, QString * post, INT32 indent)
{
    bool ok;

    EXPDEBUG(1) { cerr << "ExpressionUnary::eval(trackId=" << env->trackId << ",cycle=" << env->cycle << ") on " << toString("") << endl; }
    EXPDEBUG(1) { cerr << "ExpressionUnary::eval: cslResult             = " << env->cslResult.toString() ; }

    QString temp;

    //
    // Evaluate LHS. If "undefined", return immediately
    //
    if(left->eval(env, desc, &temp, NULL, 0) == false)
    {
        return false;
    }

    //
    // Now perform Operation
    //
    switch(op)
    {
        case OperatorLogicalNot:
            setValue(!(left->getValue()));
            * pre += "!" + temp;
            break;

        case OperatorBitwiseNot:
            setValue(~(left->getValue()));
            * pre += "~" + temp;
            break;

        case OperatorUnaryMinus:
            * pre += "-" + temp;
            if(type == ExpressionIntegerValue)
            {
                setValue(- (left->getValue()));
                break;
            }
            if(type == ExpressionFPValue)
            {
                setFPValue(- (left->getFPValue()));
                break;
            }

            Q_ASSERT(0);
            break;

        case OperatorToDouble:
            * pre += "fp64(" + temp + ")";
            setFPValue((double) left->getValue());
            break;

        case OperatorToUINT64:
            * pre += "uint64(" + temp + ")";
            setValue((UINT64) left->getFPValue());
            break;

        default:
            Q_ASSERT(0);
            break;
    }
    EXPDEBUG(1) {
      if ( op == OperatorToDouble ) {
        cerr << "ExpressionUnary::int/fp: " << ExpressionToolkit::toString(op) << " ("        << left->getValue() << ") = " << getFPValue() << endl;
      }
      else if ( op == OperatorToUINT64 ) {
        cerr << "ExpressionUnary::int/fp: " << ExpressionToolkit::toString(op) << " ("        << left->getFPValue() << ") = " << getValue() << endl;
      }
      else if ( type == ExpressionIntegerValue ) {
        cerr << "ExpressionUnary::int: " << ExpressionToolkit::toString(op) << " ("           << left->getValue() << ") = " << getValue() << endl;
        cerr << "ExpressionUnary::hex: " << ExpressionToolkit::toString(op) << " ( 0x" << hex << left->getValue() << ") = " << getValue() << dec << endl; 
      }
      else {
        cerr << "ExpressionUnary::fp:  " << ExpressionToolkit::toString(op) << " ("           << left->getFPValue() << ") = " << getFPValue() << dec << endl; 
      }
    }
     
    return true;
}

QString
ExpressionUnary::toString(QString indent)
{
    return (indent + ExpressionToolkit::toString(op) + "(" + left->toString("") + ")" );
}

INT64 
ExpressionUnary::getObjSize() const
{
    INT64 result = sizeof(*this);
    result += left->getObjSize();
    return result;
}

QString 
ExpressionUnary::getUsageDescription() const
{
    QString result = "ExpressionUnary size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n\n";
    result += "\n\nDetailed memory usage for each component:\n";
    result += left->getUsageDescription();

    return result;
}

