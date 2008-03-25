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

#include "PushoutWidgetImpl.h"

/* 
 *  Constructs a PushoutWidgetImpl which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 */
PushoutWidgetImpl::PushoutWidgetImpl( QWidget* parent,  const char* name, WFlags fl )
    : PushoutWidget( parent, name, fl )
{
}

/*  
 *  Destroys the object and frees any allocated resources
 */
PushoutWidgetImpl::~PushoutWidgetImpl()
{
    // no need to delete child widgets, Qt does it all for us
}

