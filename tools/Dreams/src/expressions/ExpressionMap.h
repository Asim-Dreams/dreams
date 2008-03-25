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
  * @file  ExpressionMap.h
  * @brief
  */

#ifndef _EXPRESSION_MAP_H_
#define _EXPRESSION_MAP_H_

// Dreams includes.
#include "DreamsDefs.h"
#include "Expression.h"
#include "CSLData.h"
#include "adf/ADFMap.h"
#include "qstring.h"

/**
 * Look up class for shading and other evaluation mechanisms. This class is responsible
 * of implementing the evaluation of an expression and using its result to index a map.
 *
 *                   MAP
 *                  /   \
 *              expr    ADFMap
 */
class ExpressionMap : public Expression
{
  private:
    /** 
     * Operands of the expressions. 
     */
    Expression * op1; ///< Expression that returns the index.
    ADFMap * map;     ///< Map that holds the return values.
    UINT64 min;       ///< Minimum value of the look up.
    UINT64 max;       ///< Maximum value of the look up.

  public:

    /**
     * Constructor
     */
    ExpressionMap(Expression * _op1, ADFMap * _map, UINT64 _min, UINT64 _max);
    ~ExpressionMap();

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
     * Returns the list of Tags used by the current expression whenever
     * "eval" is invoked.
     */
    void getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags);

    /** 
     * Evaluation method of MAP indexing expression. 
     */
    inline bool eval(RuleEvalEnv * env);

    /** 
     * Evaluation method of itag statement. 
     */
    bool eval(RuleEvalEnv * env, RuleEvalEnvDesc * desc, QString * pre, QString * post, INT32 indent);

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

#endif // _EXPRESSION_MAP_H_
