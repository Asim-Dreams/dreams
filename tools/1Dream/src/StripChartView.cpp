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

#include "StripChartView.h"

void StripChartView::drawDstRubber(int x, int y, int mx, bool isClean)
{
    QPainter p;
    p.begin( viewport() );
    p.setRasterOp( NotROP );
    p.setPen( QPen( color0, 1 ) );
    p.setBrush( NoBrush);
    p.setFont(QFont(QString("Helvetica"), 15, QFont::Bold));

    // central line:
    p.drawLine(pressed_x,pressed_y,x,y);

    // compute the vector
    double vx = (double)(x-pressed_x);
    double vy = (double)(y-pressed_y);

    // normalize:
    double mod = sqrt(vx*vx + vy*vy);
    if (mod < 4.0)
    {
        p.end();
        return;
    }

    vx /= mod;
    vy /= mod;

    // rotate 90
    double rvy = vx;
    double rvx = -vy;

    // cota marks
    p.drawLine(pressed_x,pressed_y,pressed_x,0);
    p.drawLine(pressed_x,pressed_y,pressed_x,10000);

    p.drawLine(x,y,x,0);
    p.drawLine(x,y,x,10000);

    if (mod < 16.0)
    {
        p.end();
        return;
    }
    
    // put the current measure:
    QFontMetrics fm = p.fontMetrics();
    QRect tbr = fm.boundingRect(QString("Distance: ")+QString::number(abs(mx-pressed_mx)*1000));
    
    double b = (double)(x-pressed_x);
    double a = (double)(y-pressed_y);
    
    bool inv = (b<0.0);
    if  (inv)
    {
        b = -b;
        a = -a;
    }
    
    double px,py;
    px = pressed_x-(rvx*(double)tbr.height()*2.0);
    py = pressed_y-(rvy*(double)tbr.height()*2.0);
    px += (vx*mod*0.5);
    py += (vy*mod*0.5);
    px -= (vx*(double)tbr.width()*0.5);
    py -= (vy*(double)tbr.width()*0.5);

    double rpx,rpy;
    QWMatrix matx;
    matx.rotate(-(atan2(a,b)*180.0/pi));
    matx.map(px,py,&rpx,&rpy);

    if (inv) p.translate(vx*(double)tbr.width(),vy*(double)tbr.width());
    p.rotate((atan2(a,b)*180.0/pi));
    p.drawText ( (int)rint(rpx),(int)rint(rpy),
                 QString("Distance: ")+QString::number(abs((int)(mx - pressed_mx)*1000)));

    p.end();
}


void StripChartView::scaleToFitX()
{
    int vw = visibleWidth();
    int rw = realContentsWidth;
    double sx = (double)vw/(double)rw;
    
    setScallingFactorX(sx);
}

void StripChartView::scaleToFitY()
{
    int vh = visibleHeight();
    int rh = realContentsHeight;
    double sy = (double)vh/(double)rh;
    
    setScallingFactorY(sy);
}

void StripChartView::scaleToFitInitial()
{
    scaleToFit();
}


StripChartView::StripChartView(QWidget* parent,allFilesData* db,int numsamples, int maxHeight, Ruler *rulerW, Ruler *rulerH) : 
 AScrollView(parent, numsamples*sample_size,maxHeight)
{
    myDB=db;
    numSamples = numsamples;
    rlW=rulerW;
    rlH=rulerH;
    initShadeFlags();
    setSTFitKeepARatio(false);
    setRectZoomKeepARatio(false);
    // ...
}

void
StripChartView::resizeEvent(QResizeEvent *e)
{
    int min_val=(int)MaxOnVisibleStrips(myDB);

    int vw = visibleWidth();
    int vh = visibleHeight();
    int rw = realContentsWidth;
    int rh = realContentsHeight;

    if(vh>min_val && min_val>0)
    {
        setRealContentsSize(rw,min_val);
    }

    double sx = (double)vw/(double)rw;
    double sy = (double)vh/(double)rh;
    double asx = (double)getScallingFactorX();
    double asy = (double)getScallingFactorY();
    double old_sx=asx;
    double old_sy=asy;

    while(sx!=old_sx && sy!=old_sy) 
    {
        old_sx=sx;
        old_sy=sy;
        vw = visibleWidth();
        vh = visibleHeight();
        rw = realContentsWidth;
        rh = realContentsHeight;
        sx = (double)vw/(double)rw;
        sy = (double)vh/(double)rh;
    }

    if(sx>=asx)
    {
        scaleToFitX();
    }  
      
    if(sy>=asy)
    {
        scaleToFitY();
    }  
    AScrollView::resizeEvent(e);
}

void
StripChartView::updateCycleBar(double x)
{
    if (x<0)
    {
        x=0;
    }
    
    if (x>=realContentsWidth) 
    {
        x = realContentsWidth-1;
    }
    
    int cycle = (int)(x/getBaseElementSize());
    //currentCycleLbl->setText(QString(" Cycle : ")+QString::number((int)(cycle*1000))+QString(" "));
}

void
StripChartView::updateRowBar(double y)
{
    if (y<0) 
    {
        y=0;
    }
    
    if (y>=realContentsHeight)
    {
        y = realContentsHeight-1;
    }
    
    int row = realContentsHeight-(int)(y/getBaseElementSize());
    if(((int)row%1000)<10)
    {
    	//currentRowLbl->setText(QString(" Value : ")+QString::number((int)(row/1000))+QString(".00")+QString::number((int)(row%1000))+QString(" "));
    }
    else if(((int)row%1000)<100)
    {
    	//currentRowLbl->setText(QString(" Value : ")+QString::number((int)(row/1000))+QString(".0")+QString::number((int)(row%1000))+QString(" "));
    }
    else
    {
    	//currentRowLbl->setText(QString(" Value : ")+QString::number((int)(row/1000))+QString(".")+QString::number((int)(row%1000))+QString(" "));
    }

}

void
StripChartView::mouseWorldMoveEvent(QMouseEvent* e, double x, double y)
{
    char *str=NULL;
    int last_cycle;
    int selected=-1;
    int cycle=(int)x;
    int cycle_min=cycle-10;
    int cycle_max=cycle+10;
    int writed=0;

    for(INT32 l=0;l<MAX_OPENED_FILES;l++) 
    {
        if(myDB->active[l]==1)
        {
            for(INT32 j=0;j<myDB->files[l].marker_elems;j++) 
            {
                if( (myDB->files[l].markers[j].cycle>cycle_min) 
                    && (myDB->files[l].markers[j].cycle<cycle_max)) 
                {
                    currentMarker->setText(QString("Cycle: ")+QString::number((int)(x*1000))+QString(" in File ")+QString::number(l)+QString(":")+QString(myDB->files[l].markers[j].description));
                    writed=1;
                }
            }
        }
    }    

    if(writed==0) 
    {
        currentMarker->setText(QString(""));
    }

    AScrollView::mouseWorldMoveEvent(e,x,y);
}

void
StripChartView::setStatusBarInc(QStatusBar* value)
{
    //setStatusBar(value);
    currentMarker=new QLabel(value);
    currentMarker->setText(" Marker : ");
    currentMarker->setMinimumWidth(200);
    value->addWidget(currentMarker,0,true);

    currentAspectRatioMode=new QLabel(value);
    currentAspectRatioMode->setText("Unlocked Aspect Ratio");
    currentAspectRatioMode->setMinimumWidth(140);
    value->addWidget(currentAspectRatioMode,0,true);

    //currentRowLbl->setMinimumWidth(100);
    //currentCycleLbl->setMinimumWidth(120);
}

void
StripChartView::lockAspectRatio()
{
    currentAspectRatioMode->setText("Locked Aspect Ratio");
}

void
StripChartView::unlockAspectRatio()
{
    currentAspectRatioMode->setText("Unlocked Aspect Ratio");
}

float
StripChartView::SmoothValue(int pos,int radix,int elems,float *data)
{
    if(radix==0)
    {
        return data[pos];
    }

    int x,y;
    float accumx,accumy,accum;

    x=((pos-radix)<0)?0:pos-radix;
    y=((pos+radix)>elems)?elems:pos+radix;

    accumx=(x==0)?((float)(pos-radix))*-1.0*data[0]:0.0;
    accumy=(y==elems)?((float)(pos+radix-elems))*-1.0*data[elems-1]:0.0;

    accum=accumx+accumy;

    for(INT32 i=x;i<=y;i++)
    {
        accum+=data[i];
    }

    accum=accum/((((float)radix)*2.0)+1.0);

    return accum;
}

bool
StripChartView::drawWorldCoordinatesContents(QPainter* p, int ncx, int ncy, int ncw, int nch)
{
    if(myDB->active[0]==0)
    {
        return true;
    }

    float p1,p2;

    rlH->setZoomFactor(getScallingFactorY());
    rlW->setZoomFactor(getScallingFactorX());

    // compute affected area with the CUBE_SIDE...
    int x1 = ncx - (ncx%sample_size)-5*sample_size;
    int x2 = ncx+ncw+(ncx%sample_size)+5*sample_size;
    int y1 = ncy - (ncy%sample_size)-5*sample_size;
    int y2 = ncy+nch+(ncy%sample_size)+5*sample_size;

    x1 -= AXIS_SIZE;
    x2 += AXIS_SIZE;
    y1 -= AXIS_SIZE;
    y2 += AXIS_SIZE;

    if (x1<0)
    {
        x1=0;
    }

    if (y1<0)
    {
        y1=0;
    }

    if (x2>=realContentsWidth)
    {
        x2=realContentsWidth-1;
    }

    if (y2>=realContentsHeight)
    {
        y2=realContentsHeight-1;
    }

    // clean the affected area
    p->fillRect(x1, y1, x2-x1+1, y2-y1+1, QColor(255,255,255));

    // refresh affected samples:
        int i,j;
    int posi0,posi1;

    // right now ignore vertical coordinates
    // review this when more than 1 strip is showed...

    for(INT32 l=0;l<MAX_OPENED_FILES;l++)
    {
        if(myDB->active[l]==1)
        {
            int steps_freq=myDB->files[l].frequency/1000;
            for(INT32 j=0;j<myDB->files[l].marker_elems;j++)
            {
                if((myDB->files[l].markers[j].cycle>x1) && (myDB->files[l].markers[j].cycle<x2) && (myDB->files[l].markers[j].visible==true))
                {
                    p->setPen( QPen(myDB->generalProperties.Markers,1));
                    p->drawLine(myDB->files[l].markers[j].cycle,0,myDB->files[l].markers[j].cycle,realContentsHeight-1);
                }
            }

            for(INT32 j=0;j<myDB->files[l].elems;j++)
            {
                for(INT32 k=0;k<myDB->files[l].fullData[j].threads;k++)
                {
                    if(myDB->files[l].fullData[j].visible[k]==1)
                    {
                        if(myDB->files[l].fullData[j].breakView[k]==1)
                        {
                            // put the black pen and go ahead
                            p->setPen( QPen(myDB->files[l].fullData[j].color[k],myDB->files[l].fullData[j].lineWidth[k],(PenStyle)myDB->files[l].fullData[j].lineStyle[k]));
                            i=x1/steps_freq;
                            posi0=(x1/steps_freq)-1;
                            posi1=(x2/steps_freq)+1;
                            p1=SmoothValue(i/sample_size,myDB->files[l].fullData[j].breakSmooth[k],myDB->files[l].lines,&(myDB->files[l].fullData[j].values[k][0]));
                            p2=SmoothValue((i/sample_size)+1,0,myDB->files[l].lines,&(myDB->files[l].fullData[j].values[k][0]));

                            if((myDB->files[l].fullData[j].maxView[k]==1) && (myDB->files[l].fullData[j].maxValue[k]>0))
                            {
                                p1=(p1*100.0)/myDB->files[l].fullData[j].maxValue[k];
                                p2=(p2*100.0)/myDB->files[l].fullData[j].maxValue[k];
                            }
                            else if ((myDB->files[l].fullData[j].relativeView[k]==1) && (myDB->files[l].fullData[j].maxInStripValue[k]>0))
                            {
                                p1=(p1*100.0)/myDB->files[l].fullData[j].maxInStripValue[k];
                                p2=(p2*100.0)/myDB->files[l].fullData[j].maxInStripValue[k];
                            }

                            while ((i<(x2*steps_freq)) && (i<(myDB->files[l].lines-1)))
                            {
                                p->drawLine(posi0,realContentsHeight-(int)(p1*1000),posi0+(sample_size*steps_freq),realContentsHeight-(int)(p2*1000));
                                i+=sample_size;
                                posi0+=(sample_size*steps_freq);
                                p1=SmoothValue(i/sample_size,myDB->files[l].fullData[j].breakSmooth[k],myDB->files[l].lines,&(myDB->files[l].fullData[j].values[k][0]));
                                p2=SmoothValue((i/sample_size)+1,0,myDB->files[l].lines,&(myDB->files[l].fullData[j].values[k][0]));

                                if((myDB->files[l].fullData[j].maxView[k]==1) && (myDB->files[l].fullData[j].maxValue[k]>0))
                                {
                                    p1=(p1*100.0)/myDB->files[l].fullData[j].maxValue[k];
                                    p2=(p2*100.0)/myDB->files[l].fullData[j].maxValue[k];
                                }
                                else if ((myDB->files[l].fullData[j].relativeView[k]==1) && (myDB->files[l].fullData[j].maxInStripValue[k]>0))
                                {
                                    p1=(p1*100.0)/myDB->files[l].fullData[j].maxInStripValue[k];
                                    p2=(p2*100.0)/myDB->files[l].fullData[j].maxInStripValue[k];
                                }

                            }
                        }
                        else if (myDB->files[l].fullData[j].smoothView[k]==1)
                        {
                            // put the black pen and go ahead
                            p->setPen( QPen(myDB->files[l].fullData[j].color[k],myDB->files[l].fullData[j].lineWidth[k],(PenStyle)myDB->files[l].fullData[j].lineStyle[k]));
                                i=x1/steps_freq;
                            posi0=(x1/steps_freq)-1;
                            posi1=(x2/steps_freq)+1;
                            p1=SmoothValue(i/sample_size,myDB->files[l].fullData[j].smooth[k],myDB->files[l].lines,&(myDB->files[l].fullData[j].values[k][0]));
                            p2=SmoothValue((i/sample_size)+1,myDB->files[l].fullData[j].smooth[k],myDB->files[l].lines,&(myDB->files[l].fullData[j].values[k][0]));

                            if((myDB->files[l].fullData[j].maxView[k]==1) && (myDB->files[l].fullData[j].maxValue[k]>0))
                            {
                                p1=(p1*100.0)/myDB->files[l].fullData[j].maxValue[k];
                                p2=(p2*100.0)/myDB->files[l].fullData[j].maxValue[k];
                            }
                            else if ((myDB->files[l].fullData[j].relativeView[k]==1) && (myDB->files[l].fullData[j].maxInStripValue[k]>0))
                            {
                                p1=(p1*100.0)/myDB->files[l].fullData[j].maxInStripValue[k];
                                p2=(p2*100.0)/myDB->files[l].fullData[j].maxInStripValue[k];
                            }

                            while ((i<(x2*steps_freq)) && (i<(myDB->files[l].lines-1)))
                            {
                                p->drawLine(posi0,realContentsHeight-(int)(p1*1000),posi0+(sample_size*steps_freq),realContentsHeight-(int)(p2*1000));
                                i+=sample_size;
                                posi0+=(sample_size*steps_freq);
                                p1=p2;
                                p2=SmoothValue((i/sample_size)+1,myDB->files[l].fullData[j].smooth[k],myDB->files[l].lines,&(myDB->files[l].fullData[j].values[k][0]));

                                if((myDB->files[l].fullData[j].maxView[k]==1) && (myDB->files[l].fullData[j].maxValue[k]>0))
                                {
                                    p2=(p2*100.0)/myDB->files[l].fullData[j].maxValue[k];
                                }
                                else if ((myDB->files[l].fullData[j].relativeView[k]==1) && (myDB->files[l].fullData[j].maxInStripValue[k]>0))
                                {
                                    p2=(p2*100.0)/myDB->files[l].fullData[j].maxInStripValue[k];
                                }

                            }
                        }
                        else
                        {
                            // put the black pen and go ahead
                            p->setPen( QPen(myDB->files[l].fullData[j].color[k],myDB->files[l].fullData[j].lineWidth[k],(PenStyle)myDB->files[l].fullData[j].lineStyle[k]));
                                i=x1/steps_freq;
                            posi0=(x1/steps_freq)-1;
                            posi1=(x2/steps_freq)+1;
                            p1=SmoothValue(i/sample_size,0,myDB->files[l].lines,&(myDB->files[l].fullData[j].values[k][0]));
                            p2=SmoothValue((i/sample_size)+1,0,myDB->files[l].lines,&(myDB->files[l].fullData[j].values[k][0]));

                            if((myDB->files[l].fullData[j].maxView[k]==1) && (myDB->files[l].fullData[j].maxValue[k]>0))
                            {
                                p1=(p1*100.0)/myDB->files[l].fullData[j].maxValue[k];
                                p2=(p2*100.0)/myDB->files[l].fullData[j].maxValue[k];
                            }
                            else if ((myDB->files[l].fullData[j].relativeView[k]==1) && (myDB->files[l].fullData[j].maxInStripValue[k]>0))
                            {
                                p1=(p1*100.0)/myDB->files[l].fullData[j].maxInStripValue[k];
                                p2=(p2*100.0)/myDB->files[l].fullData[j].maxInStripValue[k];
                            }

                            while ((i<(x2*steps_freq)) && (i<(myDB->files[l].lines-1)))
                            {
                                p->drawLine(posi0,realContentsHeight-(int)(p1*1000),posi0+(sample_size*steps_freq),realContentsHeight-(int)(p2*1000));
                                i+=sample_size;
                                posi0+=(sample_size*steps_freq);
                                p1=p2;
                                p2=SmoothValue((i/sample_size)+1,0,myDB->files[l].lines,&(myDB->files[l].fullData[j].values[k][0]));

                                if((myDB->files[l].fullData[j].maxView[k]==1) && (myDB->files[l].fullData[j].maxValue[k]>0))
                                {
                                    p2=(p2*100.0)/myDB->files[l].fullData[j].maxValue[k];
                                }
                                else if ((myDB->files[l].fullData[j].relativeView[k]==1) && (myDB->files[l].fullData[j].maxInStripValue[k]>0))
                                {
                                    p2=(p2*100.0)/myDB->files[l].fullData[j].maxInStripValue[k];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

void StripChartView::doAnimationStep()
{
    // ...
}

double StripChartView::getBaseElementSize() 
{ 
return (double)sample_size; 
}

bool StripChartView::changeSelectedElement(int x, int y) 
{
    return true;
}

QString StripChartView::getWelcomeMessage()
{ 
    return "StripChartViewer... for help press F1";
}

