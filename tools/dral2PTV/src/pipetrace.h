/*
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
 */

// reorder_buffer.h: interface for the rob_t class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIPETRACE_H__D9F0E25D_8B06_486E_9F59_0E4263F1D55B__INCLUDED_)
#define AFX_PIPETRACE_H__D9F0E25D_8B06_486E_9F59_0E4263F1D55B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <unit.h>

#include <ua_model_base.h>

class pipetrace_t;
#include "reorder_buffer.h"

#include <sui_command_base.h>  // needed for sui_rob_t
#include <sui_dump_base.h>

#include "dependency_checker.h"

#ifdef DEBUG
#define DEBUG_ROB
#endif

struct ltstr
{
    bool operator()(const char* s1, const char* s2) const
    {
        return strcmp(s1, s2) < 0;
    }
};    


class pipetrace_t : protected isis_unit_t
{
public:


    pipetrace_t();
    virtual ~pipetrace_t();

    bool initialize(ua_model_base_t& base, rob_t* rob, bool uops);

    bool hasRob;
    bool hasUops;


    virtual void ptRetire(Isis_token_ptr_t token);
    virtual void ptAcquireRes(Isis_resource_ptr_t resource, Isis_token_ptr_t token);
    virtual void ptReleaseRes(Isis_resource_ptr_t resource, Isis_token_ptr_t token);
    static  void ptAddResource(mt_resource_ptr_t resource);
    static  void printYesXml(void);

    /* pointer to platform */
    ua_model_base_t *pPlatform;


    rob_t *pRob;

    /* some private variables for managing pipedream output support */
    static bool fPipeDream;
    static unsigned int pipe_dream_id;
    static FILE *pipe_dream_out;

    /* a tiny class for managing the pipe_dream command */
    class sui_pipe_dream_t : public sui_command_base_t {
    public:
        sui_pipe_dream_t();
        virtual ~sui_pipe_dream_t();
        int DoCommand(int argC, char **argV, void *arg);
    };
    static sui_pipe_dream_t *pipe_dreamCommand;
    friend class pipetrace_t::sui_pipe_dream_t;


    /* some private variables for managing ptv output support */
    static bool fPTV;
    static unsigned int ptv_id;
    static FILE *ptv_out;

    /* a tiny class for managing the ptv command */
    class sui_ptv_t : public sui_command_base_t {
    public:
        sui_ptv_t();
        virtual ~sui_ptv_t();
        int DoCommand(int argC, char **argV, void *arg);
    };
    static sui_ptv_t *ptvCommand;
    friend class pipetrace_t::sui_ptv_t;

    static map<const char*,bool,ltstr>  resList;      
    static map<const char*,unsigned int,ltstr>  resCap;      

    map<const char*,map<unsigned int, Isis_clock_t> >  resTracker;      


};

#endif // !defined(AFX_PIPETRACE_H__D9F0E25D_8B06_486E_9F59_0E4263F1D55B__INCLUDED_)



