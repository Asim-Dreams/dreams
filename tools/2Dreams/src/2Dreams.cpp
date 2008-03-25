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

/**
  * @file 2Dreams.cpp
  */

#include <stdlib.h>

// AGT Library
#include <asim/xpm/help_contents.xpm>

// 2Dreams
#include "2Dreams.h"
//#include "xpm/2dreams.xpm"
#include "xpm/2dreams_logo2k_metal2.xpm"
#include "xpm/2dreams_logo2k_metal5.xpm"
#include "xpm/selectToolBar.xpm"
#include "preferencesImpl.h"

#include "HighLightMgr.h"
#include "DumpMode.h"

// for docked windows start-up
bool dockInit;
bool dump_mode;

// -----------------------------------------------------------------------------
// -- The Main Constructor
// -----------------------------------------------------------------------------
TwoDreams::TwoDreams(DreamsLineParams p, QWidget* parent, const char* name, WFlags f) :
    Main(parent,name,f), params(p)
{
    sfn=QString::null;
    
    numNextWindow=1;
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
    about2d = NULL;
    vb = NULL;
    logo = NULL;
    browser = NULL;
    AcceptButton = NULL;
    dreamsDB = NULL;
    actionFullScreen = NULL;
    draldbabout = NULL;
    fndDialog=NULL;
    dis_win_id=-1;
    event_win_id=-1;
    anc_win_id=-1;
    log_win_id=-1;
    dis_last_id=-1;
    dis_all_id=-1;
    show_criteria_id=-1;
    find_id=-1;
    close_id=-1;
    dis_shade_id=-1;
    reset_rscan_id=-1;

    forcedShutdown = false;
    fullScreenMode = false;
    fstList = new QValueList<int>();
    tgsel=NULL;
    
    // --------------------------------------------------------------------
    // Load 2Dreams preferences
    // --------------------------------------------------------------------
    prefMgr = PreferenceMgr::getInstance("BSSAD");
    prefMgr->setGUIEnabled(true);
    bool ok = prefMgr->loadPreferences();
    if (!ok)
    {
        qWarning("Error detected loading application's preferences!");
    }

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

    connect (qApp,SIGNAL(aboutToQuit()),this,SLOT(aboutToQuit()));
    connect (qApp,SIGNAL(lastWindowClosed()),this,SLOT(lastWindowClosed()));
    connect (this,SIGNAL(dockWindowPositionChanged(QDockWindow*)),this,SLOT(dockWindowPositionChanged(QDockWindow*)));

    grte = new textOverviewImpl(NULL);
    grte->textEdit->setReadOnly(true);

    adfte = new textOverviewImpl(NULL);
    sh = new KXESyntaxHighlighter(adfte->textEdit);
    adfte->textEdit->setReadOnly(true);
    
    // deb
    //printf ("sizeof DisListViewItem = %d\n",(int)sizeof(DisListViewItem)); fflush(stdout);
    //printf ("sizeof eVENTItem = %d\n",(int)sizeof(EventItem)); fflush(stdout);
    //printf ("sizeof ColDescriptor = %d\n",(int)sizeof(ColDescriptor)); fflush(stdout);
    //printf ("sizeof TagItem = %d\n",sizeof(TagItem));

    //Q_ASSERT(false);

}

void
TwoDreams::dock_timer()
{
    //printf("dock timer launched\n");fflush(stdout);
    dockInit=false;
}

void
TwoDreams::initApplication()
{
    // --------------------------------------------------------------------
    // Create about 2dreams objects, logo, etc.
    // --------------------------------------------------------------------
    createAbout2D();
    createAboutDRALDB();

    // --------------------------------------------------------------------
    // Initialize the log file
    // --------------------------------------------------------------------
    LogMgr* log = LogMgr::getInstance(prefMgr->getLogFileName());
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
    draldb = DralDB::getInstance();
    draldb->setGUIEnabled(true);
    dreamsDB = DreamsDB::getInstance();

    // --------------------------------------------------------------------
    // create the ASIM-2Dreams-Scroller
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

    dreamsDB->setBookMarkMgr(bkMgr);

    // I don't want the options menu
    menu->removeItem(options_id);
    // I don't want the grid line generic menu
    if (view!=NULL) view->removeItem(view_grid_id);

    // --------------------------------------------------------------------
    // Create global docked windows
    // --------------------------------------------------------------------
    createLogWindow();

    logWin->resize(prefMgr->getLogWindowWidth(),prefMgr->getLogWindowHeight());

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
    /*
    if (view!=NULL)
    {
        view->setItemChecked(view_grid_id,prefMgr->getShowGridLines()); 
    }
    */
    
    if (asv !=NULL)
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
    // configure the draldb
    // --------------------------------------------------------------------
    draldb->setAutoPurge(prefMgr->getAutoPurge()) ;
    draldb->setIncrementalPurge(prefMgr->getIncrementalPurge()) ;
    draldb->setMaxIFIEnabled(prefMgr->getMaxIFIEnabled()) ;
    draldb->setMaxIFI(prefMgr->getMaxIFI()) ;
    draldb->setTagBackPropagate(prefMgr->getTagBackPropagate()) ;
    draldb->setCompressMutable(prefMgr->getCompressMutableTags()) ;
    draldb->setItemMaxAge(prefMgr->getItemMaxAge()) ;


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
    
    help = new Help2DWindow(HELP_INDEX, mypath, NULL, "2Dreams Help");
    Q_ASSERT(help!=NULL);
    help->setCaption("2Dreams Help Browser");
    DTextBrowser* helpBrowser = help->getTextBrowser();
    helpBrowser->setSource(HELP_INDEX);
    //webMimeSourceFactory = new WebMimeSF(HELP_SERVER,mypath);
    //helpBrowser->setMimeSourceFactory(webMimeSourceFactory);
    //helpBrowser->reload();
    
    // --------------------------------------------------------------------
    // DRL IO Mgr
    // --------------------------------------------------------------------
    iomgr = IOMgr::getInstance(this);
    connect(iomgr,SIGNAL(loadFinished()),this,SLOT(loadFinished()));
    connect(iomgr,SIGNAL(loadCancel()),this,SLOT(loadCancel()));

    // --------------------------------------------------------------------
    // MDI event handling
    // --------------------------------------------------------------------
    connect(ws,SIGNAL(windowActivated(QWidget*)),this,SLOT(mdiWindowActivated(QWidget*)));
    
    // --------------------------------------------------------------------
    // --------------------------------------------------------------------
    setIcon(IconFactory::getInstance()->image37);
    setIconText("2Dreams");
    
    log->addLog("Initialization Finished.");
    set_splash_status( "Initialization Finished." );
}

void
TwoDreams::createAbout2D()
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
    msg = msg + "<h4>2Dreams is a DRAL visualization tool that shows item movements as events on screen.<br>";
    msg = msg + "The Y axis is used to represent different DRAL edges and the X axis for time.</h4>";

    msg = msg + "<h4>2Dreams relies on QT, AGT, DRAL and DRALDB libraries.<br>";
    msg = msg + "This tool was developed at BSSAD, Intel Labs Barcelona.</h4>";

    msg = msg + "<h4>Staff:</h4>";
    msg = msg + "<ul>";
    msg = msg + "<li>Design and implementation: Federico Ardanaz";
    msg = msg + "<li>Design and supervision: Roger Espasa";
    msg = msg + "<li>General supervision: Joel Emer";
    msg = msg + "<li>AGT Library: Federico Ardanaz";
    msg = msg + "<li>DRAL Library: Pau Cabre and Roger Gramunt";
    msg = msg + "<li>DRALDB Library: Federico Ardanaz and Roger Espasa";
    msg = msg + "</ul>";

    msg = msg + "<br>Special thanks to Artur Klauser for his support with XML related topics, to Katia Kaeser<br>";
    msg = msg + "for her help with the 2Dreams logo design and to all the BSSAD team by their beta-testing<br>";
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

    about2d = new QDialog(this,"about2d",true);Q_ASSERT(about2d!=NULL);
    vb =  new QGridLayout( about2d, 3, 1, 15,20, "about2d layout");Q_ASSERT(vb!=NULL);

    //dreamsIcon = QPixmap(dreams2_xpm);
    dreamsIcon = QPixmap(dreams2_metal2);
    logo = new QLabel(about2d);Q_ASSERT(logo!=NULL);
    logo->setPixmap(dreamsIcon);
    vb->addWidget(logo,0,0);

    browser = new QTextBrowser(about2d,"about text browser");Q_ASSERT(browser!=NULL);
    browser->setText(msg);
    vb->addWidget(browser,1,0);

    AcceptButton = new QPushButton( about2d, "AcceptButton" );Q_ASSERT(AcceptButton!=NULL);
    AcceptButton->setText("&Dismiss");
    connect (AcceptButton,SIGNAL(clicked()),this,SLOT(about2d_clicked())) ;
    vb->addWidget(AcceptButton,2,0);
}

void
TwoDreams::createAboutDRALDB()
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
TwoDreams::about2d_clicked()
{
    about2d->close();
}

char*
TwoDreams::getApplicationCaption()
{
  return ("2Dreams Visualization Tool - Version " DREAMS2_STR_MAJOR_VERSION "."
  DREAMS2_STR_MINOR_VERSION "." DREAMS2_STR_BUILD_VERSION " - BSSAD Group at Intel Labs Barcelona");
}

AScrollView*
TwoDreams::getAScrollViewObject()
{
    //return getActiveASV();
    return NULL;
}

// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
void
TwoDreams::addToolsMenu()
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
TwoDreams::addViewMenu()
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
TwoDreams::addFileMenu()
{
    file = new QPopupMenu( menu );Q_ASSERT(file!=NULL);
    if (genericOpenEnabled)
    {
        actionGenericOpen = new Q2DMAction ("genericOpen",IconFactory::getInstance()->image1,"&Open",CTRL+Key_O,this,"genericOpen");
        actionGenericOpen->addTo(file);
        connect(actionGenericOpen, SIGNAL(activated()), this, SLOT(do_genericOpen()));
    }
    if (genericCloseEnabled)
    {
        actionGenericClose = new Q2DMAction ("genericClose",IconFactory::getInstance()->image0,"&Close",CTRL+Key_W,this,"genericClose");
        actionGenericClose->addTo(file);
		close_id = actionGenericClose->getMenuId(); 
        connect(actionGenericClose, SIGNAL(activated()), this, SLOT(do_genericClose()));
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
TwoDreams::addEditMenu()
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
TwoDreams::addHighlightMenu()
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
TwoDreams::addWindowMenu()
{
    Main::addWindowMenu();
    window_docked = new QPopupMenu(window);
    Q_ASSERT(window_docked!=NULL);

    dis_win_id = window_docked->insertItem("&Item Window", this, SLOT(do_switchDisWin()));
    window_docked->setItemChecked(dis_win_id,prefMgr->getShowDisassemblyWindow());

    event_win_id = window_docked->insertItem("&Tag Window", this, SLOT(do_switchEventWin()));
    window_docked->setItemChecked(event_win_id,prefMgr->getShowEventWindow());

    anc_win_id = window_docked->insertItem("&Relationship Window", this, SLOT(do_switchAncWin()));
    window_docked->setItemChecked(anc_win_id,prefMgr->getShowRelationshipWindow());

    log_win_id = window_docked->insertItem("&Log Window", this, SLOT(do_switchLogWin()));
    window_docked->setItemChecked(log_win_id,prefMgr->getShowLogWindow());
    window->insertItem("&Docked windows...",window_docked);
    
    window->insertSeparator();

    actionNewWindow = new Q2DMAction ("New Window",IconFactory::getInstance()->image31,"&New Window",CTRL+Key_A,this,"New Window");
    actionNewWindow->addTo(window);
    nview_win_id = actionNewWindow->getMenuId();
    connect(actionNewWindow, SIGNAL(activated()), this, SLOT(do_newWindow()));
    
    nfview_win_id = window->insertItem("New &Free Window", this, SLOT(do_newFWindow()));
    
    window->insertSeparator();

    tileh_win_id = window->insertItem(IconFactory::getInstance()->image34,"Tile &Horizontal", this, SLOT(do_tileHorizontal()));
    tilev_win_id = window->insertItem(IconFactory::getInstance()->image35,"Tile &Vertical", this, SLOT(do_tileVertical()));
    cascade_win_id = window->insertItem(IconFactory::getInstance()->image38,"&Cascade", this, SLOT(do_cascade()));
}


// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
void
TwoDreams::addHelpMenu()
{
    menu->insertSeparator();
    QPopupMenu* help = new QPopupMenu( menu );Q_ASSERT(help!=NULL);
    help->insertItem("About &QT", this, SLOT(do_about_qt()));
    help->insertItem("About &AGT", this, SLOT(do_about_agt()));
    help->insertItem("About &DralDB", this, SLOT(do_about_draldb()));
    help->insertItem(IconFactory::getInstance()->image25,"About &2Dreams", this, SLOT(do_about_2dreams()));
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
TwoDreams::do_about_2dreams()
{
    about2d->exec();
}

void
TwoDreams::do_about_draldb()
{
    draldbabout->show();
}


void
TwoDreams::do_bug_report()
{
    QMessageBox::about (this, "Bug Report Instructions",
     "Please report bugs to 2dreams@bssad.intel.com\n"
     "including the bug description, the ADF (*.xml) and\n"
     "the event file (*.drl) you have been using.\n"
     "\nThanks in advance:\n2Dreams development team.\n"    );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_help()
{
    /*
    QMessageBox::about (this, "2Dreams Help",
     "Due to a problem with QT 3.0.4 plugin mechanism, the integrated help browser is disabled.\n"
     "As soon as ASIM upgrades to QT 3.0.5 or later the help can be reactivated.\n"
     "By now please open a web browser and go to\n"
     "http://bssad.intel.com/Software/2Dreams/help/index.htm.\n"
     "\n2Dreams development team.\n"    );
     
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
TwoDreams::do_genericOpen()
{
    //printf ("TwoDreams::do_genericOpen() called\n");

    if (iomgr->isLoading())
    {
        qApp->beep();
        return;
    }

    // clear adf manager from previous executions...
    ADFMgr* adfmgr = ADFMgr::getInstance();
    adfmgr->reset();

    bool dadf = adfmgr->setDefaultADF(prefMgr->getArchitectureDefinitionFile());
    bool eadf = true;
    if (params.hasADFFile())
    {
        eadf = adfmgr->setCommandLineADF(params.getADFFile());
    }
    if (!dadf && !eadf) { return; }

    // decides drl file name
    sfn = iomgr->getEventFile(params.getDRLFile());
    if (sfn==QString::null) { return; }

    // stetic preventive lbl
    setCaption((QString(getApplicationCaption())));

    // create the first MDI
    MDIWindow* mdi = newView();
    iomgr->setAVT((TwoDAvtView*)(mdi->getAScrollView()));

    // read the drl header to get the dral-graph and may be the emmbeded adf string
    bool hok = iomgr->loadDRLHeader(sfn);
    if (!hok) { return ; }

    // decides the adf, parse it...
    bool adfok = iomgr->getADF();
    if (!adfok) { return ; }

    // some stetic stuff:
    QString cap = "2Dreams - " + sfn;
    setCaption(cap);
    do_resetAspectRatio();
    do_z100();

    // start the reading procss
    iomgr->startLoading();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_genericPrint()
{
    qWarning("Not implemented");
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_saveSession()
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
    dreamsDB->saveSession(&file);
    file.close();
}

void
TwoDreams::do_loadSession()
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

    dreamsDB->loadSession(&file);
    file.close();

    // refresh it all...
    MDIWindow* cmdi;
    TwoDAvtView* casv;
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv=(TwoDAvtView*)(cmdi->getAScrollView());
        casv->updateContents();
    }    
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::quit()
{
    //printf ("TwoDreams::quit() called\n");fflush(stdout);
    do_quit();
    qApp->quit();
}

void
TwoDreams::do_quit()
{
    //printf ("TwoDreams::do_quit() called\n");fflush(stdout);
    if (fullScreenMode) do_switchFullScreen();

    // ---------------------------------------------------------
    // ---------------------------------------------------------
    // save 2Dremas GUI preferences
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

    // ---------------------------------------------------------
    // and really quit
    // ---------------------------------------------------------
    //exit(1);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_genericSave()
{}

void
TwoDreams::dockWindowPositionChanged(QDockWindow*)
{
    //printf ("global position slot changed\n");fflush(stdout);
    saveDockingState();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::saveDockingState()
{
    //printf ("saveDockingState called\n");fflush(stdout);
    
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
    TwoDAvtView* casv=(TwoDAvtView*)(getActiveASV());
    MDIWindow* mdi=getActiveMDI();
    DisDockWin* disWin = NULL;
    if (casv) disWin = casv->getDisWindow(); 
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

    EventDockWin* eventWin = NULL;
    if (casv) eventWin = casv->getEventWindow(); 
    if (eventWin)
    {
        ok = mdi->getLocation (eventWin,dockArea,dockIdx,dockNl,dockOffset);
        if (ok)
        {
            prefMgr->setEventWindowDockPolicy(dockArea);
            prefMgr->setEventWindowIdx(dockIdx);
            prefMgr->setEventWindowNl(dockNl);
            prefMgr->setEventWindowOffset(dockOffset);
            prefMgr->setEventWindowWidth(eventWin->width());
            prefMgr->setEventWindowHeight(eventWin->height());
        }
    }
    
    AncestorDockWin* ancWin = NULL;
    if (casv) ancWin = casv->getAncestorWindow(); 
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
    if (eventWin) prefMgr->setShowEventWindow(eventWin->isVisible());
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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_clearShadedColumns()
{
    TwoDAvtView* casv = (TwoDAvtView*)getActiveASV(); 
    if (casv) casv->clearShadedColumns(); 
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void TwoDreams::do_switchDisWin()
{
    TwoDAvtView* casv=(TwoDAvtView*)(getActiveASV());
    DisDockWin* disWin = NULL;
    if (casv) disWin = casv->getDisWindow(); 
    if (disWin)
    {
        bool s = !window->isItemChecked(dis_win_id);
        window->setItemChecked(dis_win_id,s);
    
        if (s)
        {
            disWin->show();
        }
        else
        {
            disWin->hide();
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void 
TwoDreams::do_switchAncWin()
{
    TwoDAvtView* casv=(TwoDAvtView*)(getActiveASV());
    AncestorDockWin* ancWin = NULL;
    if (casv) ancWin = casv->getAncestorWindow(); 
    if (ancWin)
    {
        bool s = !window->isItemChecked(anc_win_id);
        window->setItemChecked(anc_win_id,s);
    
        if (s)
            ancWin->show();
        else
            ancWin->hide();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void 
TwoDreams::do_switchEventWin()
{
    TwoDAvtView* casv=(TwoDAvtView*)(getActiveASV());
    EventDockWin* eventWin = NULL;
    if (casv) eventWin = casv->getEventWindow(); 
    if (eventWin)
    {
        bool s = !window->isItemChecked(event_win_id);
        window->setItemChecked(event_win_id,s);
        if (s)
            eventWin->show();
        else
            eventWin->hide();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void 
TwoDreams::do_switchLogWin()
{
    bool s = !window->isItemChecked(log_win_id);
    window->setItemChecked(log_win_id,s);

    if (s)
        logWin->show();
    else
        logWin->hide();

}

void 
TwoDreams::do_newWindow()
{
    if ( ws->windowList().isEmpty() )
    {
        qApp->beep();
        return;
    }
    else
    {
        newView();
    }
}

void 
TwoDreams::do_newFWindow()
{
    if ( ws->windowList().isEmpty() )
    {
        qApp->beep();
        return;
    }
    else
    {
        MDIWindow* newW = newView();
        TwoDAvtView* navt = (TwoDAvtView*)newW->getAScrollView();
        navt->do_switchRebel();
    }
}

void 
TwoDreams::disWinVisibilityChanged(bool visible)
{
    TwoDAvtView* casv=(TwoDAvtView*)(getActiveASV());
    if (casv) casv->setEnabledDisWindow(visible); 

    window->setItemChecked(dis_win_id,visible);
    if (!dockInit)
    {
        saveDockingState();
    }
    
    //printf ("dis visibility changed\n");fflush(stdout);
}

void 
TwoDreams::disWinPlaceChanged(QDockWindow::Place)
{
    //printf ("dis place changed\n");fflush(stdout);
    saveDockingState();
}

void 
TwoDreams::disWinOrientationChanged(Orientation)
{
    //printf ("dis orientation changed\n");fflush(stdout);
    saveDockingState();
}

void 
TwoDreams::ancWinVisibilityChanged(bool visible)
{
    TwoDAvtView* casv=(TwoDAvtView*)(getActiveASV());
    window->setItemChecked(anc_win_id,visible);
    if (casv) casv->setEnabledAncestorWindow(visible);
    if (!dockInit)
    {
        saveDockingState();
    }
}

void 
TwoDreams::ancWinPlaceChanged(QDockWindow::Place)
{
    saveDockingState();
}

void 
TwoDreams::ancWinOrientationChanged(Orientation)
{
    saveDockingState();
}

void 
TwoDreams::eventWinVisibilityChanged(bool visible)
{
    TwoDAvtView* casv=(TwoDAvtView*)(getActiveASV());
    window->setItemChecked(event_win_id,visible);
    if (casv) casv->setEnabledEventWindow(visible);
    if (!dockInit)
    {
        saveDockingState();
    } 
}

void 
TwoDreams::eventWinPlaceChanged(QDockWindow::Place)
{
    saveDockingState();
}

void 
TwoDreams::eventWinOrientationChanged(Orientation)
{
    saveDockingState();
}

void 
TwoDreams::logWinVisibilityChanged(bool visible)
{
    window->setItemChecked(log_win_id,visible);
    logWin->doVisibilityChanged(visible);
    if (!dockInit)
    {
        saveDockingState();
    }    
}

void 
TwoDreams::logWinPlaceChanged(QDockWindow::Place)
{
    saveDockingState();
}

void 
TwoDreams::logWinOrientationChanged(Orientation)
{
    saveDockingState();
}


void 
TwoDreams::do_disableLastHighlight()
{
    TwoDAvtView* casv = (TwoDAvtView*)getActiveASV();
    if (casv) casv->disableLastHightlight();
}

void 
TwoDreams::do_disableAllHighlight()
{
    TwoDAvtView* casv = (TwoDAvtView*)getActiveASV();
    if (casv) casv->disableAllHighlight();
}

void
TwoDreams::do_showHighlightCriteria()
{
    TwoDAvtView* casv = (TwoDAvtView*)getActiveASV();
    if (casv) casv->showHighlightCriteria();
}

void
TwoDreams::do_highlightViewChanged(QAction* act)
{
    MDI2DWindow* mdi = (MDI2DWindow*)getActiveMDI();
    TwoDAvtView* casv = (TwoDAvtView*)getActiveASV();
    if (casv)
    {
        if (act==actionVHIGH)
        {
            casv->viewHEvents();
            actionVHIGH->setOn(true);
            mdi->setVHIGH(true);
        }
        else if (act==actionVNHIGH)
        {
            casv->viewNHEvents();
            actionVNHIGH->setOn(true);
            mdi->setVNHIGH(true);
        }
        else if (act==actionVBHIGH)
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
TwoDreams::~TwoDreams()
{
    /** @todo check this destructor */
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::mdiCloseEvent(MDIWindow* mdi)
{
    if (iomgr->isLoading())
    {
        qApp->beep();
        return;
    }

    bool rok = mdiList->remove(mdi);
    Q_ASSERT(rok);
    
    QWidgetList windows = ws->windowList();
    if (windows.count()==1)
    {
        reset();
    }
    else if (windows.count()==2)
    {
        mdiTools->hide();
    }
    
}


void 
TwoDreams::do_genericClose()
{
    if (iomgr->isLoading())
    {
        qApp->beep();
        return;
    }
    
    QWidgetList windows = ws->windowList();
    if (windows.count()==1)
    {
        do_closeAll();
        return;
    }
    else if (windows.count()==2)
    {
        mdiTools->hide();
    }
    
    MDIWindow* cdmi = getActiveMDI();
    if (cdmi) cdmi->close(); 
}

void 
TwoDreams::do_closeAll()
{
    if (iomgr->isLoading())
    {
        qApp->beep();
        return;
    }

    // close all mdi 
    QWidgetList windows = ws->windowList();
    if ( windows.count() ) 
    {
        for ( int i = 0; i < int(windows.count()); ++i ) 
        {
            QWidget *window = windows.at( i );
            window->close(); 
        }
    }
    reset();    
}

void
TwoDreams::reset()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    LogMgr* log = LogMgr::getInstance();
    dreamsDB->reset();
    setCaption(getApplicationCaption());
    numNextWindow=1;
    QString logmsg = "-- File " + sfn + " closed.";
    log->addLog(logmsg);

    enableMenuPurgeLast(false);
    enableMenuShowCriteria(false);
    enableMenuPurgeAll(false);
    enableCloseMenu(false);
    enableMenuOptions(false);
    
    QApplication::restoreOverrideCursor();
}

void 
TwoDreams::loadFinished(void)
{
    MDIWindow* cmdi;
    TwoDAvtView* casv;
    DisDockWin* ddw;
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv=(TwoDAvtView*)(cmdi->getAScrollView());
        //casv->setEnabledDisWindow(visible);
        ddw=casv->getDisWindow();
        ddw->createTabs();
    }
    // If is running in Load, Dump and exit mode.
    if(params.LDEMode())
    {
        // Dumps the contents.
        draldb->dumpRegression();
        ZDBase::getInstance()->dumpRegression();
        // Exits succesfully.
        quit();
    }
}

void 
TwoDreams::loadCancel(void)
{
    do_closeAll();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_preferences()
{
    preferencesImpl* pref = new preferencesImpl(this,"pref", true);
    int result = pref->exec();
    if (!result) return;

    QApplication::setOverrideCursor( Qt::WaitCursor );

    // --------------------------------------------------------------------
    // (re)configure the draldb
    // --------------------------------------------------------------------
    draldb->setAutoPurge(prefMgr->getAutoPurge()) ;
    draldb->setIncrementalPurge(prefMgr->getIncrementalPurge()) ;
    draldb->setMaxIFIEnabled(prefMgr->getMaxIFIEnabled()) ;
    draldb->setMaxIFI(prefMgr->getMaxIFI()) ;
    draldb->setTagBackPropagate(prefMgr->getTagBackPropagate()) ;
    draldb->setItemMaxAge(prefMgr->getItemMaxAge()) ;
    draldb->setCompressMutable(prefMgr->getCompressMutableTags()) ;
    
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

    MDIWindow* cmdi;
    TwoDAvtView* casv;
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv=(TwoDAvtView*)(cmdi->getAScrollView());

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
        casv->setAncestorTrackingTag(prefMgr->getRelationshipTag());
        casv->refreshFavoriteTagList();
    
        // refresh it all...
        casv->repaintContents(false);
    }
    QApplication::restoreOverrideCursor();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_find()
{
    TwoDAvtView* casv = (TwoDAvtView*) getActiveASV();
    fndDialog = new findImpl(casv,this,"findDialog",true);
    fndDialog->aboutToFind();
    fndDialog->exec();
    delete fndDialog;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_find_next()
{
    findImpl::find_next();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_go_to_cycle()
{
    bool ok;
    int result = QInputDialog::getInteger( "Go To Cycle", "Enter Cycle Number:",0,0,
        2000000000,1,&ok,this, "gotocycle");
    if (!ok)
    {
        qApp->beep();
        return;
    }
    ZDBase* zdb = ZDBase::getInstance();
    INT32 column = result - zdb->getFirstEffectiveCycle();
    if ((column >= zdb->getNumCycles()) || (column<0) )
    {
        qApp->beep();
        return;
    }
    TwoDAvtView* casv = (TwoDAvtView*)getActiveASV();
    casv->goToColumn(column);
    casv->shadeColumn(column,true);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_selectEvents()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_memrep()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    QString report = dreamsDB->getUsageDescription();
    LogMgr* log = LogMgr::getInstance();
    log->addLog(report);
    log->flush();
    logWin->refresh();
    if (! (window->isItemChecked(log_win_id)))
    {
        do_switchLogWin();
    }
    QApplication::restoreOverrideCursor();
}

void
TwoDreams::do_viewadf()
{
	QDomDocument* ddoc = ADFMgr::getInstance()->getDomDocument();
	if (ddoc==NULL) { return; }
    adfte->textEdit->setText(ddoc->toString(4));
    adfte->exec();
}

void
TwoDreams::do_viewdralgraph()
{
    grte->textEdit->setText(draldb->getGraphDescription());
    grte->exec();
}

void
TwoDreams::do_viewtagdesc()
{
    QStrList strlist (false);
    strlist = draldb->getKnownTags();

    ShowTagDescDialog *dlg = new ShowTagDescDialog(this,"ctd",true);
    char *str;
    for (str=strlist.first();str!=NULL;str=strlist.next())
    {
        dlg->insertItem(QString(str),draldb->getTagLongDesc(str));
    }
    dlg->exec();
    delete dlg;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::ensureRScanDWinVisible()
{
    if (! (window->isItemChecked(anc_win_id)))
    {
        do_switchAncWin();
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_dbstats()
{
    QApplication::setOverrideCursor( Qt::WaitCursor );
    QString report = dreamsDB->getStats();
    LogMgr* log = LogMgr::getInstance();
    log->addLog(report);
    log->flush();
    logWin->refresh();
    if (! (window->isItemChecked(log_win_id)))
    {
        do_switchLogWin();
    }
    QApplication::restoreOverrideCursor();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_resetRelationshipScan()
{
    TwoDAvtView* cavt = (TwoDAvtView*)getActiveASV();
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
TwoDreams::do_switchFullScreen()
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

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::createLogWindow()
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
TwoDreams::enableMenuPurgeLast(bool value)
{ if (highlight!=NULL) highlight->setItemEnabled(dis_last_id,value); }

void
TwoDreams::enableMenuShowCriteria(bool value)
{ if (highlight!=NULL) highlight->setItemEnabled(show_criteria_id,value);}

void
TwoDreams::enableMenuPurgeAll(bool value)
{ if (highlight!=NULL) highlight->setItemEnabled(dis_all_id,value); }

void
TwoDreams::enableCloseMenu(bool value)
{
    if (file!=NULL) file->setItemEnabled(close_id,value); 
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::enableMenuOptions(bool openedFile)
{
    if (file!=NULL)
    {
        file->setItemEnabled(close_id,openedFile);
        file->setItemEnabled(closeAll_id,openedFile);
        file->setItemEnabled(save_session_id,openedFile);
        file->setItemEnabled(load_session_id,openedFile);
    }

    if (edit!=NULL)
    {
        edit->setItemEnabled(goto_id,openedFile);
        edit->setItemEnabled(find_id,openedFile);
        edit->setItemEnabled(find_next_id,openedFile);
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

    if (tools!=NULL)
    {
        bool ascan=false;
        TwoDAvtView* cavt = (TwoDAvtView*)getActiveASV();
        if (cavt)
        {
            AncestorDockWin* adw = cavt->getAncestorWindow();
            ascan = adw->hasActiveScan();
        }
        tools->setItemEnabled(reset_rscan_id, (openedFile && ascan));
        tools->setItemEnabled(dis_shade_id,openedFile);
        tools->setItemEnabled(toolshighlight_id,openedFile);
        tools->setItemEnabled(pointers_id,openedFile);
        tools->setItemEnabled(dbstats_id,openedFile);
        tools->setItemEnabled(mem_rep_id,openedFile);
        tools->setItemEnabled(adf_id,openedFile);
		tools->setItemEnabled(dral_id,openedFile);
		tools->setItemEnabled(tgdesc_id,openedFile);
    }

    if (bookmark!=NULL)
    {
        bookmark->setItemEnabled(addbookmrk_id,openedFile);
        bookmark->setItemEnabled(mbookmrk_id,openedFile);
        bookmark->setItemEnabled(sbookmrk_id,openedFile);
        bookmark->setItemEnabled(ibookmrk_id,openedFile);
    }

    if (window!=NULL)
    {
        window->setItemEnabled(dis_win_id,openedFile);
        window->setItemEnabled(event_win_id,openedFile);
        window->setItemEnabled(anc_win_id,openedFile);
        //window->setItemEnabled(log_win_id,openedFile);
        window->setItemEnabled(nview_win_id,openedFile);
        window->setItemEnabled(tileh_win_id,openedFile);
        window->setItemEnabled(tilev_win_id,openedFile);
        window->setItemEnabled(cascade_win_id,openedFile);
        window->setItemEnabled(nfview_win_id,openedFile);
    }
    
    if(viewTools!=NULL)
    {
        viewTools->setEnabled(openedFile);
    }
    if(pointerTools!=NULL)
    {
        pointerTools->setEnabled(openedFile);
    }

    if(fileTools!=NULL)
    {
        saveSBtn->setEnabled(openedFile);
        closeMDIBtn->setEnabled(openedFile);
    }
    
    if(colorTools!=NULL)
    {
    }
    if(annotationsTools!=NULL)
    {
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::aboutToQuit()
{
    //printf("about to quit called!\n");fflush(stdout);
    //do_quit();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::lastWindowClosed()
{
    //printf("lastWindowClosed called!\n");fflush(stdout);
    forcedShutdown=true;
    do_quit();
}


// -----------------------------------------------------------------------------
// -- Some aux code to splash window
// -----------------------------------------------------------------------------

QLabel* TwoDreams::splash=NULL;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::set_splash_status( const QString &txt )
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
TwoDreams::showSplash()
{
    QRect screen = QApplication::desktop()->screenGeometry();

	splash = new QLabel( 0, "splash", WDestructiveClose | WStyle_Customize | WStyle_NoBorder | WX11BypassWM | WStyle_StaysOnTop );
	splash->setFrameStyle( QFrame::WinPanel | QFrame::Raised );
	//splash->setPixmap(QPixmap(dreams2_xpm));
	splash->setPixmap(QPixmap(dreams2_metal5));
	splash->adjustSize();
	splash->setCaption( "2Dreams" );
	splash->move( screen.center() - QPoint( splash->width() / 2, splash->height() / 2 ) );
	splash->show();
	splash->repaint( FALSE );
	QApplication::flush();
	set_splash_status( "Starting application..." );

    return splash;
}

void
TwoDreams::addFileToolBar()
{
    Main::addFileToolBar();
    
    saveSBtn=new QToolButton(IconFactory::getInstance()->image4,"Save Session",QString::null,this,SLOT(do_saveSession()),fileTools,"Save Session");
    closeMDIBtn=new QToolButton(IconFactory::getInstance()->image0,"Close",QString::null,this,SLOT(do_genericClose()),fileTools,"Close");
    //QToolButton* closeAllBtn=new QToolButton(IconFactory::getInstance()->image30,"Close All",QString::null,this,SLOT(do_closeAll()),fileTools,"Close All");
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::closeSplash()
{
    splash->hide();
}

void 
TwoDreams::do_makeStyle(const QString &style)
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

MDIWindow* TwoDreams::newView()
{
    TwoDAvtView* pasv = (TwoDAvtView*)(getActiveASV());
    MDIWindow*   pmdi = getActiveMDI();

    MDI2DWindow* w = new MDI2DWindow( this, ws, 0, WDestructiveClose );
    w->init();
    mdiList->append(w);
    w->resize(800,600); // default non-maximized size
    int myMDINumber = numNextWindow++;
    
    // attach the diswin reference
    TwoDAvtView* cv = (TwoDAvtView*)(w->getAScrollView());
    QString capBase = sfn;
    QString cap = capBase + " ("+QString::number(myMDINumber)+")";
    cv->setMDINumber(myMDINumber);
    
    // show the very first window in maximized mode
    if ( ws->windowList().isEmpty() )
    {
        w->setCaption(capBase);
        w->showMaximized();
    }
    else
    {
        mdiTools->show();
        INT32 ncycles = dreamsDB->getNumCycles();
        INT32 nrows   = dreamsDB->getNumRows();
        cv->setRealContentsSize (ncycles,nrows);
        if (pasv!=NULL) 
        {
            cv->copyColumnShadows(pasv);
            cv->setPointerType(pasv->getPointerType());
            cv->setHighLightMgr(new HighLightMgr(pasv->getHighLightMgr()));
            cv->cloneAnnotationCtrl(pasv->getAnnotationCtrl());
        }
        if (pmdi!=NULL) pmdi->showNormal();
        DisDockWin* ddw=cv->getDisWindow();
        ddw->createTabs();
        w->setCaption(cap);
        w->show();
    }
    
    // by default the MDI will obei
    w->setIcon(IconFactory::getInstance()->image32);
    w->setIconText("Locked");

    myStatusBar->message("New Window Created",2000);
    connect(w,SIGNAL(mdiCloseEvent(MDIWindow*)),this,SLOT(mdiCloseEvent(MDIWindow*)));
    connect(cv,SIGNAL(status_msg_changed(QString)),this,SLOT(status_msg_changed(QString)));
    connect(cv,SIGNAL(cycle_msg_changed(QString)),this,SLOT(cycle_msg_changed(QString)));
    connect(cv,SIGNAL(row_msg_changed(QString)),this,SLOT(row_msg_changed(QString)));
    connect(cv,SIGNAL(progress_cnt_changed(int)),this,SLOT(progress_cnt_changed(int)));
    connect(cv,SIGNAL(progress_reinit(int)),this,SLOT(progress_reinit(int)));
    connect(cv,SIGNAL(progress_show()),this,SLOT(progress_show()));
    connect(cv,SIGNAL(progress_hide()),this,SLOT(progress_hide()));
    connect(cv,SIGNAL(zoomStatusChanged(int,int)),this,SLOT(zoomStatusChanged(int,int)));
    connect(cv,SIGNAL(asvMoved(double,double)),this,SLOT(asvMoved(double,double)));
    connect(cv,SIGNAL(asvZoomed(double,double)),this,SLOT(asvZoomed(double,double)));
    connect(cv,SIGNAL(newZoomWindow(int,int,int,int,double,double,int,int)),this,SLOT(newZoomWindow(int,int,int,int,double,double,int,int)));
    connect(cv,SIGNAL(toggledShadowCol(int,bool)),this,SLOT(toggledShadowCol(int,bool)));
    connect(cv,SIGNAL(clearAllShadedCols()),this,SLOT(clearAllShadedCols()));
    connect(cv,SIGNAL(contentsSelectedEvent(int,int,bool,QString,HighLightType,bool)),this,SLOT(contentsSelectedEvent(int,int,bool,QString,HighLightType,bool))); 
    connect(cv,SIGNAL(purgeLastHighlight()),this,SLOT(purgeLastHighlight()));
    connect(cv,SIGNAL(purgeAllHighlight()),this,SLOT(purgeAllHighlight()));
    connect(cv,SIGNAL(currentAnnItemChanged(AnnotationItem*)),this,SLOT(currentAnnItemChanged(AnnotationItem*)));    
    connect(cv->getAnnotationCtrl(),SIGNAL(annotationAdded(AnnotationItem*)),this,SLOT(annotationAdded(AnnotationItem*)));    
    connect(cv->getAnnotationCtrl(),SIGNAL(annotationRemoved(AnnotationItem*)),this,SLOT(annotationRemoved(AnnotationItem*)));    
    connect(cv->getAnnotationCtrl(),SIGNAL(annotationChanged(AnnotationItem*,AnnotationItem*)),this,SLOT(annotationChanged(AnnotationItem*,AnnotationItem*)));    
    return w;
}

void 
TwoDreams::mdiWindowActivated(QWidget* w)
{
    if (w)
    {
        MDI2DWindow* m = (MDI2DWindow*)w;
        TwoDAvtView* c = (TwoDAvtView*)m->getAScrollView(); 
        HighLightMgr* hl = c->getHighLightMgr();
        
        // update menus accordingly with new MDI state
        // highlighting related
        highlightChanged(hl->getNumActiveCriteria());
        
        // file and rel.scan menus
        enableMenuOptions(true);
        
        // view highlight,nhigh,etc
        actionVHIGH->setOn(m->getVHIGH());
        actionVNHIGH->setOn(m->getVNHIGH());
        actionVBHIGH->setOn(m->getVBHIGH());
        
        // update bkMgr state
        bkMgr->setAScrollView(c);
        
        // update enabled/disabled docked windows
        window->setItemChecked(dis_win_id,c->getEnabledDisWindow());
        window->setItemChecked(anc_win_id,c->getEnabledAncestorWindow());
        window->setItemChecked(event_win_id,c->getEnabledEventWindow());

        // update tagselection dialog
        tgsel->setCurrentText(c->getCurrentTagSelector());
        
        // update zoom view dialog
        zoomStatusChanged(c->getZX(),c->getZY());
        
        // update dreamsDB active objects
        dreamsDB->setColumnShadows(c->getColumnShadows());
        
    }
}

void
TwoDreams::repaintAllMDIContents(bool erase)
{
    MDIWindow* cmdi;
    TwoDAvtView* casv;
    ws->setUpdatesEnabled( TRUE );
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv=(TwoDAvtView*)(cmdi->getAScrollView());
        casv->repaintContents(erase);
    }
}

void 
TwoDreams::highlightChanged(int count)
{
   enableMenuPurgeAll(count>1);
   enableMenuPurgeLast(count>0);
   enableMenuShowCriteria(count>0);
}

void 
TwoDreams::closeEvent(QCloseEvent *e)
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
TwoDreams::do_tileHorizontal()
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
TwoDreams::do_tileVertical()
{
    ws->tile();
}

void 
TwoDreams::do_cascade()
{
    ws->cascade();
}

QString
TwoDreams::getWelcomeMessage()
{
    return QString("Welcome to 2Dreams, for help press F1");
}

void
TwoDreams::addTagSelector(QToolBar* parent)
{
    parent->addSeparator();
    QHBox* qhb = new QHBox(parent);Q_ASSERT(qhb!=NULL);
    qhb->setSpacing(10);

    QPixmap mwToolIcon = QPixmap( selectToolBar );
    QLabel* icnLbl = new QLabel(qhb);
    Q_ASSERT(icnLbl!=NULL);
    icnLbl->setPixmap(mwToolIcon);

    tgsel = new TagSelector(qhb,FavoriteTags::getInstance());
    Q_ASSERT(tgsel!=NULL);
    tgsel->setEditable(true);
    tgsel->setAutoCompletion(true);
    QToolTip::add(tgsel,"Highlight Criteria Selector");

    connect(tgsel,SIGNAL(activated(const QString &)),this,SLOT(slotTgSelComboActivated(const QString &)));
    connect(tgsel,SIGNAL(textChanged(const QString &)),this,SLOT(slotTextChanged(const QString &)));
}

void 
TwoDreams::slotTextChanged ( const QString & string )
{
    //printf ("slotTextChanged called\n");
}


void 
TwoDreams::slotTgSelComboActivated( const QString &s )
{
    //printf ("new hi-light tag %s\n",s.latin1());
    TwoDAvtView* cavt = (TwoDAvtView*)getActiveASV();
    if (cavt)
    {
        cavt->slotTgSelComboActivated(s);
    }
}

void 
TwoDreams::asvMoved(double x,double y)
{
    if (!panLocked) return;

    MDIWindow* cmdi;
    TwoDAvtView* casv;
    TwoDAvtView* activeAsv = (TwoDAvtView*)getActiveASV();
    if (!activeAsv) return;
    
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv = (TwoDAvtView*)(cmdi->getAScrollView());
        if (casv!=activeAsv)
        {
            if (casv->getMDILocked())
                casv->scrollByMatrix(x,y);
        }
    }
}

void 
TwoDreams::toggledShadowCol(int col, bool enabled)
{
    if (!shadingLocked) return;

    MDIWindow* cmdi;
    TwoDAvtView* casv;
    TwoDAvtView* activeAsv = (TwoDAvtView*)getActiveASV();
    if (!activeAsv) return;
    
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv = (TwoDAvtView*)(cmdi->getAScrollView());
        if (casv!=activeAsv)
        {
            if (casv->getMDILocked()) casv->shadeColumn(col,enabled);
        }
    }
}

void 
TwoDreams::clearAllShadedCols()
{
    if (!shadingLocked) return;

    MDIWindow* cmdi;
    TwoDAvtView* casv;
    TwoDAvtView* activeAsv = (TwoDAvtView*)getActiveASV();
    if (!activeAsv) return;
    
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv = (TwoDAvtView*)(cmdi->getAScrollView());
        if (casv!=activeAsv)
        {
            if (casv->getMDILocked()) casv->clearShadedColumnsNoPropagate();
        }
    }
}

void 
TwoDreams::asvZoomed(double x, double y)
{
    Q_ASSERT(x!=0.0);
    Q_ASSERT(y!=0.0);
    
    if (!zoomLocked) return;

    double csfx,csfy;
    
    MDIWindow* cmdi;
    TwoDAvtView* casv;
    TwoDAvtView* activeAsv = (TwoDAvtView*)getActiveASV();
    if (!activeAsv) return;
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv = (TwoDAvtView*)(cmdi->getAScrollView());
        if (casv!=activeAsv)
        {
            if (casv->getMDILocked())
            {
                csfx=casv->getScallingFactorX();
                csfy=casv->getScallingFactorY();
                casv->setScallingFactorX(csfx*x);
                casv->setScallingFactorY(csfy*y);
            }
        }
    }
}

void 
TwoDreams::contentsSelectedEvent(int col,int row, bool adding,QString currentTagSelector, HighLightType htype,bool noErase)
{
    if (!highlightLocked) return;

    MDIWindow* cmdi;
    TwoDAvtView* casv;
    TwoDAvtView* activeAsv = (TwoDAvtView*)getActiveASV();
    if (!activeAsv) return;
    
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv = (TwoDAvtView*)(cmdi->getAScrollView());
        if (casv!=activeAsv)
        {
            if (casv->getMDILocked())
            {
                bool rep = casv->getHighLightMgr()->contentsSelectEvent(col,row,adding,currentTagSelector,htype,noErase);
                if (rep) casv->repaintContents(false);
            }
        }
    }
}

void 
TwoDreams::purgeLastHighlight()
{
    if (!highlightLocked) return;

    MDIWindow* cmdi;
    TwoDAvtView* casv;
    TwoDAvtView* activeAsv = (TwoDAvtView*)getActiveASV();
    if (!activeAsv) return;
    
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv = (TwoDAvtView*)(cmdi->getAScrollView());
        if (casv!=activeAsv)
        {
            if (casv->getMDILocked())
            {
                bool rep = casv->getHighLightMgr()->purgeLast();
                if (rep) casv->repaintContents(false);
            }
        }
    }
}

void 
TwoDreams::purgeAllHighlight()
{
    if (!highlightLocked) return;

    MDIWindow* cmdi;
    TwoDAvtView* casv;
    TwoDAvtView* activeAsv = (TwoDAvtView*)getActiveASV();
    if (!activeAsv) return;
    
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv = (TwoDAvtView*)(cmdi->getAScrollView());
        if (casv!=activeAsv)
        {
            if (casv->getMDILocked())
            {
                bool rep = casv->getHighLightMgr()->purgeAll();
                if (rep) casv->repaintContents(false);
            }
        }
    }
}

void 
TwoDreams::newZoomWindow(int pwx,int pwy,int rwx,int rwy,double fx,double fy,int cx,int cy)
{
    MDI2DWindow* nmdi = (MDI2DWindow*)newView();
    TwoDAvtView* cv = (TwoDAvtView*)(nmdi->getAScrollView());
    
    // make it tmp rebel
    cv->setMDILocked(false);
    double wd = (double)(rwx-pwx);
    double hd = (double)(rwy-pwy);
    wd = fabs(wd);
    hd = fabs(hd);
    wd*=fx;
    hd*=fy;
    cv->resize((int)ceil(wd),(int)ceil(hd));
    cv->setMinimumSize((int)ceil(wd),(int)ceil(hd));
    nmdi->adjustSize();
    nmdi->update();

    cv->mouseWorldReleaseEvent_ZoomingNWinCallback (fx,fy,cx,cy);
    // lock it back (default locked)
    cv->setMDILocked(true);
    cv->setMinimumSize(0,0);
    
}

void 
TwoDreams::currentAnnItemChanged(AnnotationItem* ann)
{
    edit->setItemEnabled(copy_id,(ann!=NULL));
    //edit->setItemEnabled(cut_id,(ann!=NULL));
    edit->setItemEnabled(remove_id,(ann!=NULL));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_copy()
{
    edit->setItemEnabled(paste_id,true);
    TwoDAvtView* cavt = (TwoDAvtView*)getActiveASV();
    if (cavt) cavt->annotationCopy();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_paste()
{
    TwoDAvtView* cavt = (TwoDAvtView*)getActiveASV();
    if (cavt) cavt->annotationPaste();
    //edit->setItemEnabled(paste_id,false);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_cut()
{
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void
TwoDreams::do_remove()
{
    TwoDAvtView* cavt = (TwoDAvtView*)getActiveASV();
    if (cavt) cavt->annotationRemove();
    currentAnnItemChanged(NULL);
}

void 
TwoDreams::annotationAdded(AnnotationItem* item)
{
    // propagate to other mdis if apporpiated
    if (!annotationLocked) return;

    MDIWindow* cmdi;
    TwoDAvtView* casv;
    TwoDAvtView* activeAsv = (TwoDAvtView*)getActiveASV();
    if (!activeAsv) return;
    
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv = (TwoDAvtView*)(cmdi->getAScrollView());
        if (casv!=activeAsv)
        {
            if (casv->getMDILocked())
            {
                AnnotationCtrl* actrl = casv->getAnnotationCtrl(); 
                actrl->addPropagatedItem(item);
            }
        }
    }
}

void 
TwoDreams::annotationRemoved(AnnotationItem* item)
{
    // propagate to other mdis if apporpiated
    if (!annotationLocked) return;

    MDIWindow* cmdi;
    TwoDAvtView* casv;
    TwoDAvtView* activeAsv = (TwoDAvtView*)getActiveASV();
    if (!activeAsv) return;
    
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv = (TwoDAvtView*)(cmdi->getAScrollView());
        if (casv!=activeAsv)
        {
            if (casv->getMDILocked())
            {
                AnnotationCtrl* actrl = casv->getAnnotationCtrl(); 
                actrl->removePropagatedItem(item);
            }
        }
    }
}

void 
TwoDreams::annotationChanged(AnnotationItem* newitem, AnnotationItem* olditem)
{
    // propagate to other mdis if apporpiated
    if (!annotationLocked) return;

    MDIWindow* cmdi;
    TwoDAvtView* casv;
    TwoDAvtView* activeAsv = (TwoDAvtView*)getActiveASV();
    if (!activeAsv) return;
    
    for ( cmdi = mdiList->first(); cmdi; cmdi = mdiList->next() )
    {
        casv = (TwoDAvtView*)(cmdi->getAScrollView());
        if (casv!=activeAsv)
        {
            if (casv->getMDILocked())
            {
                AnnotationCtrl* actrl = casv->getAnnotationCtrl(); 
                actrl->changePropagatedItem(newitem,olditem);
            }
        }
    }
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
        dockInit=true;

        // create the base qApp object:
        QApplication app(argc,argv);

        // create the main object
        TwoDreams* tdo = new TwoDreams(params, NULL);
        Q_ASSERT(tdo!=NULL);

        // put default font size
        QFont defFont = qApp->font();
        defFont.setPointSize((PreferenceMgr::getInstance())->getFontSize());
        qApp->setFont(defFont,true);

        QLabel* splash = TwoDreams::showSplash();
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
