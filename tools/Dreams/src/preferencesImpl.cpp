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
  * @file preferencesImpl.cpp
  */

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "preferencesImpl.h"
#include "DreamsDB.h"
#include "PreferenceMgr.h"
#include "tagDescDialogs.h"

// Qt includes.
#include <qstylefactory.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qwidget.h>
#include <qfiledialog.h>
#include <qcolordialog.h>
#include <qapplication.h>
#include <qinputdialog.h>
#include <qstrlist.h>

/*
 *  Constructs a preferencesImpl which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */

preferencesImpl::preferencesImpl(DreamsDB * _dreamsdb, QWidget* parent, const char * name, bool modal, WFlags fl)
    : PreferencesDialog(parent, name, modal, fl)
{
    dreamsdb = _dreamsdb;
    myParent=parent;
    lastSelectedFavoriteTag=NULL;

    // ----------------------------------------------------
    // init & connections
    // ----------------------------------------------------
    pref = PreferenceMgr::getInstance();

    // Appearence
    PixmapLabel_BgColor->setPaletteBackgroundColor(pref->getBackgroundColor());
    PixmapLabel_ShadingColor->setPaletteBackgroundColor(pref->getShadingColor());
    ComboBox_GUIStyle->insertStringList(getGUIStyles());
    ComboBox_GUIStyle->setCurrentText (pref->getGuiStyle());
    SpinBox_FontSize->setValue(pref->getFontSize());
    SpinBox_HighlightSize->setValue(pref->getHighlightSize());
    connect(ToolButton_BgColor,SIGNAL(clicked ()), this, SLOT(bgColorBtnClicked()));
    connect(ToolButton_ShadingColor,SIGNAL(clicked ()), this, SLOT(shadeColorBtnClicked()));

    // Tags section
    LineEdit_Relationship->setText(pref->getRelationshipTag());
    ListBox_Favorite->insertStringList(QStringList::fromStrList(pref->getFavoriteTags()));
    connect (ToolButton_FavoriteUp,SIGNAL(clicked ()), this, SLOT(favoriteUpBtnClicked()));
    connect (ToolButton_FavoriteDown,SIGNAL(clicked ()), this, SLOT(favoriteDownBtnClicked()));
    connect (ToolButton_AddFavorite,SIGNAL(clicked ()), this, SLOT(addFavoriteBtnClicked()));
    connect (ToolButton_RemoveFavorite,SIGNAL(clicked ()), this, SLOT(removeFavoriteBtnClicked()));
    connect (ListBox_Favorite,SIGNAL(selectionChanged(QListBoxItem*)),this,SLOT(FavoriteSelectionChanged(QListBoxItem*)));
    ToolButton_AvailableTags->setEnabled((dreamsdb != NULL) && dreamsdb->hasLoadedData());
    connect (ToolButton_AvailableTags,SIGNAL(clicked ()), this, SLOT(knownTagsBtnClicked()));


    // Files section
    LineEdit_LogFile->setText(pref->getLogFileName());
    LineEdit_ADF->setText(pref->getArchitectureDefinitionFile());
    connect (CheckBox_ValidateADF,SIGNAL(toggled(bool)), this, SLOT(validateAdfToggled(bool)));
    CheckBox_ValidateADF->setChecked(pref->getPerformValidation());
    LineEdit_ValidationCmd->setText(pref->getValidationCommand());
    connect (ToolButton_BrowseLog,SIGNAL(clicked ()), this, SLOT(logBtnClicked()));
    connect (ToolButton_BrowseADF,SIGNAL(clicked ()), this, SLOT(adfBtnClicked()));
    connect (ToolButton_BrowseCmd,SIGNAL(clicked ()), this, SLOT(cmdBtnClicked()));

    bool enabled = (dreamsdb == NULL) || !dreamsdb->hasLoadedData();
    LineEdit_LogFile->setEnabled(enabled);
    LineEdit_ADF->setEnabled(enabled);
    CheckBox_ValidateADF->setEnabled(enabled);
    LineEdit_ValidationCmd->setEnabled(enabled && pref->getPerformValidation());
    ToolButton_BrowseLog->setEnabled(enabled);
    ToolButton_BrowseADF->setEnabled(enabled);
    ToolButton_BrowseCmd->setEnabled(enabled && pref->getPerformValidation());

    if ((dreamsdb != NULL) && dreamsdb->hasLoadedData())
    {
        TextLabel_Warn2->show();
    }
    else
    {
        TextLabel_Warn2->hide();
    }


    // DBase section
    connect( Slider_MaxInflight, SIGNAL( valueChanged(int) ), this, SLOT( Slider_MaxInflightMoved(int) ) );
    Slider_MaxInflight->setValue(pref->getMaxIFI());

    // grid section
    connect( Slider_GridSize,    SIGNAL( valueChanged(int) ), this, SLOT( Slider_GridMoved(int) ) );
    connect(LineEdit_GridSize,SIGNAL(textChanged(const QString &)),this,SLOT(GridSize_textChanged(const QString &)));
    CheckBox_SnapGrid->setChecked(pref->getSnapToGrid());
    CheckBox_ShowVGrid->setChecked(pref->getShowVGridLines());
    CheckBox_ShowHGrid->setChecked(pref->getShowHGridLines());
    Slider_GridSize->setValue(pref->getSnapGap());
    LineEdit_GridSize->setText(QString::number(pref->getSnapGap()));

    // Behaviour Section
    CheckBox_AutoPurge->setChecked(pref->getAutoPurge());
    CheckBox_IncPurge->setChecked(pref->getIncrementalPurge());
    SpinBox_MaxAge->setEnabled(pref->getIncrementalPurge());
    SpinBox_MaxAge->setValue(pref->getItemMaxAge());
    connect (CheckBox_IncPurge,SIGNAL(toggled(bool)),this,SLOT(incPurgeToggled(bool)));

    CheckBox_TagWinFavouriteOnly->setChecked(pref->getTagWinFavouriteOnly());
    CheckBox_SynchTabs->setChecked(pref->getKeepSynchTabs());
    CheckBox_RelaxedEENode->setChecked(pref->getRelaxedEENode());
    CheckBox_BackPropagate->setChecked(pref->getTagBackPropagate());
    CheckBox_CompressMutable->setChecked(pref->getCompressMutableTags());

    connect(LineEdit_MaxInflight,SIGNAL(textChanged(const QString &)),this,SLOT(MaxInflight_textChanged(const QString &)));
    connect(CheckBox_MaxPurge,SIGNAL(toggled(bool)),this,SLOT(maxIFIPurgeToggled(bool)));
    LineEdit_MaxInflight->setText(QString::number(pref->getMaxIFI()));
    CheckBox_MaxPurge->setChecked(pref->getMaxIFIEnabled());
    CheckBox_IgnoreEdgeBandwidth->setChecked(pref->getIgnoreEdgeBandwidthConstrain());
}

/*
 *  Destroys the object and frees any allocated resources
 */
preferencesImpl::~preferencesImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

void
preferencesImpl::Slider_GridMoved(int  value)
{
    LineEdit_GridSize->setText(QString::number(value));
}

void
preferencesImpl::Slider_MaxInflightMoved(int  value)
{
    LineEdit_MaxInflight->setText(QString::number(value));
}

void
preferencesImpl::validateAdfToggled(bool value)
{
    LineEdit_ValidationCmd->setEnabled(value);
    ToolButton_BrowseCmd->setEnabled(value);
}

void
preferencesImpl::logBtnClicked()
{
    QString fn = QFileDialog::getSaveFileName( QString::null,
      "Log Files (*.txt)", myParent,
      "log file dialog","Select log output file");

    if (fn==QString::null) return;
    LineEdit_LogFile->setText(fn);
}

void
preferencesImpl::adfBtnClicked()
{
    QString fn = QFileDialog::getOpenFileName( QString::null,
      "ADF Files (*.xml)", myParent,
      "adf file dialog","Select ADF default file");

    if (fn==QString::null) return;
    LineEdit_ADF->setText(fn);
}

void
preferencesImpl::cmdBtnClicked()
{
    QString fn = QFileDialog::getOpenFileName( QString::null,
      "Binary Files", myParent,
      "cmd file dialog","Select External Tool Path");

    if (fn==QString::null) return;
    LineEdit_ValidationCmd->setText(fn);
}


void
preferencesImpl::bgColorBtnClicked()
{
    QColor result = QColorDialog::getColor (pref->getBackgroundColor(),myParent,"bgColor");
    if (result.isValid())
    {
        PixmapLabel_BgColor->setPaletteBackgroundColor(result);
    }
}

void
preferencesImpl::shadeColorBtnClicked()
{
    QColor result = QColorDialog::getColor (pref->getShadingColor(),myParent,"shadeColor");
    if (result.isValid())
    {
        PixmapLabel_ShadingColor->setPaletteBackgroundColor(result);
    }
}

void
preferencesImpl::favoriteUpBtnClicked()
{
    if (lastSelectedFavoriteTag!=NULL)
    {
        int pos = ListBox_Favorite->currentItem();
        if (pos>0)
        {
            QString prevStr = (ListBox_Favorite->item(pos-1))->text();
            ListBox_Favorite->changeItem(lastSelectedFavoriteTag->text(),pos-1);
            ListBox_Favorite->changeItem(prevStr,pos);
            ListBox_Favorite->setSelected(pos-1,true);
            ListBox_Favorite->setCurrentItem(pos-1);
            ListBox_Favorite->ensureCurrentVisible();
        }
    }
    else
    {
        qApp->beep();
    }
}

void
preferencesImpl::favoriteDownBtnClicked()
{
    if (lastSelectedFavoriteTag!=NULL)
    {
        int pos = ListBox_Favorite->currentItem();
        if (pos< ((int)ListBox_Favorite->count() - 1))
        {
            QString prevStr = (ListBox_Favorite->item(pos+1))->text();
            ListBox_Favorite->changeItem(lastSelectedFavoriteTag->text(),pos+1);
            ListBox_Favorite->changeItem(prevStr,pos);
            ListBox_Favorite->setSelected(pos+1,true);
            ListBox_Favorite->setCurrentItem(pos+1);
            ListBox_Favorite->ensureCurrentVisible();
        }
    }
    else
    {
        qApp->beep();
    }
}

void
preferencesImpl::addFavoriteBtnClicked()
{
    bool ok=false;
    QString neotag = QInputDialog::getText ( "New TAG", "Enter a TAG Name", QLineEdit::Normal,
                     QString::null, &ok, myParent,"neotag");

    if (ok)
    {
        ListBox_Favorite->insertItem(neotag);
    }
}

void
preferencesImpl::removeFavoriteBtnClicked()
{
    if (lastSelectedFavoriteTag!=NULL)
    {
        delete lastSelectedFavoriteTag;
        lastSelectedFavoriteTag = NULL;
    }
    else
    {
        qApp->beep();
    }
}

void
preferencesImpl::FavoriteSelectionChanged(QListBoxItem* item)
{ lastSelectedFavoriteTag=item; }

void
preferencesImpl::knownTagsBtnClicked()
{
    if(dreamsdb != NULL)
    {
        QStrList strlist (false);
        strlist = dreamsdb->getKnownTags();
        // create a simple dialog to select/cancel a given tag
        ChooseTagDialog* dlg = new ChooseTagDialog( this, "seleTag", true);
        QStrListIterator it_str(strlist);
        char *str;

        while((str = it_str.current()) != NULL)
        {
            ++it_str;
            dlg->insertItem(QString(str), dreamsdb->getDralDB()->getTagDesc(QString(str)));
        }
        dlg->exec();
        if (dlg->getSelected()!=QString::null)
        {
            LineEdit_Relationship->setText(dlg->getSelected());
        }
    }
}

void
preferencesImpl::GridSize_textChanged(const QString &)
{
    bool atoi;
    int tmp = LineEdit_GridSize->text().toInt(&atoi);
    if ( (atoi) && (tmp<=1000) && (tmp>=4))
    {
        Slider_GridSize->setValue(tmp);
    }
}

void
preferencesImpl::MaxInflight_textChanged(const QString &)
{
    bool atoi;
    int tmp = LineEdit_MaxInflight->text().toInt(&atoi);
    if ( (atoi) && (tmp<=10000000) && (tmp>=1000))
    {
        Slider_MaxInflight->setValue(tmp);
    }
}

void
preferencesImpl::incPurgeToggled(bool on)
{
    SpinBox_MaxAge->setEnabled(on);
}

void
preferencesImpl::maxIFIPurgeToggled(bool on)
{
    Slider_MaxInflight->setEnabled(on);
    LineEdit_MaxInflight->setEnabled(on);
}

void
preferencesImpl::accept()
{
    // Appearence
    pref->setBackgroundColor(PixmapLabel_BgColor->paletteBackgroundColor());
    pref->setShadingColor(PixmapLabel_ShadingColor->paletteBackgroundColor());
    pref->setGuiStyle(ComboBox_GUIStyle->currentText());
    pref->setFontSize(SpinBox_FontSize->value());
    pref->setHighlightSize(SpinBox_HighlightSize->value());

    // Tags section
    pref->setRelationshipTag(LineEdit_Relationship->text());

    QListBoxItem* qlbitem = ListBox_Favorite->item(0);
    QStrList strlst2;
    while (qlbitem!=NULL)
    {
        strlst2.append(qlbitem->text());
        qlbitem = qlbitem->next();
    }
    pref->setFavoriteTags(strlst2) ;


    // Files section
    if ((dreamsdb == NULL) || !dreamsdb->hasLoadedData())
    {
        pref->setLogFileName(LineEdit_LogFile->text());
        pref->setArchitectureDefinitionFile(LineEdit_ADF->text());
        pref->setPerformValidation(CheckBox_ValidateADF->isChecked());
        pref->setValidationCommand(LineEdit_ValidationCmd->text());
    }

    // grid section
    pref->setSnapToGrid(CheckBox_SnapGrid->isChecked());
    pref->setShowHGridLines(CheckBox_ShowHGrid->isChecked());
    pref->setShowVGridLines(CheckBox_ShowVGrid->isChecked());
    pref->setSnapGap(Slider_GridSize->value());

    // behaviour section
    pref->setAutoPurge(CheckBox_AutoPurge->isChecked());
    pref->setIncrementalPurge(CheckBox_IncPurge->isChecked());
    pref->setItemMaxAge(SpinBox_MaxAge->value());
    pref->setTagWinFavouriteOnly(CheckBox_TagWinFavouriteOnly->isChecked());
    pref->setKeepSynchTabs(CheckBox_SynchTabs->isChecked());
    pref->setRelaxedEENode(CheckBox_RelaxedEENode->isChecked());
    pref->setTagBackPropagate(CheckBox_BackPropagate->isChecked());
    pref->setCompressMutableTags(CheckBox_CompressMutable->isChecked());
    pref->setIgnoreEdgeBandwidthConstrain(CheckBox_IgnoreEdgeBandwidth->isChecked());
    pref->setMaxIFI(Slider_MaxInflight->value());
    pref->setMaxIFIEnabled(CheckBox_MaxPurge->isChecked());

    done(1);
}

// --------------------------------------------------------------------
// Styles stuff
// --------------------------------------------------------------------
QStringList  
preferencesImpl::getGUIStyles()
{
    QStringList list = QStyleFactory::keys();
    list.append(QString("Dreams"));
    list.sort();
    return list;
}
