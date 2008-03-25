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

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "avt/FloorplanDAvtView.h"
#include "dockwin/tag/TagDockWin.h"
#include "mdi/MDIFloorplanWindow.h"
#include "Dreams.h"

// Qt includes.
#include <qslider.h>
#include <qtimer.h>
#include <qvbox.h>
#include <qmainwindow.h>

MDIFloorplanWindow::MDIFloorplanWindow(DreamsDB * dreamsdb, QMainWindow * m, QWidget * parent, const char * name, int wflags)
    : MDIDreamsWindow(dreamsdb, parent, name, wflags)
{
    myParent = parent;
    mainWin = m;

    nFPS = 1;
    nCPF = 1;
    fastStep = 1;
    lastValue = 0;

    VHIGH = false;
    VNHIGH = false;
    VBHIGH = true;
    setOpaqueMoving(false);

    pqvbLayout = new QVBox(this, NULL, wflags);

    pqhbLayout = new QHBox(pqvbLayout, NULL, wflags);

    // We want to generate one event per cycle moved to give the feeling of accelerated
    // animation
    psldCycle = new QSlider(Horizontal, pqhbLayout);
    psldCycle->setTracking(true);
    psldCycle->setTickmarks(QSlider::NoMarks);

    // WARNING: don't set this value to the same value than a CPF, cause
    // we use this to distinguish the setFrames produced by the user from
    // the produced by ourselves.
    psldCycle->setPageStep(50);

    // Combo box with the different clock domains. Fills the combo box with all the clocks.
    qcbClockDomain = new QComboBox(pqhbLayout, NULL);
    UINT32 num_clocks = draldb->getNumClocks();

    for(UINT32 i = 0; i < num_clocks; i++)
    {
        qcbClockDomain->insertItem(draldb->getClockDomain(i)->toString());
    }

    // Sets as default the fastest clock.
    qcbClockDomain->setCurrentItem((* draldb->getClockDomainFreqIterator())->getId());

    pTimer = new QTimer(this, "timer");
    connect(pTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));

    FloorplanDAvtView *ptdView = new FloorplanDAvtView(dreamsdb, pqvbLayout, this, mainWin);
    asv = ptdView;

    sliderDrag = false;

    // If the user sets a specific cycle, update the animation
    connect(psldCycle, SIGNAL(sliderMoved(int)), this, SLOT(setFrame(int)));
    connect(psldCycle, SIGNAL(valueChanged(int)), this, SLOT(setFrame(int)));
    connect(psldCycle, SIGNAL(sliderPressed()), this, SLOT(sliderPressed()));
    connect(psldCycle, SIGNAL(sliderReleased()), this, SLOT(sliderReleased()));
    connect(qcbClockDomain, SIGNAL(activated(int)), this, SLOT(setClockDomain(int)));
}

MDIFloorplanWindow::~MDIFloorplanWindow()
{
    delete pTimer;
    delete psldCycle;
    delete pqvbLayout;

    // MDIWindow deletes asv if it is different from NULL
    asv = NULL;
}

bool
MDIFloorplanWindow::init()
{
    if(!MDIWindow::init())
    {
        return false;
    }
    createTagWindow();
    // Override the set done by the default MDIWindow::init
    setCentralWidget(pqvbLayout);
    return true;
}

/**
 * Public slot that attend the timer signal. This method will not 
 * generate a signal because everybody will be listening to the timer
 */
void
MDIFloorplanWindow::nextFrame()
{
    psldCycle->setValue(cycleToSliderValue(currentCycle) + (nextCycle * nCPF));
    static_cast<FloorplanDAvtView *>(asv)->showCycle(currentCycle);
}

/**
 * Public slot that attends the signal of the slider being pressed.
 */
void
MDIFloorplanWindow::sliderPressed()
{
    sliderDrag = true;
}

/**
 * Public slot that attends the signal of the slider being released.
 */
void
MDIFloorplanWindow::sliderReleased()
{
    sliderDrag = false;
}

/**
 * Public slot that attend the slider movement signal. This method will 
 * generate a signal to notify the rest of the application in order to
 * synchronize their respective cycle counters
 */
void
MDIFloorplanWindow::setFrame(int cycle)
{
    INT32 diff = abs(cycle - lastValue);

    // Given that we are sure that we're the active window (our slider has just moved)
    // we generate the movement signal.
    currentCycle = sliderValueToCycle(cycle);

    // HACK: we only forward a cycle change if the change is equal
    // to the page step of the slider or the slider is being dragged.
    if(getMDILocked() && ((diff == psldCycle->pageStep()) || sliderDrag))
    {
        CYCLE tempCycle = currentCycle;

        // Computes the difference in cycles using the last value of the slider and the actual one.
        tempCycle = sliderValueToCycle(diff);

        // We compute negative displacements manually...
        if(cycle < lastValue)
        {
            tempCycle.cycle = -tempCycle.cycle;
            tempCycle.division = -tempCycle.division;
        }

        // We must draw the canvas.
        static_cast<FloorplanDAvtView *>(asv)->showCycle(currentCycle);

        emit cycleChanged(static_cast<Q_INT64>(tempCycle.toPs()));
    }

    lastValue = cycle;
}

/*
 * Public slot that attend the combo box changes. This is used to change
 * the clock domain of the slider.
 */
void
MDIFloorplanWindow::setClockDomain(int clockId)
{
    INT64 temp;

    // Saves the actual cycle.
    temp = currentCycle.toLCMCycles();

    // Changes the current cycle to the new clock.
    currentCycle.clock = draldb->getClockDomain(clockId);
    currentCycle.fromLCMCycles(temp);

    // Changes the slider limits.
    psldCycle->setMinValue(cycleToSliderValue(draldb->getFirstEffectiveCycle(clockId)));
    psldCycle->setMaxValue(cycleToSliderValue(draldb->getLastEffectiveCycle(clockId)));
    psldCycle->setTickInterval(10);

    // Sets the new cycle.
    // Avoid forwarding the cycle change as diff will be 0.
    lastValue = cycleToSliderValue(currentCycle);
    psldCycle->setValue(lastValue);
}

/*
 * Method to display the current cycle in all the widgdegs shown in the window
 * 
 * @param cycle current cycle to show.
 */
void
MDIFloorplanWindow::showCycle(CYCLE cycle)
{
    currentCycle.fromPs(cycle.toPs());

    currentCycle.cycle = QMAX(currentCycle.cycle, psldCycle->minValue() / currentCycle.clock->getDivisions());
    currentCycle.cycle = QMIN(currentCycle.cycle, psldCycle->maxValue() / currentCycle.clock->getDivisions());

    if((cycleToSliderValue(currentCycle) >= (UINT32) psldCycle->maxValue()) && (nextCycle == 1))
    {
        stop();
        emit lastCycle(this);
    }
    else if((cycleToSliderValue(currentCycle) <= (UINT32) psldCycle->minValue()) && (nextCycle == -1))
    {
        stop();
        emit firstCycle(this);
    }

    // We don't want to forward a cycle change.
    lastValue = cycleToSliderValue(currentCycle);
    psldCycle->setValue(lastValue);

    // Repaints the content.
    static_cast<FloorplanDAvtView *>(asv)->showCycle(currentCycle);
}

void
MDIFloorplanWindow::play(void)
{
    // DEBUG
    //cerr << " MDIFloorplanWindow::start" << endl;
    nextCycle = 1;
    fastStep = 1;
    pTimer->start(BASE_INTERVAL / nFPS);
    static_cast<FloorplanDAvtView *>(asv)->play();
}

void 
MDIFloorplanWindow::stepForward(void)
{
    // DEBUG
    //cerr << " MDIFloorplanWindow::step" << endl;
    pTimer->stop();
    static_cast<FloorplanDAvtView *>(asv)->stop();
    nextCycle = 1;
    fastStep = 1;
    nextFrame();
}

void 
MDIFloorplanWindow::stepBackward(void)
{
    // DEBUG
    //cerr << " MDIFloorplanWindow::step" << endl;
    pTimer->stop();
    static_cast<FloorplanDAvtView *>(asv)->stop();
    nextCycle = -1;
    fastStep = 1;
    nextFrame();
}

void 
MDIFloorplanWindow::stop(void)
{
    // DEBUG
    //cerr << " MDIFloorplanWindow::stop" << endl;
    pTimer->stop();
    static_cast<FloorplanDAvtView *>(asv)->stop();
}

void 
MDIFloorplanWindow::forward(void)
{
    // DEBUG
    //cerr << " MDIFloorplanWindow::forward" << endl;
    nextCycle = 1;
    fastStep = 2;
    pTimer->changeInterval(BASE_INTERVAL / (nFPS * fastStep));
    static_cast<FloorplanDAvtView *>(asv)->forward();
}

void 
MDIFloorplanWindow::rewind(void)
{
    // DEBUG
    //cerr << " MDIFloorplanWindow::rewind" << endl;
    nextCycle = -1;
    fastStep = 2;
    pTimer->changeInterval(BASE_INTERVAL / (nFPS * fastStep));
    static_cast<FloorplanDAvtView *>(asv)->rewind();
}

/**
 * Sets the velocity of the animation based on the standard
 *
 * @param Total amount of cycles of the animation
 */
void 
MDIFloorplanWindow::setFPS(const QString &fps)
{
    QString sTmp = fps;
    // Remove the first space
    sTmp.remove(0, 1);
    sTmp.remove(sTmp.length() - 4, 4);
    nFPS = sTmp.toInt();
    bool bActive = pTimer->isActive();
    pTimer->changeInterval(BASE_INTERVAL / (nFPS * fastStep));

    if(!bActive) pTimer->stop();
}

void 
MDIFloorplanWindow::setCPF(const QString &cpf)
{
    QString sTmp = cpf;
    // Remove the first space
    sTmp.remove(0, 1);
    sTmp.remove(sTmp.length() - 4, 4);
    nCPF = sTmp.toInt();
}

void 
MDIFloorplanWindow::closeEvent(QCloseEvent *e)
{
    emit mdiCloseEvent(this);
    e->accept();
}

void 
MDIFloorplanWindow::attendCycleChanged(INT64 ps)
{
    displacement += ps;
    CYCLE cycle = getAccCycleChanged(currentCycle.clock, (currentCycle.clock->getDivisions() > 1));

    currentCycle.fromPs(currentCycle.toPs() + cycle.toPs());
    showCycle(currentCycle);
}

void 
MDIFloorplanWindow::attendThreeDMoved(double x, double y)
{
    // Prevent the emision of the movement signals
    // It must be changed by a check if the asv is active or not...
    bool oldMdiLocked = asv->getMDILocked();
    asv->setMDILocked(false);

    asv->scrollByMatrix(x, y);

    asv->setMDILocked(oldMdiLocked);
}
