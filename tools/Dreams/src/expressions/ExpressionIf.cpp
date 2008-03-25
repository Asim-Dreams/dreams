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

// General includes.
#include <iostream>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "ExpressionIf.h"

ExpressionIf::ExpressionIf(Expression *cond, ExpressionList *trueBlock, ExpressionList *falseBlock)
    : Expression(ExpressionIntegerValue), op1(cond), op2(trueBlock), op3(falseBlock) 
{
    Q_ASSERT(op1);
}

ExpressionIf::~ExpressionIf()
{
    delete op1;
    delete op2;
    delete op3;
}

bool
ExpressionIf::eval(RuleEvalEnv * env)
{
    EXPDEBUG(1) { cerr << "In ExpressionIf::eval trackid=" << env->trackId << " cycle=" << env->cycle << toString(""); }
    // Evaluates the condition expression.
    // Here we assume that a null result is equivalent to a false
    bool bOk = op1->eval(env);
    setValue(op1->getValue());

    EXPDEBUG(1) { cerr << "Condition evaluates to=" << bOk << " value=" << getValue() << endl; }
    // If it has evaluated to something and is different than 0 evaluates the true branch.
    if(bOk && getValue())
    {
        if(op2)
        {
            ExpressionIterator it_expr(* op2);
            Expression * expr;

            it_expr.toLast();

            // Executes all the true branch expressions.
            while((expr = it_expr.current()) != NULL)
            {
                --it_expr;
                EXPDEBUG(1) { cerr << "Expr    --> 0b" << QString::number(expr->getComputeMask(),2) << " expr=" << expr->toString("") << endl; }
                EXPDEBUG(1) { cerr << "CSL     --> 0b" << QString::number(env->cslResult.getComputeMask(),2) << " csl=" << env->cslResult.toString() << endl; }
                if(env->cslResult.getComputeMask() & expr->getComputeMask())
                {
                    expr->eval(env);
                    EXPDEBUG(1) { cerr << "CSL (new)-> 0b" << QString::number(env->cslResult.getComputeMask(),2) << " csl=" << env->cslResult.toString() << endl; }
                }
            }
        }
    }
    // Else we evaluate the false branch expressions (if present).
    else if(op3)
    {
        ExpressionIterator it_expr(* op3);
        Expression * expr;

        it_expr.toLast();

        // Executes all the false branch expressions.
        while((expr = it_expr.current()) != NULL)
        {
            --it_expr;
            if(env->cslResult.getComputeMask() & expr->getComputeMask())
            {
                expr->eval(env);
            }
        }
    }

    return bOk;
}

bool
ExpressionIf::eval(RuleEvalEnv * env, RuleEvalEnvDesc * desc, QString * pre, QString * post, INT32 indent)
{
    EXPDEBUG(1) { cerr << "In ExpressionIf::eval trackid=" << env->trackId << " cycle=" << env->cycle << toString(""); }
    // Evaluates the condition expression.
    // Here we assume that a null result is equivalent to a false
    QString strIndent = "";
    for(INT32 i = 0; i < indent; i++)
    {
        strIndent += "    ";
    }

    * pre += strIndent + "if(";
    QString temp = "";
    bool bOk = op1->eval(env, desc, pre, &temp, 0);
    setValue(op1->getValue());

    * pre += ")\n" + strIndent + "{\n";
    * post = strIndent + "}\n" + * post;

    EXPDEBUG(1) { cerr << "Condition evaluates to=" << bOk << " value=" << getValue() << endl; }
    // If it has evaluated to something and is different than 0 evaluates the true branch.
    if(bOk && getValue())
    {
        if(op2)
        {
            ExpressionIterator it_expr(* op2);
            Expression * expr;

            it_expr.toLast();

            // Executes all the true branch expressions.
            while((expr = it_expr.current()) != NULL)
            {
                --it_expr;
                EXPDEBUG(1) { cerr << "Expr    --> 0b" << QString::number(expr->getComputeMask(),2) << " expr=" << expr->toString("") << endl; }
                EXPDEBUG(1) { cerr << "CSL     --> 0b" << QString::number(env->cslResult.getComputeMask(),2) << " csl=" << env->cslResult.toString() << endl; }
                if(env->cslResult.getComputeMask() & expr->getComputeMask())
                {
                    QString tempPre = * pre;
                    QString tempPost = * post;

                    expr->eval(env, desc, &tempPre, &tempPost, indent + 1);
                    EXPDEBUG(1) { cerr << "CSL (new)-> 0b" << QString::number(env->cslResult.getComputeMask(),2) << " csl=" << env->cslResult.toString() << endl; }
                }
            }
        }
    }
    // Else we evaluate the false branch expressions (if present).
    else if(op3)
    {
        ExpressionIterator it_expr(* op3);
        Expression * expr;

        it_expr.toLast();

        * pre += + strIndent + strIndent + "...\n" + strIndent + "}\n" + strIndent + "else\n" + strIndent + "{\n";
        // Executes all the false branch expressions.
        while((expr = it_expr.current()) != NULL)
        {
            --it_expr;
            if(env->cslResult.getComputeMask() & expr->getComputeMask())
            {
                QString tempPre = * pre;
                QString tempPost = * post;

                expr->eval(env, desc, &tempPre, &tempPost, indent + 1);
            }
        }
    }

    return bOk;
}

UINT16
ExpressionIf::evalComputeMask()
{
    Expression * expr;

    computeMask = op1->evalComputeMask();
    if(op2)
    {
        computeMask |= op2->evalComputeMask();
    }
    if ( op3 ) {
        computeMask |= op3->evalComputeMask();
    }
    return computeMask;
}

bool
ExpressionIf::typeCheck(QString & error)
{
    bool ok;
    Expression * expr;

    //
    // Check condition and return immediately if failed, so that we return
    // to the user the *first* error in the expression
    //
    ok = op1->typeCheck(error);
    if ( op1->getType() != ExpressionIntegerValue )
    {
     error = QString("IF condition must be of type Integer");
     ok = false;
    }
    if ( ! ok ) return false;

    //
    // Type check THEN block
    //
    if ( op2) ok = op2->typeCheck(error);
    if ( ! ok ) return false;

    //
    // Type check ELSE block
    //
    if ( op3 ) ok = op3->typeCheck(error);
    if ( ! ok ) return false;

    return true;
}

QString
ExpressionIf::toString(QString indent)
{
    QString res;

    res  =  indent + "if ( " + op1->toString("") + " ) {\n";
    if(op2)
    {
        res +=  op2->toString(indent + INDENTSTR);
    }
    res +=  indent + "}\n";
    if ( op3 ) {
     res += indent + "else {\n";
     res +=  op3->toString(indent + INDENTSTR);
     res += indent + "}\n";
    }
    return res;
}

void
ExpressionIf::getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags)
{
    op1->getUsedTags(slotTags, itemTags, nodeTags, cycleTags);
    if(op2)
    {
        op2->getUsedTags(slotTags, itemTags, nodeTags, cycleTags);
    }
    if(op3)
    {
        op3->getUsedTags(slotTags, itemTags, nodeTags, cycleTags);
    }
}

INT64 
ExpressionIf::getObjSize() const
{
    INT64 result = sizeof(*this);

    result += op1->getObjSize();
    if ( op2 ) result += op2->getObjSize();
    if ( op3 ) result += op3->getObjSize();
    return result;
}

QString 
ExpressionIf::getUsageDescription() const
{
    QString result;
    
    result = "ExpressionIf size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n";
    result += "Detailed memory usage for each component:\n";
    result += "Condition: ";
    result += op1->getUsageDescription();
    if( op2 )
    {
        result += "THEN Block: ";
        result += op2->getUsageDescription();
    }
    if ( op3 ) {
     result += "ELSE Block: ";
     result += op3->getUsageDescription();
    }
    return result;
}
