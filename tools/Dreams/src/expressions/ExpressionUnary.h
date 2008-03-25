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
  * @file  ExpressionUnary.h
  * @brief
  */

#ifndef _EXPRESSION_UNARY_H_
#define _EXPRESSION_UNARY_H_

// Dreams includes.
#include "Expression.h"
#include "CSLData.h"

/**
 * Expression responsible of executing the comparison of 2 value expressions. The
 * result of this evaluation is a boolean
 */
class ExpressionUnary : public Expression
{
  private:

    /**
     * Binary Comparison Operator
     */
    ExpressionOperatorType  op;

    /** 
     * Operand of the expression. 
     */
    Expression *left;

  public:

    /**
     * Constructor
     */
    ExpressionUnary(ExpressionOperatorType _op, Expression *_left);

    /**
     * Destructor
     */
    ~ExpressionUnary();

    /** 
     * Returns the bitmask corresponding to the properties that may be
     * modified by this expression
     */
    UINT16 evalComputeMask();

    /**
     * Returns the list of Tags used by the current expression whenever
     * "eval" is invoked.
     */
    void getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags);

    /**
     * Perform type checking on the Expression and its descendents
     */
    bool typeCheck(QString & error);

    /** 
     * Evaluation method of itag statement. 
     */
    bool eval(RuleEvalEnv * env);

    /** 
     * Evaluation method of itag statement. 
     */
    bool eval(RuleEvalEnv * env, RuleEvalEnvDesc * desc, QString * pre, QString * post, INT32 indent);

    /**
     * Return String representing this expression
     */
    QString toString(QString indent);

    /**
     * Inherited from AMEMObj: return object size
     */
    INT64 getObjSize() const;

    /**
     * Inherited from AMEMObj: return textual explanation of size used by
     * this object
     */
    QString getUsageDescription() const;
};

#endif // _EXPRESSION_UNARY_H_
