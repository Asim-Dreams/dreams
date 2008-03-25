/*****************************************************************************
 *
 * @author Oscar Rosell
 *
 * Copyright (C) 2006 Intel Corporation
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
 *****************************************************************************/
#ifndef DUMPERS_H
#define DUMPERS_H

#include "lifetime.h"
#include "svg_drawer.h"
#include <string>

class NORMAL_DUMPER : public LIFETIME_DUMPER
{
  public:
    NORMAL_DUMPER(std::ostream& o_stream);
    ~NORMAL_DUMPER();

    void dump(const LIFETIME_CLASS& lifetime) const;
    void setPrefix(std::string prefix);

  private:
    std::string prefix_;
};

class COLUMN_DUMPER : public LIFETIME_DUMPER
{
  public:
    COLUMN_DUMPER(std::ostream& o_stream);
    ~COLUMN_DUMPER();

    void dump(const LIFETIME_CLASS& lifetime) const;
    void setPrefix(std::string prefix);

  private:
    std::string prefix_;
};


class SIMPLE_DUMPER : public LIFETIME_DUMPER
{
  public:
    SIMPLE_DUMPER(std::ostream& o_stream);
    ~SIMPLE_DUMPER();

    void dump(const LIFETIME_CLASS& lifetime) const;
    void setPrefix(std::string prefix);

  private:
    std::string prefix_;
};

class SVG_DUMPER : public LIFETIME_DUMPER
{
  public:
    SVG_DUMPER(std::ostream& o_stream);
    ~SVG_DUMPER();

    void dump(const LIFETIME_CLASS& lifetime) const;
    void setArrowTag(std::string tag_name);

    mutable FLOW_DIAGRAM_DRAW draw_;
  private:
    std::string arrow_tag_name_;
};

#endif
