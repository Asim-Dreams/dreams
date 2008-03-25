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
  * @file WebMimeSF.cpp
  */

#include <qapplication.h>
#include <qdragobject.h>
#include <qregexp.h>
#include <qeventloop.h>

#include "WebMimeSF.h"
#include "asim/HelpWindow.h"

WebMimeSF::WebMimeSF(QString hostname,QString path) : QMimeSourceFactory()
{
    myHost = hostname;
    myPath = QStringList(path);
    singlePath = path;

    myBlocks = new QValueList<QByteArray>();

    op = new QUrlOperator(myHost);
    connect(op,SIGNAL(newChildren(const QValueList<QUrlInfo>&,QNetworkOperation*)),this,SLOT(newChildren(const QValueList<QUrlInfo>&,QNetworkOperation*)));
    connect(op,SIGNAL(finished(QNetworkOperation*)),this,SLOT(finished(QNetworkOperation*)));
    connect(op,SIGNAL(start(QNetworkOperation*)),this,SLOT(start(QNetworkOperation*)));
    connect(op,SIGNAL(createdDirectory(const QUrlInfo&,QNetworkOperation*)),this,SLOT(createdDirectory(const QUrlInfo&,QNetworkOperation*)));
    connect(op,SIGNAL(removed(QNetworkOperation*)),this,SLOT(removed(QNetworkOperation*)));
    connect(op,SIGNAL(itemChanged(QNetworkOperation*)),this,SLOT(itemChanged(QNetworkOperation*)));
    connect(op,SIGNAL(data(const QByteArray&,QNetworkOperation*)),this,SLOT(data(const QByteArray&,QNetworkOperation*)));
    connect(op,SIGNAL(dataTransferProgress(int,int,QNetworkOperation*)),this,SLOT(dataTransferProgress(int,int,QNetworkOperation*)));
    connect(op,SIGNAL(startedNextCopy(const QPtrList<QNetworkOperation>&)),this,SLOT(startedNextCopy(const QPtrList<QNetworkOperation>&)));
    connect(op,SIGNAL(connectionStateChanged(int,const QString&)),this,SLOT(connectionStateChanged(int,const QString&)));
}

WebMimeSF::~WebMimeSF()
{
}

QString
WebMimeSF::makeAbsolute (const QString& abs_or_rel_name, const QString& context ) const
{
    //printf ("makeAbsolute called with name=%s, context=%s\n",abs_or_rel_name.latin1(),context.latin1());fflush(stdout);
    return QMimeSourceFactory::makeAbsolute(abs_or_rel_name,context);
}


const QMimeSource*
WebMimeSF::data(const QString& abs_name) const
{
#if 0
    static bool inside = false;
    if (inside) { return NULL; }
    inside=true;
    
    QRegExp sep( "\\." );
    QString fileExt = abs_name.section( sep, -1, -1 );
    QString compName = singlePath+abs_name;

    //printf("hostname :%s\n",myHost.latin1());fflush(stdout);
    //printf("rec  name:%s\n",abs_name.latin1());fflush(stdout);
    //printf("comp name:%s\n",compName.latin1());fflush(stdout);

    opFinished = false;
    myBlocks->clear();
    //qApp->syncX();
    //qApp->flushX();
    //qApp->removePostedEvents(cons);
    //HelpWindow* hw = (HelpWindow*)cons;
    //hw->ioInProgress=true;
    operation = (const QNetworkOperation*)(op->get(compName));
    //printf ("WebMimeSF::network operation launched...\n");fflush(stdout);

    QEventLoop* el = QApplication::eventLoop();
    while ( (!opFinished) && (operation->state() != QNetworkProtocol::StFailed))
    {
        el->processEvents(QEventLoop::ExcludeUserInput,1);
    }
    bool opFailed = (operation->state() != QNetworkProtocol::StDone);

   
    if ((!opFailed) && (fusedArrays.size()>0) )
    {
        QMimeSource* result=NULL;
        //printf("I got data!\n");fflush(stdout);
        if (fileExt=="htm" || fileExt=="html" || fileExt=="HTM" || fileExt=="HTML")
        {
            //printf ("I gonna create a QTextDrag....\n");fflush(stdout);
            QString str(fusedArrays.copy());
            result = new QTextDrag(str);
            //printf ("QTextDrag created ...\n");fflush(stdout);
        }
        else if (fileExt=="png" || fileExt=="PNG")
        {
            //mutex.lock();
            //printf ("I gonna create a QImageDrag....\n");fflush(stdout);
            QImage img(fusedArrays.copy());
            //printf ("QImage done....\n");fflush(stdout);
            result = new QImageDrag(img);
            //printf ("QImageDrag created....\n");fflush(stdout);
            //mutex.unlock();
        }
        //printf ("WebMimeSF::data done!\n");fflush(stdout);
        inside=false;
        //hw->ioInProgress=false;
        return (result);
    }
    else
    {
        //printf ("WebMimeSF::data (empty) done!\n");fflush(stdout);
        inside=false;
        //hw->ioInProgress=false;
        return (NULL);
    }
#endif
        return 0;
}

void
WebMimeSF::setFilePath ( const QStringList & path )
{
    myPath = path;
}

QStringList
WebMimeSF::filePath () const
{
    //printf(">>filePath called\n");fflush(stdout);
    return myPath;
}

void
WebMimeSF::joinArrays()
{
    
    // compute needed size
    QValueList<QByteArray>::iterator it;
    int sz=0;
    int num=0;
    for ( it = myBlocks->begin(); it != myBlocks->end(); ++it )
    {
        QByteArray ar = *it;
        sz+= (int)ar.size();
        ++num;
    }
    //printf ("computed file size=%d num blocks=%d\n",sz,num);fflush(stdout);
    
    fusedArrays = QByteArray(sz);
    if (fusedArrays.isNull()) return;

    int cnt=0;
    for ( it = myBlocks->begin(); it != myBlocks->end(); ++it )
    {
        QByteArray ar = *it;
        for (int i=0;i<(int)ar.size();i++)
        {
            fusedArrays[cnt++] =ar[i];
        }
    }
}


void
WebMimeSF::newChildren ( const QValueList<QUrlInfo> & i, QNetworkOperation * op )
{
    //printf("newChildren called\n");fflush(stdout);
}

void
WebMimeSF::finished ( QNetworkOperation * op )
{
    //printf("finished called\n");fflush(stdout);
    joinArrays();
    opFinished = true;
}

void
WebMimeSF::start ( QNetworkOperation * op )
{
    //printf("start called\n");fflush(stdout);
}

void
WebMimeSF::createdDirectory ( const QUrlInfo & i, QNetworkOperation * op )
{
    //printf("createdDirectory called\n");fflush(stdout);
}

void
WebMimeSF::removed ( QNetworkOperation * op )
{
    //printf("removed called\n");fflush(stdout);
}

void
WebMimeSF::itemChanged ( QNetworkOperation * op )
{
    //printf("itemChanged called\n");fflush(stdout);
}

void
WebMimeSF::data ( const QByteArray & data, QNetworkOperation * op )
{
    //printf("data called, array size=%d\n",data.size());fflush(stdout);
    //QString strdata(data);
    //printf("%s\n",strdata.latin1());fflush(stdout);
    //QByteArray newBlock(data.size());
    //newBlock.duplicate(data);
    
    myBlocks->append(data);
}

void
WebMimeSF::dataTransferProgress ( int bytesDone, int bytesTotal, QNetworkOperation * op )
{
    //printf("dataTransferProgress called\n");fflush(stdout);
}

void
WebMimeSF::startedNextCopy ( const QPtrList<QNetworkOperation> & lst )
{
    //printf("startedNextCopy called\n");fflush(stdout);
}

void
WebMimeSF::connectionStateChanged ( int state, const QString & data )
{
    //printf("connectionStateChanged called, state=%d, data=%s\n",state,data.latin1());fflush(stdout);
}


/*
void 
WebMimeSF::setConsumer(QObject* consumer)
{ cons = consumer;}
*/

