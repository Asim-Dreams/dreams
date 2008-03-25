/*****************************************************************************
*
* @brief XML configuration file parsing helper functions. Source File.
*
* @author Oscar Rosell
*
*  Copyright (C) 2003-2006 Intel Corporation
*  
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*  
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*  
*
*****************************************************************************/

#include "readxml.h"

bool readConfigurationSetElement(const QDomElement& elem, PaintEvent** pe)
{
    printf("Set ELEMENT\n");
    QString color(elem.attribute("color","NOT FOUND"));
    if (color!="NOT FOUND")
    {
        printf("Set color: %s\n",color.latin1());
        PaintEventFill* pef = new PaintEventFill();
        pef->setColor(QColor(color));
        (*pe) = pef;
        return true;
    }
    QString letter(elem.attribute("letter","NOT FOUND"));
    if (letter!="NOT FOUND")
    {
        printf("Set letter: %s\n",letter.latin1());
        PaintEventLetter* pel = new PaintEventLetter();
        pel->setLetter(letter.latin1()[0]);
        (*pe) = pel;
        return true;
    }
    QString shape(elem.attribute("shape","NOT FOUND"));
    if (shape!="NOT FOUND")
    {
        printf("Set shape: %s\n",shape.latin1());
        if (shape=="triangle")
        {
            PaintEventShape* pes = new PaintEventShape();
            pes->setShape(EVENT_SHAPE_TRIANGLE);
            (*pe) = pes;
            return true;
        }
        return false;
    }
    return false;
}

bool readConfigurationConditionElement(const QDomElement& elem, TagInspectorRoot** ti)
{
    printf("Condition\n");
    assert(elem.tagName()=="condition");

    QString tag(elem.attribute("tag","NOT FOUND"));
    if (tag=="NOT FOUND")
        return false;
    QString minvalue(elem.attribute("minvalue","NOT FOUND"));
    QString maxvalue(elem.attribute("maxvalue","NOT FOUND"));
    if ((minvalue!="NOT FOUND")&&maxvalue!="NOT FOUND")
    {
        IntervalTag it;
        it.setMinimum(minvalue.toUInt());
        it.setMaximum(maxvalue.toUInt());
        TagInspectorComplex<UINT64,IntervalTag>* tiTemp;
        tiTemp = new TagInspectorComplex<UINT64,IntervalTag>(tag);
        tiTemp->add(it,0);
        (*ti) = tiTemp;
        return true;
    }
    QString value(elem.attribute("value","NOT FOUND"));
    if (value != "NOT FOUND")
    {
       return true;
    }
    return false;
}

bool readConfigurationOrElement(const QDomElement& elem, TagInspectorRoot** ti)
{
    printf("Or\n");
    assert(elem.tagName()=="or");

    TagInspectorRoot* ti_tempA;
    TagInspectorRoot* ti_tempB;

    QDomNode n = elem.firstChild();

    if (!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull())
        {
            if (e.tagName()=="condition")
            {
                readConfigurationConditionElement(e,&ti_tempA);
            }
            if (e.tagName()=="and")
            {
                readConfigurationAndElement(e,&ti_tempA);
            }
            if (e.tagName()=="or")
            {
                readConfigurationOrElement(e,&ti_tempA);
            }
        }
        else
        {
            return false;
        }
    }

    n = n.nextSibling();

    if (!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull())
        {
            printf("Or correct\n");
            if (e.tagName()=="condition")
            {
                readConfigurationConditionElement(e,&ti_tempB);
            }
            if (e.tagName()=="and")
            {
                readConfigurationAndElement(e,&ti_tempB);
            }
            if (e.tagName()=="or")
            {
                readConfigurationOrElement(e,&ti_tempB);
            }
        }
        else
        {
            return false;
        }
    }
    PaintEventLetter* pelDummy1 = new PaintEventLetter();
    PaintEventLetter* pelDummy2 = new PaintEventLetter();
    ti_tempA->setPaintEvent(pelDummy1);
    ti_tempB->setPaintEvent(pelDummy2);
    TagInspectorOr* tio = new TagInspectorOr(ti_tempA,ti_tempB);
    (*ti) = tio;

    return (true);
}

bool readConfigurationAndElement(const QDomElement& elem, TagInspectorRoot** ti)
{
    printf("And\n");
    assert(elem.tagName()=="and");

    TagInspectorRoot* ti_tempA;
    TagInspectorRoot* ti_tempB;

    QDomNode n = elem.firstChild();

    if (!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull())
        {
            if (e.tagName()=="condition")
            {
                readConfigurationConditionElement(e,&ti_tempA);
            }
            if (e.tagName()=="and")
            {
                readConfigurationAndElement(e,&ti_tempA);
            }
            if (e.tagName()=="or")
            {
                readConfigurationOrElement(e,&ti_tempA);
            }
        }
        else
        {
            return false;
        }
    }

    n = n.nextSibling();

    if (!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull())
        {
            if (e.tagName()=="condition")
            {
                readConfigurationConditionElement(e,&ti_tempB);
            }
            if (e.tagName()=="and")
            {
                readConfigurationAndElement(e,&ti_tempB);
            }
            if (e.tagName()=="or")
            {
                readConfigurationOrElement(e,&ti_tempB);
            }
        }
        else
        {
            return false;
        }
    }
    PaintEventLetter* pelDummy1 = new PaintEventLetter();
    PaintEventLetter* pelDummy2 = new PaintEventLetter();
    ti_tempA->setPaintEvent(pelDummy1);
    ti_tempB->setPaintEvent(pelDummy2);

    TagInspectorAnd* tia = new TagInspectorAnd(ti_tempA,ti_tempB);
    (*ti) = tia;
    return (true);
}

bool readConfigurationIfElement(const QDomElement& elem, TagInspectorRoot** ti)
{
    printf("If\n");
    QDomNode n = elem.firstChild();

    if (!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull())
        {
            if (e.tagName()=="condition")
            {
                return readConfigurationConditionElement(e,ti);
            }
            if (e.tagName()=="and")
            {
                return readConfigurationAndElement(e,ti);
            }
            if (e.tagName()=="or")
            {
                return readConfigurationOrElement(e,ti);
            }
        }
    }
    return false;
}

bool readConfigurationThenElement(const QDomElement& elem, TagInspectorRoot** ti)
{
    printf("Then\n");
    QDomNode n = elem.firstChild();

    if (!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull())
        {
            printf("Tag: %s\n",e.tagName().latin1());
            if (e.tagName()=="set")
            {
                PaintEvent* pe;
                bool ok;
                ok=readConfigurationSetElement(e,&pe);
                (*ti)->setPaintEvent(pe);
                return ok;
            }
            if (e.tagName()=="temperature")
            {
                bool ok;
                PaintEvent* pe;
                ok=readConfigurationSetElement(e,&pe);
                (*ti)->setPaintEvent(pe);
                return ok;
            }
        }
    }
    return false;
}


bool readConfigurationIfThenElement(const QDomElement& elem, TagInspectorRoot** ti)
{
    printf("If Then\n");
    bool ok = false;

    QDomNode n = elem.firstChild();

    if (!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull())
        {
            if (e.tagName()!="if")
                return false;
            else
                ok = readConfigurationIfElement(e,ti);
        }
    }

    n = n.nextSibling();
    if (!n.isNull())
    {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if (!e.isNull())
        {
            if (e.tagName()!="then")
                return false;
            else
                ok = ok && readConfigurationThenElement(e,ti);
        }
    }
    return ok;
}
