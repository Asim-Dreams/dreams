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

#ifndef Ruler_h_
#define Ruler_h_

#include <qwidget.h>
#include <qframe.h>
#include <qpixmap.h>

class StripChartView;

class Ruler : public QFrame {
    Q_OBJECT
  public:
    enum Orientation { Horizontal, Vertical };

    Ruler (Orientation o, QWidget *parent = 0L,
           const char* name = 0L);

    void setStripChartView(StripChartView *vw);

  public slots:
    void setZoomFactor (float zf);
    void updatePointer (int x, int y);
    void updateVisibleArea (int xpos, int ypos);

    void hide ();
    void show ();
    void drawRuler (int dummy=0);

  signals:
    /*emit signal for drawing a ruler, note: the position is in sceen positions
      and might be out of the drawing area; in such a case the position should
      be ignored by the slot of drawRuler and -- if drawn -- remove the
      draw'ed-Ruler; note: the can be only one drawRuler at a time    
      orientationHoriz = true <=> horizontal
     */
    void drawHelpline(int x, int y, bool orientationHoriz);
    void addHelpline (int x, int y, bool orientationHoriz);

  protected:
    void paintEvent  (QPaintEvent *e);
    void resizeEvent (QResizeEvent *e);

    void recalculateSize (QResizeEvent *e);

  protected:
    void initMarker (int w, int h);

  protected slots:
    void mousePressEvent ( QMouseEvent * );
    void mouseReleaseEvent ( QMouseEvent * );
    void mouseMoveEvent ( QMouseEvent * );

  private:
    bool isMousePressed;
    float zoom;
    Orientation orientation;
    QPixmap *buffer;
    int firstVisible;
    int currentPosition;
    QPixmap *marker, *bg;
    StripChartView *scv;
};

#endif
