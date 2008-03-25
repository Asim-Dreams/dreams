// ==================================================
// Copyright (C) 2005-2006 Intel Corporation
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 

/**
 * @file  RuleEngine.h
 */

#include "DreamsDefs.h"

#ifndef _RULE_ENGINE_H_
#define _RULE_ENGINE_H_

#include <qptrlist.h>

// Asim
#include "dDB/DralDBSyntax.h"

// Dreams
#include "expressions/Expression.h"

class LayoutFloorplanNode;
class LayoutFloorplanRectangle;

/**
 * Class that implements all the Rule priority policies and all the
 * evaluation methods for Resource and Floorplan. It implements the
 * addition methods to all the visualization elements as the evaluation
 * methods.
 */
class RuleEngine 
{
  public:


    /**
     * Method to add a list of rules to a leaf in the layout description
     */
    static void addDefaultRules(ExpressionList &rules, LayoutFloorplanRectangle *rect, INT32 level);
    static void addOverrideRules(ExpressionList &rules, LayoutFloorplanRectangle *rect, INT32 level);

    /**
     * Method to add a list of rules to the tree of layout elements described by the 
     * node
     */
    static void addDefaultRules(ExpressionList &rules, LayoutFloorplanNode *node, INT32 level);
    static void addOverrideRules(ExpressionList &rules, LayoutFloorplanNode *rect, INT32 level);

    /**
     * Method to evaluate the color, shape and letter given the rule list and the tag
     * values vector.
     */
    static void eval(const ExpressionList * rules, RuleEvalEnv * env);

    /**
     * Method to evaluate the color, shape and letter given the rule list and the tag
     * values vector. Sets the description of the rules that set a tag.
     */
    static void eval(const ExpressionList * rules, RuleEvalEnv * env, RuleEvalEnvDesc * desc);
};

#endif /* _RULE_ENGINE_H_ */
