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
#include "ExpressionMap.h"

ExpressionMap::ExpressionMap(Expression * _op1, ADFMap * _map, UINT64 _min, UINT64 _max) : Expression(ExpressionIntegerValue)
{
    Q_ASSERT(_min < _max);
    Q_ASSERT(_op1);
    op1 = _op1;
    map = _map;
    min = _min;
    max = _max;
}

ExpressionMap::~ExpressionMap()
{
    delete op1;
}

UINT16
ExpressionMap::evalComputeMask()
{
    computeMask = op1->evalComputeMask();
    return computeMask;
}

bool
ExpressionMap::typeCheck(QString & error)
{
    bool ok = op1->typeCheck(error);
    if(ok)
    {
        // Checks if that the index expression is of type integer.
        if(op1->getType() != ExpressionIntegerValue)
        {
            ok = false;
            error = QString("Trying to make a look up in a table using a non integer expression.");
        }
    }
    return ok;
}

void
ExpressionMap::getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags)
{
    op1->getUsedTags(slotTags, itemTags, nodeTags, cycleTags);
}

bool 
ExpressionMap::eval(RuleEvalEnv * env)
{
    bool ok;

    EXPDEBUG(1) { cerr << "ExpressionMap::eval(trackId=" << env->trackId << ",cycle=" << env->cycle << ") on " << toString("") << endl; }
    EXPDEBUG(1) { cerr << "ExpressionMap::eval: cslResult             = " << env->cslResult.toString() ; }

    ok = op1->eval(env);

    // If a value is returned.
    if ( ok )
    {
        UINT64 key;

        key = (UINT64) op1->getValue();
        if ( key > max ) key = max;
        if ( key < min ) key = min;
        key = (UINT64) (((double) (key - min) / (double) (max - min)) * 100.0);

        // Gets the value from the map.
        UINT64 temp_value = getValue();
        ok = map->getValue(key, &temp_value);
        setValue(temp_value);
        EXPDEBUG(1) { cerr << "ExpressionMap::eval: ok=" << ok << " key=" << key << " getValue() =" << getValue() << endl; }
    }
    return ok;
}

bool 
ExpressionMap::eval(RuleEvalEnv * env, RuleEvalEnvDesc * desc, QString * pre, QString * post, INT32 indent)
{
    bool ok;

    EXPDEBUG(1) { cerr << "ExpressionMap::eval(trackId=" << env->trackId << ",cycle=" << env->cycle << ") on " << toString("") << endl; }
    EXPDEBUG(1) { cerr << "ExpressionMap::eval: cslResult             = " << env->cslResult.toString() ; }

    * pre += "map(";
    ok = op1->eval(env, desc, pre, NULL, 0);

    * pre += ", " + map->getMapName() + ", " + QString::number(min) + ", " + QString::number(max) + ")";

    // If a value is returned.
    if ( ok )
    {
        UINT64 key;

        key = (UINT64) op1->getValue();
        if ( key > max ) key = max;
        if ( key < min ) key = min;
        key = (UINT64) (((double) (key - min) / (double) (max - min)) * 100.0);

        // Gets the value from the map.
        UINT64 temp_value = getValue();
        ok = map->getValue(key, &temp_value);
        setValue(temp_value);
        EXPDEBUG(1) { cerr << "ExpressionMap::eval: ok=" << ok << " key=" << key << " getValue()=" << getValue() << endl; }
    }
    return ok;
}

INT64 
ExpressionMap::getObjSize() const
{
    INT64 result = sizeof(*this);
    result += op1->getObjSize();

    // TODO
    // Implement map memory usage dumping
    //result += map->getObjSize();

    return result;
}

QString 
ExpressionMap::getUsageDescription() const
{
    QString result = "ExpressionIf size: " + QString::number(static_cast<long>(getObjSize())) + " bytes\n\n";
    result += "\n\nDetailed memory usage for each component:\n";
    result += op1->getUsageDescription();

    // TODO
    // Implement map memory usage dumping
    //result += map->getUsageDescription();

    return result;
}

QString
ExpressionMap::toString(QString indent)
{
 QString res;
 QString mymapname = "<unknown>";

 res  =  indent + "map(" + op1->toString("") + "," + mymapname + "," + QString::number(min) + "," + QString::number(max) + ")";
 return res;
}
