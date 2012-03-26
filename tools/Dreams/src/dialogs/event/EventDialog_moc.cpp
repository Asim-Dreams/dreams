/****************************************************************************
** EventDialog meta object code from reading C++ file 'EventDialog.h'
**
** Created: Tue Oct 31 00:50:54 2006
**      by: The Qt MOC ($Id: EventDialog_moc.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "EventDialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *EventDialog::className() const
{
    return "EventDialog";
}

QMetaObject *EventDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_EventDialog( "EventDialog", &EventDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString EventDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "EventDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString EventDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "EventDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* EventDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"AcceptButton_clicked", 0, 0 };
    static const QUMethod slot_1 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "AcceptButton_clicked()", &slot_0, QMetaData::Public },
	{ "languageChange()", &slot_1, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"EventDialog", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_EventDialog.setMetaObject( metaObj );
    return metaObj;
}

void* EventDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "EventDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool EventDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: AcceptButton_clicked(); break;
    case 1: languageChange(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool EventDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool EventDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool EventDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
