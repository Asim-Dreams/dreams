/*
 * Copyright (C) 2003-2006 Intel Corporation
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


#ifndef _StripChartVIEW_H
#define _StripChartVIEW_H

#include <stdio.h>
#include "Ruler.h"

#define AXIS_SIZE    0

#include "asim/AScrollView.h"
#include "StripEntry.h"

extern float MaxOnVisibleStrips(allFilesData *myDB);

const int sample_size = 1;

class StripChartView : public AScrollView
{

  public:
    StripChartView(QWidget* parent,allFilesData* db,int numsamples, int maxHeight, Ruler *, Ruler *);
    ~StripChartView(){};
    void setStatusBarInc(QStatusBar *value);
    double getBaseElementSize();
    bool changeSelectedElement(int x, int y);
    QString getWelcomeMessage();
    float SmoothValue(int pos,int radix,int elems,float *data);

    virtual bool drawWorldCoordinatesContents(QPainter* p, int ncx, int ncy, int ncw, int nch);
    void doAnimationStep();                
    virtual void mouseWorldMoveEvent(QMouseEvent* e, double x, double y);
    virtual void resizeEvent(QResizeEvent *e);
    
    virtual void scaleToFitInitial();
    virtual void scaleToFitX();
    virtual void scaleToFitY();

    virtual void mouseWorldDoubleClickEvent(QMouseEvent* e, double x, double y) { };
    virtual void rightClickPressEvent(QMouseEvent* e) { };
    virtual void rightClickReleaseEvent(QMouseEvent* e) { };
    virtual bool hasContentsOn(int col, int row) { return true; };
    virtual void contentsSelectEvent(int col,int row, bool adding,bool shifted) { };
    virtual void updateRowBar(double y);
    virtual void updateCycleBar(double x);
    void lockAspectRatio();
    void unlockAspectRatio();
        void drawDstRubber(int x, int y,int mx, bool isClean);
    
  private:
    allFilesData* myDB;
    int numSamples;
    QLabel     *currentMarker;
    QLabel     *currentAspectRatioMode;
  public:    
    Ruler    *rlW;
    Ruler    *rlH;

};
 

#endif
