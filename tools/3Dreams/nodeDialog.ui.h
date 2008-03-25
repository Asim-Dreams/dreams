/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename slots use Qt Designer which will
** update this file, preserving your code. Create an init() slot in place of
** a constructor, and a destroy() slot in place of a destructor.
*****************************************************************************/

/*****************************************************************************
*
* @brief Node Dialog code.
*
* @author Oscar Rosell
*
*  Copyright (C) 2003-2006 Intel Corporation
*  
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*  
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*  
*
*****************************************************************************/


void NodeDialog::setNItems( int n_items )
{
    n_items_ = n_items;
}

void NodeDialog::setCapacity( int capacity )
{
    capacity_ = capacity;
    capacityBar_->setProgress(100*n_items_/capacity_);
}


void NodeDialog::addVisualization( QCacheVisualization * vis )
{
    ListBox1->insertItem(vis->label(), vis_idx_++) ;
    visualizations_.push_back(vis);
}

void NodeDialog::visualizationSelected(int idx )
{
    assert(idx > 0);
    assert(static_cast<unsigned int>(idx) < visualizations_.size());
    
    visualizations_[idx]->configure();
}

void NodeDialog::init()
{
    vis_idx_ = 0;
    connect(ListBox1,SIGNAL(selected(int)), this, SLOT(visualizationSelected(int)));
}

void NodeDialog::setNodeName( QString name )
{
    nodeName->setText(name);
}
