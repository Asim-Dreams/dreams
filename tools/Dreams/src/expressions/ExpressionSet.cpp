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
#include "ExpressionSet.h"

ExpressionSet::ExpressionSet(DralDB * _draldb, ExpressionOperatorType _op, Expression *_rhs, UINT8 _fade)
    : Expression(ExpressionNoValue), op(_op), rhs(_rhs), fade(_fade)
{
    Q_ASSERT(rhs);
    draldb = _draldb;
}

ExpressionSet::~ExpressionSet()
{
    delete rhs;
}

bool
ExpressionSet::eval(RuleEvalEnv *env)
{
    bool bOk;
    bool transp = false;

    EXPDEBUG(1) { cerr << "ExpressionSet::eval: trackId=" << env->trackId <<  " cycle=" << env->cycle << endl; }
    EXPDEBUG(1) { cerr << "CSL data before eval = " << env->cslResult.toString(); }

    //
    // Evaluate the expression to get the RHS value
    //
    bOk = rhs->eval(env);
    if(!bOk)
    {
        return false;
    }

    switch ( op ) {
        case OperatorSetFColor:
                if ( !env->cslResult.getFillColorSet() )
                {
                    env->cslResult.setFillColor(rhs->getColorValue(draldb, &transp));
                    env->cslResult.setTransparency(transp);
                    if ( fade > 0 ) env->cslResult.setFillColorFade(fade);
                }
                break;
                
        case OperatorSetBColor:
                if ( !env->cslResult.getBorderColorSet() ) env->cslResult.setBorderColor(rhs->getColorValue(draldb, &transp));
                break;

        case OperatorSetLColor:
                if ( !env->cslResult.getLetterColorSet() ) env->cslResult.setLetterColor(rhs->getColorValue(draldb, &transp));
                break;

        case OperatorSetLetter:
                if ( !env->cslResult.getLetterSet() ) env->cslResult.setLetter(rhs->getLetterValue());
                break;

        case OperatorSetShape:
                if ( !env->cslResult.getShapeSet() ) env->cslResult.setShape(rhs->getShapeValue());
                break;

        case OperatorSetFont:
                if ( !env->cslResult.getFontSet() ) env->cslResult.setFont(rhs->getFontValue());
                break;

        case OperatorSetItalic:
                if ( !env->cslResult.getItalicSet() ) env->cslResult.setItalic((bool) rhs->getValue());
                break;

        case OperatorSetUnderline:
                if ( !env->cslResult.getUnderlineSet() ) env->cslResult.setUnderline((bool) rhs->getValue());
                break;

        case OperatorSetBold:
                if ( !env->cslResult.getBoldSet() ) env->cslResult.setBold((bool) rhs->getValue());
                break;

        default:
               Q_ASSERT(0);
    }

    EXPDEBUG(1) { cerr << "CSL after.......eval = " << env->cslResult.toString() << endl; }
    return true;
}

bool
ExpressionSet::eval(RuleEvalEnv *env, RuleEvalEnvDesc * desc, QString * pre, QString * post, INT32 indent)
{
    bool bOk;
    bool transp = false;

    EXPDEBUG(1) { cerr << "ExpressionSet::eval: trackId=" << env->trackId <<  " cycle=" << env->cycle << endl; }
    EXPDEBUG(1) { cerr << "CSL data before eval = " << env->cslResult.toString(); }

    QString temp;
    QString strIndent = "";
    for(INT32 i = 0; i < indent; i++)
    {
        strIndent += "    ";
    }


    //
    // Evaluate the expression to get the RHS value
    //
    bOk = rhs->eval(env, desc, &temp, NULL, 0);
    if(!bOk)
    {
        return false;
    }

    switch ( op ) {
        case OperatorSetFColor:
                if ( !env->cslResult.getFillColorSet() )
                {
                    env->cslResult.setFillColor(rhs->getColorValue(draldb, &transp));
                    env->cslResult.setTransparency(transp);
                    * desc->fill_color = * pre + strIndent + "fcolor = " + temp + ";\n" + * post;

                    if ( fade > 0 )
                    {
                        env->cslResult.setFillColorFade(fade);
                    }
                }
                break;
                
        case OperatorSetBColor:
                if ( !env->cslResult.getBorderColorSet() )
                {
                    env->cslResult.setBorderColor(rhs->getColorValue(draldb, &transp));
                    * desc->border_color = * pre + strIndent + "bcolor = " + temp + ";\n" + * post;
                }
                break;

        case OperatorSetLColor:
                if ( !env->cslResult.getLetterColorSet() )
                {
                    env->cslResult.setLetterColor(rhs->getColorValue(draldb, &transp));
                    * desc->letter_color = * pre + strIndent + "lcolor = " + temp + ";\n" + * post;
                }
                break;

        case OperatorSetLetter:
                if ( !env->cslResult.getLetterSet() )
                {
                    env->cslResult.setLetter(rhs->getLetterValue());
                    * desc->letter = * pre + strIndent + "letter = " + temp + ";\n" + * post;
                }
                break;

        case OperatorSetShape:
                if ( !env->cslResult.getShapeSet() )
                {
                    env->cslResult.setShape(rhs->getShapeValue());
                    * desc->shape = * pre + strIndent + "shape = " + temp + ";\n" + * post;
                }
                break;

        case OperatorSetFont:
                if ( !env->cslResult.getFontSet() )
                {
                    env->cslResult.setFont(rhs->getFontValue());
                    * desc->font = * pre + strIndent + "font = " + temp + ";\n" + * post;
                }
                break;

        case OperatorSetItalic:
                if ( !env->cslResult.getItalicSet() )
                {
                    env->cslResult.setItalic((bool) rhs->getValue());
                    * desc->font = * pre + strIndent + "italic = " + temp + ";\n" + * post;
                }
                break;

        case OperatorSetUnderline:
                if ( !env->cslResult.getUnderlineSet() )
                {
                    env->cslResult.setUnderline((bool) rhs->getValue());
                    * desc->font = * pre + strIndent + "underline = " + temp + ";\n" + * post;
                }
                break;

        case OperatorSetBold:
                if ( !env->cslResult.getBoldSet() )
                {
                    env->cslResult.setBold((bool) rhs->getValue());
                    * desc->font = * pre + strIndent + "bold = " + temp + ";\n" + * post;
                }
                break;

        default:
               Q_ASSERT(0);
    }

    EXPDEBUG(1) { cerr << "CSL after.......eval = " << env->cslResult.toString() << endl; }
    return true;
}

UINT16
ExpressionSet::evalComputeMask()
{
    computeMask = ExpressionToolkit::toMask(op);
    return ExpressionToolkit::toMask(op);
}

void
ExpressionSet::getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags)
{
    rhs->getUsedTags(slotTags, itemTags, nodeTags, cycleTags);
}

bool
ExpressionSet::typeCheck(QString & error)
{
    bool ok;

    ok = rhs->typeCheck(error);
    if(!ok)
    {
        return false;
    }

    ExpressionType rhsType = rhs->getType();
    switch(op)
    {
        case OperatorSetFColor:
        case OperatorSetBColor:
        case OperatorSetLColor:
            ok = ((rhsType == ExpressionStringValue) || (rhsType == ExpressionIntegerValue));
            break;

        case OperatorSetLetter:
            ok = (rhsType == ExpressionLetterValue);
            break;

        case OperatorSetShape:
            ok = (rhsType == ExpressionShapeValue);
            break;

        case OperatorSetItalic:
        case OperatorSetUnderline:
        case OperatorSetBold:
            ok = (rhsType == ExpressionIntegerValue);
            break;

        default:
            ok = false;
    }
    if(!ok)
    {
        error = ExpressionToolkit::toString(op) + " can not be assigned a value of type " + ExpressionToolkit::toString(rhsType);
    }
    return ok;
}

QString
ExpressionSet::toString(QString indent)
{
    return (indent + ExpressionToolkit::toString(op) + " = " + rhs->toString("") + ";\n");
}

INT64 ExpressionSet::getObjSize() const
{
    return sizeof(*this);
}

QString ExpressionSet::getUsageDescription() const
{
    return "ExpressionSet size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n\n";
}

