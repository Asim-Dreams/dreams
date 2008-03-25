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

// reorder_buffer.cpp: implementation of the rob_t class.
//
//////////////////////////////////////////////////////////////////////

#include "pipetrace.h"
#include "reorder_buffer.h"
#include <ua_plat_util.h>
#include <plat_isis_cb.h>
#include <isis_export.h>
#include <isis_internal.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

pipetrace_t::pipetrace_t() {
    if(!pipe_dreamCommand) pipe_dreamCommand = new sui_pipe_dream_t;
    if(!ptvCommand) {
        ptvCommand = new sui_ptv_t;
    } 

}

bool pipetrace_t::fPipeDream = false;
FILE* pipetrace_t::pipe_dream_out = NULL;
unsigned int pipetrace_t::pipe_dream_id = 0;

bool  pipetrace_t::fPTV = false;
FILE* pipetrace_t::ptv_out = NULL;
unsigned int pipetrace_t::ptv_id = 0;

map<const char*,bool,ltstr>  pipetrace_t::resList;
map<const char*,unsigned int,ltstr>  pipetrace_t::resCap;

pipetrace_t::~pipetrace_t() {
}

bool pipetrace_t::initialize(ua_model_base_t& base, rob_t* rob, bool uops) {
    pPlatform = &base;    

    if(rob) {
        hasRob = true;
        pRob = rob;
        pRob->pPipetrace = this;
    } else {
        hasRob = false;
    }

    hasUops = uops;

    return true;
}


void pipetrace_t::ptAcquireRes(Isis_resource_ptr_t resource, Isis_token_ptr_t token) {
    //printf("Acquire on resource %s by token %d\n", isis_resource_get_name(resource), isis_token_get_uid(token));
    if(resList[isis_resource_get_name(resource)]) {
        (resTracker[isis_resource_get_name(resource)])[isis_token_get_uid(token)] = pPlatform->Clock();
    }
}

void pipetrace_t::ptReleaseRes(Isis_resource_ptr_t resource, Isis_token_ptr_t token) {
    //printf("Release on resource %s by token %d\n", isis_resource_get_name(resource), isis_token_get_uid(token));
//  printf("Looking for '%s'\n", isis_resource_get_name(resource));
    if(resList[isis_resource_get_name(resource)]) {
        Isis_clock_t acq = (resTracker[isis_resource_get_name(resource)])[isis_token_get_uid(token)];
        Isis_clock_t rel = pPlatform->Clock();

        ptv_id++;
        fprintf(ptv_out, "%u resource_token\n", ptv_id);
        fprintf(ptv_out, "%u %s 1 %u %u\n", ptv_id, isis_resource_get_name(resource), (unsigned int)(rel-acq), (unsigned int)acq);
        //fprintf(ptv_out, "%u %s 1 %u %u\n", ptv_id, isis_resource_get_name(resource), (unsigned int)(rel-acq), (unsigned int)acq+1);
        fprintf(ptv_out, "%u TokenId %u\n", ptv_id, isis_token_get_uid(token));

        (resTracker[isis_resource_get_name(resource)]).erase(isis_token_get_uid(token));
    }
}

void pipetrace_t::ptAddResource(mt_resource_ptr_t resource) {
    //fprintf(ptv_out, "0 ConfigEvent %s r 6 Resource\n", resource->name);
    //resList[resource->name] = 1;
    printf("Adding resource %s of capacity %d\n", resource->name,resource->resource_list[0]->capacity);
    resList[resource->name] = true;
    if(resource->resource_list[0]->capacity != INT_MAX) {
        resCap[resource->name] = resource->resource_list[0]->capacity;
    } else {
        resCap[resource->name] = 256;
    }
}

void pipetrace_t::printYesXml(void) {
    bool fPipe = false;
    FILE* xmlOut = fopen("dante.xml", "w");

    fprintf(xmlOut,"<!-- jPTV Machine Graph File for dante --> \n");
    fprintf(xmlOut,"<machine name=\"dante\" configuration=\"dante\" version=\"1.0\"> \n");
    fprintf(xmlOut,"\n");

    for(map<const char*,unsigned int,ltstr>::iterator i=pipetrace_t::resCap.begin(); 
            i != pipetrace_t::resCap.end(); 
            i++) { 

        fprintf(xmlOut,"<node name=\"%s\" instances=\"%d\" displayAll=\"true\" color=\"Green\"> \n",i->first,i->second);
        fprintf(xmlOut,"    <mapping type=\"event\" name=\"%s\" mappedName=\"%s\" char=\"I\" color=\"White\"> \n",i->first,i->first);
        fprintf(xmlOut,"      <portmap type=\"open_slot\"/> \n");
        fprintf(xmlOut,"   </mapping> \n");
        fprintf(xmlOut," \n");
        fprintf(xmlOut,"</node> \n");

    }

    fprintf(xmlOut,"\n");
    fprintf(xmlOut,"<!-- insert a blank line at end to get around jPTV bug that hides last line --> \n");
    fprintf(xmlOut,"<blank/> \n");
    fprintf(xmlOut," \n");
    fprintf(xmlOut,"</machine> \n");

    fclose(xmlOut);

}

void pipetrace_t::ptRetire(Isis_token_ptr_t token) {

    rob_t::rob_entry_info_t* entry_info;
    if(hasRob) {
        entry_info = pRob->getEntry(token);
    }

    /* if pipedream output is turned on at the current time then print the pipedream info for this uop 
     * but don't even try if we don't have a rob */
    if(fPipeDream && hasRob) {
        sib_ptr_t sib = GET_TOKEN_SIB(token);
        char dst_buf[128], src_buf[128];
        char inst_buf[64];
        char cruft[64];
        dst_buf[0]=src_buf[0]='\0';

        isrc_inst_info_t *pInst = pPlatform->UopGetInst(pPlatform->UopGet(token));
        isrc_arch_state_t *pas;

        for(pas = pInst->pasSrc; pas!=NULL; pas = pas->pNext) {
            if(pas->_class!=ISRC_SC_MEM) {
                if(strlen(src_buf)<118) {
                    sprintf(src_buf, "%s(%s)", src_buf, InstInfoStateGetStr(pas, false, cruft));
                }
            }
        }

        for(pas = pInst->pasDst; pas!=NULL; pas = pas->pNext) {
            if(pas->_class!=ISRC_SC_MEM) {
                if(strlen(dst_buf)<118) {
                    sprintf(dst_buf, "%s(%s)", dst_buf, InstInfoStateGetStr(pas, false, cruft));
                }
            }
        }

        fprintf(pipe_dream_out, "%d: %s | fetch %"LL_FMT"d"
                " ready %"LL_FMT"d"
                " dis %"LL_FMT"d"
                " done %"LL_FMT"d"
                " retire %"LL_FMT"d dest %s src %s\n",
                pipe_dream_id++, PlatGetInstStr(&sib->inst.dec, false, 0, inst_buf, NULL), sib->clk[SIB_STAT_FETCH], 
                entry_info->clock[rob_t::WAIT], 
                entry_info->clock[rob_t::IN_EXEC], 
                entry_info->clock[rob_t::AFTER_EXEC],
                pPlatform->Clock(),
                dst_buf, src_buf);
    }

    if(fPTV) {
        sib_ptr_t sib = GET_TOKEN_SIB(token);
        isrc_inst_info_t *pInst;

        if(hasUops) {
            pInst = pPlatform->UopGetInst(pPlatform->UopGet(token));
        } else {
            pInst = &sib->inst;       
        }

        isrc_arch_state_t *pas;
        u8int forked_tokens[200]; // If we have more forked tokens than this, we have a problem!
        int numForks;
        char dst_buf[128], src_buf[128];
        char inst_buf[64];
        char cruft[64];
        dst_buf[0]=src_buf[0]='\0';

        for(pas = pInst->pasSrc; pas!=NULL; pas = pas->pNext) {
            if(pas->_class!=ISRC_SC_MEM) {
                if(strlen(src_buf)<118) {
                    sprintf(src_buf, "%s(%s)", src_buf, InstInfoStateGetStr(pas, false, cruft));
                }
            }
        }

        for(pas = pInst->pasDst; pas!=NULL; pas = pas->pNext) {
            if(pas->_class!=ISRC_SC_MEM) {
                if(strlen(dst_buf)<118) {
                    sprintf(dst_buf, "%s(%s)", dst_buf, InstInfoStateGetStr(pas, false, cruft));
                }
            }
        }

        ptv_id++;

        fprintf(ptv_out, "%u token\n",ptv_id);
        fprintf(ptv_out, "%u Inst_ID %u\n",ptv_id, sib->seq);
        fprintf(ptv_out, "%u TokenId %u\n",ptv_id, isis_token_get_uid(token));
        fprintf(ptv_out, "%u SibId %d\n",ptv_id, sib->seq);
        fprintf(ptv_out, "%u LPID %d\n",ptv_id, sib->lpid);
        fprintf(ptv_out, "%u CoreId %d\n",ptv_id, pPlatform->coreId);
        fprintf(ptv_out, "%u IP 0x%"LL_FMT"x\n",ptv_id, sib->inst.ip.v);
        fprintf(ptv_out, "%u Token_Fetch 1 1 %"LL_FMT"d\n", ptv_id, sib->clk[SIB_STAT_FETCH]);
        fprintf(ptv_out, "%u IP 0x%"LL_FMT"x\n",ptv_id, sib->inst.ip.v);
        fprintf(ptv_out, "%u Disassembly %s \n",ptv_id, PlatGetInstStr(&sib->inst.dec, false, 0, inst_buf, NULL));
        if(hasRob) {
            fprintf(ptv_out, "%u Token_Wait 1 1 %"LL_FMT"d \n", ptv_id, entry_info->clock[rob_t::WAIT]);
            fprintf(ptv_out, "%u IP 0x%"LL_FMT"x\n",ptv_id, sib->inst.ip.v);
            fprintf(ptv_out, "%u Token_In_Exec 1 1 %"LL_FMT"d\n", ptv_id, entry_info->clock[rob_t::IN_EXEC]);
            fprintf(ptv_out, "%u IP 0x%"LL_FMT"x\n",ptv_id, sib->inst.ip.v);
            fprintf(ptv_out, "%u Token_After_Exec 1 1 %"LL_FMT"d\n", ptv_id, entry_info->clock[rob_t::AFTER_EXEC]);
            fprintf(ptv_out, "%u IP 0x%"LL_FMT"x\n",ptv_id, sib->inst.ip.v);
            fprintf(ptv_out, "%u Token_Issued 1 1 %"LL_FMT"d\n", ptv_id, entry_info->clock[rob_t::ISSUED]);
            fprintf(ptv_out, "%u IP 0x%"LL_FMT"x\n",ptv_id, sib->inst.ip.v);
        }
        fprintf(ptv_out, "%u Token_Retire 1 1 %"LL_FMT"d\n", ptv_id, pPlatform->Clock());
        fprintf(ptv_out, "%u IP 0x%"LL_FMT"x\n",ptv_id, sib->inst.ip.v);
        fprintf(ptv_out, "%u Sources %s \n",ptv_id, src_buf);
        fprintf(ptv_out, "%u Destinations %s \n",ptv_id, dst_buf);

        //(FILE *fp, int lpid, _pd_event_t *pev) 
        int lpid = 0;
        _pd_event_t *pev = sib->plPDEvent;
        char                        szNew[4], szOld[4]; /* state abbreviation   */

        numForks = 0;

        //printf("%s", (pev) ? "    pde " : "");
        //printf("\nuop seq #: %d\n",sib->seq);
        if (pev) pev->pPrev = NULL;
        for ( ; pev; pev = pev->pNext) {
            if(pev->pNext) pev->pNext->pPrev = pev;
            if(pev->event==-1) {
                //                                      printf("%lld uid %d '%s' <- '%s'%s", pev->clock, pev->uidToken,
                //                                              pPlatform->PlatGetStateAbbrev(lpid, pev->iNewState, szNew),
                //                                              pPlatform->PlatGetStateAbbrev(lpid, pev->iOldState, szOld),
                //                                              (pev->pNext) ? "; " : "\n");
                if(pev->uidToken == isis_token_get_uid(token)) {
                    fprintf(ptv_out, "%u PDEvent 1 1 %"LL_FMT"d \n", ptv_id, pev->clock);
                    fprintf(ptv_out, "%u Event_Type %s\n", ptv_id, 
                            isis_state_get_name(pPlatform->PlatGetSId(sib->lpid), pev->iNewState));
                } else {
                    // Next generate events for the forked tokens.
                    // Add this one to the list of forked tokens if it isn't already there
                    bool already_there=false;
                    for(int i=0;i<numForks;i++) {
                        if(forked_tokens[i]==pev->uidToken) already_there = true;
                    } 
                    if(!already_there) {
                        forked_tokens[numForks] = pev->uidToken;
                        numForks++;
                        if(numForks>200) assert(0);
                    }
                }

            }
        }



        if(hasUops && pPlatform->UopGetNum(token)==0){
            for(int i=0;i<numForks;i++) {
                uop_ptr_t pU;
                if(sib->totalUops > 1) {
                    // Is this guy just another uop?  If he is, we don't want to do this
                    bool just_another_uop = false;
                    for(pU=sib->uop; pU != NULL; pU = pU->next_uop) {
                        if(pU->valid && pU->num && (isis_token_get_uid(pU->token))==forked_tokens[i]) {
                            just_another_uop = true;
                        }
                    }
                    if(just_another_uop) continue;
                }

                ptv_id++;
                fprintf(ptv_out, "%u forked_token\n",ptv_id);
                fprintf(ptv_out, "%u TokenId %u\n",ptv_id, forked_tokens[i]);
                fprintf(ptv_out, "%u SibId %u\n",ptv_id, sib->seq);
                fprintf(ptv_out, "%u LPID %d\n",ptv_id, sib->lpid);
                fprintf(ptv_out, "%u CoreId %d\n",ptv_id, pPlatform->coreId);
                //fprintf(ptv_out, "%d Disassembly ...forked token \n",ptv_id);
                pev = sib->plPDEvent; 
                for ( ; pev->pNext != NULL; pev = pev->pNext) { NULL;}
                //pev = pev->pPrev;
                for ( ; pev != NULL; pev = pev->pPrev) {
                    if(pev->event==-1) {
                        if(pev->uidToken == forked_tokens[i]) {
                            if(pev->iNewState != (unsigned int)(-1)) {                                      
                                fprintf(ptv_out, "%u PDEvent 1 1 %"LL_FMT"d \n", ptv_id, pev->clock);
                                fprintf(ptv_out, "%u Event_Type %s\n", ptv_id, 
                                        isis_state_get_name(pPlatform->PlatGetSId(sib->lpid), pev->iNewState));
                            } else {
                                fprintf(ptv_out, "%u PDEvent 1 1 %"LL_FMT"d \n", ptv_id, pev->clock);
                                fprintf(ptv_out, "%u Event_Type %s\n", ptv_id, "Forked token");
                            }
                        }
                    }
                }
            }
        }

    } 


}



#include <sui_core.h>

 
pipetrace_t::sui_pipe_dream_t *pipetrace_t::pipe_dreamCommand = NULL;

pipetrace_t::sui_pipe_dream_t::sui_pipe_dream_t() {
    static char aszPipeDreamHelp[] = "[args]\t\t\t\tturn on/off pipe dream output generation";
    static char aszPipeDreamSubHelp[] = 
"  Arguments:\n"
"    on filename     turns pipe dream output generation on and directs pipe dream output to filename\n"
"    off             turns pipe dream output generation off\n"
"\n";
    AssignHelpStringPtrs(aszPipeDreamHelp, aszPipeDreamSubHelp);
    SetCmdName("pipe_dream");
}

pipetrace_t::sui_pipe_dream_t::~sui_pipe_dream_t() {

}

int pipetrace_t::sui_pipe_dream_t::DoCommand(int argC, char **argV, void *arg) {

    if( ua_model_base_t::numCores() > 1 ) {
        fprintf(dante_err, "WARNING:  the pipe_dream command only works for single core simulations, and multiple cores are detected.  pipe_dream command ignored\n");
        return 0;
    }

    if(argC<=1) {
        fprintf(dante_err, "WARNING:  the pipe_dream command requires arguments\n"); 
        return 0;
    }

    bool fPipe = false;

    if(strcmp(argV[1],"on")==0) {
        if(argC<=2) {
            fprintf(dante_err, "WARNING:  the on option for the pipe_dream command requires an output filename be specified\n");
            return 0;
        }


        if(fPipeDream) {
            SUICloseFileOrPipe(pipe_dream_out, fPipe);
            fPipeDream = false;
        }
        pipe_dream_out = SUIOpenFileOrPipe(argV[0], false, argV[2], &fPipe);
        if(pipe_dream_out==NULL) {
            return -1;
        }

        fprintf(dante_out, "pipe dream output enabled\n");

        fPipeDream = true;
        pipe_dream_id = 0;

    } else if(strcmp(argV[1], "off")==0) {

        if(fPipeDream) {
            SUICloseFileOrPipe(pipe_dream_out, fPipe);
            fPipeDream = false;
            fprintf(dante_out, "pipe dream output disabled\n");
        } else {
            fprintf(dante_err, "cannot turn off pipe dream output because pipe dream output is not currently enabled\n");
        }
    } else {
        fprintf(dante_err, "WARNING:  unknown arguments to pipe_dream command\n");
    }

    return 0;
}

pipetrace_t::sui_ptv_t *pipetrace_t::ptvCommand = NULL;

pipetrace_t::sui_ptv_t::sui_ptv_t() {
    static char aszPTVHelp[] = "[args]\t\t\t\tturn on/off ptv output generation";
    static char aszPTVSubHelp[] = 
"  Arguments:\n"
"    on filename     turns ptv output generation on and directs ptv output to filename\n"
"    pause           pauses ptv output; use same command again to unpause\n"
"    off             turns ptv output generation off\n"
"\n";
    AssignHelpStringPtrs(aszPTVHelp, aszPTVSubHelp);
    SetCmdName("ptv");
}

pipetrace_t::sui_ptv_t::~sui_ptv_t() {

}

int pipetrace_t::sui_ptv_t::DoCommand(int argC, char **argV, void *arg) {

    printf("DoCommand for ptv\n"); 

    if(argC<=1) {
        fprintf(dante_err, "WARNING:  the ptv command requires arguments\n"); 
        return 0;
    }

    bool fPipe = false;

    if(strcmp(argV[1],"on")==0) {
        if(argC<=2) {
            fprintf(dante_err, "WARNING:  the on option for the ptv command requires an output filename be specified\n");
            return 0;
        }


        if(fPTV) {
            return 0;
        }

        ptv_out = SUIOpenFileOrPipe(argV[0], false, argV[2], &fPipe);
        if(ptv_out==NULL) {
            return -1;
        }

        fprintf(dante_out, "ptv output enabled.\n");

        for(map<Isis_session_t,ua_model_base_t*>::iterator i=ua_model_base_t::cores.begin(); 
                i != ua_model_base_t::cores.end(); 
                i++) { 
            i->second->EnableResourceTracking();
            //pPlatform->EnableResourceTracking();
        }


        fPTV = true;
        ptv_id = 0;

        fprintf(ptv_out, "PipeTrace Format Version 3.2\n");
        fprintf(ptv_out, "[Timestamp: Fri May 31 08:40:38 2002]\n");
        fprintf(ptv_out, "[Hash: 1024] [Key: 24]\n");
        fprintf(ptv_out, "[Format: ASCII]\n");
        fprintf(ptv_out, "BEGIN\n");
        fprintf(ptv_out, "0 ConfigRecord token token record\n");
        fprintf(ptv_out, "0 ConfigRecord forked_token forked token record\n");
        fprintf(ptv_out, "0 ConfigRecord resource_token forked token record\n");
        fprintf(ptv_out, "0 ConfigRecord system_token system token record\n");
        fprintf(ptv_out, "0 ConfigData TokenId Integer Token ID\n");
        fprintf(ptv_out, "0 ConfigData Inst_ID Integer Inst_ID\n");
        fprintf(ptv_out, "0 ConfigData SibId Integer SIB ID\n");
        fprintf(ptv_out, "0 ConfigData UopNum Integer Uop Num \n");
        fprintf(ptv_out, "0 ConfigData CoreId Integer Core ID\n");
        fprintf(ptv_out, "0 ConfigData LPID Integer Thread Number\n");
        fprintf(ptv_out, "0 ConfigData IP String Instruction Virtual Address\n");
        fprintf(ptv_out, "0 ConfigData Disassembly String disassembly \n");
        fprintf(ptv_out, "0 ConfigEvent Token_Fetch F 4 Token fetch\n");
        fprintf(ptv_out, "0 ConfigEvent Token_Retire R 6 Token retire\n");
        fprintf(ptv_out, "0 ConfigEvent Token_Wait W 2 Token Wait \n");
        fprintf(ptv_out, "0 ConfigEvent Token_In_Exec E 8 Token In Execute\n");
        fprintf(ptv_out, "0 ConfigEvent Token_After_Exec A 12 Token After Execute\n");
        fprintf(ptv_out, "0 ConfigEvent Token_Issued I 10 Token Issued\n");
        fprintf(ptv_out, "0 ConfigEvent PDEvent . 16 Pipe Display Event\n");
        fprintf(ptv_out, "0 ConfigData Event_Type String type of event \n");
        fprintf(ptv_out, "0 ConfigData Sources String sources \n");
        fprintf(ptv_out, "0 ConfigData Destinations String destinations\n");
        fprintf(ptv_out, "0 ConfigEvent IngressQueue I 2 System Ingress Queue\n");
        fprintf(ptv_out, "0 ConfigEvent EgressQueue E 4 System Egress Queue\n");
        fprintf(ptv_out, "0 ConfigEvent Router R 8 System Router\n");
        fprintf(ptv_out, "0 ConfigEvent SharedCacheHit H 12 Shared Cache Hit\n");
        fprintf(ptv_out, "0 ConfigEvent SharedCacheMiss M 10 Shared Cache Miss\n");
    
        for(map<const char*,unsigned int,ltstr>::iterator i=pipetrace_t::resCap.begin(); 
                i != pipetrace_t::resCap.end(); 
                i++) { 
            fprintf(ptv_out, "0 ConfigEvent %s r 6 Resource\n", i->first);
        }

    } else if(strcmp(argV[1], "off")==0) {
        if(fPTV) {
            SUICloseFileOrPipe(ptv_out, fPipe);
            fPTV = false;
            fprintf(dante_out, "ptv output disabled\n");
            // Traverse the list of resources we're tracking, and print out a Yes! xml file
            printYesXml();
        } else {
            fprintf(dante_err, "cannot turn off ptv output because ptv output is not currently enabled\n");
        }
    } else if(strcmp(argV[1], "pause")==0) {
        if(fPTV) {
            if(ptv_out == NULL) {
                fprintf(dante_err, "ERROR:  ptv can't pause, no file is open!\n");
            }
            fprintf(dante_out, "ptv output paused\n");
            fPTV = false;
        } else {
            if(ptv_out == NULL) {
                fprintf(dante_err, "ERROR:  ptv can't unpause, no file is open!\n");
            }
            fprintf(dante_out, "ptv output unpaused\n");
            fPTV = true;
        }
    } else {
        fprintf(dante_err, "WARNING:  unknown arguments to ptv command\n");
    }

    return 0;
}



