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

#ifndef TEXT2DBROWSER_H
#define TEXT2DBROWSER_H

#include "DreamsDefs.h"

#ifndef QT_H
#include "qptrlist.h"
#include "qpixmap.h"
#include "qcolor.h"
#include "qtextedit.h"
#endif // QT_H


class DTextBrowserData;

class Q_EXPORT DTextBrowser : public QTextEdit
{
    Q_OBJECT
    Q_PROPERTY( QString source READ source WRITE setSource )
    Q_OVERRIDE( int undoDepth DESIGNABLE false SCRIPTABLE false )
    Q_OVERRIDE( bool overwriteMode DESIGNABLE false SCRIPTABLE false )
    Q_OVERRIDE( bool modified SCRIPTABLE false)
    Q_OVERRIDE( bool readOnly DESIGNABLE false SCRIPTABLE false )
    Q_OVERRIDE( bool undoRedoEnabled DESIGNABLE false SCRIPTABLE false )

public:
    DTextBrowser( QWidget* parent=0, const char* name=0 );
    ~DTextBrowser();

    QString source() const;

public slots:
    virtual void setSource(const QString& name);
    virtual void backward();
    virtual void forward();
    virtual void home();
    virtual void reload();
    void setText( const QString &txt ) { setText( txt, QString::null ); }
    virtual void setText( const QString &txt, const QString &context );

signals:
    void backwardAvailable( bool );
    void forwardAvailable( bool );
    void sourceChanged( const QString& );
    void highlighted( const QString& );
    void linkClicked( const QString& );
    void anchorClicked( const QString&, const QString& );

protected:
    void keyPressEvent( QKeyEvent * e);

private:
    void popupDetail( const QString& contents, const QPoint& pos );
    bool linksEnabled() const { return TRUE; }
    void emitHighlighted( const QString &s );
    void emitLinkClicked( const QString &s );
    DTextBrowserData *d;

private:    // Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    DTextBrowser( const DTextBrowser & );
    DTextBrowser& operator=( const DTextBrowser & );
#endif

protected:
    //bool eventFilter ( QObject * watched, QEvent * e );

    public:
    volatile bool ioInProgress;
};


#endif // QTEXTBROWSER_H
