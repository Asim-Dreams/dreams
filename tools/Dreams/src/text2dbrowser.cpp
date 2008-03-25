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

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/LogMgr.h"
#include "text2dbrowser.h"

// Qt includes.
#include <qapplication.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qvaluestack.h>
#include <stdio.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qlayout.h>
#include <qbitmap.h>
#include <qtimer.h>
#include <qimage.h>
#include <qsimplerichtext.h>
#include <qdragobject.h>
#include <qurl.h>
#include <qcursor.h>

class DTextBrowserData
{
public:
    DTextBrowserData():textOrSourceChanged(FALSE) {}

    QValueStack<QString> stack;
    QValueStack<QString> forwardStack;
    QString home;
    QString curmain;
    QString curmark;
    
    /*flag necessary to give the linkClicked() signal some meaningful
      semantics when somebody connected to it calls setText() or
      setSource() */
    bool textOrSourceChanged;
};


/*!
    Constructs an empty DTextBrowser called \a name, with parent \a
    parent.
*/
DTextBrowser::DTextBrowser(QWidget *parent, const char *name)
    : QTextEdit( parent, name )
{
    ioInProgress=false;
    setReadOnly( TRUE );
    d = new DTextBrowserData;
    viewport()->setMouseTracking( TRUE );
    //installEventFilter (this); // not necesary QTextEdit alredy does that...
}

DTextBrowser::~DTextBrowser()
{
    delete d;
}


QString DTextBrowser::source() const
{
    if ( d->stack.isEmpty() )
    return QString::null;
    else
    return d->stack.top();
}

void DTextBrowser::reload()
{
    QString s = d->curmain;
    d->curmain = "";
    setSource( s );
}


void DTextBrowser::setSource(const QString& name)
{
    //printf ("DTextBrowser::setSource called on %s\n",name.latin1());fflush(stdout);
    d->textOrSourceChanged = TRUE;
    QString source = name;
    QString mark;
    int hash = name.find('#');
    if ( hash != -1) {
    source = name.left( hash );
    mark = name.mid( hash+1 );
    }

    if ( source.left(5) == "file:" )
    source = source.mid(6);

    QString url = mimeSourceFactory()->makeAbsolute( source, context() );
    QString txt;
    bool dosettext = FALSE;

    if ( !source.isEmpty() && url != d->curmain ) {
    
    ioInProgress=true;    
    const QMimeSource* m = mimeSourceFactory()->data( source, context() );
    ioInProgress=false;
        
    if ( !m ){
        LogMgr* log = LogMgr::getInstance();
        QString report = "Warning (DTextBrowser): no mimesource for ";
        report += source;
        log->addLog(report);
        txt = report;
    }
    else {
        if ( !QTextDrag::decode( m, txt ) ) {
        LogMgr* log = LogMgr::getInstance();
        QString report = "Warning (DTextBrowser): cannot decode ";
        report += source;
        log->addLog(report);
        txt=report;
        }
    }
    if ( isVisible() ) {
        QString firstTag = txt.left( txt.find( '>' ) + 1 );
        if ( firstTag.left( 3 ) == "<qt" && firstTag.contains( "type" ) && firstTag.contains( "detail" ) ) {
        popupDetail( txt, QCursor::pos() );
        return;
        }
    }

    d->curmain = url;
    dosettext = TRUE;
    }

    d->curmark = mark;

    if ( !mark.isEmpty() ) {
    url += "#";
    url += mark;
    }
    if ( !d->home )
    d->home = url;

    if ( d->stack.isEmpty() || d->stack.top() != url)
    d->stack.push( url );

    int stackCount = (int)d->stack.count();
    if ( d->stack.top() == url )
    stackCount--;
    emit backwardAvailable( stackCount > 0 );
    stackCount = (int)d->forwardStack.count();
    if ( d->forwardStack.isEmpty() || d->forwardStack.top() == url )
    stackCount--;
    emit forwardAvailable( stackCount > 0 );

    if ( dosettext )
    QTextEdit::setText( txt, url );

    if ( !mark.isEmpty() )
    scrollToAnchor( mark );
    else
    setContentsPos( 0, 0 );

    emit sourceChanged( url );
}

void DTextBrowser::backward()
{
    if ( d->stack.count() <= 1)
    return;
    d->forwardStack.push( d->stack.pop() );
    setSource( d->stack.pop() );
    emit forwardAvailable( TRUE );
}

void DTextBrowser::forward()
{
    if ( d->forwardStack.isEmpty() )
    return;
    setSource( d->forwardStack.pop() );
    emit forwardAvailable( !d->forwardStack.isEmpty() );
}

/*!
    Changes the document displayed to be the first document the
    browser displayed.
*/
void DTextBrowser::home()
{
    if (!d->home.isNull() )
    setSource( d->home );
}

void DTextBrowser::keyPressEvent( QKeyEvent * e )
{
    if ( e->state() & AltButton ) {
    switch (e->key()) {
    case Key_Right:
        forward();
        return;
    case Key_Left:
        backward();
        return;
    case Key_Up:
        home();
        return;
    }
    }
    QTextEdit::keyPressEvent(e);
}

class QTextDetailPopup : public QWidget
{
public:
    QTextDetailPopup()
    : QWidget ( 0, "automatic QText detail widget", WType_Popup | WDestructiveClose )
    {
    }

protected:

    void mousePressEvent( QMouseEvent*)
    {
    close();
    }
};


void DTextBrowser::popupDetail( const QString& contents, const QPoint& pos )
{

    const int shadowWidth = 6;   // also used as '5' and '6' and even '8' below
    const int vMargin = 8;
    const int hMargin = 12;

    QWidget* popup = new QTextDetailPopup;
    popup->setBackgroundMode( QWidget::NoBackground );

    QSimpleRichText* doc = new QSimpleRichText( contents, popup->font() );
    doc->adjustSize();
    QRect r( 0, 0, doc->width(), doc->height() );

    int w = r.width() + 2*hMargin;
    int h = r.height() + 2*vMargin;

    popup->resize( w + shadowWidth, h + shadowWidth );

    // okay, now to find a suitable location
    //###### we need a global fancy popup positioning somewhere
    popup->move(pos - popup->rect().center());
    if (popup->geometry().right() > QApplication::desktop()->width())
    popup->move( QApplication::desktop()->width() - popup->width(),
             popup->y() );
    if (popup->geometry().bottom() > QApplication::desktop()->height())
    popup->move( popup->x(),
             QApplication::desktop()->height() - popup->height() );
    if ( popup->x() < 0 )
    popup->move( 0, popup->y() );
    if ( popup->y() < 0 )
    popup->move( popup->x(), 0 );


    popup->show();

    // now for super-clever shadow stuff.  super-clever mostly in
    // how many window system problems it skirts around.

    QPainter p( popup );
    p.setPen( QApplication::palette().active().foreground() );
    p.drawRect( 0, 0, w, h );
    p.setPen( QApplication::palette().active().mid() );
    p.setBrush( QColor( 255, 255, 240 ) );
    p.drawRect( 1, 1, w-2, h-2 );
    p.setPen( black );

    doc->draw( &p, hMargin, vMargin, r, popup->colorGroup(), 0 );
    delete doc;

    p.drawPoint( w + 5, 6 );
    p.drawLine( w + 3, 6,
        w + 5, 8 );
    p.drawLine( w + 1, 6,
        w + 5, 10 );
    int i;
    for( i=7; i < h; i += 2 )
    p.drawLine( w, i,
            w + 5, i + 5 );
    for( i = w - i + h; i > 6; i -= 2 )
    p.drawLine( i, h,
            i + 5, h + 5 );
    for( ; i > 0 ; i -= 2 )
    p.drawLine( 6, h + 6 - i,
            i + 5, h + 5 );
}

void DTextBrowser::setText( const QString &txt, const QString &context )
{
    d->textOrSourceChanged = TRUE;
    d->curmark = "";
    d->curmain = "";
    QTextEdit::setText( txt, context );
}

void DTextBrowser::emitHighlighted( const QString &s ) 
{ 
    emit highlighted( s ); 
}

void DTextBrowser::emitLinkClicked( const QString &s ) 
{ 
    d->textOrSourceChanged = FALSE;
    emit linkClicked( s );
    if ( !d->textOrSourceChanged )
    setSource( s );
}

/*
bool
DTextBrowser::eventFilter ( QObject * watched, QEvent * e )
{
    int t = (int)(e->type());
    //printf ("DTextBrowser::eventFilter called, etype=%d, ioInProgress=%d\n",(int)e->type(),(int)ioInProgress);fflush(stdout);
    if (!ioInProgress) return QTextEdit::eventFilter(watched,e);

    if (watched!=this)
    {
        //printf("watched no this!\n");fflush(stdout);
        //return QTextEdit::eventFilter(watched,e);
        return QScrollView::eventFilter(watched,e);
    }


    //if (((t>=70) && (t<=72)) || (t==17) )
    //if ((t>=70) && (t<=72)) 
    //    return QScrollView::eventFilter(watched,e);
    //else
    return true;
    
}
*/

