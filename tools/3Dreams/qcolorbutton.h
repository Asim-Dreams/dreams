/*****************************************************************************
*
* @brief Color chooser button.
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

#ifndef QCOLOR_BUTTON_H
#define QCOLOR_BUTTON_H

#include <qpushbutton.h>
#include <qcolordialog.h>
#include <qpainter.h>

class QColorButton : public QPushButton
{
  Q_OBJECT
  public:
    QColorButton(QWidget * parent, const char * name = 0);

    void setColor(QColor color)
    {
        color_ = color;
    }

    QColor color() const
    {
        return color_;
    };

  protected:
    void drawButton(QPainter* painter);
    
  private:
    QColor color_;

  public slots:
    void changeColor();
};

#endif
