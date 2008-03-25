/*
 * Copyright (C) 2005-2006 Intel Corporation
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

#ifndef MTAGFORMIMPL_H
#define MTAGFORMIMPL_H
#include "DreamsDefs.h"
#include "MutableTagDialogForm.h"

class MutableTagDialogFormImpl : public MutableTagDialogForm
{
    Q_OBJECT

public:
    MutableTagDialogFormImpl( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MutableTagDialogFormImpl();

    inline QListView* getListView() { return MTagListView; }
    
public slots:
    void AcceptButton_clicked();
};

#endif // MTAGFORMIMPL_H
