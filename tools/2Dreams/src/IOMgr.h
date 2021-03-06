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
  * @file IOMgr.h
  */

#ifndef _IOMGR_H
#define _IOMGR_H

// QT Library
#include <qapplication.h>
#include <qfile.h>
#include <qstring.h>
#include <qfiledialog.h>
#include <qprogressdialog.h>
#include <qprogressbar.h>
#include <qdom.h>
#include <qpushbutton.h>

#include "asim/DralDB.h"

// 2Dreams
#include "2DreamsDefs.h"
#include "DreamsDB.h"
#include "ADFParserBuilder.h"
#include "TwoDAvtView.h"
#include "ADFMgr.h"

#define IOMGR_PROCESS_STEP 4096
#define DTD_VALIDATION_TIMEOUT 10000
#define ADF_SUBPROCESS_TIMEOUT 15000

class AProgressDialog : public QProgressDialog
{
    Q_OBJECT
    public:
        AProgressDialog (QWidget * creator = 0, const char * name = 0, bool modal = FALSE, WFlags f = 0) :
          QProgressDialog (creator,name,modal,f)
        {
            this->reparent (creator,WType_TopLevel, pos(),false);
            //this->reparent (creator,WStyle_Customize|WStyle_DialogBorder|WStyle_Minimize,pos(),false);
            abortButton = new QPushButton("&Abort",this,"abbort_button");
            stopButton = new QPushButton("&Stop Reading",this,"stop_button");
            setCancelButton(abortButton);
          	connect( stopButton, SIGNAL(clicked()), this, SIGNAL(stopped()) );
          	connect( abortButton, SIGNAL(clicked()), this, SIGNAL(aborted()) );
      	    layout();
      	    stopButton->setFocus();
        }

        QSize sizeHint() const;
        void resizeEvent( QResizeEvent *e );
        void styleChange(QStyle& s);

    signals:
        void stopped();
        void aborted();

    private:
        void layout();

    private:
        QPushButton* stopButton;
        QPushButton* abortButton;
};

class IOMgr : public QObject
{
    Q_OBJECT

    public:
        static IOMgr* getInstance(QWidget* parent=NULL);
        static void   setAVT(TwoDAvtView* _avt);
        
    public:
        QString getEventFile(QString fn);
        bool    loadDRLHeader(QString fn);
        bool    getADF();
        void    startLoading();
        bool    isLoading();

    public slots:
        void processEvent();
        void cancelRead();
        void stopRead();

    signals:
        void loadFinished(void);
        void loadCancel(void);

    protected:
        IOMgr  (QWidget* parent);
        ~IOMgr ();

        void reset();
        bool versionCheck(QDomElement docElem, QString filename);
        void refreshAvt();

    private:
        DralDB*   draldb;
        DreamsDB* dreamsDB;
        INT32 steps;
        QTimer *t;
        AProgressDialog* progress;
        QProgressBar* qpbar;
        QFileDialog* fdlg;
        QWidget* parent;
        QLabel* plabel;
        QFont lblfnt;
        INT32 lastLblRefresh;
        INT32 lastVPRefresh;

    private:
        static IOMgr* _myInstance;
        static TwoDAvtView *avt;
};


#endif

