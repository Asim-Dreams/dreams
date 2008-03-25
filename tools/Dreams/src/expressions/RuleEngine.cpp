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

/*
 * @file RuleEngine.cpp
 */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "RuleEngine.h"
#include "layout/floorplan/LayoutFloorplanNode.h"
#include "layout/floorplan/LayoutFloorplanRectangle.h"
#include "CSLData.h"

void 
RuleEngine::addDefaultRules(ExpressionList &rules, LayoutFloorplanNode *node, INT32 level)
{
    Q_ASSERT(node);
    Q_ASSERT(node->pqlistSon);

    QPtrListIterator<LayoutFloorplanNode> it_node(*(node->pqlistSon));
    LayoutFloorplanNode * nodeFloorplan;

    while((nodeFloorplan = it_node.current()) != NULL)
    {
        ++it_node;
        nodeFloorplan->addDefaultRules(rules, level);
    }
}

void 
RuleEngine::addOverrideRules(ExpressionList &rules, LayoutFloorplanNode *node, INT32 level)
{
    Q_ASSERT(node);
    Q_ASSERT(node->pqlistSon);

    QPtrListIterator<LayoutFloorplanNode> it_node(*(node->pqlistSon));
    LayoutFloorplanNode * nodeFloorplan;
    while((nodeFloorplan = it_node.current()) != NULL)
    {
        ++it_node;
        nodeFloorplan->addOverrideRules(rules, level);
    }
}

/**
 *
 * Given the next layout definition the order of application of the defined rules will be the 
 * described by the number in parenthesis
 *
 *  <table>
 *      <default> ... </default>            (1) | Level 1
 *      <override> ... </override>          (5) |
 *      <tr>
 *          <default> ... </default>        (2) | Level 2
 *          <override> ... </override>      (4) |
 *          <td>
 *          <rectangle>
 *              <if> ... </if>              (3) | Level 3
 *          </td>
 *      </tr>
 *
 * The list of rules that are appicable to a rectangle will follow the next scheme:
 *
 *  list     (1) (2) (3) (4) (5)
 *  level     1   2   3   2   1
 *
 *  The 2 next methods add the rule to the list in the Node in order to maintain the
 *  the previously definition
 */
void 
RuleEngine::addDefaultRules(ExpressionList &rules, LayoutFloorplanRectangle *rect, INT32 level)
{
    Q_ASSERT(rect);

    //cerr << "RuleEngine::addDefaultRules: Rectangle: name = " << rect->getNodeName() << endl;

    ExpressionIterator iterExp(rules);
    iterExp.toLast();
    while(iterExp.current())
    {
        rect->qlistRules.prepend(iterExp.current());
        --iterExp;
    }
}

void 
RuleEngine::addOverrideRules(ExpressionList &rules, LayoutFloorplanRectangle *rect, INT32 level)
{
    Q_ASSERT(rect);

    ExpressionIterator iterExp(rules);
    iterExp.toLast();
    while(iterExp.current())
    {
        rect->qlistRules.append(iterExp.current());
        --iterExp;
    }
}

void
RuleEngine::eval(const ExpressionList * rules, RuleEvalEnv * env)
{
    ExpressionIterator it_rule(* rules);
    Expression * rule;
    UINT32 i = 0;

    it_rule.toLast();

    // Evaluates all the expressions of the given list in highest to lowest priority.
    while((rule = it_rule.current()) != NULL)
    {
        i++;
        --it_rule;

        // Skips the evaluation of the expression when not needed.
        // Compute mask of CSL result has to 1 all the fields that aren't computed
        // yet, and compute mask of an expression has to 1 all the fields that might
        // change when evaluated. If we perform a logical and over this two components
        // and we obtain a result different than zero, then we must evaluate the
        // expression because the CSL result might be changed. Otherwise the CSL result
        // won't be changed, so we can skip its evaluation to speed up the whole
        // process.
        if(env->cslResult.getComputeMask() & rule->getComputeMask())
        {
            rule->eval(env);
        }
    }
}

void
RuleEngine::eval(const ExpressionList * rules, RuleEvalEnv * env, RuleEvalEnvDesc * desc)
{
    ExpressionIterator it_rule(* rules);
    Expression * rule;
    UINT32 i = 0;

    it_rule.toLast();

    // Evaluates all the expressions of the given list in highest to lowest priority.
    while((rule = it_rule.current()) != NULL)
    {
        i++;
        --it_rule;

        if(env->cslResult.getComputeMask() & rule->getComputeMask())
        {
            QString pre, post;

            rule->eval(env, desc, &pre, &post, 1);
        }
    }
}
