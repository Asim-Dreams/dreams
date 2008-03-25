// ==================================================
// Copyright (C) 2004-2006 Intel Corporation
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

/*
 * @file  Dreams.h
 * @brief Defines Dreams Main class
 */

#ifndef _DREAMS_H
#define _DREAMS_H

// General includes.
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <list>

// qt
#include <qglobal.h>

using namespace std;

// Dreams includes.
#include "highlight/HighLightMgr.h"

// Asim includes.
#include "asim/Main.h"

// Class forwarding.
class IOMgr;
class DreamsDB;
class PreferenceMgr;
class LogDockWin;
class DralDB;
class TempDir;
class MDIFloorplanWindow;
class TagSelector;
class TextOverviewDialogImpl;
class KXESyntaxHighlighter;
class HelpDreamsWindow;
class MDIDreamsWindow;

/*
 * @brief
 * This class holds all the command line parameter information
 * of the program.
 *
 * @description
 * This class parses all the command line parameters and stores
 * the gathered information.
 *
 * @version 0.1
 * @date started at 2004-06-08
 * @author Guillem Sole
 */
class DreamsLineParams
{
  public:
    DreamsLineParams();

    bool parseLineParams(int argc, char * argv[]);

    void setADFFile(QString file);

    QString getADFFile() const;
    void popADFFile();
    QString getDRLFile() const;
    void popDRLFile();
    bool hasADFFile() const;
    bool hasDRLFile() const;
    bool LDEMode() const;
    bool noGUIMode() const;
    bool DEVMode() const;
    UINT32 expDebugLevel() const;

    void reset(void);

  private:
    list<QString> adf_file; ///< List of file names of the adf used.
    list<QString> drl_file; ///< List of file names of the drl traces used.
    bool has_lde;           ///< Load, Dump and Exit mode.
    bool has_no_gui;        ///< GUI Enabled. If disabled lde must be set.
    bool has_dev;           ///< Developer mode.
    UINT32 expLevel;        ///< Debugging Levelfor expressions (0=off, 1=on, larger=more verbose) 
};

/*
 * @brief
 * This class holds all the information related to one trace.
 *
 * @description
 * this class is used to add and remove all the information and
 * GUI objects related to one trace.
 *
 * @version 0.1
 * @date started at 2005-03-09
 * @author Guillem Sole
 */
class DreamsTraceData
{
  public:
    DreamsTraceData(DreamsDB * _dreamsdb, INT32 _id);
    ~DreamsTraceData();

    DreamsDB * getDreamsDB() const;
    QPopupMenu * getWindowMenu() const;
    INT32 getWindowMenuId() const;
    QPopupMenu * getWatchWindowMenu() const;
    INT32 getCloseMenuId() const;
    INT32 getId() const;

    void setWindowMenu(QPopupMenu * value);
    void setWindowMenuId(INT32 value);
    void setWatchWindowMenu(QPopupMenu * value);
    void setCloseMenuId(INT32 value);
    void decId();

  private:
    DreamsDB * dreamsdb;       ///< Pointer to the trace.
    QPopupMenu * window_menu;  ///< Pointer to the new window menu.
    QPopupMenu * watch_window; ///< Pointer to the watch window menu.
    INT32 window_menu_id;      ///< Id inside the window menu of the new window menu.
    INT32 close_menu_id;       ///< Id inside the close menu.
    INT32 id;                  ///< Id of the trace. Decremented if a previous trace is deleted.
} ;

typedef list<DreamsTraceData *> TraceList;
typedef list<DreamsTraceData *>::iterator TraceListIter;

/**
  * This class create the fundamental application objects to launch Dreams.
  *
  * Put long explanation here
  * @version 0.6
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class Dreams : public Main
{
    Q_OBJECT

  public:

    Dreams(DreamsLineParams p, QWidget* parent=0, const char* name=0, WFlags f=0);
    ~Dreams();

    char* getApplicationCaption();
    AScrollView* getAScrollViewObject();
    void initApplication();

    void do_makeStyle (const QString &style);
    void repaintAllMDIContents(bool erase);
    
    static void closeSplash();
    static void set_splash_status( const QString &txt );
    static QLabel* showSplash();

    void enableMenuPurgeLast(bool value);
    void enableMenuShowCriteria(bool value);
    void enableMenuPurgeAll(bool value);
    void enableMenuOptions(bool openedFile);
    void enableAncestorMenu(bool value);
    void enableHighlightSubMenu(bool value);
    void saveDockingState();
    void ensureRScanDWinVisible();
    void addFileToolBar();

    inline QToolButton * getPtbRewind() const;
    inline QToolButton * getPtbStepBackward() const;
    inline QToolButton * getPtbStop() const;
    inline QToolButton * getPtbPlay() const;
    inline QToolButton * getPtbStepForward() const;
    inline QToolButton * getPtbForward() const;

    inline TagSelector * getHighLightTagSelector() const;
    inline TagSelector * getItemSyncTagSelector() const;

    MDIWindow * newResourceDView(DreamsDB * trace);
    MDIWindow * newFloorplanDView(DreamsDB * trace);
    MDIWindow * newWatchDView(DreamsDB * trace, QString watch_string);
    MDIWindow * newWaterfallDView(DreamsDB * trace);

  public slots:
    void asvZoomed(double x, double y);
    void newZoomWindow(int,int,int,int,double,double,int,int);
    void toggledShadowCol(int,bool);
    void clearAllShadedCols();
    void contentsSelectedEvent(TAG_ID tagId, UINT64 tagValue, bool adding, HighLightType htype, bool noErase);
    void purgeLastHighlight();
    void purgeAllHighlight();
    void unhighlightCriteria(TAG_ID tagId, UINT64 tagValue);
    void loadFinish(void);
    void loadCancel(void);
    void highlightChanged(int);
    void mdiWindowActivated(QWidget*);
    void mdiCloseEvent(MDIWindow* mdi);
    void currentAnnItemChanged(AnnotationItem*);
    void annotationAdded(AnnotationItem*);
    void annotationRemoved(AnnotationItem*);
    void annotationChanged(AnnotationItem*,AnnotationItem*);
    
    // file
    void addFileMenu();
    void do_genericOpen();
    void do_genericOpenADF();
    void do_genericEmbeddedADF();
    void do_genericSave();
    void do_genericPrint();
    void do_genericClose();
    void do_genericTraceClose(int);
    void do_closeAll();
    void do_quit();
    void do_saveSession();
    void do_loadSession();
    void quit();

    // edit
    void addEditMenu();
    void do_find();
    void do_find_next();
    void do_go_to_cycle();
    void do_copy();
    void do_paste();
    void do_cut();
    void do_remove();

    // view
    void addViewMenu();
    void do_switchFullScreen();

    // help
    void do_help();
    void addHelpMenu();
    void do_about_dreams();
    void aboutd_clicked();
    void do_bug_report();
    void do_about_draldb();
    
    // tools
    void addToolsMenu();
    void do_preferences();
    void do_disableLastHighlight();
    void do_disableAllHighlight();
    void do_showHighlightCriteria();
    void do_highlightViewChanged( QAction* );
    void do_selectEvents();
    void do_clearShadedColumns();
    void do_memrep();
    void do_dbstats();
    void do_resetRelationshipScan();
    void do_viewadf();
    void do_viewdralgraph();
    void do_viewtagdesc();
    
    // Floorplan toolbar
    void do_rewindFloorplan();
    void do_stepBackward(void);
    void do_playFloorplan();
    void do_stopFloorplan();
    void do_stepForward(void);
    void do_forwardFloorplan();
    void setFPS(const QString &fps);
    void setCPF(const QString &fps);
    void clockNonClockedViews(void);

    // view movement synch
    void do_cycleChanged(Q_INT64 ps);
    void do_resourceChanged(double resource);
    void do_itemChanged(double item);
    void do_threeDMoved(double x, double y);
    void do_fourDMoved(double x, double y, double z);

    void do_syncToCycle(CYCLE cycle);
    void do_syncToCycleTrace(CYCLE cycle);
    void do_syncToItem(QString edgeName, QString fromNodeName, QString toNodeName, QString tagName, UINT64 tagValue);

    // window
    void addWindowMenu();
    void addWatchWindowMenu(QPopupMenu * watch_window, INT32 watch_win_id);
    void do_switchDisWin();
    void do_switchTagWin();
    void do_switchAncWin();
    void do_switchPushWin();
    void do_switchLogWin();
    void do_newResourceWindow();
    void do_newFloorplanWindow();
    void do_newWaterfallWindow();
    void do_newWatchWindow(int id);
    void do_closeAllTraceWindows();
    void do_tileHorizontal();
    void do_tileVertical();
    void do_cascade();
    void do_windowChanged(int id);
    
    void disWinVisibilityChanged(bool visible);
    void disWinOrientationChanged(Orientation);
    void disWinPlaceChanged(QDockWindow::Place);

    void tagWinVisibilityChanged(bool visible);
    void tagWinOrientationChanged(Orientation);
    void tagWinPlaceChanged(QDockWindow::Place);

    void ancWinVisibilityChanged(bool visible);
    void ancWinOrientationChanged(Orientation);
    void ancWinPlaceChanged(QDockWindow::Place);

    void pushWinVisibilityChanged(bool visible);
    void pushWinOrientationChanged(Orientation);
    void pushWinPlaceChanged(QDockWindow::Place);

    void logWinVisibilityChanged(bool visible);
    void logWinOrientationChanged(Orientation);
    void logWinPlaceChanged(QDockWindow::Place);

    void dockWindowPositionChanged(QDockWindow*);

    void aboutToQuit ();
    void lastWindowClosed ();
    void dock_timer();

    void slotTextChanged ( const QString & string );
    void slotTgSelComboActivated( const QString &s );
    
    // Floorplan animation event
    void firstCycleReached(MDIWindow *stopped);
    void lastCycleReached(MDIWindow *stopped);

    void setCurrentClock(const ClockDomainEntry * clock);
signals:
    void status_msg_changed_signal(QString);
    void cycle_msg_changed_signal(QString);
    void row_msg_changed_signal(QString);

protected:
    void closeEvent(QCloseEvent *);
    QString getWelcomeMessage();
    
private:
    void addHighlightMenu();
    void addTagSelector (QToolBar* parent);

    // Floorplan toolbar
    void createFloorplanToolbar();

    // docked windows
    void createLogWindow();
    void createAboutDreams();
    void createAboutDRALDB();
    
    void reset();

    bool isLoading() const;
    void setLoading(bool value);

public:
    DreamsLineParams params;

private:
    static QLabel* splash;

private:    // docked windows stuff
    LogDockWin* logWin;

private:    
    int numNextWindow;
    int numPlayWindow;
    int numFloorplanWindowStopped;
    int numWFWindow;

    struct rusage ini, end;
    
    QPopupMenu* highlight;
    QPopupMenu* view_highlight;
    QPopupMenu* window_docked;
    QPopupMenu* trace_close;
    Q2DMAction* actionVHIGH;
    Q2DMAction* actionVNHIGH;
    Q2DMAction* actionVBHIGH;
    QActionGroup *grp;

    Q2DMAction* actionFullScreen;
    Q2DMAction* actionClearShadedColums;
    Q2DMAction* actionResetRelationshipScan;
    Q2DMAction* actionGenericOpen;
    Q2DMAction* actionGenericPrint;
    Q2DMAction* actionSaveSession;
    Q2DMAction* actionLoadSession;
    Q2DMAction* actionExit;
    Q2DMAction* actionFind;
    Q2DMAction* actionFindNext;
    Q2DMAction* actionHelp;
    Q2DMAction* actionGotoCycle;
    Q2DMAction* actionCloseAll;
    Q2DMAction* actionNewWindow;

    QToolButton* saveSBtn;
    QToolButton* closeMDIBtn;
    
    // Floorplan ToolBars

    QToolBar    *playTools;
    QToolButton *ptbRewind;
    QToolButton *ptbStepBackward;
    QToolButton *ptbStop;
    QToolButton *ptbPlay;
    QToolButton *ptbStepForward;
    QToolButton *ptbForward;
    QComboBox   *pcmbFPS;
    QComboBox   *pcmbCPF;

    // Clock synchronization
    QTimer      *pTimer;            ///< This timer is used to synch the non clocked views with the clocked ones

    UINT32 nFPS;                    ///< Velocity to update the non-clocked elements
    UINT32 nCPF;

    INT32 nextCycle;                   ///< Direction of the time
                                       ///  If 1, forward, if -1 backward
    const ClockDomainEntry * curClock; ///< The current clock domain.

    UINT32 fastStep;                ///< Indicates if we are playng at normat rate (play) or fast (rewind/forward)

    // Windows ids
    int dis_win_id;
    int tag_win_id;
    int anc_win_id;
    int push_win_id;
    int log_win_id;

    int find_id;
    int find_next_id;
    int goto_id;

    int dis_shade_id;

    int actionVHIGH_id;
    int actionVNHIGH_id;
    int actionVBHIGH_id;
    int highlightSubMenu_id;
    int mem_rep_id;
    int dbstats_id;
    int adf_id;
    int dral_id;
    
    int toolshighlight_id;
    int dis_last_id;
    int dis_all_id;
    int show_criteria_id;
    int save_session_id;
    int load_session_id;
    int reset_rscan_id;
    int fullScreen_id;
    int tgdesc_id;
    int nview_win_id;
    int closeAll_id; 
    int tileh_win_id;
    int tilev_win_id;
    int cascade_win_id;
    //int nfview_win_id;
    int watch_win_id;


    bool forcedShutdown;

    bool fullScreenMode;
    QValueList<int>* fstList;

  private:
    TraceListIter findTraceFromDB(DreamsDB * trace);
    TraceListIter findTraceFromWindow(INT32 window_id);
    TraceListIter findTraceFromClose(INT32 window_id);
    void removeTrace(DreamsDB * trace);

    INT32 trace_menu_id; ///< Saves the last trace menu id.

    // Dreams components (dbase, etc)
    DreamsDB * dreamsdb; ///< Actual dreams trace.
    TraceList traces;    ///< Set of opened traces.
    PreferenceMgr * prefMgr;
    QFileDialog *   sessionFileDlg;
    QFileDialog *   fdlg;
    LogMgr *        log;
    bool            loading;

private:           // parameter parsing
    TempDir * temp;
    TextOverviewDialogImpl * grte;
    TextOverviewDialogImpl * adfte;
    KXESyntaxHighlighter * sh;
    TagSelector * tgsel;
    TagSelector * syncTagSel;

private:    // Dreams about stuff
    QDialog* aboutd;
    QMessageBox* draldbabout;
    QGridLayout* vb;
    QPixmap dreamsIcon;
    QLabel* logo;
    QTextBrowser*  browser;
    QPushButton*   AcceptButton;
    
private: // help browser stuff
    HelpDreamsWindow *help;
};

QToolButton *
Dreams::getPtbRewind() const
{
    return ptbRewind;
}

QToolButton *
Dreams::getPtbStepBackward() const
{
    return ptbStepBackward;
}

QToolButton *
Dreams::getPtbStop() const
{
    return ptbStop;
}

QToolButton *
Dreams::getPtbPlay() const
{
    return ptbPlay;
}

QToolButton *
Dreams::getPtbStepForward() const
{
    return ptbStepForward;
}

QToolButton *
Dreams::getPtbForward() const
{
    return ptbForward;
}

TagSelector *
Dreams::getHighLightTagSelector() const
{
    return tgsel;
}

TagSelector *
Dreams::getItemSyncTagSelector() const
{
    return syncTagSel;
}

#endif
