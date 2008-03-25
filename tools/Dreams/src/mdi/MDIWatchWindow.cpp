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

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "dialogs/tag/TagDialogImpl.h"
#include "Dreams.h"
#include "DreamsDB.h"
#include "MDIWatchWindow.h"
#include "WatchWindow.h"

// Qt includes.
#include <qslider.h>
#include <qtimer.h>
#include <qvbox.h>
#include <qpopupmenu.h>
#include <qmainwindow.h>

// Externs.
extern QWidget * focusMDI;

MDIWatchWindow::WatchWindowTable::WatchWindowTable(MDIWatchWindow *parentWindow, QWidget *parent) 
    : QTable(parent)
{
    this->parentWindow = parentWindow;
}

/**
 * Method that resize the MDIWatchWindow in order to make fit the 
 * table width. This slot is called when de user change any column 
 * width.
 */
void 
MDIWatchWindow::WatchWindowTable::columnWidthChanged(int col)
{
    // We recompute the width each time because I have no idea
    // of the previous column width
    parentWindow->setTableSize();
    QTable::columnWidthChanged(col);
}

MDIWatchWindow::MDIWatchWindow(DreamsDB * dreamsdb, QMainWindow* m, QWidget* parent, const char* name, int wflags)
    : MDIDreamsWindow(dreamsdb, parent, name, wflags)
{
    myParent=parent;
    mainWin=m;

    mdiLocked = true;

    nFPS = 1;
    nCPF = 1;
    fastStep = 1;
    lastValue = 0;

    VHIGH=false;
    VNHIGH=false;
    VBHIGH=true;
    setOpaqueMoving(false);

    pqvbLayout = new QVBox(this, NULL, wflags);

    psldCycle = new QSlider(Horizontal, pqvbLayout);
    // We want to generate one event per cycle moved to give the feeling of accelerated
    // animation
    psldCycle->setTracking(true);
    psldCycle->setTickmarks(QSlider::NoMarks);

    sliderDrag = false;

    // WARNING: don't set this value to the same value than a CPF, cause
    // we use this to distinguish the setFrames produced by the user from
    // the produced by ourselves.
    psldCycle->setPageStep(50);

    pTimer = new QTimer(this, "timer");
    connect(pTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));


    // In watch window we don't need an AScrollView, because we will use a
    // QTable to display all the elements
    ptblSlots = new WatchWindowTable(this, pqvbLayout);
    ptblSlots->setReadOnly(true); 

    addContextMenu();
    connect(ptblSlots, SIGNAL(contextMenuRequested(int, int, const QPoint &)), this, SLOT(showContextMenu(int, int, const QPoint &)));

    // If the user sets a specific cycle, update the animation
    connect(psldCycle, SIGNAL(sliderMoved(int)), this, SLOT(setFrame(int)) );
    connect(psldCycle, SIGNAL(valueChanged(int)), this, SLOT(setFrame(int)) );
    connect(psldCycle, SIGNAL(sliderPressed()), this, SLOT(sliderPressed()));
    connect(psldCycle, SIGNAL(sliderReleased()), this, SLOT(sliderReleased()));
}

MDIWatchWindow::~MDIWatchWindow()
{
    delete pTimer;
    delete psldCycle;
    delete contextMenu;
    delete ptblSlots;
    delete pqvbLayout;
}

void
MDIWatchWindow::addContextMenu(void)
{
    contextMenu = new QPopupMenu(ptblSlots);
    contextMenuSlotTools = new QPopupMenu(contextMenu);
    contextMenuPlayTools = new QPopupMenu(contextMenu);
    contextMenuSyncTools = new QPopupMenu(contextMenu);

    // Add the slot tools menu.
    contextMenu->insertItem("S&lot Tools", contextMenuSlotTools);
    show_slot_info_id = contextMenuSlotTools->insertItem("&Show Slot Info", this, SLOT(do_showSlotInfo()));

    // Add the play tools menu.
    contextMenu->insertItem("&Play Tools", contextMenuPlayTools);
    Dreams *twoDMain = dynamic_cast<Dreams *>(mainWin);
    Q_ASSERT(twoDMain);
    rew_id   = contextMenuPlayTools->insertItem(QIconSet(IconFactory::getInstance()->image42), "&Rewind"       , twoDMain->getPtbRewind(), SLOT(animateClick()));
    stepb_id = contextMenuPlayTools->insertItem(QIconSet(IconFactory::getInstance()->image43), "Step &Backward", twoDMain->getPtbStepBackward(), SLOT(animateClick()));
    play_id  = contextMenuPlayTools->insertItem(QIconSet(IconFactory::getInstance()->image44), "&Play"         , twoDMain->getPtbPlay(), SLOT(animateClick()));
    stop_id  = contextMenuPlayTools->insertItem(QIconSet(IconFactory::getInstance()->image45), "&Stop"         , twoDMain->getPtbStop(), SLOT(animateClick()));
    stepf_id = contextMenuPlayTools->insertItem(QIconSet(IconFactory::getInstance()->image46), "Step For&ward" , twoDMain->getPtbStepForward(), SLOT(animateClick()));
    forw_id  = contextMenuPlayTools->insertItem(QIconSet(IconFactory::getInstance()->image47), "F&orward"      , twoDMain->getPtbForward(), SLOT(animateClick()));

    // Synchronization tools.
    contextMenu->insertItem("&Sync Tools", contextMenuSyncTools);
    contextMenuSyncTools->insertItem("Sync To This &Cycle", this, SLOT(do_syncToCycle()));
    contextMenuSyncTools->insertItem("Sync To This Cycle (same &trace)", this, SLOT(do_syncToCycleTrace()));

    // Free window.
    contextMenu->insertSeparator();
    rebel_id = contextMenu->insertItem("&Free Window", this, SLOT(do_switchRebel()));
    // by default MDI is NOT rebel 
    contextMenu->setItemChecked(rebel_id, false);
}

void
MDIWatchWindow::showContextMenu(int row, int col, const QPoint &point)
{
    // We record where the popup has been requested
    popUpRow = row;

    // launch popup menu
    contextMenu->exec(point);
}

void
MDIWatchWindow::do_showSlotInfo()
{
    TRACK_ID trackId = watchData->getTrackIdForCell(popUpRow);
    Q_ASSERT(trackId != TRACK_ID_INVALID);

    TagDialogImpl td(draldb, trackId, currentCycle, this, "etd", true);
    td.exec();
}

void
MDIWatchWindow::do_syncToCycle()
{
    emit syncToCycle(currentCycle);
}

void
MDIWatchWindow::do_syncToCycleTrace()
{
    emit syncToCycleTrace(currentCycle);
}

void
MDIWatchWindow::do_switchRebel()
{
    mdiLocked = !mdiLocked;
    contextMenu->setItemChecked(rebel_id, !mdiLocked);
    if(mdiLocked)
    {
        setIcon(IconFactory::getInstance()->image32);
        setIconText("Locked");
        ptblSlots->setFrameStyle(QFrame::Panel | QFrame::Raised);
        ptblSlots->setLineWidth(style().pixelMetric(QStyle::PM_DefaultFrameWidth, this));
    }
    else
    {
        setIcon(IconFactory::getInstance()->image33);
        setIconText("Unlocked");
        ptblSlots->setFrameStyle( QFrame::Box | QFrame::Raised );
        ptblSlots->setLineWidth(3);
    }
}

bool
MDIWatchWindow::init()
{
    if(!MDIWindow::init())
    {
        return false;
    }
    // Override the set done by the default MDIWindow::init
    setCentralWidget(pqvbLayout);
    return true;
}

void
MDIWatchWindow::setWatchWindow(WatchWindow *window)
{
    Q_ASSERT(window != NULL);
    watchData = window;

    // Configure the table
    window->initDraw(ptblSlots);
    UINT16 dralId = window->getDralId();
    UINT16 clockId;

    if(window->getWatchType() == WatchWindow::Node)
    {
        DralGraphNode * node = draldb->getNode(dralId);
        clockId = node->getClockId();
    }
    else if(window->getWatchType() == WatchWindow::Edge)
    {
        DralGraphEdge * edge = draldb->getEdge(dralId);
        clockId = edge->getClockId();
    }
    else
    {
        Q_ASSERT(false);
    }

    currentCycle = draldb->getFirstEffectiveCycle(clockId);

    psldCycle->setMinValue(cycleToSliderValue(draldb->getFirstEffectiveCycle(clockId)));
    psldCycle->setMaxValue(cycleToSliderValue(draldb->getLastEffectiveCycle(clockId)));
    psldCycle->setTickInterval(10);

    showCycle(currentCycle);
}

void
MDIWatchWindow::attendCycleChanged(INT64 ps)
{
    displacement += ps;
    CYCLE cycle = getAccCycleChanged(currentCycle.clock, (currentCycle.clock->getDivisions() > 1));

    currentCycle.fromPs(currentCycle.toPs() + cycle.toPs());
    showCycle(currentCycle);
}

/**
 * Public slot that attends the signal of the slider being pressed.
 */
void
MDIWatchWindow::sliderPressed()
{
    sliderDrag = true;
}

/**
 * Public slot that attends the signal of the slider being released.
 */
void
MDIWatchWindow::sliderReleased()
{
    sliderDrag = false;
}

/**
 * Public slot that attend the timer signal
 */
void
MDIWatchWindow::nextFrame()
{
    psldCycle->setValue(cycleToSliderValue(currentCycle) + (nextCycle * nCPF));
    watchData->draw(ptblSlots, currentCycle);

    if(focusMDI == this)
    {
        emit cycle_msg_changed(getCycleString());
    }
}

/**
 * Public slot that attend the timer signal
 */
void
MDIWatchWindow::setFrame(int cycle)
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
        watchData->draw(ptblSlots, currentCycle);

        if(focusMDI == this)
        {
            emit cycle_msg_changed(getCycleString());
        }

        emit cycleChanged(static_cast<Q_INT64>(tempCycle.toPs()));
    }

    lastValue = cycle;
}

/**
 * Method to display the current cycle in all the widgdegs shown in the window
 * 
 * @param cycle current cycle to show.
 */
void
MDIWatchWindow::showCycle(CYCLE cycle)
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

    // We display the new cycle
    watchData->draw(ptblSlots, currentCycle);

    if(focusMDI == this)
    {
        emit cycle_msg_changed(getCycleString());
    }
}

void
MDIWatchWindow::play(void)
{
    nextCycle = 1;
    fastStep = 1;
    pTimer->start(BASE_INTERVAL / nFPS);
}

void
MDIWatchWindow::stepForward(void)
{
    pTimer->stop();
    nextCycle = 1;
    fastStep = 1;
    nextFrame();
}

void
MDIWatchWindow::stepBackward(void)
{
    pTimer->stop();
    nextCycle = -1;
    fastStep = 1;
    nextFrame();
}

void
MDIWatchWindow::stop(void)
{
    pTimer->stop();
}

void
MDIWatchWindow::forward(void)
{
    nextCycle = 1;
    fastStep = 2;
    pTimer->changeInterval(BASE_INTERVAL / (nFPS * fastStep));
}

void
MDIWatchWindow::rewind(void)
{
    nextCycle = -1;
    fastStep = 2;
    pTimer->changeInterval(BASE_INTERVAL / (nFPS * fastStep));
}

/**
 * Sets the velocity of the animation based on the standard
 *
 * @param Total amount of cycles of the animation
 */
void
MDIWatchWindow::setFPS(const QString &fps)
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
MDIWatchWindow::setCPF(const QString &cpf)
{
    QString sTmp = cpf;
    // Remove the first space
    sTmp.remove(0, 1);
    sTmp.remove(sTmp.length() - 4, 4);
    nCPF = sTmp.toInt();
}

void
MDIWatchWindow::closeEvent(QCloseEvent *e)
{
    emit mdiCloseEvent(this);
    e->accept();
}

void
MDIWatchWindow::setTableSize(void)
{
    // The size is not changed if maximized.
    if(isMaximized())
    {
        return;
    }

    bool bHeightCut = false;

    //
    // Compute the initial height of the MDI window
    //

    // Obtain the height of the horizontal header
    INT32 hhHeight = ptblSlots->horizontalHeader()->sectionRect(0).height();
    INT32 vhHeight = ptblSlots->verticalHeader()->headerWidth();

    INT32 height = 2 * hhHeight + vhHeight;
    if(height > myParent->height())
    {
        bHeightCut = true;
        height = myParent->height() / 2;
    }

    //
    // Compute the initial height of the MDI window
    //

    // Obtain the witdh of the vertical header
    INT32 vhWidth = ptblSlots->verticalHeader()->sectionRect(0).width();
    INT32 width = (bHeightCut ? 20 : 10) + vhWidth + ptblSlots->horizontalHeader()->headerWidth();

    // Set the size
    resize(width, height);
}

void
MDIWatchWindow::setFocus()
{
    emit cycle_msg_changed(getCycleString());
    emit row_msg_changed("");
    emit status_msg_changed("");
}

QString
MDIWatchWindow::getCycleString() const
{
    QString temp;

    if(currentCycle.division == 0)
    {
        temp = " High";
    }
    else
    {
        temp = " Low";
    }

    temp = " Cycle : " + QString::number(currentCycle.cycle) + temp + " @ " + currentCycle.clock->getFormattedFreq(CLOCK_GHZ) + " ";
    return temp;
}
