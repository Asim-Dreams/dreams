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
#ifndef COMMANDS_H
#define COMMANDS_H

#include "listener.h"
#include "lifetime.h"
#include <string>
#include <vector>

class COMMAND
{
  public:
    virtual ~COMMAND(){};
    virtual void readConfig(std::string config) = 0;
    virtual void execute(LISTENER_CLASS* listener) const= 0;

    bool isCorrect() const
    {
        return correct_;
    }

  protected:
    bool correct_;
    std::string command_line_;
};


class DUMP_COMMAND : public COMMAND
{
  public:
    DUMP_COMMAND();
    virtual ~DUMP_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;

  private:
    SELECTOR* selector_;
    std::string substring_;
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

class DUMP_AVG_COMMAND : public COMMAND
{
  public:
    DUMP_AVG_COMMAND();
    virtual ~DUMP_AVG_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;

  private:
    SELECTOR* selector_;
    INT32 stage_number_;
    std::string substring_;
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

class DUMP_ITEM_COMMAND : public COMMAND
{
  public:
    DUMP_ITEM_COMMAND();
    virtual ~DUMP_ITEM_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;

  private:
    UINT32 item_id_;
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

class DUMP_SVG_COMMAND : public COMMAND
{
  public:
    DUMP_SVG_COMMAND();
    virtual ~DUMP_SVG_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;

  private:
    typedef LIFETIME_CLASS::TransactionId TransactionId;
    std::vector<TransactionId> transaction_ids_;
    std::string label_tag_;
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};



class DUMP_TRANSACTION_COMMAND : public COMMAND
{
  public:
    DUMP_TRANSACTION_COMMAND();
    virtual ~DUMP_TRANSACTION_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;

  private:
    typedef LIFETIME_CLASS::TransactionId TransactionId;
    TransactionId transaction_id_;
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

class DUMP_HIST_COMMAND : public COMMAND
{
  public:
    DUMP_HIST_COMMAND();
    virtual ~DUMP_HIST_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;

  private:
    SELECTOR* selector_;
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

class DUMP_WORSE_COMMAND : public COMMAND
{
  public:
    DUMP_WORSE_COMMAND();
    virtual ~DUMP_WORSE_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;
  
  private:
    SELECTOR* selector_;
    UINT32 num_items_;
    INT32 stage_num_;
    std::string substring_;
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

class DUMP_BEST_COMMAND : public COMMAND
{
  public:
    DUMP_BEST_COMMAND();
    virtual ~DUMP_BEST_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;
  
  private:
    SELECTOR* selector_;
    UINT32 num_items_;
    INT32 stage_num_;
    std::string substring_;
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

class DUMP_CLOSER_COMMAND : public COMMAND
{
  public:
    DUMP_CLOSER_COMMAND();
    virtual ~DUMP_CLOSER_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;
  
  private:
    SELECTOR* selector_;
    UINT32 ref_val_;
    UINT32 num_items_;
    INT32 stage_num_;
    std::string substring_;
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

class DUMP_TOTALS_COMMAND : public COMMAND
{
  public:
    DUMP_TOTALS_COMMAND();
    virtual ~DUMP_TOTALS_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;
  
  private:
    SELECTOR* selector_;
    std::string stage_;
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

class DUMP_MEAN_COMMAND : public COMMAND
{
  public:
    DUMP_MEAN_COMMAND();
    virtual ~DUMP_MEAN_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;

  private:
    SELECTOR* selector_;
    INT32 stage_num_;
    std::string substring_;

    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

class DUMP_ADHOC_COMMAND : public COMMAND
{
  public:
    DUMP_ADHOC_COMMAND();
    virtual ~DUMP_ADHOC_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;

  private:
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};



class SET_COMMAND: public COMMAND
{
};

class SET_OUTPUT_FILE_COMMAND : public SET_COMMAND
{
  public:
    SET_OUTPUT_FILE_COMMAND();
    virtual ~SET_OUTPUT_FILE_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;
  
  private:
    std::string filename_;
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

class SET_TEXT_MODE_COMMAND : public SET_COMMAND
{
  public:
    SET_TEXT_MODE_COMMAND();
    virtual ~SET_TEXT_MODE_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;

  private:
    LISTENER_CLASS::OUTPUT_MODE textmode_;
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

class EXIT_COMMAND : public COMMAND
{
  public:
    EXIT_COMMAND();
    virtual ~EXIT_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;

  private:
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

class LOAD_COMMAND : public COMMAND
{
  public:
    LOAD_COMMAND();
    virtual ~LOAD_COMMAND();
    virtual void readConfig(std::string config);
    virtual void execute(LISTENER_CLASS* listener) const;

  private:
    std::string events_file_;
    static bool registered_;
    static const char* const info_;
    static const char* const name_;
};

#endif
