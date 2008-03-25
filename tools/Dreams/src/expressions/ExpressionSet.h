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
  * @file  ExpressionSet.h
  * @brief
  */

#ifndef _EXPRESSION_SET_H_
#define _EXPRESSION_SET_H_

#include <stdio.h>

// QT includes.
#include <qstring.h>
#include <qcolor.h>

// Dreams includes.
#include "Expression.h"
#include "CSLData.h"
#include "dDB/DralDB.h"

/**
 * Expression that evaluates a "SET" statement. "SET" staments are used to
 * defined the properties of a dreams cell on screen (fillcolor, bordercolor, letter, shape, etc..).
 */
class ExpressionSet : public Expression
{
  private:

    /**
     * Needed to convert integers to strings.
     */
    DralDB * draldb;

    /** 
     * Operator for this Set statement:
     */
    ExpressionOperatorType op;

    /** 
     * Holds pointer to expression defining the value (the RHS) for this Set statement
     */
    Expression *rhs; 

    /** 
     * For operations setting color, this boolean indicates the * pseudo-color called "transparent"
     */
    bool isTransparent;

    /** 
     * For operations setting color, indicates the fading period
     */
    UINT8 fade;

  public:

    /** 
     * Constructor
     */
    ExpressionSet(DralDB * _draldb, ExpressionOperatorType _op, Expression *_rhs, UINT8 _fade);
    ~ExpressionSet();

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
    bool eval(RuleEvalEnv *env);

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

#endif // _EXPRESSION_SET_H_
