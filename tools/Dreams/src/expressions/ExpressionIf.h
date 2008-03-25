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
  * @file  ExpressionIf.h
  * @brief
  */

#ifndef _EXPRESSION_IF_H_
#define _EXPRESSION_IF_H_

// Dreams includes.
#include "DreamsDefs.h"
#include "Expression.h"
#include "CSLData.h"

/**
 * If class for all the rule evaluation mechanism. This class is resposible
 * of implementing the IF statement. It will hold a conditiona a true branch and
 * false on (if it's defined). The value returned by the IF statement will be
 * the result of the condition evaluation.
 *
 *                     If
 *                  /  |   \
 *              cond  true  false
 */
class ExpressionIf : public Expression
{

    /** 
     * Pointer to the Condition expression used to choose which way the IF
     * evaluates.
     */
    Expression     *op1;

    /** 
     * Pointer to a list of expressions forming the "THEN" block
     */
    ExpressionList *op2;

    /** 
     * Pointer to a list of expressions forming the "ELSE" block
     */
    ExpressionList *op3;

  public:

    ExpressionIf(Expression *cond, ExpressionList *trueBlock, ExpressionList *falseBlock);
    ~ExpressionIf();

    /** 
     * Returns the bitmask corresponding to the properties that may be
     * modified by this expression
     */
    UINT16 evalComputeMask();

    /**
     * Returns the list of Tags used by the current expression whenever
     * "eval" is invoked.
     */
    inline void getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags);

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






#endif // _EXPRESSION_IF_H_
