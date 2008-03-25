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

/****************************************************************************
** Form interface generated from reading ui file 'preferences.ui'
**
** Created: Thu Jan 26 14:41:59 2006
**      by: The User Interface Compiler ($Id: preferences.h 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QTabWidget;
class QWidget;
class QLabel;
class QFrame;
class QLineEdit;
class QToolButton;
class QCheckBox;
class QListBox;
class QListBoxItem;
class QComboBox;
class QSpinBox;
class QSlider;
class QPushButton;

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    PreferencesDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~PreferencesDialog();

    QTabWidget* tabWidget;
    QWidget* tab;
    QLabel* TextLabel_Warn2;
    QLabel* pixmapLabel1;
    QLabel* TextLabel1_3_5;
    QFrame* Line1_2_5;
    QLineEdit* LineEdit_LogFile;
    QToolButton* ToolButton_BrowseLog;
    QLabel* TextLabel1_3_5_2;
    QFrame* Line1_2_5_2;
    QLineEdit* LineEdit_ADF;
    QToolButton* ToolButton_BrowseADF;
    QLabel* TextLabel1_3_5_2_2;
    QFrame* Line1_2_5_2_2;
    QCheckBox* CheckBox_ValidateADF;
    QLabel* TextLabel5;
    QLineEdit* LineEdit_ValidationCmd;
    QToolButton* ToolButton_BrowseCmd;
    QWidget* Widget8;
    QLabel* pixmapLabel1_2;
    QLabel* TextLabel1;
    QFrame* Line1;
    QLineEdit* LineEdit_Relationship;
    QToolButton* ToolButton_AvailableTags;
    QLabel* TextLabel1_2_2;
    QFrame* Line2_2;
    QListBox* ListBox_Favorite;
    QToolButton* ToolButton_FavoriteUp;
    QToolButton* ToolButton_FavoriteDown;
    QToolButton* ToolButton_AddFavorite;
    QToolButton* ToolButton_RemoveFavorite;
    QWidget* tab_2;
    QLabel* pixmapLabel1_3;
    QLabel* TextLabel1_4;
    QFrame* Line1_3;
    QLabel* TextLabel6;
    QLabel* PixmapLabel_BgColor;
    QToolButton* ToolButton_BgColor;
    QLabel* TextLabel6_2;
    QLabel* PixmapLabel_ShadingColor;
    QToolButton* ToolButton_ShadingColor;
    QLabel* TextLabel6_2_2;
    QComboBox* ComboBox_GUIStyle;
    QLabel* TextLabel6_2_2_2;
    QSpinBox* SpinBox_FontSize;
    QLabel* TextLabel6_2_2_2_3;
    QSpinBox* SpinBox_HighlightSize;
    QLabel* TextLabel1_4_2;
    QFrame* Line1_3_2;
    QCheckBox* CheckBox_SnapGrid;
    QCheckBox* CheckBox_ShowHGrid;
    QCheckBox* CheckBox_ShowVGrid;
    QLabel* TextLabel6_2_2_2_2;
    QLineEdit* LineEdit_GridSize;
    QSlider* Slider_GridSize;
    QWidget* tab_3;
    QLabel* pixmapLabel1_4;
    QLabel* TextLabel1_4_2_2_2_2;
    QFrame* Line1_3_2_2_2_2;
    QCheckBox* CheckBox_TagWinFavouriteOnly;
    QLabel* TextLabel1_4_2_2_2_2_2;
    QFrame* Line1_3_2_2_2_2_2;
    QCheckBox* CheckBox_SynchTabs;
    QWidget* tab_4;
    QLabel* pixmapLabel1_5;
    QLabel* TextLabel1_4_2_2_2_2_2_2;
    QFrame* Line1_3_2_2_2_2_2_2;
    QCheckBox* CheckBox_RelaxedEENode;
    QCheckBox* CheckBox_BackPropagate;
    QCheckBox* CheckBox_CompressMutable;
    QCheckBox* CheckBox_IgnoreEdgeBandwidth;
    QLabel* TextLabel1_4_2_2_2;
    QFrame* Line1_3_2_2_2;
    QCheckBox* CheckBox_AutoPurge;
    QCheckBox* CheckBox_IncPurge;
    QSpinBox* SpinBox_MaxAge;
    QCheckBox* CheckBox_MaxPurge;
    QSlider* Slider_MaxInflight;
    QLineEdit* LineEdit_MaxInflight;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

protected:
    QVBoxLayout* PreferencesDialogLayout;
    QSpacerItem* spacer26;
    QVBoxLayout* tabLayout;
    QSpacerItem* spacer16;
    QSpacerItem* spacer16_2;
    QSpacerItem* spacer16_3;
    QSpacerItem* spacer16_4;
    QSpacerItem* spacer16_5;
    QHBoxLayout* layout45;
    QVBoxLayout* layout52;
    QHBoxLayout* layout46;
    QVBoxLayout* layout51;
    QHBoxLayout* layout47;
    QVBoxLayout* layout50;
    QVBoxLayout* layout49;
    QHBoxLayout* layout48;
    QVBoxLayout* Widget8Layout;
    QSpacerItem* spacer21;
    QVBoxLayout* layout44;
    QSpacerItem* spacer15;
    QVBoxLayout* layout42;
    QHBoxLayout* layout40;
    QHBoxLayout* layout39;
    QVBoxLayout* layout36;
    QSpacerItem* spacer14;
    QVBoxLayout* layout35;
    QSpacerItem* spacer13;
    QVBoxLayout* tabLayout_2;
    QSpacerItem* spacer25;
    QVBoxLayout* layout55;
    QHBoxLayout* layout59;
    QSpacerItem* spacer22;
    QHBoxLayout* layout60;
    QSpacerItem* spacer23;
    QHBoxLayout* layout62;
    QHBoxLayout* layout63;
    QHBoxLayout* layout63_2;
    QVBoxLayout* layout68;
    QHBoxLayout* layout67;
    QSpacerItem* spacer24;
    QVBoxLayout* layout66;
    QVBoxLayout* layout65;
    QVBoxLayout* tabLayout_3;
    QSpacerItem* spacer28;
    QSpacerItem* spacer27;
    QSpacerItem* spacer33;
    QVBoxLayout* layout71;
    QVBoxLayout* layout72;
    QVBoxLayout* tabLayout_4;
    QSpacerItem* spacer30;
    QSpacerItem* spacer31;
    QSpacerItem* spacer32;
    QVBoxLayout* layout38;
    QVBoxLayout* layout79;
    QHBoxLayout* layout76;
    QHBoxLayout* layout78;
    QSpacerItem* spacer29;
    QHBoxLayout* Layout1;
    QSpacerItem* Horizontal_Spacing2;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image2;
    QPixmap image3;
    QPixmap image4;
    QPixmap image5;
    QPixmap image6;
    QPixmap image7;
    QPixmap image8;
    QPixmap image9;
    QPixmap image10;
    QPixmap image11;
    QPixmap image12;
    QPixmap image13;

};

#endif // PREFERENCESDIALOG_H
