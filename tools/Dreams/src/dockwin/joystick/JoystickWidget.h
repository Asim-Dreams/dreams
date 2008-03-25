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
  * @file JoystickWidget.h
  */

#ifndef _JOYSTICKWIDGET_H_
#define _JOYSTICKWIDGET_H_

#include "DreamsDefs.h"
#include <qwidget.h>
#include <qtimer.h>

class JoystickWidget : public QWidget
{
    Q_OBJECT

    public:
        
        JoystickWidget(QWidget *parent, bool);

        /**
            Sets the Stick to the expected position.
        */
        void SetStick(double x,double y);

        inline void SetStickX(double x) { SetStick(x,y); }
        inline void SetStickY(double y) { SetStick(x,y); }

    public slots:
        void timerTick();

    signals:
        /**
            Is emitted whenwver the stick is moved.
            The values changes between -1 and +1.
        */
        void Moved(double x, double y);

    protected:
        virtual void mousePressEvent(QMouseEvent *e);
        virtual void mouseReleaseEvent(QMouseEvent *e);
        virtual void mouseMoveEvent(QMouseEvent *e);
        virtual void paintEvent(QPaintEvent *e);

    private:
        double x,y;
        bool repeatMovement;
        bool dragged;
        QTimer* repTimer;
};

#endif

