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
  * @file WebMimeSF.h
  */

#ifndef _WEBMIMESF_H
#define _WEBMIMESF_H

// Qt Library
#include <qstring.h>
#include <qstringlist.h>
#include <qmime.h>
#include <qurloperator.h>
#include <qhttp.h>
#include <qvaluelist.h>
#include <qcstring.h>
//#include <qmutex.h>

//typedef QValueList<QByteArray> ByteArrays;

class WebMimeSF : public QObject, public QMimeSourceFactory
{
    Q_OBJECT
    public:
        WebMimeSF(QString hostname,QString path);
        virtual ~WebMimeSF();

    public:
        const QMimeSource* data(const QString& abs_name) const;
        QString makeAbsolute (const QString& abs_or_rel_name, const QString& context )const;
        void setFilePath ( const QStringList & path );
        QStringList filePath () const;
        //void setConsumer(QObject* consumer);
    public slots:
        void newChildren ( const QValueList<QUrlInfo> & i, QNetworkOperation * op );
        void finished ( QNetworkOperation * op );
        void start ( QNetworkOperation * op );
        void createdDirectory ( const QUrlInfo & i, QNetworkOperation * op );
        void removed ( QNetworkOperation * op );
        void itemChanged ( QNetworkOperation * op );
        void data ( const QByteArray & data, QNetworkOperation * op );
        void dataTransferProgress ( int bytesDone, int bytesTotal, QNetworkOperation * op );
        void startedNextCopy ( const QPtrList<QNetworkOperation> & lst );
        void connectionStateChanged ( int state, const QString & data );

    protected:
        void joinArrays();

    private:
        //QObject* cons;
        QString myHost;
        QStringList myPath;
        QString singlePath;
        QValueList<QByteArray>* myBlocks;
        QUrlOperator *op;
        QByteArray fusedArrays;
        mutable QNetworkOperation* operation;
        mutable volatile bool opFinished;

};

#endif

