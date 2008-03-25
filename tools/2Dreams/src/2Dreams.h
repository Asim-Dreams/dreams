// ==================================================
// Copyright (C) 2003-2006 Intel Corporation
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
  * @file  2Dreams.h
  * @brief Defines 2Dreams Main class
  */


#ifndef _2DREAMS_H
#define _2DREAMS_H

// QT Library
#include <qnamespace.h>
#include <qdom.h>
#include <qlayout.h>
#include <qaction.h>
#include <qtextedit.h>
#include <qnetwork.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qassistantclient.h>
#include <qptrlist.h>

// DREAMS Library
#include "asim/dralClient.h"

// AGT Library
#include "asim/Main.h"
#include "asim/DralDB.h"
#include "asim/Q2DMAction.h"

// 2Dreams
#include "2DreamsDefs.h"
#include "DreamsDB.h"
#include "TwoDAvtView.h"
#include "PreferenceMgr.h"
#include "IOMgr.h"
#include "LogDockWin.h"
#include "DisDockWin.h"
#include "EventDockWin.h"
#include "AncestorDockWin.h"
#include "findImpl.h"
#include "kxesyntaxhighlighter.h"
#include "textOverviewImpl.h"
#include "tagDescDialogs.h"
#include "WebMimeSF.h"
#include "Help2DWindow.h"
#include "MDI2DWindow.h" 
#include "TagSelector.h"

/**
  * @brief
  * This class holds all the command line parameter information
  * of the program.
  *
  * @description
  * This class is needed because
  *
  * @version 0.1
  * @date started at 2004-06-08
  * @author Guillem Sole
  */
class DreamsLineParams
{
    public:

        /**
          * Sets the initial values of the attributes of the class.
          *
          * @return new object.
          */
        DreamsLineParams()
        {
            adf_file = QString::null;
            drl_file = QString::null;
            has_adf = false;
            has_drl = false;
            has_lde = false;
            has_no_gui = false;
        }

        /**
          * Parses the command line parameters of Dreams.
          *
          * @return if the parameters are correct.
          */
        bool
        parseLineParams(int argc, char * argv[])
        {
            INT32 act_arg; ///< Used to index argv to obtain the actual parameter.
            QString arg;   ///< Used to read the actual argument.

            // Skips the application name parameter.
            act_arg = 1;
            argc--;

            // Reads all the parameters.
            while(argc > 0)
            {
                arg = QString(argv[act_arg]);

                // ADF flag.
                if(arg == "-adf")
                {
                    // Just can be set one time.
                    if(has_adf)
                    {
                        printf("Input error: -adf flag set twice\n");
                        return false;
                    }

                    // The name of the adf file must be specified.
                    if(argc < 2)
                    {
                        printf("Input error: -adf flag with no file\n");
                        return false;
                    }

                    adf_file = QString(argv[act_arg + 1]);
                    has_adf = true;
                    argc--;
                    act_arg++;
                }

                // Load, Dump and exit argument.
                else if(arg == "-lde")
                {
                    // Just can be set one time.
                    if(has_lde)
                    {
                        printf("Input error: -lde flag set twice\n");
                        return false;
                    }

                    has_lde = true;
                }

                // No GUI argument.
                else if(arg == "-no-gui")
                {
                    // Just can be set one time.
                    if(has_no_gui)
                    {
                        printf("Input error: -no-gui flag set twice\n");
                        return false;
                    }

                    has_no_gui = true;
                }

                // Drl trace file argument.
                else
                {
                    if(argv[act_arg][0] == '-')
                    {
                        printf("Input error: invalid argument %s\n", argv[act_arg]);
                        return false;
                    }
                    else
                    {
                        // Just can be set one time.
                        if(has_drl)
                        {
                            printf("Input error: only one drl trace can be entered\n");
                            return false;
                        }

                        drl_file = QString(argv[act_arg]);
                        has_drl = true;
                    }
                }

                argc--;
                act_arg++;
            }

            // Some coherence asserts.
            if(has_lde)
            {
                // With -lde flag always a drl file must be specified.
                if(!has_drl)
                {
                    printf("Input error: -lde requires a drl file parameter\n");
                    return false;
                }
            }
            if(has_no_gui)
            {
                // -no-gui requires -lde flag.
                if(!has_lde)
                {
                    printf("Input error: -no-gui requires -lde flag\n");
                    return false;
                }
                // -no-gui requires an adf file.
                if(!has_adf)
                {
                    printf("Input error: -no-gui requires an adf file parameter\n");
                    return false;
                }
            }

            return true;
        }

        /**
          * Returns the ADF file name.
          *
          * @return ADF file.
          */
        QString getADFFile()
        {
            return adf_file;
        }

        /**
          * Returns the DRL file name.
          *
          * @return DRL file.
          */
        QString getDRLFile()
        {
            return drl_file;
        }

        /**
          * Returns if the -adf flag is set.
          *
          * @return if an ADF file is set.
          */
        bool hasADFFile()
        {
            return has_adf;
        }

        /**
          * Returns if an input DRL file is set.
          *
          * @return if an DRL file is set.
          */
        bool hasDRLFile()
        {
            return has_drl;
        }

        /**
          * Returns if the -lde flag is set.
          *
          * @return if -lde is set.
          */
        bool LDEMode()
        {
            return has_lde;
        }

        /**
          * Returns if the -no-gui flag is set.
          *
          * @return if -no-gui is set.
          */
        bool noGUIMode()
        {
            return has_no_gui;
        }

    private:

        QString adf_file; ///< File name of the adf used.
        QString drl_file; ///< File name of the drl trace used.
        bool has_adf;     ///< User has defined an adf.
        bool has_drl;     ///< User has defined a drl trace.
        bool has_lde;     ///< Load, Dump and Exit mode.
        bool has_no_gui;  ///< GUI Enabled. If disabled lde must be set.
} ;

/**
  * This class create the fundamental application objects to launch 2Dreams.
  *
  * Put long explanation here
  * @version 0.6
  * @date started at 2002-04-01
  * @author Federico Ardanaz
  */
class TwoDreams : public Main
{
    Q_OBJECT

public:

    TwoDreams(DreamsLineParams p, QWidget* parent=0, const char* name=0, WFlags f=0);
    ~TwoDreams();

    char* getApplicationCaption();
    AScrollView* getAScrollViewObject();
    void initApplication();

    void do_makeStyle (const QString &style);
    void repaintAllMDIContents(bool erase);
    
public:
    static void closeSplash();
    static void set_splash_status( const QString &txt );
    static QLabel* showSplash();

public:
    void enableMenuPurgeLast(bool value);
    void enableMenuShowCriteria(bool value);
    void enableMenuPurgeAll(bool value);
    void enableCloseMenu(bool value);
    void enableMenuOptions(bool openedFile);
    void saveDockingState();
    void ensureRScanDWinVisible();
    void addFileToolBar();
    
public slots:
    void asvMoved(double x,double y);
    void asvZoomed(double x, double y);
    void newZoomWindow(int,int,int,int,double,double,int,int);
    void toggledShadowCol(int,bool);
    void clearAllShadedCols();
    void contentsSelectedEvent(int,int,bool,QString,HighLightType,bool);
    void purgeLastHighlight();
    void purgeAllHighlight();
    void loadFinished(void);
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
    void do_genericSave();
    void do_genericPrint();
    void do_genericClose();
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
    void do_about_2dreams();
    void about2d_clicked();
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
    
    // window
    void addWindowMenu();
    void do_switchDisWin();
    void do_switchEventWin();
    void do_switchAncWin();
    void do_switchLogWin();
    void do_newWindow();
    void do_newFWindow();
    void do_tileHorizontal();
    void do_tileVertical();
    void do_cascade();
    
    void disWinVisibilityChanged(bool visible);
    void disWinOrientationChanged(Orientation);
    void disWinPlaceChanged(QDockWindow::Place);

    void eventWinVisibilityChanged(bool visible);
    void eventWinOrientationChanged(Orientation);
    void eventWinPlaceChanged(QDockWindow::Place);

    void ancWinVisibilityChanged(bool visible);
    void ancWinOrientationChanged(Orientation);
    void ancWinPlaceChanged(QDockWindow::Place);

    void logWinVisibilityChanged(bool visible);
    void logWinOrientationChanged(Orientation);
    void logWinPlaceChanged(QDockWindow::Place);

    void dockWindowPositionChanged(QDockWindow*);

    void aboutToQuit ();
    void lastWindowClosed ();
    void dock_timer();

    void slotTextChanged ( const QString & string );
    void slotTgSelComboActivated( const QString &s );
    
protected:
    void closeEvent(QCloseEvent *);
    QString getWelcomeMessage();
    
private:
    void addHighlightMenu();
    void addTagSelector (QToolBar* parent);

    // docked windows
    void createLogWindow();
    void createAbout2D();
    void createAboutDRALDB();
    MDIWindow* newView();
    void reset();
    
private:
    static QLabel* splash;

private:    // docked windows stuff
    LogDockWin* logWin;

private:    
    int numNextWindow;
    QString sfn;
    
    QPopupMenu* highlight;
    QPopupMenu* view_highlight;
    QPopupMenu* window_docked;
    Q2DMAction* actionVHIGH;
    Q2DMAction* actionVNHIGH;
    Q2DMAction* actionVBHIGH;
    QActionGroup *grp;

    Q2DMAction* actionFullScreen;
    Q2DMAction* actionClearShadedColums;
    Q2DMAction* actionResetRelationshipScan;
    Q2DMAction* actionGenericOpen;
    Q2DMAction* actionGenericClose;
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
    
    int dis_win_id;
    int event_win_id;
    int anc_win_id;
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
    int nfview_win_id;

    bool forcedShutdown;

    bool fullScreenMode;
    QValueList<int>* fstList;

private:    // 2Dreams components (dbase, etc)
    IOMgr* iomgr;
    DralDB*        draldb;
    DreamsDB*      dreamsDB;
    PreferenceMgr* prefMgr;
    findImpl*      fndDialog;
    QFileDialog*   sessionFileDlg;

private:           // parameter parsing
    DreamsLineParams params;

    textOverviewImpl *grte;
    textOverviewImpl *adfte;
    KXESyntaxHighlighter *sh;
    TagSelector*    tgsel;

private:    // 2Dreams about stuff
    QDialog* about2d;
    QMessageBox* draldbabout;
    QGridLayout* vb;
    QPixmap dreamsIcon;
    QLabel* logo;
    QTextBrowser*  browser;
    QPushButton*   AcceptButton;
    
private: // help browser stuff
    Help2DWindow *help;
    WebMimeSF* webMimeSourceFactory;    
};

#endif


