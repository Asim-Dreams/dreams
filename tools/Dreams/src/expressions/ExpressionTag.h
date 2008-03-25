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
  * @file  ExpressionTag.h
  * @brief
  */

#ifndef _EXPRESSION_TAG_H_
#define _EXPRESSION_TAG_H_

#include "DreamsDefs.h"
// Dreams includes.
#include "Expression.h"
#include "CSLData.h"
#include "dDB/DralDB.h"

/**
 * Expression that evaluates an itag in a given slot/cycle. 
 */
class ExpressionTag : public Expression
{
  private:

    /** 
     * Operator: STAG, ITAG, CTAG or NTAG
     */
    ExpressionOperatorType op;

    /** 
     * Tag id of the tag that will be read.
     */
    TAG_ID tagId;     

    /**
     * DralDB used to map the tag string name to its tag id. Also used
     * when RuleEnv contains a slot to access that slot info.
     */
    DralDB *draldb;  

    /**
     * Tag string for this itag function. Used exclusively for debugging &
     * dumping
     */
    QString myTagName; 

    /**
     * Used to store errors.
     */
    bool hasError;
    QString myError;

  public:

    /** 
     * Constructor
     */
    ExpressionTag(ExpressionOperatorType _op, DralDB *_dralDB, QString itagName);

    /** 
     * Destructor
     */
    ~ExpressionTag();

    /** 
     * Returns the bitmask corresponding to the properties that may be
     * modified by this expression
     */
    UINT16 evalComputeMask();

    /**
     * Perform type checking on the Expression and its descendents
     */
    bool typeCheck(QString & error);

    /**
     * Returns the list of Tags used by the current expression whenever
     * "eval" is invoked.
     */
    void getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags);

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

#endif // _EXPRESSION_TAG_H_
