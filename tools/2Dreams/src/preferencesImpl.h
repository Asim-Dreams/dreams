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
  * @file preferencesImpl.h
  */

#ifndef PREFERENCESIMPL_H
#define PREFERENCESIMPL_H

#include <qlistview.h>

#include "asim/DralDB.h"

#include "preferences.h"
#include "PreferenceMgr.h"
#include "tagDescDialogs.h"

class preferencesImpl : public PreferencesDialog
{
    Q_OBJECT

    public:
        preferencesImpl( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
        ~preferencesImpl();

    public slots:
        void incPurgeToggled(bool);
        void maxIFIPurgeToggled(bool);
        void Slider_GridMoved(int);
        void Slider_MaxInflightMoved(int);
        void validateAdfToggled(bool);
        void logBtnClicked();
        void adfBtnClicked();
        void cmdBtnClicked();
        void bgColorBtnClicked();
        void shadeColorBtnClicked();
        void favoriteUpBtnClicked();
        void favoriteDownBtnClicked();
        void addFavoriteBtnClicked();
        void removeFavoriteBtnClicked();
        void FavoriteSelectionChanged(QListBoxItem*);
        void knownTagsBtnClicked();
        void GridSize_textChanged(const QString &);
        void MaxInflight_textChanged(const QString &);
        void accept();

    protected:
        QStringList getGUIStyles();
        
    private:
        DralDB* draldb; 
        PreferenceMgr* pref;
        QWidget* myParent;
        QListBoxItem* lastSelectedFavoriteTag;
};

#endif // PREFERENCESIMPL_H



