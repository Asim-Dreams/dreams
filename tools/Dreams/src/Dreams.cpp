/*
 * Copyright (C) 2004-2006 Intel Corporation
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

/**
  * @file Dreams.cpp
  */

// General includes.
#include <stdlib.h>
#include <iostream>
#include <string>

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDB.h"
#include "adf/ADFMgr.h"
#include "avt/ResourceDAvtView.h"
#include "avt/WaterfallDAvtView.h"
#include "avt/FloorplanDAvtView.h"
#include "dialogs/find/FindDialogImpl.h"
#include "dialogs/gotocycle/GoToCycleDialogImpl.h"
#include "dialogs/textoverview/TextOverviewDialogImpl.h"
#include "dialogs/viewchoice/ViewChoiceDialogImpl.h"
#include "dockwin/ancestor/AncestorDockWin.h"
#include "dockwin/disassembly/DisDockWin.h"
#include "dockwin/log/LogDockWin.h"
#include "dockwin/pushout/PushoutDockWin.h"
#include "dockwin/tag/TagDockWin.h"
#include "highlight/HighLightMgrResource.h"
#include "mdi/MDIResourceWindow.h" 
#include "mdi/MDIFloorplanWindow.h" 
#include "mdi/MDIWaterfallWindow.h" 
#include "mdi/MDIWatchWindow.h" 
#include "xpm/d4_logo.xpm"
#include "xpm/selectToolBar.xpm"
#include "Dreams.h"
#include "DreamsDefs.h"
#include "FavoriteTags.h"
#include "DreamsDB.h"
#include "PreferenceMgr.h"
#include "TempDir.h"
#include "TagSelector.h"
#include "kxesyntaxhighlighter.h"
#include "tagDescDialogs.h"
#include "HelpDreamsWindow.h"
#include "preferencesImpl.h"
#include "DumpMode.h"
#include "text2dbrowser.h"
#include "WatchWindow.h"

// Asim includes.
#include <asim/xpm/help_contents.xpm>
#include "asim/Main.h"
#include "asim/Q2DMAction.h"
#include "asim/dralClient.h"
#include "asim/dralCommonDefines.h"

// Qt includes.
#include <qglobal.h>
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
#include <qtoolbar.h>
#include <qcombobox.h>
#include <qnamespace.h>
#include <qdom.h>
#include <qaction.h>
#include <qtextedit.h>
#include <qnetwork.h>
#include <qinputdialog.h>
#include <qassistantclient.h>
#include <qptrlist.h>

// Displacement inside the window menu where the new window
// of each trace is added.
#define NEW_WIN_DISP 3

// for docked windows start-up
bool dockInit;
bool dump_mode;

// For focus correct behaviour.
QWidget * focusMDI;

UINT32 rogerLevel;

// -----------------------------------------------------------------------------
// -- The Main Constructor
// -----------------------------------------------------------------------------
Dreams::Dreams(DreamsLineParams p, QWidget * parent, const char * name, WFlags f)
    : Main(parent, name, f), params(p)
{
    numNextWindow      = 1;
    numPlayWindow      = 0;
    numFloorplanWindowStopped = 0;
    numWFWindow = 0;

    // init all pointers to null
    splash = NULL;
    highlight = NULL;
    view_highlight = NULL;
    actionVHIGH = NULL;
    actionVNHIGH = NULL;
    actionVBHIGH = NULL;
    window_docked=NULL;
    grp = NULL;
    logWin = NULL;
    prefMgr = NULL;
    aboutd = NULL;
    vb = NULL;
    logo = NULL;
    browser = NULL;
    AcceptButton = NULL;
    dreamsdb = NULL;
    actionFullScreen = NULL;
    draldbabout = NULL;
    dis_win_id=-1;
    tag_win_id=-1;
    anc_win_id=-1;
    push_win_id=-1;
    log_win_id=-1;
    dis_last_id=-1;
    dis_all_id=-1;
    show_criteria_id=-1;
    find_id=-1;
    dis_shade_id=-1;
    reset_rscan_id=-1;

    forcedShutdown = false;
    fullScreenMode = false;
    fstList = new QValueList<int>();
    tgsel=NULL;
    syncTagSel = NULL;

    curClock = NULL;
    focusMDI = NULL;

    // --------------------------------------------------------------------
    // Load Dreams preferences
    // --------------------------------------------------------------------
    prefMgr = PreferenceMgr::getInstance("BSSAD");
    prefMgr->setGUIEnabled(true);
    bool ok = prefMgr->loadPreferences();
    if (!ok)
    {
        qWarning("Error detected loading application's preferences!");
    }

    log = LogMgr::getInstance(prefMgr->getLogFileName());
    QRect r;
    r.setX(prefMgr->getMainWindowX());
    r.setY(prefMgr->getMainWindowY());
    r.setWidth(prefMgr->getMainWindowWidth());
    r.setHeight(prefMgr->getMainWindowHeight());

    QRect desk = QApplication::desktop()->geometry();
    QRect inter = desk.intersect( r );
    resize( r.size() );
    if ( inter.width() * inter.height() > ( r.width() * r.height() / 20 ) )
    {
        move( r.topLeft() );
    }

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));
    connect(qApp, SIGNAL(lastWindowClosed()), this, SLOT(lastWindowClosed()));
    connect(this, SIGNAL(dockWindowPositionChanged(QDockWindow *)), this, SLOT(dockWindowPositionChanged(QDockWindow *)));

    connect(this, SIGNAL(status_msg_changed_signal(QString)), this, SLOT(status_msg_changed(QString)));
    connect(this, SIGNAL(cycle_msg_changed_signal(QString)), this, SLOT(cycle_msg_changed(QString)));
    connect(this, SIGNAL(row_msg_changed_signal(QString)), this, SLOT(row_msg_changed(QString)));

    grte = new TextOverviewDialogImpl(NULL);
    grte->textEdit->setReadOnly(true);

    adfte = new TextOverviewDialogImpl(NULL);
    sh = new KXESyntaxHighlighter(adfte->textEdit);
    adfte->textEdit->setReadOnly(true);
    
    temp = TempDir::getInstance();
    // Creates a temporal directory using the sessionId.
    temp->create(qApp->sessionId());

    // Clock synch mechanism init
    nFPS = 1;
    nCPF = 1;
    fastStep = 1;

    // Loading state.
    setLoading(false);

    // Colors.
    AColorPalette::reset();

    // Creates the file dialog.
    QString lastUsedDir = prefMgr->getLastUsedDir();
    fdlg = new QFileDialog(lastUsedDir, "DRAL Event-Files (*.drl *.drl.gz)", this, "opendlg", TRUE);
}

void
Dreams::dock_timer()
{
    dockInit=false;
}

void
Dreams::initApplication()
{
    // --------------------------------------------------------------------
    // Create about dreams objects, logo, etc.
    // --------------------------------------------------------------------
    createAboutDreams();
    createAboutDRALDB();

    // --------------------------------------------------------------------
    // Initialize the log file
    // --------------------------------------------------------------------
    log->clearLogFile();
    log->addLog("Application Launched.");
    log->flush();

    // --------------------------------------------------------------------
    // Init QT Protocol stack
    // --------------------------------------------------------------------
    set_splash_status( "Initializing Network Protocols" );
    qInitNetworkProtocols();

    // --------------------------------------------------------------------
    // Data base low level init
    // --------------------------------------------------------------------
    set_splash_status( "Initializing Internal Structures..." );

    // --------------------------------------------------------------------
    // create the ASIM-Dreams-Scroller
    // --------------------------------------------------------------------
    set_splash_status( "Creating GUI Objects..." );

    // --------------------------------------------------------------------
    // invoke generic (parent) start-up process
    // --------------------------------------------------------------------

    // by now disable save & print & annotation tools
    setAnnotationToolsEnabled(false);
    setGenericSaveEnabled(false);
    setGenericPrintEnabled(false);
    setBookmarkIOEnabled(false);
    Main::initApplication();

    // I don't want the options menu
    menu->removeItem(options_id);
    // I don't want the grid line generic menu
    if (view!=NULL) view->removeItem(view_grid_id);

    // --------------------------------------------------------------------
    // Create global docked windows
    // --------------------------------------------------------------------
    createLogWindow();
    logWin->resize(prefMgr->getLogWindowWidth(),prefMgr->getLogWindowHeight());

    createFloorplanToolbar();

    // --------------------------------------------------------------------
    // put toolbars in saved position
    // --------------------------------------------------------------------
    moveDockWindow (annotationsTools, prefMgr->getAnnotationsToolbarDockPolicy(),
    prefMgr->getAnnotationsToolbarNl(), prefMgr->getAnnotationsToolbarIdx(),
    prefMgr->getAnnotationsToolbarOffset());

    moveDockWindow (viewTools, prefMgr->getViewToolbarDockPolicy(),
    prefMgr->getViewToolbarNl(), prefMgr->getViewToolbarIdx(),
    prefMgr->getViewToolbarOffset());

    moveDockWindow (colorTools, prefMgr->getColorToolbarDockPolicy(),
    prefMgr->getColorToolbarNl(), prefMgr->getColorToolbarIdx(),
    prefMgr->getColorToolbarOffset());
    
    moveDockWindow (fileTools, prefMgr->getFileToolbarDockPolicy(),
    prefMgr->getFileToolbarNl(), prefMgr->getFileToolbarIdx(),
    prefMgr->getFileToolbarOffset());

    moveDockWindow (pointerTools, prefMgr->getToolsToolbarDockPolicy(),
    prefMgr->getToolsToolbarNl(), prefMgr->getToolsToolbarIdx(),
    prefMgr->getToolsToolbarOffset());

    moveDockWindow (mdiTools, prefMgr->getMDIToolbarDockPolicy(),
    prefMgr->getMDIToolbarNl(), prefMgr->getMDIToolbarIdx(),
    prefMgr->getMDIToolbarOffset());

    if (!prefMgr->getShowToolsToolbar()) pointerTools->hide();
    if (!prefMgr->getShowFileToolbar())  fileTools->hide();
    if (!prefMgr->getShowColorToolbar()) colorTools->hide();
    if (!prefMgr->getShowViewToolbar())  viewTools->hide();
    if (!prefMgr->getShowAnnotationsToolbar()) annotationsTools->hide();
    if (!prefMgr->getShowMDIToolbar()) mdiTools->hide();

    window_toolbar->setItemChecked(win_tool_bar_id,prefMgr->getShowToolsToolbar());
    window_toolbar->setItemChecked(win_file_bar_id,prefMgr->getShowFileToolbar());
    window_toolbar->setItemChecked(win_view_bar_id,prefMgr->getShowViewToolbar());
    if (annotationToolsEnabled)
    {
        window_toolbar->setItemChecked(win_annotations_bar_id,prefMgr->getShowAnnotationsToolbar());
        window_toolbar->setItemChecked(win_color_bar_id,prefMgr->getShowColorToolbar());
    }

    // -- add tag selector widget
    addTagSelector(pointerTools);

    // --------------------------------------------------------------------
    // grid lines stuff
    // --------------------------------------------------------------------
    if(asv != NULL)
    {
        asv->setShowHGridLines(prefMgr->getShowHGridLines()); 
        asv->setShowVGridLines(prefMgr->getShowVGridLines());
        asv->setGridLinesSize(prefMgr->getSnapGap());
    }

    // --------------------------------------------------------------------
    // put GUI style
    // --------------------------------------------------------------------
    do_makeStyle(prefMgr->getGuiStyle());

    // disable highlight menus at first
    enableMenuPurgeLast(false);
    enableMenuShowCriteria(false);
    enableMenuPurgeAll(false);


    // --------------------------------------------------------------------
    // --------------------------------------------------------------------
    enableMenuOptions(false);

    // --------------------------------------------------------------------
    // pre-create session load/save dialog
    // --------------------------------------------------------------------
    sessionFileDlg = new QFileDialog(prefMgr->getLastUsedDir(),
                   "2DRAL Session-Files (*.2ds)", this, "sessiondlg",TRUE);


    // --------------------------------------------------------------------
    // help browser
    // --------------------------------------------------------------------
    char* dreams2_dir = getenv("DREAMS2_HELP_DIR");
    QString mypath;
    if (dreams2_dir!=NULL)
    {
        mypath = QString(dreams2_dir);
    }
    else
    {
        //mypath = HELP_REMOTE_PATH;
        mypath = QString("/proj/asim/common/share/2Dreams/help");
    }
    
    help = new HelpDreamsWindow(HELP_INDEX, mypath, NULL, "Dreams Help");
    Q_ASSERT(help!=NULL);
    help->setCaption("Dreams Help Browser");
    DTextBrowser* helpBrowser = help->getTextBrowser();
    helpBrowser->setSource(HELP_INDEX);
    //helpBrowser->reload();
    
    // --------------------------------------------------------------------
    // MDI event handling
    // --------------------------------------------------------------------
    connect(ws, SIGNAL(windowActivated(QWidget*)), this, SLOT(mdiWindowActivated(QWidget*)));
    
    // --------------------------------------------------------------------
    // --------------------------------------------------------------------
    setIcon(IconFactory::getInstance()->image37);
    setIconText("Dreams");
    
    log->addLog("Initialization Finished.");
    set_splash_status( "Initialization Finished." );

}

void
Dreams::createAboutDreams()
{
    QString stros(STR_OS);
#ifdef PLATFORM_IA32_WINDOWS_VS
    stros = "Microsoft Windows";
    WindowsVersion wver = qApp->winVersion();
    char* strwver;
    switch (wver)
    {
        case WV_95:
        strwver = "95";
        break;

        case WV_98:
        strwver = "98";
        break;

        case WV_Me:
        strwver = "Millennium Edition";
        break;

        case WV_NT:
        strwver = "NT";
        break;

        case WV_2000:
        strwver = "2000";
        break;

        case WV_XP:
        strwver = "XP";
        break;

        default;
        strwver = "(Unknown version)";
    }
#endif
    QString cap = QString(getApplicationCaption());
    QString msg;
    msg = msg + "<h2>"+cap+"</h2>" ;
    msg = msg + "<h4>Dreams is a DRAL visualization tool that shows item movements as events on screen.<br>";
    msg = msg + "The Y axis is used to represent different DRAL edges and the X axis for time.</h4>";

    msg = msg + "<h4>Dreams relies on QT, AGT and DRAL libraries.<br>";
    msg = msg + "This tool was developed at BSSAD, Intel Labs Barcelona.</h4>";

    msg = msg + "<h4>Staff:</h4>";
    msg = msg + "<ul>";
    msg = msg + "<li>Design and implementation: Federico Ardanaz";
    msg = msg + "<li>Design and supervision: Roger Espasa";
    msg = msg + "<li>General supervision: Joel Emer";
    msg = msg + "<li>AGT Library: Federico Ardanaz";
    msg = msg + "<li>DRAL Library: Pau Cabre and Roger Gramunt";
    msg = msg + "<li>Database: Federico Ardanaz, Roger Espasa and Guillem Sole";
    msg = msg + "</ul>";

    msg = msg + "<br>Special thanks to Artur Klauser for his support with XML related topics, to Katia Kaeser<br>";
    msg = msg + "for her help with the Dreams logo design and to all the BSSAD team by their beta-testing<br>";
    msg = msg + "support and sugguestions.<br>";
    msg = msg + "<br>";

    msg = msg + "<h4>Build information: </h4>";
    msg = msg + "<ul>";
    msg = msg + "<li>Platform: " STR_PLATFORM ;
    msg = msg + "<li>OS: " + stros;
#ifdef PLATFORM_IA32_WINDOWS_VS
    msg = msg + " " + strwver;
#endif
    msg = msg + "</ul><br>" ;
    msg = msg + "</ul>";

    aboutd = new QDialog(this,"aboutd",true);Q_ASSERT(aboutd!=NULL);
    vb =  new QGridLayout( aboutd, 3, 1, 15,20, "aboutd layout");Q_ASSERT(vb!=NULL);

    //dreamsIcon = QPixmap(dreams2_xpm);
    //dreamsIcon = QPixmap(dreams2_metal2);
    dreamsIcon = QPixmap(d4_logo);
    logo = new QLabel(aboutd);Q_ASSERT(logo!=NULL);
    logo->setPixmap(dreamsIcon);
    vb->addWidget(logo,0,0);

    browser = new QTextBrowser(aboutd,"about text browser");Q_ASSERT(browser!=NULL);
    browser->setText(msg);
    vb->addWidget(browser,1,0);

    AcceptButton = new QPushButton( aboutd, "AcceptButton" );Q_ASSERT(AcceptButton!=NULL);
    AcceptButton->setText("&Dismiss");
    connect (AcceptButton,SIGNAL(clicked()),this,SLOT(aboutd_clicked())) ;
    vb->addWidget(AcceptButton,2,0);
}

void
Dreams::createAboutDRALDB()
{
    QString stros(STR_OS);
#ifdef PLATFORM_IA32_WINDOWS_VS
    stros = "Microsoft Windows";
    WindowsVersion wver = qApp->winVersion();
    char* strwver;
    switch (wver)
    {
        case WV_95:
        strwver = "95";
        break;

        case WV_98:
        strwver = "98";
        break;

        case WV_Me:
        strwver = "Millennium Edition";
        break;

        case WV_NT:
        strwver = "NT";
        break;

        case WV_2000:
        strwver = "2000";
        break;

        case WV_XP:
        strwver = "XP";
        break;

        default;
        strwver = "(Unknown version)";
    }
#endif
    QString msg="";
    msg = msg + "<h2>Dral Data Base Support Library</h2>" ;
    msg = msg + "<h3>DralDB implements a generic purpose database for drl files<br>";
    msg = msg + "designed to have good balance between performance and memory footprint.<br></h3>";
    msg = msg + "<h3>This library was developed at BSSAD,<br>Intel Labs Barcelona</h3>";
    msg = msg + "<h4>Build information: </h4>";
    msg = msg + "<ul>";
    msg = msg + "<li>Library Version: " DRALDB_STR_MAJOR_VERSION "." DRALDB_STR_MINOR_VERSION;
    msg = msg + "<li>Platform: " STR_PLATFORM ;
    msg = msg + "<li>OS: " + stros;
#ifdef PLATFORM_IA32_WINDOWS_VS
    msg = msg + " " + strwver;
#endif
    msg = msg + "</ul><br>" ;
    msg = msg + "</ul>";
    draldbabout = new QMessageBox("DralDB Information",msg,QMessageBox::Information,1, 0, 0, this, 0, FALSE);
    Q_ASSERT(draldbabout!=NULL);
    draldbabout->setButtonText( 1, "Dismiss" );
}

void
Dreams::aboutd_clicked()
{
    aboutd->close();
}

char*
Dreams::getApplicationCaption()
{
  return ("Dreams Visualization Tool - Version " DREAMS_STR_MAJOR_VERSION "."
  DREAMS_STR_MINOR_VERSION "." DREAMS_STR_BUILD_VERSION " - BSSAD Group at Intel Labs Barcelona");
}

AScrollView*
Dreams::getAScrollViewObject()
{
    //return getActiveASV();
    return NULL;
}

// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
void
Dreams::addToolsMenu()
{
    Main::addToolsMenu();
    addHighlightMenu();

    actionClearShadedColums = new Q2DMAction ("clearShadedColumns","&Clear Shaded Columns",CTRL+Key_E,this,"clearShadedColumns");
    actionClearShadedColums->addTo(tools);
    dis_shade_id = actionClearShadedColums->getMenuId();
    connect(actionClearShadedColums, SIGNAL(activated()), this, SLOT( do_clearShadedColumns()));

    actionResetRelationshipScan = new Q2DMAction ("resetRelationshipScan","&Reset Relationship Scan",0,this,"resetRelationshipScan");
    actionResetRelationshipScan->addTo(tools);
    reset_rscan_id = actionResetRelationshipScan->getMenuId();
    connect(actionResetRelationshipScan, SIGNAL(activated()), this, SLOT( do_resetRelationshipScan()));

    tools->insertSeparator();
    tools->insertItem(IconFactory::getInstance()->image6,"Pre&ferences...", this, SLOT(do_preferences()));
    tools->insertSeparator();
    mem_rep_id = tools->insertItem(IconFactory::getInstance()->image21,"&Memory Usage Report", this, SLOT(do_memrep()));
    dbstats_id = tools->insertItem(IconFactory::getInstance()->image16,"&DBase Statistics", this, SLOT(do_dbstats()));
    adf_id = tools->insertItem(IconFactory::getInstance()->image22,"View &ADF", this, SLOT(do_viewadf()));
    dral_id = tools->insertItem(IconFactory::getInstance()->image23,"View Trace DRAL-Graph", this, SLOT(do_viewdralgraph()));
    tgdesc_id = tools->insertItem(IconFactory::getInstance()->image25,"View TAG Descriptions", this, SLOT(do_viewtagdesc()));
}

// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
void
Dreams::addViewMenu()
{
    Main::addViewMenu();

    //view->insertSeparator();
    view_highlight = new QPopupMenu(view);
    Q_ASSERT(view_highlight!=NULL);

    grp = new QActionGroup( this );
    grp->setExclusive( TRUE );
    connect( grp, SIGNAL( selected( QAction* ) ), this, SLOT( do_highlightViewChanged( QAction* ) ) );


    actionVHIGH = new Q2DMAction ( "view_highlighted", "View &Highlighted Events", CTRL+Key_H , grp, "highlighted");
    actionVHIGH->addTo( view_highlight );
    actionVHIGH_id = actionVHIGH->getMenuId(); 
    actionVHIGH->setToggleAction(TRUE);

    actionVNHIGH = new Q2DMAction ( "view_nhighlighted", "View &Non Highlighted Events", CTRL+Key_N , grp, "nhighlighted");
    actionVNHIGH->addTo( view_highlight );
    actionVNHIGH_id = actionVNHIGH->getMenuId(); 
    actionVNHIGH->setToggleAction(TRUE);

    actionVBHIGH = new Q2DMAction ( "view_bhighlighted", "View &All Events", CTRL+Key_A , grp, "bhighlighted");
    actionVBHIGH->addTo( view_highlight );
    actionVBHIGH_id = actionVBHIGH->getMenuId();  
    actionVBHIGH->setToggleAction(TRUE);

    // by default view all
    actionVBHIGH->setOn(true);

    highlightSubMenu_id = view->insertItem("&Highlight...",view_highlight);

    // full screen stuff
    actionFullScreen = new Q2DMAction ("fullscreen",IconFactory::getInstance()->image12,"&Full Screen",CTRL+Key_Space,this,"fullscreen");
    actionFullScreen->addTo(view);
    fullScreen_id = actionFullScreen->getMenuId(); 
    connect(actionFullScreen, SIGNAL(activated()), this, SLOT( do_switchFullScreen()));
}


void
Dreams::addFileMenu()
{
    file = new QPopupMenu(menu);
    Q_ASSERT(file != NULL);

    if(genericOpenEnabled)
    {
        actionGenericOpen = new Q2DMAction("genericOpenDRL", IconFactory::getInstance()->image1, "&Open DRL", CTRL + Key_O, this, "genericOpenDRL");
        actionGenericOpen->addTo(file);
        connect(actionGenericOpen, SIGNAL(activated()), this, SLOT(do_genericOpen()));

        if(params.DEVMode())
        {
            QPopupMenu * open_adf_docked = new QPopupMenu(window);
            Q_ASSERT(open_adf_docked != NULL);

            open_adf_docked->insertItem(IconFactory::getInstance()->image1, "&Open ADF", this, SLOT(do_genericOpenADF()));
            open_adf_docked->insertItem(IconFactory::getInstance()->image0, "Use &Embedded ADF", this, SLOT(do_genericEmbeddedADF()));
            file->insertItem(IconFactory::getInstance()->image1, "O&pen ADF...", open_adf_docked);
        }
    }

    if(genericCloseEnabled)
    {
        trace_close = new QPopupMenu(file);
        file->insertItem(IconFactory::getInstance()->image0, "&Close DRL", trace_close);
    }

    actionCloseAll = new Q2DMAction ("genericClose",IconFactory::getInstance()->image30,"Close &All",0,this,"closeAll");
    actionCloseAll->addTo(file);
    closeAll_id = actionCloseAll->getMenuId(); 
    connect(actionCloseAll, SIGNAL(activated()), this, SLOT(do_closeAll()));

    if (genericPrintEnabled)
    {
        actionGenericPrint = new Q2DMAction ("genericPrint","&Print",CTRL+Key_P,this,"genericPrint");
        actionGenericPrint->addTo(file);
        connect(actionGenericPrint, SIGNAL(activated()), this, SLOT(do_genericPrint()));
    }
    file->insertSeparator();

    actionSaveSession = new Q2DMAction ("saveSession",IconFactory::getInstance()->image4,"&Save Session",CTRL+Key_S,this,"saveSession");
    actionSaveSession->addTo(file);
    save_session_id = actionSaveSession->getMenuId();
    connect(actionSaveSession, SIGNAL(activated()), this, SLOT(do_saveSession()));
    actionLoadSession = new Q2DMAction ("loadSession",IconFactory::getInstance()->image29,"&Load Session",CTRL+Key_L,this,"loadSession");
    actionLoadSession->addTo(file);
    load_session_id = actionLoadSession->getMenuId();
    connect(actionLoadSession, SIGNAL(activated()), this, SLOT(do_loadSession()));
    file->insertSeparator();

    actionExit = new Q2DMAction ("quit",IconFactory::getInstance()->image7,"E&xit",CTRL+Key_Q,this,"quit");
    actionExit->addTo(file);
    connect(actionExit, SIGNAL(activated()), this, SLOT(quit()));

    menu->insertItem("&File", file);
}

// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
void
Dreams::addEditMenu()
{
    Main::addEditMenu();

    // by now disable cut & paste options:
    edit->setItemEnabled(copy_id,false);
    edit->setItemEnabled(paste_id,false);
    edit->setItemEnabled(cut_id,false);
    edit->setItemEnabled(remove_id,false);

    actionGotoCycle = new Q2DMAction ("gotocycle",IconFactory::getInstance()->image17,"&Go To Cycle",CTRL+Key_G,this,"gotocycle");
    actionGotoCycle->addTo(edit);
    goto_id = actionGotoCycle->getMenuId();
    connect(actionGotoCycle, SIGNAL(activated()), this, SLOT(do_go_to_cycle()));

    actionFind = new Q2DMAction ("find",IconFactory::getInstance()->image2,"&Find...",CTRL+Key_F,this,"find");
    actionFind->addTo(edit);
    find_id = actionFind->getMenuId();
    connect(actionFind, SIGNAL(activated()), this, SLOT(do_find()));

    actionFindNext = new Q2DMAction ("findnext","&Find Next",Key_F3,this,"findnext");
    actionFindNext->addTo(edit);
    find_next_id = actionFindNext->getMenuId();
    connect(actionFindNext, SIGNAL(activated()), this, SLOT(do_find_next()));
}

// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
void
Dreams::addHighlightMenu()
{
    highlight = new QPopupMenu(tools);Q_ASSERT(highlight!=NULL);
    dis_last_id = highlight->insertItem("Disable &Last", this, SLOT(do_disableLastHighlight()));
    dis_all_id = highlight->insertItem("Disable &All", this, SLOT(do_disableAllHighlight()));
    show_criteria_id = highlight->insertItem("&Show Criteria", this, SLOT(do_showHighlightCriteria()));

    toolshighlight_id = tools->insertItem("&Highlight...",highlight);
}

// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
void
Dreams::addWindowMenu()
{
    Main::addWindowMenu();
    window_docked = new QPopupMenu(window);
    Q_ASSERT(window_docked!=NULL);

    dis_win_id = window_docked->insertItem("&Item Window", this, SLOT(do_switchDisWin()));
    window_docked->setItemChecked(dis_win_id,prefMgr->getShowDisassemblyWindow());

    tag_win_id = window_docked->insertItem("&Tag Window", this, SLOT(do_switchTagWin()));
    window_docked->setItemChecked(tag_win_id, prefMgr->getShowTagWindow());

    anc_win_id = window_docked->insertItem("&Relationship Window", this, SLOT(do_switchAncWin()));
    window_docked->setItemChecked(anc_win_id,prefMgr->getShowRelationshipWindow());

    push_win_id = window_docked->insertItem("&Pushout Window", this, SLOT(do_switchPushWin()));
    window_docked->setItemChecked(push_win_id, prefMgr->getShowPushoutWindow());

    log_win_id = window_docked->insertItem("&Log Window", this, SLOT(do_switchLogWin()));
    window_docked->setItemChecked(log_win_id,prefMgr->getShowLogWindow());
    window->insertItem("&Docked windows...",window_docked);
    
    window->insertSeparator();
    window->insertSeparator();
    
    connect(window, SIGNAL(highlighted(int)), this, SLOT(do_windowChanged(int)));

    tileh_win_id = window->insertItem(IconFactory::getInstance()->image34,"Tile &Horizontal", this, SLOT(do_tileHorizontal()));
    tilev_win_id = window->insertItem(IconFactory::getInstance()->image35,"Tile &Vertical", this, SLOT(do_tileVertical()));
    cascade_win_id = window->insertItem(IconFactory::getInstance()->image38,"&Cascade", this, SLOT(do_cascade()));
}

void
Dreams::addWatchWindowMenu(QPopupMenu * watch_window, INT32 watch_win_id)
{
    DreamsDB * trace = ((MDIDreamsWindow *) getActiveMDI())->getDreamsDB();
    Q_ASSERT(trace != NULL);
    QDictIterator<WatchWindow> iter = trace->layoutWatchGetWindowIterator();
    WatchWindow *wwindow = NULL;

    watch_window->clear();
    UINT32 nWatch = 0;
    while((wwindow = iter.current()) != NULL)
    {
        ++iter;
        nWatch++;
        watch_window->insertItem(wwindow->getCaption(), this, SLOT(do_newWatchWindow(int)));
    }

    // We enable the option menu if any window is defined
    //cerr << "Dreams::addWatchWindowMenu: nWatch = " << nWatch << endl;
    window->setItemEnabled(watch_win_id, (nWatch != 0));
}

// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
void
Dreams::addHelpMenu()
{
    menu->insertSeparator();
    QPopupMenu* help = new QPopupMenu( menu );Q_ASSERT(help!=NULL);
    help->insertItem("About &QT", this, SLOT(do_about_qt()));
    help->insertItem("About &AGT", this, SLOT(do_about_agt()));
    help->insertItem("About &DralDB", this, SLOT(do_about_draldb()));
    help->insertItem(IconFactory::getInstance()->image25,"About &Dreams", this, SLOT(do_about_dreams()));
    help->insertSeparator();
    help->insertItem("&Bug report...", this, SLOT(do_bug_report()));
    help->insertSeparator();

    actionHelp = new Q2DMAction ("contents",IconFactory::getInstance()->image18,"&Contents",Key_F1,this,"contents");
    actionHelp->addTo(help);
    connect(actionHelp, SIGNAL(activated()), this, SLOT(do_help()));

    menu->insertItem("&Help",help);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_about_dreams()
{
    aboutd->exec();
}

void
Dreams::do_about_draldb()
{
    draldbabout->show();
}


void
Dreams::do_bug_report()
{
    QMessageBox::about (this, "Bug Report Instructions",
     "Please report bugs to dreams@bssad.intel.com\n"
     "including the bug description, the ADF (*.xml) and\n"
     "the event file (*.drl) you have been using.\n"
     "\nThanks in advance:\nDreams development team.\n"    );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_help()
{
    /*
     QMessageBox::about (this, "Dreams Help",
     "Due to a problem with QT 3.0.4 plugin mechanism, the integrated help browser is disabled.\n"
     "As soon as ASIM upgrades to QT 3.0.5 or later the help can be reactivated.\n"
     "By now please open a web browser and go to\n"
     "http://bssad.intel.com/Software/Dreams/help/index.htm.\n"
     "\nDreams development team.\n"    );
     */

    qApp->flushX();
    //qApp->syncX();
    static bool first=true;
    
    DTextBrowser* helpBrowser = help->getTextBrowser();
    if (first)
    {
        helpBrowser->setSource(HELP_INDEX);
        helpBrowser->reload();
        if ( QApplication::desktop()->width() >= 800 && QApplication::desktop()->height() >= 1024 ) help->resize(800,1024);
        first=false;
    }
    
    if ( QApplication::desktop()->width() >= 800 && QApplication::desktop()->height() >= 1024 )
    {
        help->show();
    }
    else
    {
        help->showMaximized();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_genericOpen()
{
    if(isLoading())
    {
        qApp->beep();
        return;
    }

    setLoading(true);

    // Gets the file name and erases it from the list of traces to load.
    QString trace_file = params.getDRLFile();
    params.popDRLFile();

    // If no trace file defined in the parameter command line.
    if(trace_file == QString::null)
    {
        INT32 dresult = QDialog::Accepted;

        dresult = fdlg->exec();
        trace_file = fdlg->selectedFile();

        // Checks that a valid name has been selected.
        if(trace_file != QString::null)
        {
            prefMgr->setLastUsedDir(fdlg->dirPath());
        }

        // If no name or no accepted button is pressed, nothing more is done.
        if((trace_file == QString::null) || (dresult != QDialog::Accepted))
        {
            setLoading(false);
            return;
        }
    }

    // Stetic preventive lbl.
    setCaption((QString(getApplicationCaption())));

    //getrusage(RUSAGE_SELF, &ini);

    // Creates a new dreams database for this trace.
    Q_ASSERT(dreamsdb == NULL);
    dreamsdb = new DreamsDB(trace_file, bkMgr, log);
    connect(dreamsdb, SIGNAL(loadFinishSignal()), this, SLOT(loadFinish()));
    connect(dreamsdb, SIGNAL(loadCancelSignal()), this, SLOT(loadCancel()));

    // Tries to load the dral header.
    if(!dreamsdb->loadDRLHeader())
    {
        setLoading(false);
        delete dreamsdb;
        dreamsdb = NULL;
        logWin->show();
        return;
    }

    // Tries to parse the ADF.
    if(!dreamsdb->parseADF(prefMgr->getArchitectureDefinitionFile(), params.getADFFile()))
    {
        setLoading(false);
        delete dreamsdb;
        dreamsdb = NULL;
        logWin->show();
        return;
    }

    // Erases the actual adf file.
    params.popADFFile();

    // some stetic stuff:
    QString cap = "Dreams - " + dreamsdb->getTraceName();
    setCaption(cap);
    do_resetAspectRatio();
    do_z100();

    // start the reading procss
    dreamsdb->startLoading();
}

/*
 * Opens a dialog to allow the user the selection of the ADF file.
 *
 * @return void.
 */
void
Dreams::do_genericOpenADF()
{
    // Avoids strange conditions.
    if(isLoading())
    {
        qApp->beep();
        return;
    }

    //cerr << "Dreams::do_genericOpenADF: pre ask for adf file name" << endl;
    
    QFileDialog adfdlg(prefMgr->getLastUsedDir(), "Architecture Description Files (*.adf *.xml)", this, "opendlgadf", TRUE);

    // Executes the dialog.
    if(adfdlg.exec() == QDialog::Accepted)
    {
        params.setADFFile(adfdlg.selectedFile());
        prefMgr->setLastUsedDir(adfdlg.dirPath());
    }
}

/*
 * Sets the state of the program to use the embedded adf of the trace.
 *
 * @return void.
 */
void
Dreams::do_genericEmbeddedADF()
{
    // Avoids strange conditions.
    if(isLoading())
    {
        qApp->beep();
        return;
    }

    params.setADFFile(QString::null);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_genericPrint()
{
    qWarning("Not implemented");
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_saveSession()
{
    int dresult=QDialog::Accepted;
    QString fn = QString::null;

    sessionFileDlg->setMode( QFileDialog::AnyFile );
    dresult = sessionFileDlg->exec();
    fn = sessionFileDlg->selectedFile();

    if ((fn==QString::null)||(dresult!=QDialog::Accepted) )
    {
        return;
    }
    prefMgr->setLastUsedDir(sessionFileDlg->dirPath());
    QFile file(fn);
    bool ok = file.open(IO_WriteOnly);
    if (!ok)
    {
        QMessageBox::critical (this,"Session Saving Error",
        "IO Error writing "+fn,QMessageBox::Ok,QMessageBox::NoButton,
        QMessageBox::NoButton);
        return;
    }
    dreamsdb->saveSession(&file);
    file.close();
}

void
Dreams::do_loadSession()
{
    int dresult=QDialog::Accepted;
    QString fn = QString::null;

    sessionFileDlg->setMode( QFileDialog::ExistingFile );
    dresult = sessionFileDlg->exec();
    fn = sessionFileDlg->selectedFile();

    if ((fn==QString::null)||(dresult!=QDialog::Accepted) )
    {
        return;
    }
    prefMgr->setLastUsedDir(sessionFileDlg->dirPath());
    QFile file(fn);
    bool ok = file.open(IO_ReadOnly);
    if (!ok)
    {
        QMessageBox::critical (this,"Session Loading Error",
        "IO Error reading "+fn,QMessageBox::Ok,QMessageBox::NoButton,
        QMessageBox::NoButton);
        return;
    }

    dreamsdb->loadSession(&file);
    file.close();

    // refresh it all...
    MDIWindow * cmdi;
    AScrollView * casv;

    QPtrListIterator<MDIWindow> iter(* mdiList);

    while(iter.current() != NULL)
    {
        cmdi = iter.current();
        ++iter;
        casv = cmdi->getAScrollView();
        if(casv != NULL)
        {
            casv->updateContents();
        }
    }    
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::quit()
{
    do_quit();
    qApp->quit();
}

void
Dreams::do_quit()
{
    if (fullScreenMode) do_switchFullScreen();

    // ---------------------------------------------------------
    // ---------------------------------------------------------
    // save Dremas GUI preferences
    // ---------------------------------------------------------
    // ---------------------------------------------------------

    // ---------------------------------------------------------
    // main window
    // ---------------------------------------------------------
    prefMgr->setMainWindowX(pos().x());
    prefMgr->setMainWindowY(pos().y());
    prefMgr->setMainWindowWidth(width());
    prefMgr->setMainWindowHeight(height());

    if (!forcedShutdown)
    {
        saveDockingState();
    }

    // --------------------------------------------------------------------
    // grid lines stuff
    // --------------------------------------------------------------------
    //if (view!=NULL) { prefMgr->setShowGridLines(view->isItemChecked(view_grid_id)); }

    // ---------------------------------------------------------
    // Really save all preferences
    // ---------------------------------------------------------
    prefMgr->savePreferences();

    TempDir::destroy();
    // ---------------------------------------------------------
    // and really quit
    // ---------------------------------------------------------
    //exit(1);
}

void
Dreams::do_genericSave()
{
}

void
Dreams::dockWindowPositionChanged(QDockWindow*)
{
    saveDockingState();
}

void
Dreams::saveDockingState()
{
    if (dockInit)
    {
        return;
    }

    Dock dockArea;
    int  dockIdx;
    bool dockNl;
    int  dockOffset;
    bool ok;

    // ---------------------------------------------------------
    // check docked windows
    // ---------------------------------------------------------
    DAvtView* casv = (DAvtView *)getActiveASV();
    MDIDreamsWindow* mdi=static_cast<MDIDreamsWindow*>(getActiveMDI());
    TagDockWin * tagWin = NULL;
    if(casv)
    {
        tagWin = mdi->getTagWindow();
    }
    if(tagWin)
    {
        ok = mdi->getLocation(tagWin,dockArea,dockIdx,dockNl,dockOffset);
        if (ok)
        {
            prefMgr->setTagWindowDockPolicy(dockArea);
            prefMgr->setTagWindowIdx(dockIdx);
            prefMgr->setTagWindowNl(dockNl);
            prefMgr->setTagWindowOffset(dockOffset);
            prefMgr->setTagWindowWidth(tagWin->width());
            prefMgr->setTagWindowHeight(tagWin->height());
        }
    }

    ResourceDAvtView* cresourceasv = (dynamic_cast<ResourceDAvtView *>(getActiveASV()) ? (ResourceDAvtView *)getActiveASV() : NULL);
    DisDockWin* disWin = NULL;
    if (cresourceasv) disWin = cresourceasv->getDisWindow(); 
    if (disWin)
    {
        ok = mdi->getLocation (disWin,dockArea,dockIdx,dockNl,dockOffset);
        if (ok)
        {
            prefMgr->setDisassemblyWindowDockPolicy(dockArea);
            prefMgr->setDisassemblyWindowIdx(dockIdx);
            prefMgr->setDisassemblyWindowNl(dockNl);
            prefMgr->setDisassemblyWindowOffset(dockOffset);
            prefMgr->setDisassemblyWindowWidth(disWin->width());
            prefMgr->setDisassemblyWindowHeight(disWin->height());
        }
    }

    AncestorDockWin* ancWin = NULL;
    if (cresourceasv) ancWin = cresourceasv->getAncestorWindow(); 
    if (ancWin)
    {
        ok = mdi->getLocation (ancWin,dockArea,dockIdx,dockNl,dockOffset);
        if (ok)
        {
            prefMgr->setRelationshipWindowDockPolicy(dockArea);
            prefMgr->setRelationshipWindowIdx(dockIdx);
            prefMgr->setRelationshipWindowNl(dockNl);
            prefMgr->setRelationshipWindowOffset(dockOffset);
            prefMgr->setRelationshipWindowWidth(ancWin->width());
            prefMgr->setRelationshipWindowHeight(ancWin->height());
        }
    }

    PushoutDockWin * pushWin = NULL;
    if (cresourceasv) pushWin = cresourceasv->getPushoutWindow(); 
    if (pushWin)
    {
        ok = mdi->getLocation(pushWin, dockArea, dockIdx, dockNl, dockOffset);
        if (ok)
        {
            prefMgr->setPushoutWindowDockPolicy(dockArea);
            prefMgr->setPushoutWindowIdx(dockIdx);
            prefMgr->setPushoutWindowNl(dockNl);
            prefMgr->setPushoutWindowOffset(dockOffset);
            prefMgr->setPushoutWindowWidth(pushWin->width());
            prefMgr->setPushoutWindowHeight(pushWin->height());
        }
    }

    if (logWin)
    {
        ok = getLocation (logWin,dockArea,dockIdx,dockNl,dockOffset);
        if (ok)
        {
            prefMgr->setLogWindowDockPolicy(dockArea);
            prefMgr->setLogWindowIdx(dockIdx);
            prefMgr->setLogWindowNl(dockNl);
            prefMgr->setLogWindowOffset(dockOffset);
            prefMgr->setLogWindowWidth(logWin->width());
            prefMgr->setLogWindowHeight(logWin->height());
        }
    }
    
    if (disWin) prefMgr->setShowDisassemblyWindow(disWin->isVisible());
    if (ancWin) prefMgr->setShowRelationshipWindow(ancWin->isVisible());
    if (pushWin) prefMgr->setShowPushoutWindow(pushWin->isVisible());
    if (tagWin) prefMgr->setShowTagWindow(tagWin->isVisible());
    if (logWin) prefMgr->setShowLogWindow(logWin->isVisible());

    // ---------------------------------------------------------
    // check docked toolbars
    // ---------------------------------------------------------
    ok = getLocation (annotationsTools,dockArea,dockIdx,dockNl,dockOffset);
    if (ok)
    {
        prefMgr->setAnnotationsToolbarDockPolicy(dockArea);
        prefMgr->setAnnotationsToolbarIdx(dockIdx);
        prefMgr->setAnnotationsToolbarNl(dockNl);
        prefMgr->setAnnotationsToolbarOffset(dockOffset);
    }
    
    ok = getLocation (viewTools,dockArea,dockIdx,dockNl,dockOffset);
    if (ok)
    {
        prefMgr->setViewToolbarDockPolicy(dockArea);
        prefMgr->setViewToolbarIdx(dockIdx);
        prefMgr->setViewToolbarNl(dockNl);
        prefMgr->setViewToolbarOffset(dockOffset);
    }

    ok = getLocation (pointerTools,dockArea,dockIdx,dockNl,dockOffset);
    if (ok)
    {
        prefMgr->setToolsToolbarDockPolicy(dockArea);
        prefMgr->setToolsToolbarIdx(dockIdx);
        prefMgr->setToolsToolbarNl(dockNl);
        prefMgr->setToolsToolbarOffset(dockOffset);
    }

    ok = getLocation (fileTools,dockArea,dockIdx,dockNl,dockOffset);
    if (ok)
    {
        prefMgr->setFileToolbarDockPolicy(dockArea);
        prefMgr->setFileToolbarIdx(dockIdx);
        prefMgr->setFileToolbarNl(dockNl);
        prefMgr->setFileToolbarOffset(dockOffset);
    }

    ok = getLocation (colorTools,dockArea,dockIdx,dockNl,dockOffset);
    if (ok)
    {
        prefMgr->setColorToolbarDockPolicy(dockArea);
        prefMgr->setColorToolbarIdx(dockIdx);
        prefMgr->setColorToolbarNl(dockNl);
        prefMgr->setColorToolbarOffset(dockOffset);
    }
    
    ok = getLocation (mdiTools,dockArea,dockIdx,dockNl,dockOffset);
    if (ok)
    {
        prefMgr->setMDIToolbarDockPolicy(dockArea);
        prefMgr->setMDIToolbarIdx(dockIdx);
        prefMgr->setMDIToolbarNl(dockNl);
        prefMgr->setMDIToolbarOffset(dockOffset);
    }
    
    prefMgr->setShowAnnotationsToolbar(annotationsTools->isVisible());
    prefMgr->setShowViewToolbar(viewTools->isVisible());
    prefMgr->setShowToolsToolbar(pointerTools->isVisible());
    prefMgr->setShowFileToolbar(fileTools->isVisible());
    prefMgr->setShowColorToolbar(colorTools->isVisible());
    prefMgr->setShowMDIToolbar(mdiTools->isVisible());
}

void
Dreams::do_clearShadedColumns()
{
    ResourceDAvtView * tasv = dynamic_cast<ResourceDAvtView *>(getActiveASV());
    WaterfallDAvtView * wasv = dynamic_cast<WaterfallDAvtView *>(getActiveASV());

    if(tasv)
    {
        tasv->clearShadedColumns();
    }
    if(wasv)
    {
        wasv->clearShadedColumns();
    }
}

void Dreams::do_switchDisWin()
{
    DAvtView * casv = (DAvtView *) getActiveASV();
    DisDockWin * disWin = NULL;
    if(casv)
    {
        disWin = casv->getDisWindow();
    }
    if(disWin)
    {
        bool s = !window->isItemChecked(dis_win_id);
        window->setItemChecked(dis_win_id, s);

        if(s)
        {
            disWin->show();
        }
        else
        {
            disWin->hide();
        }
    }
}

void 
Dreams::do_switchAncWin()
{
    ResourceDAvtView * casv=dynamic_cast<ResourceDAvtView *>(getActiveASV());
    AncestorDockWin * ancWin = NULL;

    if(casv)
    {
        ancWin = casv->getAncestorWindow(); 
    }
    if(ancWin)
    {
        bool s = !window->isItemChecked(anc_win_id);
        window->setItemChecked(anc_win_id, s);
    
        if(s)
        {
            ancWin->show();
        }
        else
        {
            ancWin->hide();
        }
    }
}

void 
Dreams::do_switchPushWin()
{
    ResourceDAvtView * casv = dynamic_cast<ResourceDAvtView *>(getActiveASV());
    PushoutDockWin * pushWin = NULL;

    if(casv)
    {
        pushWin = casv->getPushoutWindow(); 
    }
    if(pushWin)
    {
        bool s = !window->isItemChecked(push_win_id);
        window->setItemChecked(push_win_id, s);
    
        if(s)
        {
            pushWin->show();
        }
        else
        {
            pushWin->hide();
        }
    }
}

void 
Dreams::do_switchTagWin()
{
    MDIDreamsWindow* mdi = (MDIDreamsWindow*) getActiveMDI();
    TagDockWin* tagWin = NULL;
    if(mdi)
    {
        tagWin = mdi->getTagWindow();
    }
    if(tagWin)
    {
        bool s = !window->isItemChecked(tag_win_id);
        window->setItemChecked(tag_win_id, s);
        if(s)
        {
            tagWin->show();
        }
        else
        {
            tagWin->hide();
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void 
Dreams::do_switchLogWin()
{
    bool s = !window->isItemChecked(log_win_id);
    window->setItemChecked(log_win_id,s);

    if(s)
    {
        logWin->show();
    }
    else
    {
        logWin->hide();
    }
}

void 
Dreams::do_newResourceWindow()
{
    TraceListIter it = findTraceFromWindow(trace_menu_id);
    newResourceDView((* it)->getDreamsDB());
}

/**
 * @author Santi Galan
 * @brief Code to open a new window with the Floorplan view. 
 * For now we create the same windows as the Free Window.
 */
void 
Dreams::do_newFloorplanWindow()
{
    TraceListIter it = findTraceFromWindow(trace_menu_id);
    newFloorplanDView((* it)->getDreamsDB());
}

/**
 * @author Santi Galan
 * @brief Code to open a new window with the "WatchWindow" view. 
 * For now we create the same windows as the Free Window.
 */
void 
Dreams::do_newWatchWindow(int id)
{
    TraceListIter it = findTraceFromWindow(trace_menu_id);
    newWatchDView((* it)->getDreamsDB(), (* it)->getWatchWindowMenu()->text(id));
}

void 
Dreams::do_newWaterfallWindow()
{
    TraceListIter it = findTraceFromWindow(trace_menu_id);
    newWaterfallDView((* it)->getDreamsDB());
}

void
Dreams::do_closeAllTraceWindows()
{
    TraceListIter it = findTraceFromWindow(trace_menu_id);
    DreamsDB * trace = (* it)->getDreamsDB();

    QPtrListIterator<MDIWindow> iter(* mdiList);

    // First closes all the mdis of the closed trace.
    while(iter.current() != NULL)
    {
        MDIDreamsWindow * cmdi = (MDIDreamsWindow *) iter.current();
        ++iter;

        if(cmdi->getDreamsDB() == trace)
        {
            cmdi->close();
        }
    }
}

void 
Dreams::disWinVisibilityChanged(bool visible)
{
    DAvtView * casv= (DAvtView *) getActiveASV();
    if(casv)
    {
        casv->setEnabledDisWindow(visible); 
    }

    window->setItemChecked(dis_win_id,visible);
    if(!dockInit)
    {
        saveDockingState();
    }
}

void 
Dreams::disWinPlaceChanged(QDockWindow::Place)
{
    saveDockingState();
}

void 
Dreams::disWinOrientationChanged(Orientation)
{
    saveDockingState();
}

void 
Dreams::ancWinVisibilityChanged(bool visible)
{
    ResourceDAvtView* casv= dynamic_cast<ResourceDAvtView *>(getActiveASV());
    window->setItemChecked(anc_win_id, visible);

    if(casv)
    {
        casv->setEnabledAncestorWindow(visible);
    }
    if(!dockInit)
    {
        saveDockingState();
    }
}

void 
Dreams::ancWinPlaceChanged(QDockWindow::Place)
{
    saveDockingState();
}

void 
Dreams::ancWinOrientationChanged(Orientation)
{
    saveDockingState();
}

void 
Dreams::pushWinVisibilityChanged(bool visible)
{
    ResourceDAvtView * casv= dynamic_cast<ResourceDAvtView *>(getActiveASV());
    window->setItemChecked(push_win_id, visible);

    if(casv)
    {
        casv->setEnabledPushoutWindow(visible);
    }
    if(!dockInit)
    {
        saveDockingState();
    }
}

void 
Dreams::pushWinPlaceChanged(QDockWindow::Place)
{
    saveDockingState();
}

void
Dreams::pushWinOrientationChanged(Orientation)
{
    saveDockingState();
}

void 
Dreams::tagWinVisibilityChanged(bool visible)
{
    window->setItemChecked(tag_win_id, visible);
    MDIDreamsWindow* mdi = (MDIDreamsWindow*) getActiveMDI();
    if(mdi)
    {
        mdi->setEnabledTagWindow(visible);
    }
    if(!dockInit)
    {
        saveDockingState();
    } 
}

void 
Dreams::tagWinPlaceChanged(QDockWindow::Place)
{
    saveDockingState();
}

void 
Dreams::tagWinOrientationChanged(Orientation)
{
    saveDockingState();
}

void 
Dreams::logWinVisibilityChanged(bool visible)
{
    window->setItemChecked(log_win_id,visible);
    logWin->doVisibilityChanged(visible);
    if(!dockInit)
    {
        saveDockingState();
    }    
}

void 
Dreams::logWinPlaceChanged(QDockWindow::Place)
{
    saveDockingState();
}

void 
Dreams::logWinOrientationChanged(Orientation)
{
    saveDockingState();
}

void 
Dreams::do_disableLastHighlight()
{
    DAvtView * casv = (DAvtView *) getActiveASV();
    if(casv)
    {
        casv->disableLastHightlight();
    }
}

void 
Dreams::do_disableAllHighlight()
{
    DAvtView * casv = (DAvtView *) getActiveASV();
    if(casv)
    {
        casv->disableAllHighlight();
    }
}

void
Dreams::do_showHighlightCriteria()
{
    DAvtView * casv = (DAvtView *) getActiveASV();
    if(casv)
    {
        casv->showHighlightCriteria();
    }
}

void
Dreams::do_highlightViewChanged(QAction* act)
{
    MDIDreamsWindow * mdi = (MDIDreamsWindow *) getActiveMDI();
    DAvtView * casv = (DAvtView *) getActiveASV();

    if(casv)
    {
        if(act == actionVHIGH)
        {
            casv->viewHEvents();
            actionVHIGH->setOn(true);
            mdi->setVHIGH(true);
        }
        else if(act == actionVNHIGH)
        {
            casv->viewNHEvents();
            actionVNHIGH->setOn(true);
            mdi->setVNHIGH(true);
        }
        else if(act == actionVBHIGH)
        {
            casv->viewAllEvents();
            actionVBHIGH->setOn(true);
            mdi->setVBHIGH(true);
        }
        else
        {
            Q_ASSERT(false);
        }
    }
}



// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
Dreams::~Dreams()
{
    /** @todo check this destructor */
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::mdiCloseEvent(MDIWindow * mdi)
{
    if(isLoading())
    {
        qApp->beep();
        return;
    }

    // Erases the window from the mdi list.
    bool rok = mdiList->remove(mdi);
    Q_ASSERT(rok);

    // Updates the count of windows that need the play tools.
    if(dynamic_cast<MDIFloorplanWindow *>(mdi) || dynamic_cast<MDIWatchWindow *>(mdi))
    {
        numPlayWindow--;
        if(numPlayWindow == 0)
        {
            playTools->hide();
            pTimer->stop();
        }
    }

    // Updates the count of waterfall windows. They are needed to maintain mdi tools coherency.
    if(dynamic_cast<MDIWaterfallWindow *>(mdi))
    {
        numWFWindow--;
    }

    if(mdiList->count() == 0)
    {
        mdiTools->hide();
        reset();
    }
    else if(mdiList->count() == 1)
    {
        // When we try to hide the mdi lock tools, we must be sure that no waterfall
        // mdi with more than one tab is opened, to allow the user sync the views inside
        // this mdi.
        if(numWFWindow == 0)
        {
            // If no waterfall opened, then we can hide them.
            mdiTools->hide();
        }
        else
        {
            // We get the first mdi and assert that is a wf layout.
            MDIWaterfallWindow * mdiwf = dynamic_cast<MDIWaterfallWindow *>(mdiList->first());
            Q_ASSERT(mdiwf);

            // If only one tab is defined, we can hide the lock tools.
            if(mdiwf->getListAScrollView().size() == 1)
            {
                mdiTools->hide();
            }
        }
    }

    // Clears the application status, cycle and row bars.
    emit status_msg_changed_signal("");
    emit cycle_msg_changed_signal("");
    emit row_msg_changed_signal("");
}

void 
Dreams::do_genericClose()
{
    if(isLoading())
    {
        qApp->beep();
        return;
    }

    MDIDreamsWindow * cmdi = (MDIDreamsWindow *) getActiveMDI();
    if(cmdi)
    {
        // Gets the trace and erases it if no more windows are used.
        DreamsDB * trace = cmdi->getDreamsDB();
        trace->decNumMDI();

        cmdi->close();
    }
}

void
Dreams::do_genericTraceClose(int close_id)
{
    DreamsDB * trace = (* findTraceFromClose(close_id))->getDreamsDB();
    QPtrListIterator<MDIWindow> iter(* mdiList);

    // First closes all the mdis of the closed trace.
    while(iter.current() != NULL)
    {
        MDIDreamsWindow * cmdi = (MDIDreamsWindow *) iter.current();
        ++iter;

        if(cmdi->getDreamsDB() == trace)
        {
            qApp->processEvents();
            cmdi->close();
            qApp->processEvents();
        }
    }

    removeTrace(trace);
}

void 
Dreams::do_closeAll()
{
    if(isLoading())
    {
        qApp->beep();
        return;
    }

    // close all mdi 
    QWidgetList windows = ws->windowList();
    for(INT32 i = 0; i < (INT32) windows.count(); i++) 
    {
        qApp->processEvents();
        QWidget * window = windows.at(i);
        window->close();
        qApp->processEvents();
    }

    // Closes the trace if opened.
    while(traces.size() > 0)
    {
        DreamsTraceData * trace;

        trace = * traces.begin();
        removeTrace(trace->getDreamsDB());
    }

    qApp->flush();
    reset();
}

void
Dreams::reset()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    setCaption(getApplicationCaption());
    numNextWindow = 1;
    numPlayWindow = 0;
    numWFWindow = 0;

    enableMenuPurgeLast(false);
    enableMenuShowCriteria(false);
    enableMenuPurgeAll(false);
    enableMenuOptions(false);

    QApplication::restoreOverrideCursor();
}

void 
Dreams::loadFinish(void)
{
    MDIWindow * cmdi; ///< 
    DAvtView * casv;  ///<

    //getrusage(RUSAGE_SELF, &end);
    //printf("Load Trace time: %.3f\n", (float) (end.ru_utime.tv_sec + end.ru_stime.tv_sec - ini.ru_utime.tv_sec - ini.ru_stime.tv_sec) + ((float) (end.ru_utime.tv_usec + end.ru_stime.tv_usec - ini.ru_utime.tv_usec - ini.ru_stime.tv_usec) / 1000000.0f));

    if(!dreamsdb->getHasFatalError(true))
    {
        // Here we flush the dictionaries, just to free space
        dreamsdb->clearDict();

        // Shows the dialog to choose which views to preload.
        ViewChoiceDialogImpl views(dreamsdb, this, "choice view", true);

        views.exec();

        // Loads the requested views.
        if(views.loadLayoutResource())
        {
            newResourceDView(dreamsdb);
        }
        if(views.loadLayoutFloorplan())
        {
            newFloorplanDView(dreamsdb);
        }
        if(views.loadLayoutWatch())
        {
            QDictIterator<WatchWindow> iter = dreamsdb->layoutWatchGetWindowIterator();
            WatchWindow * wwindow = NULL;

            // Opens all the watch windows (maximum 3).
            INT32 i = 0;
            while(((wwindow = iter.current()) != NULL) && (i < 3))
            {
                newWatchDView(dreamsdb, wwindow->getCaption());
                ++iter;
                i++;
            }
        }
        if(views.loadLayoutWaterfall())
        {
            newWaterfallDView(dreamsdb);
        }

        // If nothing selected, then Dreams view is opened by default.
        // Must be done before addWatchWindowMnu, which gets the current MDI...
        if(!views.loadSomething())
        {
            if(dreamsdb->layoutResourceHasLayout() && dreamsdb->layoutResourceGetOpened())
            {
                newResourceDView(dreamsdb);
            }
            else if(dreamsdb->layoutFloorplanHasLayout() && dreamsdb->layoutFloorplanGetOpened())
            {
                newFloorplanDView(dreamsdb);
            }
            else if(dreamsdb->layoutWatchHasLayout() && dreamsdb->layoutWatchGetOpened())
            {
                newWatchDView(dreamsdb, (* dreamsdb->layoutWatchGetWindowIterator())->getCaption());
            }
            else if(dreamsdb->layoutWaterfallHasLayout() && dreamsdb->layoutWaterfallGetOpened())
            {
                newWaterfallDView(dreamsdb);
            }
            else
            {
                // No view has been defined in the ADF, so we just delete the trace.
                delete dreamsdb;
                QMessageBox::critical(this, "ADF Error", "No layout specified in the ADF. Closing the file...", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
                dreamsdb = NULL;
                setLoading(false);
                return;
            }
        }

        // Purges all the allocated vectors.
        dreamsdb->getDralDB()->purgeTrackHeapAllocatedVectors();

        // If is running in Load, Dump and exit mode.
        if(params.LDEMode())
        {
            // Dumps the content of the loaded trace.
            dreamsdb->dumpRegression(prefMgr->getGUIEnabled());

            // Exits succesfully.
            quit();
        }

        DreamsTraceData * trace_data;

        trace_data = new DreamsTraceData(dreamsdb, traces.size());

        // Adds the trace new window menu.
        trace_data->setWindowMenu(new QPopupMenu(window));
        trace_data->setWindowMenuId(window->insertItem(dreamsdb->getTraceName(), trace_data->getWindowMenu(), -1, NEW_WIN_DISP + traces.size()));

        // Adds the trace close entry.
        trace_data->setCloseMenuId(trace_close->insertItem(dreamsdb->getTraceName(), this, SLOT(do_genericTraceClose(int))));

        // Adds the trace to the trace list.
        traces.push_back(trace_data);

        INT32 temp;
        INT32 watch_win_id;

        temp = trace_data->getWindowMenu()->insertItem("New &Resource Window", this, SLOT(do_newResourceWindow()));
        trace_data->getWindowMenu()->setItemEnabled(temp, dreamsdb->layoutResourceHasLayout() && dreamsdb->layoutResourceGetOpened());

        temp = trace_data->getWindowMenu()->insertItem("New &Floorplan Window", this, SLOT(do_newFloorplanWindow()));
        trace_data->getWindowMenu()->setItemEnabled(temp, dreamsdb->layoutFloorplanHasLayout() && dreamsdb->layoutFloorplanGetOpened());

        temp = trace_data->getWindowMenu()->insertItem("New &Waterfall Window", this, SLOT(do_newWaterfallWindow()));
        trace_data->getWindowMenu()->setItemEnabled(temp, dreamsdb->layoutWaterfallHasLayout() && dreamsdb->layoutWaterfallGetOpened());

        trace_data->setWatchWindowMenu(new QPopupMenu(trace_data->getWindowMenu()));
        watch_win_id = trace_data->getWindowMenu()->insertItem("New W&atch Window...", trace_data->getWatchWindowMenu());
        trace_data->getWindowMenu()->setItemEnabled(watch_win_id, dreamsdb->layoutWatchHasLayout() && dreamsdb->layoutWatchGetOpened());

        trace_data->getWindowMenu()->insertSeparator();
        trace_data->getWindowMenu()->insertItem("&Close All Windows", this, SLOT(do_closeAllTraceWindows()));

        // After parsing the ADF, we create all the watch window menu selection
        if(dreamsdb->layoutWatchHasLayout() && dreamsdb->layoutWatchGetOpened())
        {
            addWatchWindowMenu(trace_data->getWatchWindowMenu(), watch_win_id);
        }
    }
    else
    {
        delete dreamsdb;
        logWin->show();
    }

    dreamsdb = NULL;
    setLoading(false);

    // Now we check if we still have some traces to load (only in case that some files have been specified
    // in the command line.
    if(params.hasDRLFile())
    {
        do_genericOpen();
    }
}

void
Dreams::loadCancel(void)
{
    delete dreamsdb;
    dreamsdb = NULL;
    setLoading(false);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_preferences()
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();
    DreamsDB * trace = NULL;

    if(myMDI != NULL)
    {
        trace = myMDI->getDreamsDB();
    }

    preferencesImpl* pref = new preferencesImpl(trace, this, "pref", true);
    int result = pref->exec();
    if(!result)
    {
        return;
    }

    QApplication::setOverrideCursor( Qt::WaitCursor );

    // --------------------------------------------------------------------
    // (re)configure the draldb
    // --------------------------------------------------------------------
    TraceListIter it;
    it = traces.begin();

    while(it != traces.end())
    {
        (* it)->getDreamsDB()->setDralDBPreferences(prefMgr);
        ++it;
    }
    
    // ------------------------------------------------------------------------
    // files
    // ------------------------------------------------------------------------
    bool logfileok = (LogMgr::getInstance())->changeFileName(prefMgr->getLogFileName());
    if (!logfileok)
    {
        QMessageBox::critical (this,"IO Error",
        "Unable to write open log file "+prefMgr->getLogFileName(),
        QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
    }
    else
    {
        logWin->setEnabled(false);
        logWin->logFileChanged();
        logWin->setEnabled(true);
    }

    // ------------------------------------------------------------------------
    // update font size
    // ------------------------------------------------------------------------
    QFont appFont = qApp->font();
    appFont.setPointSize(prefMgr->getFontSize());
    qApp->setFont(appFont,true);

    // ------------------------------------------------------------------------
    // update GUI Style & co
    // ------------------------------------------------------------------------
    do_makeStyle(prefMgr->getGuiStyle());
    tgsel->refresh();
    syncTagSel->refresh();

    MDIWindow* cmdi;
    DAvtView* casv;
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv=(DAvtView*)(cmdi->getAScrollView());

        if(casv == NULL)
        {
            continue;
        }
        // ------------------------------------------------------------------------
        // update bg & shading color
        // ------------------------------------------------------------------------
        casv->setBgColor(prefMgr->getBackgroundColor());
        casv->setShadingColor(prefMgr->getShadingColor());
    
        // ------------------------------------------------------------------------
        // grid lines
        // ------------------------------------------------------------------------
        casv->setShowVGridLines(prefMgr->getShowVGridLines());
        casv->setShowHGridLines(prefMgr->getShowHGridLines());
        casv->setGridLinesSize(prefMgr->getSnapGap());
        casv->setSnapToGrid(prefMgr->getSnapToGrid());
        //if (view!=NULL) { view->setItemChecked(view_grid_id,prefMgr->getShowGridLines()); }
    
        // ------------------------------------------------------------------------
        // tags
        // ------------------------------------------------------------------------
        ResourceDAvtView * d2asv;
        d2asv = dynamic_cast<ResourceDAvtView *>(casv);
        if(d2asv)
        {
            d2asv->setAncestorTrackingTag(prefMgr->getRelationshipTag());
            d2asv->refreshFavoriteTagList();
        }
    
        // Highlight mgr size
        casv->changeHighlightSize(prefMgr->getHighlightSize());

        // refresh it all...
        casv->repaintContents(false);
    }
    QApplication::restoreOverrideCursor();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_find()
{
    DAvtView* casv = (DAvtView *)getActiveASV();

    // TODO
    // Not all views contain a Avt (for example WatchWindows)
    if(casv)
        casv->find();
}

void
Dreams::do_find_next()
{
    DAvtView* casv = (DAvtView *)getActiveASV();

    // TODO
    // Not all views contain a Avt (for example WatchWindows)
    if(casv)
        casv->findNext();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_go_to_cycle()
{
    bool ok;
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();

    if(myMDI != NULL)
    {
        GoToCycleDialogImpl dial(myMDI->getDreamsDB(), this, "gotocycle");

        if(dial.exec() == QDialog::Accepted)
        {
            MDIDreamsWindow * cmdi = static_cast<MDIDreamsWindow *>(getActiveMDI());

            cmdi->showCycle(dial.getEnteredCycle());
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_selectEvents()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_memrep()
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();

    if(myMDI != NULL)
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );
        DreamsDB * trace = myMDI->getDreamsDB();
        QString report = trace->getUsageDescription();
        log->addLog(report);
        log->flush();
        logWin->refresh();
        if (! (window->isItemChecked(log_win_id)))
        {
            do_switchLogWin();
        }
        QApplication::restoreOverrideCursor();
    }
}

void
Dreams::do_viewadf()
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();

    if(myMDI != NULL)
    {
        DreamsDB * trace = myMDI->getDreamsDB();
        QDomDocument * ddoc = trace->getDomDocument();
        if(ddoc == NULL)
        {
            return;
        }
        adfte->textEdit->setText(ddoc->toString(4));
        adfte->exec();
    }
}

void
Dreams::do_viewdralgraph()
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();

    if(myMDI != NULL)
    {
        DreamsDB * trace = myMDI->getDreamsDB();
        grte->textEdit->setText(trace->getDralDB()->getGraphDescription());
        grte->exec();
    }
}

void
Dreams::do_viewtagdesc()
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();

    if(myMDI != NULL)
    {
        DreamsDB * trace = myMDI->getDreamsDB();
        QStrList strlist (false);
        DralDB * draldb = trace->getDralDB();
        strlist = draldb->getKnownTags();

        ShowTagDescDialog *dlg = new ShowTagDescDialog(this,"ctd",true);
        QStrListIterator it_str(strlist);
        char * str;

        while((str = it_str.current()) != NULL)
        {
            ++it_str;
            string tmp = string(str);
            TAG_ID tag = draldb->getTagId(QString(str));
            if(tmp.find("__color") != 0)
            {
                dlg->insertItem(QString(str), TagValueToolkit::toString(draldb->getTagValueType(tag)), draldb->getTagDesc(str));
            }
        }

        dlg->exec();
        delete dlg;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::ensureRScanDWinVisible()
{
    if (! (window->isItemChecked(anc_win_id)))
    {
        do_switchAncWin();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_dbstats()
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();

    if(myMDI != NULL)
    {
        QApplication::setOverrideCursor( Qt::WaitCursor );
        DreamsDB * trace = myMDI->getDreamsDB();
        QString report = trace->getStats();
        log->addLog(report);
        log->flush();
        logWin->refresh();
        if (! (window->isItemChecked(log_win_id)))
        {
            do_switchLogWin();
        }
        QApplication::restoreOverrideCursor();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_resetRelationshipScan()
{
    ResourceDAvtView* cavt = dynamic_cast<ResourceDAvtView*>(getActiveASV());
    if (cavt)
    {
        AncestorDockWin* adw = cavt->getAncestorWindow();
        adw->reset();
        enableMenuOptions(true);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_switchFullScreen()
{
    if (fullScreenMode)
    {
        QValueList<int>::iterator it;
        for ( it = fstList->begin(); it != fstList->end(); ++it )
        {
            int id_num = *it;
            if (id_num == win_view_bar_id)
            {
                do_switchViewBar();
            }
            else if (id_num == win_tool_bar_id)
            {
                do_switchToolsBar();
            }
            else if (id_num == win_annotations_bar_id)
            {
                do_switchAnnotationsBar();
            }
            else if (id_num == win_file_bar_id)
            {
                do_switchFileBar();
            }
            else if (id_num == win_color_bar_id)
            {
                do_switchColorsBar();
            }
            else
            {
                Q_ASSERT(false);
            }
        }
        menuBar()->show();
        showNormal();
        fullScreenMode = false;
    }
    else
    {
        bool visible;
        fstList->clear();

        visible = window->isItemChecked(win_view_bar_id);
        if (visible)
        {
            do_switchViewBar();
            fstList->append(win_view_bar_id);
        }

        visible = window->isItemChecked(win_tool_bar_id);
        if (visible)
        {
            do_switchToolsBar();
            fstList->append(win_tool_bar_id);
        }

        visible = window->isItemChecked(win_annotations_bar_id);
        if (visible)
        {
            do_switchAnnotationsBar();
            fstList->append(win_annotations_bar_id);
        }

        visible = window->isItemChecked(win_file_bar_id);
        if (visible)
        {
            do_switchFileBar();
            fstList->append(win_file_bar_id);
        }

        visible = window->isItemChecked(win_color_bar_id);
        if (visible)
        {
            do_switchColorsBar();
            fstList->append(win_color_bar_id);
        }

        menuBar()->hide();
        showFullScreen();
        fullScreenMode = true;
    }
}

/**
 * Creates, but doesn't show the floorplan toolbar that control the flow of the time in the
 * floorplan views
 */
void
Dreams::createFloorplanToolbar(void)
{
    playTools = new QToolBar( this, "Floorplan controls");
    Q_ASSERT(playTools != NULL);
    playTools->setLabel("Floorplan controls");

    ptbRewind = new QToolButton(QIconSet(IconFactory::getInstance()->image42), "Rewind", QString::null, this, SLOT(do_rewindFloorplan()), playTools, "rewind");
    ptbRewind->setToggleButton(true);

    ptbStepBackward = new QToolButton(QIconSet(IconFactory::getInstance()->image43), "Step backward", QString::null, this, SLOT(do_stepBackward()), playTools, "step backward");

    ptbStop = new QToolButton(QIconSet(IconFactory::getInstance()->image45), "Stop", QString::null, this, SLOT(do_stopFloorplan()), playTools, "stop");

    ptbPlay = new QToolButton(QIconSet(IconFactory::getInstance()->image44), "Play", QString::null, this, SLOT(do_playFloorplan()), playTools, "play");
    ptbPlay->setToggleButton(true);

    ptbStepForward = new QToolButton(QIconSet(IconFactory::getInstance()->image46), "Step forward", QString::null, this, SLOT(do_stepForward()), playTools, "step forward");

    ptbForward = new QToolButton(QIconSet(IconFactory::getInstance()->image47), "Forward", QString::null, this, SLOT(do_forwardFloorplan()), playTools, "forward");
    ptbForward->setToggleButton(true);

    playTools->addSeparator();

    pcmbFPS    = new QComboBox(false, playTools, "FPS");
    QToolTip::add(pcmbFPS, "Animation frames per second");

    pcmbFPS->insertItem(" 1fps ");
    pcmbFPS->insertItem(" 2fps ");
    pcmbFPS->insertItem(" 4fps ");
    pcmbFPS->insertItem(" 8fps ");
    pcmbFPS->insertItem(" 16fps ");
    pcmbFPS->insertItem(" 32fps ");
    pcmbFPS->insertItem(" 64fps ");
    pcmbFPS->insertItem(" 128fps ");
    pcmbFPS->setCurrentItem(0);
    

    pcmbCPF    = new QComboBox(false, playTools, "CPF");
    QToolTip::add(pcmbCPF, "Animation cycles show per frame");

    pcmbCPF->insertItem(" 1cpf ");
    pcmbCPF->insertItem(" 2cpf ");
    pcmbCPF->insertItem(" 4cpf ");
    pcmbCPF->insertItem(" 8cpf ");
    pcmbCPF->insertItem(" 16cpf ");
    pcmbCPF->insertItem(" 32cpf ");
    pcmbCPF->insertItem(" 64cpf ");
    pcmbCPF->insertItem(" 128cpf ");
    pcmbCPF->setCurrentItem(0);
    playTools->hide();

    pTimer = new QTimer(this, "timer");

    connect(pTimer, SIGNAL(timeout()), this, SLOT(clockNonClockedViews()));
    connect(pcmbFPS, SIGNAL(activated(const QString &)), this, SLOT(setFPS(const QString &)));
    connect(pcmbCPF, SIGNAL(activated(const QString &)), this, SLOT(setCPF(const QString &)));
}

void 
Dreams::do_rewindFloorplan()
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();
    curClock = myMDI->getCycle().clock;

    // Reset the others buttons
    ptbRewind->setOn(true);
    ptbPlay->setOn(false);
    ptbForward->setOn(false);
    numFloorplanWindowStopped = 0;

    // Update the clock state
    nextCycle = -1;
    fastStep = 2;
    pTimer->changeInterval(BASE_INTERVAL / (nFPS * fastStep));

}

void 
Dreams::do_stepBackward(void)
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();
    curClock = myMDI->getCycle().clock;

    do_stopFloorplan();
    pTimer->stop();
    nextCycle = -1;
    fastStep = 1;
    clockNonClockedViews();
}

void 
Dreams::do_playFloorplan()
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();
    curClock = myMDI->getCycle().clock;

    // Reset the others buttons
    ptbPlay->setOn(true);
    ptbRewind->setOn(false);
    ptbForward->setOn(false);
    numFloorplanWindowStopped = 0;

    // Update the clock state
    nextCycle = 1;
    fastStep = 1;
    pTimer->start(BASE_INTERVAL / nFPS);
}

void 
Dreams::do_stopFloorplan()
{
    // Reset the others buttons
    ptbRewind->setOn(false);
    ptbPlay->setOn(false);
    ptbForward->setOn(false);

    // Update the clock state
    pTimer->stop();
}

void 
Dreams::do_stepForward(void)
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();
    curClock = myMDI->getCycle().clock;

    do_stopFloorplan();

    // Update the clock state
    nextCycle = 1;
    fastStep = 1;
    clockNonClockedViews();
}

void 
Dreams::do_forwardFloorplan()
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();
    curClock = myMDI->getCycle().clock;

    // Reset the others buttons
    ptbForward->setOn(true);
    ptbRewind->setOn(false);
    ptbPlay->setOn(false);
    numFloorplanWindowStopped = 0;

    // Update the clock state
    nextCycle = 1;
    fastStep = 2;
    pTimer->changeInterval(BASE_INTERVAL / (nFPS * fastStep));
}

/**
 * Sets the velocity of the animation based on the standard
 *
 * @param Total amount of cycles of the animation
 */
void 
Dreams::setFPS(const QString &fps)
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();
    curClock = myMDI->getCycle().clock;

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
Dreams::setCPF(const QString &cpf)
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();
    curClock = myMDI->getCycle().clock;

    QString sTmp = cpf;
    // Remove the first space
    sTmp.remove(0, 1);
    sTmp.remove(sTmp.length() - 4, 4);
    nCPF = sTmp.toInt();
}

void
Dreams::clockNonClockedViews(void)
{
    if(!panLocked)
    {
        return;
    }

    // FIXME
    // Maybe the clock system that synchronizes the views clocks first the
    // non-clocked views. In that case, we use the first cycle in the views
    // this will obviously doesn't work when 
    //
    MDIDreamsWindow *cmdi = static_cast<MDIDreamsWindow *>(mdiList->first()); 

    CYCLE temp(curClock);

    INT32 steps = nextCycle * nCPF;
    INT32 absSteps = abs(steps);

    if(curClock->getDivisions() > 1)
    {
        temp.cycle = absSteps >> 1;
        temp.division = absSteps & 1;
        if(steps < 0)
        {
            temp.cycle = -temp.cycle;
            temp.division = -temp.division;
        }
    }
    else
    {
        temp.cycle = steps;
        temp.division = 0;
    }

    INT64 ps = temp.toPs();

    while(cmdi != NULL)
    {
        if(cmdi->getMDILocked() && !cmdi->getIsClocked())
        {
            cmdi->attendCycleChanged(ps);
        }

        cmdi = static_cast<MDIDreamsWindow *>(mdiList->next());
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::createLogWindow()
{
    // create the docked window
    logWin = new LogDockWin(this);Q_ASSERT(logWin!=NULL);
    connect( logWin, SIGNAL(visibilityChanged(bool)), SLOT(logWinVisibilityChanged(bool)));
    connect( logWin, SIGNAL(placeChanged(QDockWindow::Place)), SLOT(logWinPlaceChanged(QDockWindow::Place)));
    connect( logWin, SIGNAL(orientationChanged(Orientation)), SLOT(logWinOrientationChanged(Orientation)));

    if ( prefMgr->getShowLogWindow() )
    {
        logWin->show();
    }
    else
    {
        logWin->hide();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::enableMenuPurgeLast(bool value)
{
    if(highlight != NULL)
    {
        highlight->setItemEnabled(dis_last_id,value);
    }
}

void
Dreams::enableMenuShowCriteria(bool value)
{
    if(highlight != NULL)
    {
        highlight->setItemEnabled(show_criteria_id,value);
    }
}

void
Dreams::enableMenuPurgeAll(bool value)
{
    if(highlight != NULL)
    {
        highlight->setItemEnabled(dis_all_id,value);
    }
}

/**
 * Method used by the Floorplan MDI window, to disable the ancestor menu
 * given taht it has no ancestor window
 */
void
Dreams::enableAncestorMenu(bool value)
{
    if(window != NULL)
    {
        window->setItemEnabled(anc_win_id, value);
    }
}

/**
 * Method used by the Floorplan MDI window, to disable the ancestor menu
 * given taht it has no ancestor window
 */
void
Dreams::enableHighlightSubMenu(bool value)
{
    if(window != NULL)
    {
        view->setItemEnabled(highlightSubMenu_id, value);
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::enableMenuOptions(bool openedFile)
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();
    DreamsDB * trace = NULL;

    if(myMDI != NULL)
    {
        trace = myMDI->getDreamsDB();
    }
    else
    {
        openedFile = false;
    }

    if (file!=NULL)
    {
        file->setItemEnabled(closeAll_id,openedFile);
        file->setItemEnabled(save_session_id,openedFile);
        actionSaveSession->setEnabled(openedFile);
        file->setItemEnabled(load_session_id,openedFile);
        actionLoadSession->setEnabled(openedFile);
    }

    if (edit!=NULL)
    {
        edit->setItemEnabled(goto_id,openedFile);
        actionGotoCycle->setEnabled(openedFile);
        edit->setItemEnabled(find_id,openedFile);
        actionFind->setEnabled(openedFile);
        edit->setItemEnabled(find_next_id,openedFile);
        actionFindNext->setEnabled(openedFile);
    }

    if (view!=NULL)
    {
        view->setItemEnabled(zoomin_id,openedFile);
        view->setItemEnabled(zoomout_id,openedFile);
        view->setItemEnabled(zoominh_id,openedFile);
        view->setItemEnabled(zoominv_id,openedFile);
        view->setItemEnabled(zoomouth_id,openedFile);
        view->setItemEnabled(zoomoutv_id,openedFile);
        view->setItemEnabled(rstartio_id,openedFile);
        view->setItemEnabled(z200_id,openedFile);
        view->setItemEnabled(z100_id,openedFile);
        view->setItemEnabled(z50_id,openedFile);
        view->setItemEnabled(z25_id,openedFile);
        view->setItemEnabled(scaletofit_id,openedFile);
        view->setItemEnabled(showgl_id,openedFile);
        view->setItemEnabled(zoomaxes_id,openedFile);
        view->setItemEnabled(scaleto_id,openedFile);
        view->setItemEnabled(highlightSubMenu_id,openedFile);
        view->setItemEnabled(fullScreen_id,openedFile);
    }

    if(tools != NULL)
    {
        bool ascan = false;
        ResourceDAvtView * cavt = dynamic_cast<ResourceDAvtView*>(getActiveASV());
        if(cavt)
        {
            AncestorDockWin * adw = cavt->getAncestorWindow();
            ascan = adw->hasActiveScan();
        }
        tools->setItemEnabled(reset_rscan_id, openedFile && ascan);
        tools->setItemEnabled(dis_shade_id, openedFile);
        actionClearShadedColums->setEnabled(openedFile);
        tools->setItemEnabled(toolshighlight_id, openedFile);
        tools->setItemEnabled(pointers_id, openedFile);
        tools->setItemEnabled(dbstats_id, openedFile);
        tools->setItemEnabled(mem_rep_id, openedFile);
        tools->setItemEnabled(adf_id, openedFile);
        tools->setItemEnabled(dral_id, openedFile);
        tools->setItemEnabled(tgdesc_id, openedFile);
    }

    if(bookmark != NULL)
    {
        bookmark->setItemEnabled(addbookmrk_id, openedFile);
        bookmark->setItemEnabled(mbookmrk_id, openedFile);
        bookmark->setItemEnabled(sbookmrk_id, openedFile);
        bookmark->setItemEnabled(ibookmrk_id, openedFile);
    }

    if(window != NULL)
    {
        window->setItemEnabled(dis_win_id, openedFile);
        window->setItemEnabled(tag_win_id, openedFile);
        window->setItemEnabled(anc_win_id, openedFile);
        window->setItemEnabled(push_win_id, openedFile);
        window->setItemEnabled(nview_win_id, openedFile);
        window->setItemEnabled(tileh_win_id, openedFile);
        window->setItemEnabled(tilev_win_id, openedFile);
        window->setItemEnabled(cascade_win_id, openedFile);
        //window->setItemEnabled(nfview_win_id, openedFile);
    }
    
    if(viewTools != NULL)
    {
        viewTools->setEnabled(openedFile);
    }
    if(pointerTools != NULL)
    {
        pointerTools->setEnabled(openedFile);
    }

    if(fileTools != NULL)
    {
        saveSBtn->setEnabled(openedFile);
        closeMDIBtn->setEnabled(openedFile);
    }
    
    if(colorTools != NULL)
    {
    }
    if(annotationsTools != NULL)
    {
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::aboutToQuit()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::lastWindowClosed()
{
    forcedShutdown=true;
    do_quit();
}


// -----------------------------------------------------------------------------
// -- Some aux code to splash window
// -----------------------------------------------------------------------------

QLabel* Dreams::splash=NULL;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::set_splash_status( const QString &txt )
{
    splash->repaint( FALSE );
    QPainter p( splash );
    p.setPen( Qt::black );
    p.drawText( splash->width() - splash->fontMetrics().width(txt) - 10, splash->fontMetrics().height(), txt );
    QApplication::flush();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
QLabel*
Dreams::showSplash()
{
    QRect screen = QApplication::desktop()->screenGeometry();

    splash = new QLabel( 0, "splash", WDestructiveClose | WStyle_Customize | WStyle_NoBorder | WX11BypassWM | WStyle_StaysOnTop );
    splash->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
    //splash->setPixmap(QPixmap(dreams2_xpm));
    splash->setPixmap(QPixmap(d4_logo));
    splash->adjustSize();
    splash->setCaption( "Dreams" );
    splash->move( screen.center() - QPoint( splash->width() / 2, splash->height() / 2 ) );
    splash->show();
    splash->repaint( FALSE );
    QApplication::flush();
    set_splash_status( "Starting application..." );

    return splash;
}

void
Dreams::addFileToolBar()
{
    Main::addFileToolBar();
    
    saveSBtn=new QToolButton(IconFactory::getInstance()->image4,"Save Session",QString::null,this,SLOT(do_saveSession()),fileTools,"Save Session");
    closeMDIBtn=new QToolButton(IconFactory::getInstance()->image0,"Close",QString::null,this,SLOT(do_genericClose()),fileTools,"Close");
    //QToolButton* closeAllBtn=new QToolButton(IconFactory::getInstance()->image30,"Close All",QString::null,this,SLOT(do_closeAll()),fileTools,"Close All");
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::closeSplash()
{
    splash->hide();
}

void 
Dreams::do_makeStyle(const QString &style)
{
    QFont appFont = QApplication::font();

    if (style!="Dreams")
    {
        Main::do_makeStyle(style);
        return;
    }
    qApp->setStyle("Platinum");
    QPalette p( QColor(239,239,245), QColor(239,239,245) );
    qApp->setPalette( p, TRUE );
    qApp->setFont( appFont, TRUE );

    /*
        QPalette p( QColor( 75, 123, 130 ) );
        p.setColor( QPalette::Active, QColorGroup::Base, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Inactive, QColorGroup::Base, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Disabled, QColorGroup::Base, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Active, QColorGroup::Highlight, Qt::white );
        p.setColor( QPalette::Active, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Inactive, QColorGroup::Highlight, Qt::white );
        p.setColor( QPalette::Inactive, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Disabled, QColorGroup::Highlight, Qt::white );
        p.setColor( QPalette::Disabled, QColorGroup::HighlightedText, QColor( 55, 77, 78 ) );
        p.setColor( QPalette::Active, QColorGroup::Foreground, Qt::white );
        p.setColor( QPalette::Active, QColorGroup::Text, Qt::white );
        p.setColor( QPalette::Active, QColorGroup::ButtonText, Qt::white );
        p.setColor( QPalette::Inactive, QColorGroup::Foreground, Qt::white );
        p.setColor( QPalette::Inactive, QColorGroup::Text, Qt::white );
        p.setColor( QPalette::Inactive, QColorGroup::ButtonText, Qt::white );
        p.setColor( QPalette::Disabled, QColorGroup::Foreground, Qt::lightGray );
        p.setColor( QPalette::Disabled, QColorGroup::Text, Qt::lightGray );
        p.setColor( QPalette::Disabled, QColorGroup::ButtonText, Qt::lightGray );
        qApp->setPalette( p, TRUE );
        qApp->setFont( QFont( "times", appFont.pointSize() ), TRUE );
    */
}

/**
 * Creates a new WatchWindow view taking as starting point for the animation the
 * current cycle displayed
 *
 * TODO
 * Clean all the stuff related with Floorplan view...
 */
MDIWindow * 
Dreams::newWatchDView(DreamsDB * trace, QString watch_string)
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();
    MDIWatchWindow * w = new MDIWatchWindow(trace, this, ws, NULL, WDestructiveClose);

    // Computes the WatchWindow data.
    trace->layoutWatchComputeData();

    w->init();
    w->setWatchWindow(trace->layoutWatchGetWindow(watch_string));
    DralDB * draldb = trace->getDralDB();
    mdiList->append(w);

    int myMDINumber = numNextWindow++;
    numPlayWindow++;

    trace->incNumMDI();

    QString capBase = trace->getTraceName() + ": WatchWindow";
    QString cap = capBase + " ("+QString::number(myMDINumber) + "): " + watch_string;
    
    // show the very first window in maximized mode
    if(ws->windowList().isEmpty())
    {
        // Better show this mdi normal and not maximized...
        w->setCaption(capBase);
    }
    else
    {
        mdiTools->show();

        if(myMDI != NULL)
        {
            myMDI->showNormal();
        }
        w->setCaption(cap);
    }

    w->show();

    // After the creation of a Watch Window we make visible the Floorplan control toolbar
    playTools->show();

    // by default the MDI will obei
    w->setIcon(IconFactory::getInstance()->image32);
    w->setIconText("Locked");

    myStatusBar->message("New Window Created", 2000);
    connect(w, SIGNAL(mdiCloseEvent(MDIWindow *)), this, SLOT(mdiCloseEvent(MDIWindow*)));
    connect(w, SIGNAL(firstCycle(MDIWindow *)) , this, SLOT(firstCycleReached(MDIWindow*)));
    connect(w, SIGNAL(lastCycle(MDIWindow *))  , this, SLOT(lastCycleReached(MDIWindow*)));

    // Movement synch signals
    connect(w, SIGNAL(cycleChanged(Q_INT64))    , this, SLOT(do_cycleChanged(Q_INT64)));

    connect(ptbRewind      , SIGNAL(clicked())                 , w, SLOT(rewind()));
    connect(ptbStepBackward, SIGNAL(clicked())                 , w, SLOT(stepBackward()));
    connect(ptbStop        , SIGNAL(clicked())                 , w, SLOT(stop()));
    connect(ptbPlay        , SIGNAL(clicked())                 , w, SLOT(play()));
    connect(ptbStepForward , SIGNAL(clicked())                 , w, SLOT(stepForward()));
    connect(ptbForward     , SIGNAL(clicked())                 , w, SLOT(forward()));
    connect(pcmbFPS        , SIGNAL(activated(const QString &)), w, SLOT(setFPS(const QString &)));
    connect(pcmbCPF        , SIGNAL(activated(const QString &)), w, SLOT(setCPF(const QString &)));

    connect(w, SIGNAL(syncToCycle(CYCLE)), this, SLOT(do_syncToCycle(CYCLE)));
    connect(w, SIGNAL(syncToCycleTrace(CYCLE)), this, SLOT(do_syncToCycleTrace(CYCLE)));
    connect(w, SIGNAL(status_msg_changed(QString)),this,SLOT(status_msg_changed(QString)));
    connect(w, SIGNAL(cycle_msg_changed(QString)),this,SLOT(cycle_msg_changed(QString)));
    connect(w, SIGNAL(row_msg_changed(QString)),this,SLOT(row_msg_changed(QString)));

    // We set the size to the contained size
    w->setTableSize(); 

    if(pTimer->isActive())
    {
        w->play();
    }

    return w;
}

/**
 * Creates a new Floorplan view taking as starting point for the animation the
 * current cycle displayed
 */
MDIWindow *
Dreams::newFloorplanDView(DreamsDB * trace)
{
    DAvtView* pDasv = (DAvtView *)(getActiveASV());
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();

    // We need to compute the geometry before creating the FloorplanDAvtView, that is
    // created inside the creator of MDIFloorplanWindow, so we compute the CSL and geom
    // before creating the MDIFloorplan.
    trace->layoutFloorplanComputeData();

    MDIFloorplanWindow * w = new MDIFloorplanWindow(trace, this, ws, NULL, WDestructiveClose );
    w->init();

    DralDB * draldb = trace->getDralDB();
    ClockDomainFreqIterator it = draldb->getClockDomainFreqIterator();
    const ClockDomainEntry * clock = * it;
    w->setClockDomain(clock->getId());
    mdiList->append(w);
    w->resize(800,600); // default non-maximized size
    int myMDINumber = numNextWindow++;
    numPlayWindow++;
    
    // attach the diswin reference
    FloorplanDAvtView* currView = static_cast<FloorplanDAvtView *>(w->getAScrollView());
    QString capBase = trace->getTraceName() + ": Floorplan";
    QString cap = capBase + " ("+QString::number(myMDINumber)+")";
    currView->setMDINumber(myMDINumber);

    trace->incNumMDI();

    // show the very first window in maximized mode
    if ( ws->windowList().isEmpty() )
    {
        w->setCaption(capBase);
        w->showMaximized();
    }
    else
    {
        mdiTools->show();

        if(pDasv != NULL) 
        {
            currView->setPointerType(pDasv->getPointerType());
            // Protects from waterfall asv that don't have a highlightmgr.
            // Also don't do this if the actual mdi is from a different trace.
            if(pDasv->getHighLightMgr() != NULL)
            {
                currView->copyHighLightMgrState(pDasv->getHighLightMgr());
            }
            currView->cloneAnnotationCtrl(pDasv->getAnnotationCtrl());
        }
        if(myMDI != NULL)
        {
            myMDI->showNormal();
        }
        w->setCaption(cap);
        w->show();
    }

    currView->getDisWindow()->createTabs();

    // After the creation of a Floorplan window we make visible the Floorplan control toolbar
    playTools->show();

    // by default the MDI will obei
    w->setIcon(IconFactory::getInstance()->image32);
    w->setIconText("Locked");

    myStatusBar->message("New Window Created", 2000);
    connect(w, SIGNAL(mdiCloseEvent(MDIWindow*)), this, SLOT(mdiCloseEvent(MDIWindow*)));
    connect(w, SIGNAL(firstCycle(MDIWindow*)) , this, SLOT(firstCycleReached(MDIWindow*)));
    connect(w, SIGNAL(lastCycle(MDIWindow*))  , this, SLOT(lastCycleReached(MDIWindow*)));

    connect(ptbRewind      , SIGNAL(clicked())                 , w, SLOT(rewind()));
    connect(ptbStepBackward, SIGNAL(clicked())                 , w, SLOT(stepBackward()));
    connect(ptbStop        , SIGNAL(clicked())                 , w, SLOT(stop()));
    connect(ptbPlay        , SIGNAL(clicked())                 , w, SLOT(play()));
    connect(ptbStepForward , SIGNAL(clicked())                 , w, SLOT(stepForward()));
    connect(ptbForward     , SIGNAL(clicked())                 , w, SLOT(forward()));
    connect(pcmbFPS        , SIGNAL(activated(const QString &)), w, SLOT(setFPS(const QString &)));
    connect(pcmbCPF        , SIGNAL(activated(const QString &)), w, SLOT(setCPF(const QString &)));

    connect(currView, SIGNAL(status_msg_changed(QString))                                    , this, SLOT(status_msg_changed(QString)));
    connect(currView, SIGNAL(cycle_msg_changed(QString))                                     , this, SLOT(cycle_msg_changed(QString)));
    connect(currView, SIGNAL(row_msg_changed(QString))                                       , this, SLOT(row_msg_changed(QString)));
    connect(currView, SIGNAL(progress_cnt_changed(int))                                      , this, SLOT(progress_cnt_changed(int)));
    connect(currView, SIGNAL(progress_reinit(int))                                           , this, SLOT(progress_reinit(int)));
    connect(currView, SIGNAL(progress_show())                                                , this, SLOT(progress_show()));
    connect(currView, SIGNAL(progress_hide())                                                , this, SLOT(progress_hide()));
    connect(currView, SIGNAL(zoomStatusChanged(int,int))                                     , this, SLOT(zoomStatusChanged(int,int)));

    // Floorplan only generate the following signals
    connect(       w, SIGNAL(cycleChanged(Q_INT64) )        , this, SLOT(do_cycleChanged(Q_INT64)));
    connect(currView, SIGNAL(threeDMoved(double, double)) , this, SLOT(do_threeDMoved(double, double)));
    connect(currView, SIGNAL(itemChanged(double))         , this, SLOT(do_itemChanged(double)));

    connect(currView, SIGNAL(asvZoomed(double,double))                                        , this, SLOT(asvZoomed(double,double)));
    connect(currView, SIGNAL(newZoomWindow(int,int,int,int,double,double,int,int))            , this, SLOT(newZoomWindow(int,int,int,int,double,double,int,int)));
    connect(currView, SIGNAL(toggledShadowCol(int,bool))                                      , this, SLOT(toggledShadowCol(int,bool)));
    connect(currView, SIGNAL(clearAllShadedCols())                                            , this, SLOT(clearAllShadedCols()));
    connect(currView, SIGNAL(purgeAllHighlight())                                             , this, SLOT(purgeAllHighlight()));
    connect(currView, SIGNAL(purgeLastHighlight())                                            , this, SLOT(purgeLastHighlight()));

    connect(currView, SIGNAL(syncToCycle(CYCLE)), this, SLOT(do_syncToCycle(CYCLE)));
    connect(currView, SIGNAL(syncToCycleTrace(CYCLE)), this, SLOT(do_syncToCycleTrace(CYCLE)));

    connect(currView                     , SIGNAL(currentAnnItemChanged(AnnotationItem*))            , this, SLOT(currentAnnItemChanged(AnnotationItem*)));    
    connect(currView->getAnnotationCtrl(), SIGNAL(annotationAdded(AnnotationItem*))                  , this, SLOT(annotationAdded(AnnotationItem*)));    
    connect(currView->getAnnotationCtrl(), SIGNAL(annotationRemoved(AnnotationItem*))                , this, SLOT(annotationRemoved(AnnotationItem*)));    
    connect(currView->getAnnotationCtrl(), SIGNAL(annotationChanged(AnnotationItem*,AnnotationItem*)), this, SLOT(annotationChanged(AnnotationItem*,AnnotationItem*)));    

    if(pTimer->isActive())
    {
        w->play();
    }

    return w;
}

/**
 * Creates a new Waterfall  view
 */
MDIWindow *
Dreams::newWaterfallDView(DreamsDB * trace)
{
    DAvtView* pDasv = (DAvtView *)(getActiveASV());
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();

    // Computes the waterfall data.
    trace->layoutWaterfallComputeData();
    MDIWaterfallWindow * w = new MDIWaterfallWindow(trace, this, ws, NULL, WDestructiveClose);
    if(!w->init())
    {
        QMessageBox::critical(NULL, "Error", "Trying to open a Waterfall window when no tab is defined.\nAdd a tab in your ADF file.", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
        w->close();
        return NULL;
    }

    mdiList->append(w);
    w->resize(800,600); // default non-maximized size
    int myMDINumber = numNextWindow++;
    numWFWindow++;

    // attach the diswin reference
    WaterfallDAvtView* currView = static_cast<WaterfallDAvtView *>(w->getAScrollView());
    QString capBase = trace->getTraceName() + ": Waterfall";
    QString cap = capBase + " ("+QString::number(myMDINumber)+")";
    currView->setMDINumber(myMDINumber);

    trace->incNumMDI();

    // Shows the mdi lock tools if more than one ascrollview defined.
    if(w->getListAScrollView().size() > 1)
    {
        mdiTools->show();
    }

    // show the very first window in maximized mode
    if ( ws->windowList().isEmpty() )
    {
        w->setCaption(capBase);
        w->showMaximized();
    }
    else
    {
        if(pDasv != NULL) 
        {
            //currView->setPointerType(pDasv->getPointerType());
            if(pDasv->getHighLightMgr() != NULL)
            {
                currView->copyHighLightMgrState(pDasv->getHighLightMgr());
            }
            //currView->cloneAnnotationCtrl(pDasv->getAnnotationCtrl());
        }
        if(myMDI != NULL)
        {
            myMDI->showNormal();
        }
        w->setCaption(cap);
        w->show();
    }

    // After the creation of a WF view we make visible the specific toolbar 
    //wfallTools->show();

    // by default the MDI will obei
    w->setIcon(IconFactory::getInstance()->image32);
    w->setIconText("Locked");

    myStatusBar->message("New Waterfall Window Created", 2000);
    connect(w, SIGNAL(mdiCloseEvent(MDIWindow*)), this, SLOT(mdiCloseEvent(MDIWindow*)));

    vector<AScrollView *> lasv = w->getListAScrollView();

    for(UINT32 i = 0; i < lasv.size(); i++)
    {
        currView = (WaterfallDAvtView *) lasv[i];

        connect(currView, SIGNAL(status_msg_changed(QString)) , this, SLOT(status_msg_changed(QString)));
        connect(currView, SIGNAL(cycle_msg_changed(QString))  , this, SLOT(cycle_msg_changed(QString)));
        connect(currView, SIGNAL(row_msg_changed(QString))    , this, SLOT(row_msg_changed(QString)));
        connect(currView, SIGNAL(progress_cnt_changed(int))   , this, SLOT(progress_cnt_changed(int)));
        connect(currView, SIGNAL(progress_reinit(int))        , this, SLOT(progress_reinit(int)));
        connect(currView, SIGNAL(progress_show())             , this, SLOT(progress_show()));
        connect(currView, SIGNAL(progress_hide())             , this, SLOT(progress_hide()));
        connect(currView, SIGNAL(zoomStatusChanged(int,int))  , this, SLOT(zoomStatusChanged(int,int)));

        connect(currView, SIGNAL(cycleChanged(Q_INT64))               , this, SLOT(do_cycleChanged(Q_INT64)));
        connect(currView, SIGNAL(itemChanged(double))               , this, SLOT(do_itemChanged(double)));
        connect(currView, SIGNAL(threeDMoved(double, double))       , this, SLOT(do_threeDMoved(double, double)));
        connect(currView, SIGNAL(fourDMoved(double, double, double)), this, SLOT(do_fourDMoved(double, double, double)));

        connect(currView, SIGNAL(asvZoomed(double,double))                                        , this, SLOT(asvZoomed(double,double)));
        connect(currView, SIGNAL(newZoomWindow(int,int,int,int,double,double,int,int))            , this, SLOT(newZoomWindow(int,int,int,int,double,double,int,int)));
        connect(currView, SIGNAL(toggledShadowCol(int,bool))                                      , this, SLOT(toggledShadowCol(int,bool)));
        connect(currView, SIGNAL(clearAllShadedCols())                                            , this, SLOT(clearAllShadedCols()));
        connect(currView, SIGNAL(purgeAllHighlight())                                             , this, SLOT(purgeAllHighlight()));
        connect(currView, SIGNAL(purgeLastHighlight())                                            , this, SLOT(purgeLastHighlight()));

        connect(currView,                      SIGNAL(currentAnnItemChanged(AnnotationItem*))            , this, SLOT(currentAnnItemChanged(AnnotationItem*)));    
        connect(currView->getAnnotationCtrl(), SIGNAL(annotationAdded(AnnotationItem*))                  , this, SLOT(annotationAdded(AnnotationItem*)));    
        connect(currView->getAnnotationCtrl(), SIGNAL(annotationRemoved(AnnotationItem*))                , this, SLOT(annotationRemoved(AnnotationItem*)));    
        connect(currView->getAnnotationCtrl(), SIGNAL(annotationChanged(AnnotationItem*,AnnotationItem*)), this, SLOT(annotationChanged(AnnotationItem*,AnnotationItem*)));
    }

    return w;
}

MDIWindow *
Dreams::newResourceDView(DreamsDB * trace)
{
    DAvtView * pasv = static_cast<DAvtView *>(getActiveASV());
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();

    MDIResourceWindow * w = new MDIResourceWindow(trace, this, ws, 0, WDestructiveClose);
    w->init();
    mdiList->append(w);
    w->resize(800, 600); // default non-maximized size
    int myMDINumber = numNextWindow++;

    // attach the diswin reference
    ResourceDAvtView * cv = (ResourceDAvtView *) (w->getAScrollView());
    QString capBase = trace->getTraceName() + ": Resource Window";
    QString cap = capBase + " ("+QString::number(myMDINumber)+")";
    w->setMDINumber(myMDINumber);

    // Computes the Dreams data.
    trace->layoutResourceComputeData();

    trace->incNumMDI();
    cv->setRealContentsSize();

    if(ws->windowList().isEmpty())
    {
        w->setCaption(capBase);
        w->showMaximized();
    }
    else
    {
        mdiTools->show();

        if(pasv != NULL) 
        {
            cv->copyColumnShadows(pasv);
            cv->setPointerType(pasv->getPointerType());
            // Protects from waterfall asv that don't have a highlightmgr.
            // Also don't do this if the actual mdi is from a different trace.
            if(pasv->getHighLightMgr() != NULL)
            {
                cv->copyHighLightMgrState(pasv->getHighLightMgr());
            }
            cv->cloneAnnotationCtrl(pasv->getAnnotationCtrl());
        }
        if(myMDI != NULL)
        {
            myMDI->showNormal();
        }
        w->setCaption(cap);
        w->show();
    }

    // Creates the tabs of the window.
    cv->getDisWindow()->createTabs();
    
    // by default the MDI will obei
    w->setIcon(IconFactory::getInstance()->image32);
    w->setIconText("Locked");

    myStatusBar->message("New Window Created", 2000);
    connect(w,SIGNAL(mdiCloseEvent(MDIWindow *)), this, SLOT(mdiCloseEvent(MDIWindow *)));
    connect(cv, SIGNAL(status_msg_changed(QString)), this, SLOT(status_msg_changed(QString)));
    connect(cv, SIGNAL(cycle_msg_changed(QString)), this, SLOT(cycle_msg_changed(QString)));
    connect(cv, SIGNAL(row_msg_changed(QString)), this, SLOT(row_msg_changed(QString)));
    connect(cv, SIGNAL(progress_cnt_changed(int)), this, SLOT(progress_cnt_changed(int)));
    connect(cv, SIGNAL(progress_reinit(int)), this, SLOT(progress_reinit(int)));
    connect(cv, SIGNAL(progress_show()), this, SLOT(progress_show()));
    connect(cv, SIGNAL(progress_hide()), this, SLOT(progress_hide()));
    connect(cv, SIGNAL(zoomStatusChanged(int, int)), this, SLOT(zoomStatusChanged(int, int)));

    // View synch methods
    connect(cv, SIGNAL(cycleChanged(Q_INT64)), this, SLOT(do_cycleChanged(Q_INT64)));
    connect(cv, SIGNAL(resourceChanged(double)), this, SLOT(do_resourceChanged(double)));
    connect(cv, SIGNAL(itemChanged(double)), this, SLOT(do_itemChanged(double)));
    connect(cv, SIGNAL(threeDMoved(double, double)), this, SLOT(do_threeDMoved(double, double)));
    connect(cv, SIGNAL(fourDMoved(double, double, double)), this, SLOT(do_fourDMoved(double, double, double)));

    connect(cv, SIGNAL(asvZoomed(double, double)),this,SLOT(asvZoomed(double, double)));
    connect(cv, SIGNAL(newZoomWindow(int, int, int, int, double, double, int, int)), this, SLOT(newZoomWindow(int, int, int, int, double, double, int, int)));
    connect(cv, SIGNAL(toggledShadowCol(int, bool)), this, SLOT(toggledShadowCol(int, bool)));
    connect(cv, SIGNAL(clearAllShadedCols()), this, SLOT(clearAllShadedCols()));

    connect(cv, SIGNAL(syncToCycle(CYCLE)), this, SLOT(do_syncToCycle(CYCLE)));
    connect(cv, SIGNAL(syncToCycleTrace(CYCLE)), this, SLOT(do_syncToCycleTrace(CYCLE)));
    connect(cv, SIGNAL(syncToItem(QString, QString, QString, QString, UINT64)), this, SLOT(do_syncToItem(QString, QString, QString, QString, UINT64)));

    connect(cv, SIGNAL(purgeLastHighlight()), this, SLOT(purgeLastHighlight()));
    connect(cv, SIGNAL(purgeAllHighlight()), this, SLOT(purgeAllHighlight()));

    connect(cv, SIGNAL(currentAnnItemChanged(AnnotationItem *)), this, SLOT(currentAnnItemChanged(AnnotationItem *)));

    connect(cv->getAnnotationCtrl(), SIGNAL(annotationAdded(AnnotationItem *)), this, SLOT(annotationAdded(AnnotationItem *)));
    connect(cv->getAnnotationCtrl(), SIGNAL(annotationRemoved(AnnotationItem *)), this, SLOT(annotationRemoved(AnnotationItem *)));
    connect(cv->getAnnotationCtrl(), SIGNAL(annotationChanged(AnnotationItem *, AnnotationItem *)), this, SLOT(annotationChanged(AnnotationItem *, AnnotationItem *)));

    return w;
}

void 
Dreams::mdiWindowActivated(QWidget* w)
{
    // Updates the state to the current trace.
    enableMenuOptions(true);

    focusMDI = w;

    // We verify if the MDI is a Resource window, a Floorplan window, a Waterfall window or a Watch window.
    if(dynamic_cast<MDIResourceWindow *>(w))
    {
        MDIResourceWindow* m = (MDIResourceWindow*)w;
        ResourceDAvtView* c = (ResourceDAvtView*)m->getAScrollView(); 
        HighLightMgr* hl = c->getHighLightMgr();
        
        // update menus accordingly with new MDI state
        // highlighting related
        highlightChanged(hl->getNumActiveCriteria());
        
        // view highlight,nhigh,etc
        actionVHIGH->setOn(m->getVHIGH());
        actionVNHIGH->setOn(m->getVNHIGH());
        actionVBHIGH->setOn(m->getVBHIGH());
        
        // update bkMgr state
        bkMgr->setAScrollView(c);
        
        // update enabled/disabled docked windows
        window->setItemChecked(dis_win_id,c->getEnabledDisWindow());
        window->setItemChecked(anc_win_id,c->getEnabledAncestorWindow());
        window->setItemChecked(push_win_id,c->getEnabledPushoutWindow());
        window->setItemChecked(tag_win_id,m->getEnabledTagWindow());

        // update tagselection dialog
        tgsel->setCurrentText(c->getCurrentTagSelector());
        
        // update zoom view dialog
        zoomStatusChanged(c->getZX(),c->getZY());
        
        // update dreamsdb active objects
        ((MDIDreamsWindow *) w)->getDreamsDB()->setColumnShadows(c->getColumnShadows());
        
        // Disable the Floorplan toolbar
        playTools->setEnabled(false);
    }
    else if(dynamic_cast<MDIFloorplanWindow *>(w))
    {
        // DEBUG
        //cerr << "Dreams::mdiWindowActivated : Floorplan MDI window opened" << endl;
        MDIFloorplanWindow * m = (MDIFloorplanWindow *) w;
        FloorplanDAvtView * c = (FloorplanDAvtView *) m->getAScrollView(); 
        HighLightMgr * hl = c->getHighLightMgr();

        // update menus accordingly with new MDI state
        // highlighting related
        highlightChanged(hl->getNumActiveCriteria());

        // update tagselection dialog
        tgsel->setCurrentText(c->getCurrentTagSelector());

        pcmbFPS->setCurrentText(" " + QString::number(m->getFPS()) + "fps ");
        pcmbCPF->setCurrentText(" " + QString::number(m->getCPF()) + "cpf ");
        playTools->setEnabled(true);

        // update enabled/disabled docked windows
        window->setItemChecked(dis_win_id, c->getEnabledDisWindow());
        window->setItemChecked(tag_win_id, m->getEnabledTagWindow());
        window->setItemEnabled(anc_win_id, false);
        window->setItemChecked(anc_win_id, false);
        window->setItemEnabled(push_win_id, false);
        window->setItemChecked(push_win_id, false);
    }
    else if(dynamic_cast<MDIWatchWindow *>(w))
    {
        MDIWatchWindow *m = static_cast<MDIWatchWindow *>(w);

        pcmbFPS->setCurrentText(" " + QString::number(m->getFPS()) + "fps ");
        pcmbCPF->setCurrentText(" " + QString::number(m->getCPF()) + "cpf ");
        playTools->setEnabled(true);

        // update enabled/disabled docked windows
        window->setItemEnabled(anc_win_id, false);
        window->setItemChecked(anc_win_id, false);
        window->setItemEnabled(push_win_id, false);
        window->setItemChecked(push_win_id, false);
    }
    else if(dynamic_cast<MDIWaterfallWindow *>(w))
    {
        MDIWaterfallWindow * m = (MDIWaterfallWindow *) w;
        WaterfallDAvtView * c = (WaterfallDAvtView *) m->getAScrollView(); 
        HighLightMgr * hl = c->getHighLightMgr();

        // update menus accordingly with new MDI state
        // highlighting related
        highlightChanged(hl->getNumActiveCriteria());

        // update tagselection dialog
        tgsel->setCurrentText(c->getCurrentTagSelector());

        playTools->setEnabled(false);
        window->setItemEnabled(anc_win_id, false);
        window->setItemChecked(anc_win_id, false);
        window->setItemEnabled(push_win_id, false);
        window->setItemChecked(push_win_id, false);
    }
}

void
Dreams::repaintAllMDIContents(bool erase)
{
    MDIWindow* cmdi;
    DAvtView* casv;
    ws->setUpdatesEnabled( TRUE );
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv=(DAvtView*)(cmdi->getAScrollView());
        if(casv != NULL)
        {
            casv->repaintContents(erase);
        }
    }
}

void 
Dreams::highlightChanged(int count)
{
   enableMenuPurgeAll(count>1);
   enableMenuPurgeLast(count>0);
   enableMenuShowCriteria(count>0);
}

void 
Dreams::closeEvent(QCloseEvent *e)
{
    QWidgetList windows = ws->windowList();
    if ( windows.count() ) 
    {
        for ( int i = 0; i < int(windows.count()); ++i ) 
        {
            QWidget *window = windows.at( i );
            if ( !window->close() ) 
            {
                e->ignore();
                return;
            }
        }
    }
    QMainWindow::closeEvent( e );
}

void 
Dreams::do_tileHorizontal()
{
    // primitive horizontal tiling
    QWidgetList windows = ws->windowList();
    if ( !windows.count() )
    return;
    
    int heightForEach = ws->height() / windows.count();
    int y = 0;
    for ( int i = 0; i < int(windows.count()); ++i ) {
    QWidget *window = windows.at(i);
    if ( window->testWState( WState_Maximized ) ) {
        // prevent flicker
        window->hide();
        window->showNormal();
    }
    int preferredHeight = window->minimumHeight()+window->parentWidget()->baseSize().height();
    int actHeight = QMAX(heightForEach, preferredHeight);
    
    window->parentWidget()->setGeometry( 0, y, ws->width(), actHeight );
    y += actHeight;
    }
}

void 
Dreams::do_tileVertical()
{
    ws->tile();
}

void 
Dreams::do_cascade()
{
    ws->cascade();
}

void
Dreams::do_windowChanged(int id)
{
    trace_menu_id = id;
}

QString
Dreams::getWelcomeMessage()
{
    return QString("Welcome to Dreams, for help press F1");
}

void
Dreams::addTagSelector(QToolBar* parent)
{
    parent->addSeparator();
    QHBox * qhb = new QHBox(parent);
    qhb->setSpacing(10);

    QPixmap mwToolIcon = QPixmap( selectToolBar );
    QLabel * icnLbl = new QLabel(qhb);
    icnLbl->setPixmap(mwToolIcon);

    tgsel = new TagSelector(qhb, FavoriteTags::getInstance());
    tgsel->setEditable(true);
    tgsel->setAutoCompletion(true);
    QToolTip::add(tgsel, "Highlight Criteria Selector");

    connect(tgsel, SIGNAL(activated(const QString &)), this, SLOT(slotTgSelComboActivated(const QString &)));
    connect(tgsel, SIGNAL(textChanged(const QString &)), this, SLOT(slotTextChanged(const QString &)));

    parent->addSeparator();
    qhb = new QHBox(parent);
    qhb->setSpacing(10);

    icnLbl = new QLabel(qhb);
    icnLbl->setPixmap(IconFactory::getInstance()->image36);

    syncTagSel = new TagSelector(qhb, FavoriteTags::getInstance());
    syncTagSel->setEditable(true);
    syncTagSel->setAutoCompletion(true);
    QToolTip::add(tgsel, "Item Sync Criteria Selector");
}

void 
Dreams::slotTextChanged ( const QString & string )
{
    //printf ("slotTextChanged called\n");
}


void 
Dreams::slotTgSelComboActivated( const QString &s )
{
    MDIDreamsWindow * cmdi = (MDIDreamsWindow *) mdiList->first();

    while(cmdi != NULL)
    {
        // We verify if the MDI is a Resource window or a Floorplan window
        if(dynamic_cast<MDIResourceWindow *>(cmdi) || dynamic_cast<MDIFloorplanWindow *>(cmdi))
        {
            DAvtView * avt = (DAvtView *) cmdi->getAScrollView(); 
            avt->slotTgSelComboActivated(s);
        }
        else if(dynamic_cast<MDIWaterfallWindow *>(cmdi))
        {
            vector<AScrollView *> lasv = ((MDIWaterfallWindow *) cmdi)->getListAScrollView();

            for(UINT32 i = 0; i < lasv.size(); i++)
            {
                ((DAvtView *) lasv[i])->slotTgSelComboActivated(s);
            }
        }

        cmdi = (MDIDreamsWindow *) mdiList->next();
    }
}

/**
 * Slot to attend the lastCycle signal of the MDIFloorplanWindow class.
 * It decrements the number of active Floorplan window and updates the
 * Floorplan controls properly
 *
 * @param stopped The MDI window that has stopped its animation
 */
void 
Dreams::lastCycleReached(MDIWindow *stopped)
{
    numFloorplanWindowStopped++;
    if(numFloorplanWindowStopped == numPlayWindow) do_stopFloorplan();
}

/**
 * Slot to attend the firstCycle signal of the MDIFloorplanWindow class.
 * It decrements the number of active Floorplan window and updates the
 * Floorplan controls properly
 *
 * @param stopped The MDI window that has stopped its animation
 */
void 
Dreams::firstCycleReached(MDIWindow *stopped)
{
    numFloorplanWindowStopped++;
    if(numFloorplanWindowStopped == numPlayWindow) do_stopFloorplan();
}

void 
Dreams::do_cycleChanged(Q_INT64 ps)
{
    if(!panLocked)
    {
        return;
    }

    MDIDreamsWindow * activeMdi = static_cast<MDIDreamsWindow *>(getActiveMDI());
    AScrollView * asv = static_cast<AScrollView *>(getActiveASV());
    MDIDreamsWindow * cmdi = static_cast<MDIDreamsWindow *>(mdiList->first()); 

    while(cmdi != NULL)
    {
        if(dynamic_cast<MDIWaterfallWindow *>(cmdi))
        {
            if(cmdi->getMDILocked())
            {
                ((MDIWaterfallWindow *) cmdi)->attendCycleChanged(ps, asv);
            }
        }
        else
        {
            if((cmdi != activeMdi) && cmdi->getMDILocked())
            {
                cmdi->attendCycleChanged(ps);
            }
        }

        cmdi = static_cast<MDIDreamsWindow *>(mdiList->next());
    }
}

void 
Dreams::do_resourceChanged(double resource)
{
    if (!panLocked) return;

    MDIDreamsWindow * activeMdi = static_cast<MDIDreamsWindow *>(getActiveMDI());
    
    MDIDreamsWindow *cmdi = static_cast<MDIDreamsWindow *>(mdiList->first()); 
    while(cmdi != NULL)
    {
        if((cmdi != activeMdi) && cmdi->getMDILocked())
            cmdi->attendResourceChanged(resource);

        cmdi = static_cast<MDIDreamsWindow *>(mdiList->next());
    }
}

void 
Dreams::do_itemChanged(double item)
{
    if (!panLocked) return;

    MDIDreamsWindow * activeMdi = static_cast<MDIDreamsWindow *>(getActiveMDI());
    
    MDIDreamsWindow *cmdi = static_cast<MDIDreamsWindow *>(mdiList->first()); 
    while(cmdi != NULL)
    {
        if(dynamic_cast<MDIWaterfallWindow *>(cmdi))
        {
            ((MDIWaterfallWindow *) cmdi)->attendItemChanged(item, getActiveASV());
        }
        else
        {
            if((cmdi != activeMdi) && cmdi->getMDILocked())
            {
                cmdi->attendItemChanged(item);
            }
        }

        cmdi = (MDIDreamsWindow *) mdiList->next();
    }
}

void
Dreams::do_threeDMoved(double x, double y)
{
    if (!panLocked) return;

    MDIDreamsWindow * activeMdi = static_cast<MDIDreamsWindow *>(getActiveMDI());
    
    MDIDreamsWindow *cmdi = static_cast<MDIDreamsWindow *>(mdiList->first()); 
    while(cmdi != NULL)
    {
        if((cmdi != activeMdi) && cmdi->getMDILocked())
            cmdi->attendThreeDMoved(x, y);

        cmdi = static_cast<MDIDreamsWindow *>(mdiList->next());
    }
}

void 
Dreams::do_fourDMoved(double x, double y, double z)
{
    if(!panLocked)
    {
        return;
    }

    MDIDreamsWindow * activeMdi = static_cast<MDIDreamsWindow *>(getActiveMDI());
    
    MDIDreamsWindow *cmdi = static_cast<MDIDreamsWindow *>(mdiList->first()); 
    while(cmdi != NULL)
    {
        if((cmdi != activeMdi) && cmdi->getMDILocked())
            cmdi->attendFourDMoved(x, y, z);

        cmdi = static_cast<MDIDreamsWindow *>(mdiList->next());
    }
}

void
Dreams::do_syncToCycle(CYCLE cycle)
{
    MDIDreamsWindow *cmdi = static_cast<MDIDreamsWindow *>(mdiList->first()); 
    while(cmdi != NULL)
    {
        if(cmdi->getMDILocked())
        {
            cmdi->showCycle(cycle);
        }

        cmdi = static_cast<MDIDreamsWindow *>(mdiList->next());
    }
}

void
Dreams::do_syncToCycleTrace(CYCLE cycle)
{
    MDIDreamsWindow * cmdi = static_cast<MDIDreamsWindow *>(mdiList->first()); 
    MDIDreamsWindow * activeMdi = static_cast<MDIDreamsWindow *>(getActiveMDI());
    DreamsDB * trace = activeMdi->getDreamsDB();

    while(cmdi != NULL)
    {
        if(cmdi->getMDILocked() && (cmdi != activeMdi) && (cmdi->getDreamsDB() == trace))
        {
            cmdi->showCycle(cycle);
        }

        cmdi = static_cast<MDIDreamsWindow *>(mdiList->next());
    }
}

void
Dreams::do_syncToItem(QString edgeName, QString fromNodeName, QString toNodeName, QString tagName, UINT64 tagValue)
{
    MDIDreamsWindow * cmdi;
    MDIDreamsWindow * activeMdi = static_cast<MDIDreamsWindow *>(getActiveMDI());

    QPtrListIterator<MDIWindow> iter(* mdiList);

    while(iter.current() != NULL)
    {
        cmdi = (MDIDreamsWindow *) iter.current();
        ++iter;
        MDIResourceWindow * mdiresource = dynamic_cast<MDIResourceWindow *>(cmdi);

        if((mdiresource != NULL) && (mdiresource != activeMdi) && cmdi->getMDILocked())
        {
            mdiresource->showItem(edgeName, fromNodeName, toNodeName, tagName, tagValue);
        }
    }
}

void 
Dreams::toggledShadowCol(int col, bool enabled)
{
    if(!shadingLocked)
    {
        return;
    }

    MDIWindow * cmdi;
    DAvtView * activeAsv = (DAvtView *) getActiveASV();

    if(!activeAsv)
    {
        return;
    }

    for(cmdi = mdiList->first(); cmdi; cmdi = mdiList->next())
    {
        if(dynamic_cast<MDIResourceWindow *>(cmdi))
        {
            ResourceDAvtView * asv = (ResourceDAvtView *) cmdi->getAScrollView();
            if((asv != activeAsv) && asv->getMDILocked())
            {
                asv->shadeColumn(col, enabled);
            }
        }
        else if(dynamic_cast<MDIWaterfallWindow *>(cmdi))
        {
            vector<AScrollView *> lasv = ((MDIWaterfallWindow *) cmdi)->getListAScrollView();

            for(UINT32 i = 0; i < lasv.size(); i++)
            {
                WaterfallDAvtView * asv = (WaterfallDAvtView *) lasv[i];

                if((asv != activeAsv) && asv->getMDILocked())
                {
                    asv->shadeColumn(col, enabled);
                }
            }
        }
    }
}

void 
Dreams::clearAllShadedCols()
{
    if(!shadingLocked)
    {
        return;
    }

    MDIWindow * cmdi;
    DAvtView * activeAsv = (DAvtView *) getActiveASV();

    if(!activeAsv)
    {
        return;
    }

    for(cmdi = mdiList->first(); cmdi; cmdi = mdiList->next())
    {
        if(dynamic_cast<MDIResourceWindow *>(cmdi))
        {
            ResourceDAvtView * asv = (ResourceDAvtView *) cmdi->getAScrollView();
            if((asv != activeAsv) && asv->getMDILocked())
            {
                asv->clearShadedColumnsNoPropagate();
            }
        }
        else if(dynamic_cast<MDIWaterfallWindow *>(cmdi))
        {
            vector<AScrollView *> lasv = ((MDIWaterfallWindow *) cmdi)->getListAScrollView();

            for(UINT32 i = 0; i < lasv.size(); i++)
            {
                WaterfallDAvtView * asv = (WaterfallDAvtView *) lasv[i];

                if((asv != activeAsv) && asv->getMDILocked())
                {
                    asv->clearShadedColumnsNoPropagate();
                }
            }
        }
    }
}

void 
Dreams::asvZoomed(double x, double y)
{
    Q_ASSERT(x != 0.0);
    Q_ASSERT(y != 0.0);
    
    if(!zoomLocked)
    {
        return;
    }

    double csfx, csfy;
    
    MDIWindow * cmdi;
    DAvtView * activeAsv = (DAvtView *) getActiveASV();
    if(!activeAsv)
    {
        return;
    }

    for(cmdi = mdiList->first(); cmdi; cmdi = mdiList->next())
    {
        DAvtView * avt = (DAvtView *) cmdi->getAScrollView();

        if((avt != activeAsv) && (avt->getMDILocked()))
        {
            if(dynamic_cast<ResourceDAvtView *>(cmdi->getAScrollView()) || dynamic_cast<WaterfallDAvtView *>(cmdi->getAScrollView()))
            {
                csfx = avt->getScallingFactorX();
                csfy = avt->getScallingFactorY();
                avt->setScallingFactorX(csfx * x);
                avt->setScallingFactorY(csfy * y);
            }
        }
    }
}

void 
Dreams::contentsSelectedEvent(TAG_ID tagId, UINT64 tagValue, bool adding, HighLightType htype, bool noErase)
{
    if(!highlightLocked)
    {
        return;
    }

    MDIWindow* cmdi;
    DAvtView* casv;
    DAvtView* activeAsv = static_cast<DAvtView *>(getActiveASV());

    if(!activeAsv)
    {
        return;
    }

    QPtrListIterator<MDIWindow> iter(*mdiList);
    while(cmdi = iter.current())
    {
        if(dynamic_cast<MDIWaterfallWindow *>(cmdi))
        {
            vector<AScrollView *> lasv = ((MDIWaterfallWindow *) cmdi)->getListAScrollView();
            for(UINT32 i = 0; i < lasv.size(); i++)
            {
                casv = (DAvtView *) lasv[i];

                if((casv != NULL) && (casv != activeAsv))
                {
                    if(casv->getMDILocked() && (casv->getHighLightMgr() != NULL))
                    {
                        bool rep = casv->getHighLightMgr()->contentsSelectEvent(tagId, tagValue, adding, htype, noErase, false);
                        if(rep)
                        {
                            casv->repaintContents(false);
                        }
                    }
                }
            }
        }
        else
        {
            casv = static_cast<DAvtView*>(cmdi->getAScrollView());
            // TODO: we must redesign the contentsSelectedEvent broadcast
            // because not all the views have an AScrollView!!!
            if((casv != NULL) && (casv != activeAsv))
            {
                if(casv->getMDILocked() && (casv->getHighLightMgr() != NULL))
                {
                    bool rep = casv->getHighLightMgr()->contentsSelectEvent(tagId, tagValue, adding, htype, noErase, false);
                    if(rep)
                    {
                        casv->repaintContents(false);
                    }
                }
            }
        }
        ++iter;
    }
}

void 
Dreams::purgeLastHighlight()
{
    if (!highlightLocked) return;

    MDIWindow* cmdi;
    DAvtView* casv;
    DAvtView* activeAsv = static_cast<DAvtView *>(getActiveASV());
    if (!activeAsv) return;

    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        if(dynamic_cast<MDIWaterfallWindow *>(cmdi))
        {
            vector<AScrollView *> lasv = ((MDIWaterfallWindow *) cmdi)->getListAScrollView();
            for(UINT32 i = 0; i < lasv.size(); i++)
            {
                casv = (DAvtView *) lasv[i];

                if((casv != NULL) && (casv != activeAsv))
                {
                    if(casv->getMDILocked() && (casv->getHighLightMgr() != NULL))
                    {
                        bool rep = casv->getHighLightMgr()->purgeLast();
                        if(rep)
                        {
                            casv->repaintContents(false);
                        }
                    }
                }
            }
        }
        else
        {
            casv = (DAvtView *) cmdi->getAScrollView();
            // TODO: we must redesign the contentsSelectedEvent broadcast
            // because not all the views have an AScrollView!!!
            if((casv != NULL) && (casv != activeAsv))
            {
                if(casv->getMDILocked() && (casv->getHighLightMgr() != NULL))
                {
                    bool rep = casv->getHighLightMgr()->purgeLast();
                    if(rep)
                    {
                        casv->repaintContents(false);
                    }
                }
            }
        }
    }
}

void 
Dreams::purgeAllHighlight()
{
    if (!highlightLocked) return;

    MDIWindow* cmdi;
    DAvtView* casv;
    DAvtView* activeAsv = static_cast<DAvtView*>(getActiveASV());
    if (!activeAsv) return;
    
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        if(dynamic_cast<MDIWaterfallWindow *>(cmdi))
        {
            vector<AScrollView *> lasv = ((MDIWaterfallWindow *) cmdi)->getListAScrollView();
            for(UINT32 i = 0; i < lasv.size(); i++)
            {
                casv = (DAvtView *) lasv[i];

                if((casv != NULL) && (casv != activeAsv))
                {
                    if(casv->getMDILocked() && (casv->getHighLightMgr() != NULL))
                    {
                        bool rep = casv->getHighLightMgr()->purgeAll();
                        if(rep)
                        {
                            casv->repaintContents(false);
                        }
                    }
                }
            }
        }
        else
        {
            casv = (DAvtView *) cmdi->getAScrollView();
            // TODO: we must redesign the contentsSelectedEvent broadcast
            // because not all the views have an AScrollView!!!
            if((casv != NULL) && (casv != activeAsv))
            {
                if(casv->getMDILocked() && (casv->getHighLightMgr() != NULL))
                {
                    bool rep = casv->getHighLightMgr()->purgeAll();
                    if(rep)
                    {
                        casv->repaintContents(false);
                    }
                }
            }
        }
    }
}

void 
Dreams::unhighlightCriteria(TAG_ID tagId, UINT64 tagValue)
{
    if(!highlightLocked)
    {
        return;
    }

    MDIWindow * cmdi;
    DAvtView * casv;
    DAvtView * activeAsv = static_cast<DAvtView *>(getActiveASV());

    if(!activeAsv)
    {
        return;
    }

    for(cmdi = mdiList->first(); cmdi; cmdi = mdiList->next())
    {
        if(dynamic_cast<MDIWaterfallWindow *>(cmdi))
        {
            vector<AScrollView *> lasv = ((MDIWaterfallWindow *) cmdi)->getListAScrollView();
            for(UINT32 i = 0; i < lasv.size(); i++)
            {
                casv = (DAvtView *) lasv[i];

                if((casv != NULL) && (casv != activeAsv))
                {
                    if(casv->getMDILocked() && (casv->getHighLightMgr() != NULL))
                    {
                        bool rep = casv->getHighLightMgr()->unhighlightCriteria(tagId, tagValue);
                        if(rep)
                        {
                            casv->repaintContents(false);
                        }
                    }
                }
            }
        }
        else
        {
            casv = (DAvtView *) cmdi->getAScrollView();
            // TODO: we must redesign the contentsSelectedEvent broadcast
            // because not all the views have an AScrollView!!!
            if((casv != NULL) && (casv != activeAsv))
            {
                if(casv->getMDILocked() && (casv->getHighLightMgr() != NULL))
                {
                    bool rep = casv->getHighLightMgr()->unhighlightCriteria(tagId, tagValue);
                    if(rep)
                    {
                        casv->repaintContents(false);
                    }
                }
            }
        }
    }
}

void 
Dreams::newZoomWindow(int pwx,int pwy,int rwx,int rwy,double fx,double fy,int cx,int cy)
{
    MDIDreamsWindow * myMDI = (MDIDreamsWindow *) getActiveMDI();
    MDIWindow * nmdi = NULL;

    if(dynamic_cast<MDIResourceWindow *>(myMDI))
    {
        nmdi = (MDIResourceWindow *) newResourceDView(myMDI->getDreamsDB());
    }
    else if(dynamic_cast<MDIFloorplanWindow *>(myMDI))
    {
        nmdi = newFloorplanDView(myMDI->getDreamsDB());
    }
    else if(dynamic_cast<MDIWaterfallWindow *>(myMDI))
    {
        nmdi = newWaterfallDView(myMDI->getDreamsDB());
    }

    if(nmdi != NULL)
    {
        DAvtView * cv = (DAvtView *) nmdi->getAScrollView();

        // make it tmp rebel
        cv->setMDILocked(false);
        double wd = (double)(rwx - pwx);
        double hd = (double)(rwy - pwy);
        wd = fabs(wd);
        hd = fabs(hd);
        wd *= fx;
        hd *= fy;
        cv->resize((int) ceil(wd), (int) ceil(hd));
        cv->setMinimumSize((int) ceil(wd), (int) ceil(hd));
        nmdi->adjustSize();
        nmdi->update();

        cv->mouseWorldReleaseEvent_ZoomingNWinCallback(fx, fy, cx, cy);
        // lock it back (default locked)
        cv->setMDILocked(true);
        cv->setMinimumSize(0, 0);
    }
}

void 
Dreams::currentAnnItemChanged(AnnotationItem* ann)
{
    edit->setItemEnabled(copy_id,(ann!=NULL));
    //edit->setItemEnabled(cut_id,(ann!=NULL));
    edit->setItemEnabled(remove_id,(ann!=NULL));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_copy()
{
    edit->setItemEnabled(paste_id,true);
    DAvtView* cavt = (DAvtView*)getActiveASV();
    if(cavt)
    {
        cavt->annotationCopy();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_paste()
{
    DAvtView* cavt = (DAvtView *) getActiveASV();
    if(cavt)
    {
        cavt->annotationPaste();
    }
    //edit->setItemEnabled(paste_id,false);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_cut()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
Dreams::do_remove()
{
    DAvtView * cavt = (DAvtView *) getActiveASV();
    if(cavt)
    {
        cavt->annotationRemove();
    }
    currentAnnItemChanged(NULL);
}

void 
Dreams::annotationAdded(AnnotationItem* item)
{
    // propagate to other mdis if apporpiated
    if (!annotationLocked)
    {
        return;
    }
    // Beziers are not added to other views...
    if(dynamic_cast<AnnotationAutoBezierArrow *>(item) != NULL)
    {
        return;
    }

    MDIWindow* cmdi;
    DAvtView * casv;
    DAvtView * activeAsv = (DAvtView *) getActiveASV();
    
    if(!activeAsv)
    {
        return;
    }

    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv = (DAvtView *) cmdi->getAScrollView();
        if((casv != NULL) && (casv != activeAsv) && (casv->getMDILocked()))
        {
            AnnotationCtrl * actrl = casv->getAnnotationCtrl(); 
            actrl->addPropagatedItem(item);
        }
    }
}

void 
Dreams::annotationRemoved(AnnotationItem* item)
{
    // propagate to other mdis if apporpiated
    if (!annotationLocked) return;

    MDIWindow * cmdi;
    DAvtView * casv;
    DAvtView * activeAsv = (DAvtView *) getActiveASV();
    if(!activeAsv)
    {
        return;
    }
    
    for(cmdi = mdiList->first(); cmdi; cmdi = mdiList->next())
    {
        casv = (DAvtView *)(cmdi->getAScrollView());
        if((casv != NULL) && (casv != activeAsv) && (casv->getMDILocked()))
        {
            AnnotationCtrl* actrl = casv->getAnnotationCtrl(); 
            actrl->removePropagatedItem(item);
        }
    }
}

void 
Dreams::annotationChanged(AnnotationItem* newitem, AnnotationItem* olditem)
{
    // propagate to other mdis if apporpiated
    if (!annotationLocked) return;

    MDIWindow* cmdi;
    DAvtView * casv;
    DAvtView * activeAsv = (DAvtView*) getActiveASV();
    if(!activeAsv)
    {
        return;
    }
    
    for(cmdi = mdiList->first(); cmdi; cmdi = mdiList->next())
    {
        casv = (DAvtView *) cmdi->getAScrollView();
        if((casv != NULL) && (casv != activeAsv) && (casv->getMDILocked()))
        {
            AnnotationCtrl* actrl = casv->getAnnotationCtrl(); 
            actrl->changePropagatedItem(newitem,olditem);
        }
    }
}

/*
 * Gets the loading state of the dreams application.
 *
 * @return void.
 */
bool
Dreams::isLoading() const
{
    return loading;
}

/*
 * Sets the loading state of the dreams application.
 *
 * @return void.
 */
void
Dreams::setLoading(bool value)
{
    loading = value;
}

/*
 * Sets the actual clock for clock synch.
 *
 * @return void.
 */
void
Dreams::setCurrentClock(const ClockDomainEntry * clock)
{
    curClock = clock;
}

/*
 * Returns an iterator to the requested trace.
 *
 * @return the trace iterator.
 */
TraceListIter
Dreams::findTraceFromDB(DreamsDB * trace)
{
    TraceListIter it = traces.begin();

    while(it != traces.end())
    {
        if((* it)->getDreamsDB() == trace)
        {
            return it;
        }
        ++it;
    }
    Q_ASSERT(false);
}

/*
 * Returns an iterator to the requested trace.
 *
 * @return the trace iterator.
 */
TraceListIter
Dreams::findTraceFromWindow(INT32 window_id)
{
    TraceListIter it = traces.begin();

    while(it != traces.end())
    {
        if((* it)->getWindowMenuId() == window_id)
        {
            return it;
        }
        ++it;
    }
    Q_ASSERT(false);
}

/*
 * Returns an iterator to the requested trace.
 *
 * @return the trace iterator.
 */
TraceListIter
Dreams::findTraceFromClose(INT32 close_id)
{
    TraceListIter it = traces.begin();

    while(it != traces.end())
    {
        if((* it)->getCloseMenuId() == close_id)
        {
            return it;
        }
        ++it;
    }
    Q_ASSERT(false);
}

void
Dreams::removeTrace(DreamsDB * trace)
{
    TraceListIter it, it2;
    DreamsTraceData * trace_data;

    // Erases the trace from the trace list.
    it = findTraceFromDB(trace);
    trace_data = * it;
    it2 = it;
    ++it2;
    traces.erase(it);

    // Decrements the id of all the traces that have been loaded after
    // the deleted trace.
    while(it2 != traces.end())
    {
        (* it2)->decId();
        ++it2;
    }

    // Removes all the trace related stuff in the application.
    window->removeItem(trace_data->getWindowMenuId());
    trace_close->removeItem(trace_data->getCloseMenuId());

    QString logmsg = "-- DRL " + trace->getTraceName() + " closed.";
    log->addLog(logmsg);

    // Deletes the trace.
    delete trace_data;
}

// -----------------------------------------------------------------------------
// -- The main method
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    /*
    // QT guys messed up their classes so they are not 100% backward compatible...
    // so check whether we are compiling with QT 3.0.5 or latter
#ifdef QT_REQUIRE_VERSION
    QT_REQUIRE_VERSION( argc, argv, QString("3.0.5") )
#endif
    */

    bool ok;                 ///<
    DreamsLineParams params; ///< Parses the command line parameters.

    ok = params.parseLineParams(argc, argv);

    if(!ok)
    {
        printf("Usage: %s [-lde] [-adf adf_file] [file]\n", argv[0]);
        printf("Regression usage: %s -lde -no-gui -adf adf_file drl_file\n", argv[0]);
        exit(-1);
    }

    // Running in no-gui mode.
    if(params.noGUIMode())
    {
        DumpMode dump;

        dump.start(params);
    }
    // Normal running mode.
    else
    {
        dockInit = true;

        // create the base qApp object:
        QApplication app(argc,argv);

        // create the main object
        Dreams * tdo = new Dreams(params, NULL);
        Q_ASSERT(tdo != NULL);

        // put default font size
        QFont defFont = qApp->font();
        defFont.setPointSize(PreferenceMgr::getInstance()->getFontSize());
        qApp->setFont(defFont,true);

        QLabel * splash = Dreams::showSplash();
        qApp->setMainWidget(tdo);

        tdo->setCaption(tdo->getApplicationCaption());
        tdo->initApplication();

        if(params.hasDRLFile())
        {
            tdo->closeSplash();
            tdo->do_genericOpen();
        }

        tdo->show();
        tdo->closeSplash();

        //QObject::connect( qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()) );
        qApp->setEffectEnabled (Qt::UI_FadeMenu);
        qApp->setEffectEnabled (Qt::UI_AnimateMenu);
        qApp->setEffectEnabled (Qt::UI_AnimateCombo);
        qApp->setEffectEnabled (Qt::UI_AnimateTooltip);
        qApp->setEffectEnabled (Qt::UI_FadeTooltip);

        // Aux timer to dock stuff
        QTimer::singleShot(1, tdo, SLOT(dock_timer()) );

        return qApp->exec();
    }
}


/**
 * Sets the initial values of the attributes of the class.
 *
 * @return new object.
 */
DreamsLineParams::DreamsLineParams()
{
    has_lde = false;
    has_dev = false;
    reset();
}


/**
 * Parses the command line parameters of Dreams.
 *
 * @return if the parameters are correct.
 */
bool
DreamsLineParams::parseLineParams(int argc, char * argv[])
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
            // The name of the adf file must be specified.
            if(argc < 2)
            {
                printf("Input error: -adf flag with no file\n");
                return false;
            }

            adf_file.push_back(QString(argv[act_arg + 1]));
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
        // Expression Debug Level
        else if(arg == "-exd")
        {
            // exd must be followed by an integer specifying debug level
            if(argc < 2)
            {
                printf("Input error: -exd flag with no number\n");
                return false;
            }

            bool ok;
            expLevel = atoi(argv[act_arg + 1]);
            rogerLevel = atoi(argv[act_arg + 1]);
            argc--;
            act_arg++;
        }
        // Developer mode
        else if(arg == "-dev")
        {
            if(has_dev)
            {
                printf("Input error: -dev flag set twice\n");
                return false;
            }
            has_dev = true;
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
                drl_file.push_back(QString(argv[act_arg]));
            }
        }

        argc--;
        act_arg++;
    }

    // Some coherence asserts.
    if(has_lde)
    {
        // With -lde flag always a drl file must be specified.
        if(drl_file.size() != 1)
        {
            printf("Input error: -lde requires just one drl file parameter\n");
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
    }

    if(adf_file.size() > drl_file.size())
    {
        printf("Input error: can't specify more adf files than drl files\n");
        return false;
    }

    return true;
}

/*
 * Sets the adf file.
 *
 * @return void.
 */
void
DreamsLineParams::setADFFile(QString file)
{
    adf_file.push_back(file);
}

/*
 * Returns the ADF file name.
 *
 * @return ADF file.
 */
QString
DreamsLineParams::getADFFile() const
{
    if(adf_file.size() > 0)
    {
        return (* adf_file.begin());
    }
    else
    {
        return QString::null;
    }
}

/*
 * Pops the first adf file name.
 *
 * @return pops the adf.
 */
void
DreamsLineParams::popADFFile()
{
    if(adf_file.size() > 0)
    {
        adf_file.pop_front();
    }
}

/*
 * Returns the DRL file name.
 *
 * @return DRL file.
 */
QString
DreamsLineParams::getDRLFile() const
{
    if(drl_file.size() > 0)
    {
        return (* drl_file.begin());
    }
    else
    {
        return QString::null;
    }
}

/*
 * Pops the first drl file name.
 *
 * @return pops the drl.
 */
void
DreamsLineParams::popDRLFile()
{
    if(drl_file.size() > 0)
    {
        drl_file.pop_front();
    }
}

/*
 * Returns if the -adf flag is set.
 *
 * @return if an ADF file is set.
 */
bool
DreamsLineParams::hasADFFile() const
{
    return (adf_file.size() > 0);
}

/**
 * Returns if an input DRL file is set.
 *
 * @return if an DRL file is set.
 */
bool
DreamsLineParams::hasDRLFile() const
{
    return (drl_file.size() > 0);
}

/**
 * Returns if the -lde flag is set.
 *
 * @return if -lde is set.
 */
bool
DreamsLineParams::LDEMode() const
{
    return has_lde;
}

/**
 * Returns if the -no-gui flag is set.
 *
 * @return if -no-gui is set.
 */
bool
DreamsLineParams::noGUIMode() const
{
    return has_no_gui;
}

/**
 * Returns if the -dev flag is set.
 *
 * @return if -dev is set.
 */
bool
DreamsLineParams::DEVMode() const
{
    return has_dev;
}
 
/**
 * Returns if the -exd flag is set.
 *
 * @return if -exd is set.
 */
UINT32
DreamsLineParams::expDebugLevel() const
{
    return expLevel;
}

/** Reset to empty state */
void 
DreamsLineParams::reset(void)
{
    adf_file.clear();
    drl_file.clear();
    has_no_gui = false;
    expLevel = 0;
}

DreamsTraceData::DreamsTraceData(DreamsDB * _dreamsdb, INT32 _id)
{
    dreamsdb = _dreamsdb;
    id = _id;
}

DreamsTraceData::~DreamsTraceData()
{
    delete dreamsdb;
    delete window_menu;
}

DreamsDB *
DreamsTraceData::getDreamsDB() const
{
    return dreamsdb;
}

QPopupMenu *
DreamsTraceData::getWindowMenu() const
{
    return window_menu;
}

INT32
DreamsTraceData::getWindowMenuId() const
{
    return window_menu_id;
}

QPopupMenu *
DreamsTraceData::getWatchWindowMenu() const
{
    return watch_window;
}

INT32
DreamsTraceData::getCloseMenuId() const
{
    return close_menu_id;
}

INT32
DreamsTraceData::getId() const
{
    return id;
}

void
DreamsTraceData::setWindowMenu(QPopupMenu * value)
{
    window_menu = value;
}

void
DreamsTraceData::setWindowMenuId(INT32 value)
{
    window_menu_id = value;
}

void
DreamsTraceData::setWatchWindowMenu(QPopupMenu * value)
{
    watch_window = value;
}

void
DreamsTraceData::setCloseMenuId(INT32 value)
{
    close_menu_id = value;
}

void
DreamsTraceData::decId()
{
    id--;
}
