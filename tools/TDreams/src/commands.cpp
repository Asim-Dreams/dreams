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
#include "commands.h"
#include "selector_factory.h"
#include "command_factory.h"
#include "extractors.h"
#include <asim/dralClient.h>
#include <asim/dralListenerConverter.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <functional>
#include <math.h>

using namespace std;

namespace {

template <class T>
COMMAND* 
createCommand()
{
    return new T;
}

template <class T>
SET_COMMAND*
createSetCommand()
{
    return new T;
}

SELECTOR*
getSelector(string selector_str)
{
    SELECTOR* selector;
    try {
        selector = THE_SELECTOR_FACTORY::Instance().CreateObject(selector_str);
    }
    catch (...) {
        selector = 0;
    }
    return selector;
}

vector<string>
splitLine(const string& str)
{
    vector<string> split;
    UINT32 idx = 0;
    string tmp = "";

    // Jump over initial spaces
    while (idx < str.size() && str[idx] <= 32)
        ++idx;

    while (idx < str.size())
    {
        if (str[idx] > 32)
        {
            tmp.append(1, str[idx]);
        }
        else
        {
            if (tmp.size()>0)
            {
                split.push_back(tmp);
                tmp = "";
            }
        }
        ++idx;
    }
    if (tmp!="")
    {
        split.push_back(tmp);
    }
    return split;
}

void 
showUsage()
{
    cerr << "Usage: lifetime events_file [batch_command_file]" << endl;
}

} // End anonymous namespace

bool DUMP_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("dump", createCommand<DUMP_COMMAND>);
const char* const DUMP_COMMAND::info_ = "dump SELECTOR [string] - dump information that matches selector and only stages where the name includes string";
const char* const DUMP_COMMAND::name_ = "dump";

DUMP_COMMAND::DUMP_COMMAND():
    selector_(NULL),
    substring_("")
{
}

DUMP_COMMAND::~DUMP_COMMAND()
{
    delete selector_;
}

void
DUMP_COMMAND::readConfig(std::string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "dump");

    bool input_error = true;

    if (input_line.size()>=2)
    {
        if (input_line.size()>2)
        {
            substring_ = input_line[2];
        }
        selector_ = getSelector(input_line[1]);
        if (selector_)
        {
            input_error = false;
        }
        else
        {
            substring_ = "";
        }
    }
    correct_ = !input_error;
}

void
DUMP_COMMAND::execute(LISTENER_CLASS* listener) const
{
    listener->dumpTransactions(*selector_, substring_);
}

bool DUMP_AVG_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("dumpavg", createCommand<DUMP_AVG_COMMAND>);
const char* const DUMP_AVG_COMMAND::info_ = "dumpavg SELECTOR #stage - dump stripchart of average time spent in #stage for selected requests"; 
const char* const DUMP_AVG_COMMAND::name_ = "dumpavg";

DUMP_AVG_COMMAND::DUMP_AVG_COMMAND():
    selector_(NULL),
    stage_number_(-1),
    substring_("")
{
}

DUMP_AVG_COMMAND::~DUMP_AVG_COMMAND()
{
    delete selector_;
}

void
DUMP_AVG_COMMAND::readConfig(std::string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "dumpavg");

    bool input_error = true;

    if (input_line.size()>=3)
    {
        selector_ = getSelector(input_line[1]);
        if (selector_)
        {
            char* end;
            stage_number_ = strtol(input_line[2].c_str(),&end,0);

            if (*end == '\0')
            {
                input_error = false;
            }
            else
            {
                substring_ = input_line[2];
                stage_number_ = -1;
                input_error = false;
            }
        }
    }
    correct_ = !input_error;
}

void
DUMP_AVG_COMMAND::execute(LISTENER_CLASS* listener) const
{
    if (stage_number_ == -1)
    {
        if (substring_.find("STAGE_SET") != string::npos)
        {
            get_stage_set_time extractor(substring_);
            extractor.readConfig(substring_);
            listener->dumpHistogram(*selector_, extractor);
        }
        else if (substring_.find("INTERVAL") != string::npos)
        {
            get_interval_time extractor;
            extractor.readConfig(substring_);
            listener->dumpHistogram(*selector_, extractor);
        }
        else
        {
            listener->dumpHistogram(*selector_, get_stage_set_time(substring_));
        }

    }
    else
    {
        listener->dumpHistogram(*selector_, get_stage_time(stage_number_));
    }
}

bool DUMP_ITEM_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("dumpitem", createCommand<DUMP_ITEM_COMMAND>);
const char* const DUMP_ITEM_COMMAND::info_ = "dumpitem item_id - dump all information of transaction identified by item_id";
const char* const DUMP_ITEM_COMMAND::name_ = "dumpitem";

DUMP_ITEM_COMMAND::DUMP_ITEM_COMMAND()
{
}

DUMP_ITEM_COMMAND::~DUMP_ITEM_COMMAND()
{
}

void
DUMP_ITEM_COMMAND::readConfig(string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "dumpitem");

    bool input_error = true;

    if (input_line.size() == 2)
    {
        char* end;
        item_id_ = strtol(input_line[1].c_str(),&end,0);
        if (*end == '\0')
        {
            input_error = false;
        }
    }
    correct_ = !input_error;
}

void
DUMP_ITEM_COMMAND::execute(LISTENER_CLASS* listener) const
{
    listener->dumpItem(item_id_);
}

bool DUMP_SVG_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("dumpsvg", createCommand<DUMP_SVG_COMMAND>);
const char* const DUMP_SVG_COMMAND::info_ = "dumpsvg transaction_id - dump an SVG image of transaction identified by transaction_id";
const char* const DUMP_SVG_COMMAND::name_ = "dumpsvg";

DUMP_SVG_COMMAND::DUMP_SVG_COMMAND()
{
}

DUMP_SVG_COMMAND::~DUMP_SVG_COMMAND()
{
}

bool
strToUINT64(string line, UINT64* value)
{
// There is a function in stdlib to do this, stupid. It's much better as it handles octal and hex
/*
    UINT64 value_tmp = 0;

    for (string::const_iterator it = line.begin(); it != line.end(); ++it)
    {
        value_tmp *= 10;
        char current_digit = (*it - '0');
        if (current_digit < 10)
        {
            // Ok, it's a digit
            value_tmp += current_digit;
        }
        else
        {
            return false;
        }
    }
    *value = value_tmp;
    return true;
*/
    const char* line_copy = strdup(line.c_str());
    char * endptr;
    UINT64 result = strtoull(line_copy, &endptr, 0);
    *value = result;
    if (*endptr == '\0')
    {
        return true;
    }
    else
    {   
        return false;
    }
}

void
DUMP_SVG_COMMAND::readConfig(string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "dumpsvg");

    bool input_error = true;

    if (input_line.size() >= 3)
    {
        label_tag_ = input_line[1];
        TransactionId transaction_id;
        bool correct_ids = true;
        for (UINT32 i = 2; i < input_line.size(); ++i)
        {
            if (strToUINT64(input_line[i], &transaction_id))
            {
                transaction_ids_.push_back(transaction_id);
            }
            else
            {
                correct_ids = false;
            }
        }
        input_error = false;
    }
    correct_ = !input_error;
}

void
DUMP_SVG_COMMAND::execute(LISTENER_CLASS* listener) const
{
    cout << "TR_IDS: " << transaction_ids_.size() << endl;
    listener->dumpSVG(transaction_ids_, label_tag_);
}


bool DUMP_TRANSACTION_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("dumptran", createCommand<DUMP_TRANSACTION_COMMAND>);
const char* const DUMP_TRANSACTION_COMMAND::info_ = "dumptran transaction_id - dump all information of transaction identified by transaction_id";
const char* const DUMP_TRANSACTION_COMMAND::name_ = "dumptran";

DUMP_TRANSACTION_COMMAND::DUMP_TRANSACTION_COMMAND()
{
}

DUMP_TRANSACTION_COMMAND::~DUMP_TRANSACTION_COMMAND()
{
}

void
DUMP_TRANSACTION_COMMAND::readConfig(string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "dumptran");

    bool input_error = true;

    if (input_line.size() == 2)
    {
        if (strToUINT64(input_line[1], &transaction_id_))
        {
            input_error = false;
        }
    }
    correct_ = !input_error;
}

void
DUMP_TRANSACTION_COMMAND::execute(LISTENER_CLASS* listener) const
{
    listener->dumpTransaction(transaction_id_);
}

bool DUMP_HIST_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("dumphist", createCommand<DUMP_HIST_COMMAND>);
const char* const DUMP_HIST_COMMAND::info_ = "dumphist SELECTOR - dump stripchart of selected requests";
const char* const DUMP_HIST_COMMAND::name_ = "dumphist";

DUMP_HIST_COMMAND::DUMP_HIST_COMMAND()
{
}

DUMP_HIST_COMMAND::~DUMP_HIST_COMMAND()
{
    delete selector_;
}

void
DUMP_HIST_COMMAND::execute(LISTENER_CLASS* listener) const
{
    listener->dumpHistogram(*selector_);
}

void 
DUMP_HIST_COMMAND::readConfig(string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "dumphist");

    bool input_error = true;

    if (input_line.size()>=2)
    {
        selector_ = getSelector(input_line[1]);
        if (selector_)
        {
            input_error = false;
        }
    }
    correct_ = !input_error;
}

bool DUMP_WORSE_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("dumpworse", createCommand<DUMP_WORSE_COMMAND>);
const char* const DUMP_WORSE_COMMAND::info_ = "dumpworse SELECTOR #items [#stage/stage_name]- dump the #items with longer lifetime. If an #stage is specified count only that stage."; 
const char* const DUMP_WORSE_COMMAND::name_ = "dumpworse";

DUMP_WORSE_COMMAND::DUMP_WORSE_COMMAND():
    selector_(0),
    num_items_(100),
    stage_num_(-1),
    substring_("")
{
}

DUMP_WORSE_COMMAND::~DUMP_WORSE_COMMAND()
{
    delete selector_;
}

void 
DUMP_WORSE_COMMAND::readConfig(string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "dumpworse");

    bool input_error = true;

    if (input_line.size() >= 2)
    {
        selector_ = getSelector(input_line[1]);
        if (selector_)
        {
            if (input_line.size() >= 3)
            {
                char* end;
                num_items_ = strtol(input_line[2].c_str(),&end,0);
                if (*end == '\0')
                {
                    if (input_line.size() == 4)
                    {
                        end = 0;
                        stage_num_ = strtol(input_line[3].c_str(),&end,0);
                        if (*end == '\0')
                        {
                            input_error = false;
                        }
                        else
                        {
                            substring_ = input_line[3];
                            stage_num_ = -1;
                            input_error = false;
                        }
                    }
                    else
                    {
                        input_error = false;
                    }
                }
                else
                {
                    num_items_ = 100;
                }
            }
            else
            {
                input_error = false;
            }
        }
    }
    correct_ = !input_error;
}

void
DUMP_WORSE_COMMAND::execute(LISTENER_CLASS* listener) const
{
    if (stage_num_ == -1)
    {
        if (substring_ != "")
        {
            listener->dumpWorse(num_items_, substring_, *selector_);
        }
        else
        {
            get_total_time extractor;
            general_comparator<get_total_time, std::greater<UINT32> > comparator(extractor, std::greater<UINT32>());
//            listener->dumpWorse(num_items_, *selector_);
            listener->dumpOrdered(num_items_, *selector_, comparator);

        }
    }
    else
    {
        listener->dumpWorse(num_items_, stage_num_, *selector_);
    }
}

bool DUMP_BEST_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("dumpbest", createCommand<DUMP_BEST_COMMAND>);
const char* const DUMP_BEST_COMMAND::info_ = "dumpbest SELECTOR #items [#stage/stage_name]- dump the #items with shorter lifetime. If an #stage is specified count only that stage."; 
const char* const DUMP_BEST_COMMAND::name_ = "dumpbest";

DUMP_BEST_COMMAND::DUMP_BEST_COMMAND():
    selector_(0),
    num_items_(100),
    stage_num_(-1),
    substring_("")
{
}

DUMP_BEST_COMMAND::~DUMP_BEST_COMMAND()
{
    delete selector_;
}

void 
DUMP_BEST_COMMAND::readConfig(string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "dumpbest");

    bool input_error = true;

    if (input_line.size() >= 2)
    {
        selector_ = getSelector(input_line[1]);
        if (selector_)
        {
            if (input_line.size() >= 3)
            {
                char* end;
                num_items_ = strtol(input_line[2].c_str(),&end,0);
                if (*end == '\0')
                {
                    if (input_line.size() == 4)
                    {
                        end = 0;
                        stage_num_ = strtol(input_line[3].c_str(),&end,0);
                        if (*end == '\0')
                        {
                            input_error = false;
                        }
                        else
                        {
                            substring_ = input_line[3];
                            stage_num_ = -1;
                            input_error = false;
                        }
                    }
                    else
                    {
                        input_error = false;
                    }
                }
                else
                {
                    num_items_ = 100;
                }
            }
            else
            {
                input_error = false;
            }
        }
    }
    correct_ = !input_error;
}

void
DUMP_BEST_COMMAND::execute(LISTENER_CLASS* listener) const
{
    if (stage_num_ == -1)
    {
        if (substring_ != "")
        {
            listener->dumpBest(num_items_, substring_, *selector_);
        }
        else
        {
            listener->dumpBest(num_items_, *selector_);
        }
    }
    else
    {
        listener->dumpBest(num_items_, stage_num_, *selector_);
    }
}

bool DUMP_CLOSER_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("dumpcloser", createCommand<DUMP_CLOSER_COMMAND>);
const char* const DUMP_CLOSER_COMMAND::info_ = "dumpcloser SELECTOR ref_value #items [#stage/stage_name]- dump the #items with lifetime closer to the ref_value. If an #stage is specified count only that stage."; 
const char* const DUMP_CLOSER_COMMAND::name_ = "dumpcloser";

DUMP_CLOSER_COMMAND::DUMP_CLOSER_COMMAND():
    selector_(0),
    num_items_(100),
    stage_num_(-1),
    substring_("")
{
}

template <class T>
struct closer_comparison : public binary_function<T, T, bool>
{
    explicit closer_comparison(T ref_val):
        ref_val_(ref_val)
    {
    };

    bool operator()(const T& one, const T& other)
    {
        T diff_one = static_cast<T>(fabs(static_cast<double>(one) - static_cast<double>(ref_val_)));
        T diff_other = static_cast<T>(fabs(static_cast<double>(other) - static_cast<double>(ref_val_)));
        if (diff_one < diff_other)
        {
            return true;
        }
        else 
        {
            return false;
        }
    }

    T ref_val_;
};

DUMP_CLOSER_COMMAND::~DUMP_CLOSER_COMMAND()
{
    delete selector_;
}

void 
DUMP_CLOSER_COMMAND::readConfig(string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "dumpcloser");

    bool input_error = true;

    if (input_line.size() >= 3)
    {
        selector_ = getSelector(input_line[1]);
        if (selector_)
        {
            char* end;
            ref_val_ = strtol(input_line[2].c_str(), &end, 0);
            if (*end == '\0')
            {
                if (input_line.size() >= 4)
                {
                    end = NULL;
                    num_items_ = strtol(input_line[3].c_str(),&end,0);
                    if (*end == '\0')
                    {
                        if (input_line.size() == 5)
                        {
                            end = NULL;
                            stage_num_ = strtol(input_line[4].c_str(),&end,0);
                            if (*end == '\0')
                            {
                                input_error = false;
                            }
                            else
                            {
                                substring_ = input_line[4];
                                stage_num_ = -1;
                                input_error = false;
                            }
                        }
                        else
                        {
                            input_error = false;
                        }
                    }
                    else
                    {
                        num_items_ = 100;
                    }
                }
                else
                {
                    input_error = false;
                }
            }
        }
    }
    correct_ = !input_error;
}

void
DUMP_CLOSER_COMMAND::execute(LISTENER_CLASS* listener) const
{
    if (stage_num_ == -1)
    {
        if (substring_ != "")
        {
            get_stage_set_time extractor(substring_);
            general_comparator<get_stage_set_time, closer_comparison<UINT32> > comparator(extractor, closer_comparison<UINT32>(ref_val_));
            listener->dumpOrdered(num_items_, *selector_, comparator);
        }
        else
        {
            get_total_time extractor;
            general_comparator<get_total_time, closer_comparison<UINT32> > comparator(extractor, closer_comparison<UINT32>(ref_val_));
            listener->dumpOrdered(num_items_, *selector_, comparator);
        }
    }
}



bool DUMP_TOTALS_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("dumptotals", createCommand<DUMP_TOTALS_COMMAND>);
const char* const DUMP_TOTALS_COMMAND::info_ = "dumptotals SELECTOR - dump the total lifetime of selected transactions"; 
const char* const DUMP_TOTALS_COMMAND::name_ = "dumptotals";

DUMP_TOTALS_COMMAND::DUMP_TOTALS_COMMAND():
    selector_(0),
    stage_("")
{
}

DUMP_TOTALS_COMMAND::~DUMP_TOTALS_COMMAND()
{
    delete selector_;
}
        
void 
DUMP_TOTALS_COMMAND::readConfig(string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "dumptotals");

    bool input_error = true;

    if (input_line.size()>=2)
    {
        selector_ = getSelector(input_line[1]);
        if (selector_)
        {
            if (input_line.size() == 3)
            {
                stage_ = input_line[2];
            }
            input_error = false;
        }
    }
    correct_ = !input_error;
}

void
DUMP_TOTALS_COMMAND::execute(LISTENER_CLASS* listener) const
{
    if (stage_ == "")
    {
        listener->dumpTotalTimes(*selector_);
    }
    else
    {
        if (stage_.find("STAGE_SET") != string::npos)
        {
            get_stage_set_time extractor(stage_);
            extractor.readConfig(stage_);
            listener->dumpTotalTimes(*selector_, extractor);
        }
        else if (stage_.find("INTERVAL") != string::npos)
        {
            get_interval_time extractor;
            extractor.readConfig(stage_);
            listener->dumpTotalTimes(*selector_, extractor);
        }
        else
        {
            listener->dumpTotalTimes(*selector_, get_stage_set_time(stage_));
        }
    }
}

bool DUMP_MEAN_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("dumpmean", createCommand<DUMP_MEAN_COMMAND>);
const char* const DUMP_MEAN_COMMAND::info_ = "dumpmean SELECTOR #stage/stage_name - dump mean time spent on the stage by selected transactions";
const char* const DUMP_MEAN_COMMAND::name_ = "dumpmean";

DUMP_MEAN_COMMAND::DUMP_MEAN_COMMAND():
    selector_(0),
    stage_num_(-1),
    substring_("")
{
}

DUMP_MEAN_COMMAND::~DUMP_MEAN_COMMAND()
{ 
}

void
DUMP_MEAN_COMMAND::readConfig(string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "dumpmean");

    bool input_error = false;

    if (input_line.size() == 3)
    {
        selector_ = getSelector(input_line[1]);
        if (selector_)
        {
            char* end;
            stage_num_ = strtol(input_line[2].c_str(),&end,0);
            if (*end != '\0')
            {
                stage_num_ = -1;
                substring_ = input_line[2];
            }
        }
        else
        {
            input_error = true;
        }
    }
    else
    {
        input_error = true;
    }

    correct_ = !input_error;
}

void 
DUMP_MEAN_COMMAND::execute(LISTENER_CLASS* listener) const
{
    if (stage_num_ == -1)
    {
        listener->dumpMean(*selector_, get_stage_set_time(substring_));
    }
    else
    {
        listener->dumpMean(*selector_, get_stage_time(stage_num_));
    }
}

DUMP_ADHOC_COMMAND::DUMP_ADHOC_COMMAND()
{
}

DUMP_ADHOC_COMMAND::~DUMP_ADHOC_COMMAND()
{
}

void
DUMP_ADHOC_COMMAND::readConfig(string config)
{
}

void
DUMP_ADHOC_COMMAND::execute(LISTENER_CLASS* listener) const
{
    get_interval_time extractor;
    FIRST_STAGE first;
    LAST_STAGE last;
    first.substring_ = "TRACKER";
    last.substring_ = "IMT";
    extractor.start_ = &first;
    extractor.end_ = &last;
    
    listener->dumpTotalTimes(*getSelector("reads"), extractor);
}

bool DUMP_ADHOC_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("dumpadhoc", createCommand<DUMP_ADHOC_COMMAND>);
const char* const DUMP_ADHOC_COMMAND::info_ = "dumpadhoc - dump time between TRK and IMT stages";

bool SET_OUTPUT_FILE_COMMAND::registered_ = THE_SET_COMMAND_FACTORY::Instance().Register("output", createSetCommand<SET_OUTPUT_FILE_COMMAND>);
const char* const SET_OUTPUT_FILE_COMMAND::info_ = "set output filename - redirect output to file";
const char* const SET_OUTPUT_FILE_COMMAND::name_ = "set output";

SET_OUTPUT_FILE_COMMAND::SET_OUTPUT_FILE_COMMAND():
    filename_("")
{
}

SET_OUTPUT_FILE_COMMAND::~SET_OUTPUT_FILE_COMMAND()
{
}
        
void 
SET_OUTPUT_FILE_COMMAND::readConfig(string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "set");
    assert(input_line[1] == "output");

    bool input_error = true;

    if (input_line.size() == 3)
    {
        filename_ = input_line[2];
    }
    input_error = false;

    correct_ = !input_error;
}

void
SET_OUTPUT_FILE_COMMAND::execute(LISTENER_CLASS* listener) const
{
    bool ok = listener->setOutputFile(filename_);
    if (!ok)
    {
        cout << "Unable to open file " << filename_ << " for writing" << endl;
    }
    else
    {
        cout << "Using " << (filename_ != "" ? filename_ : "STDOUT") << " as output" << endl;
    }
}

bool SET_TEXT_MODE_COMMAND::registered_ = THE_SET_COMMAND_FACTORY::Instance().Register("textmode", createSetCommand<SET_TEXT_MODE_COMMAND>);
const char* const SET_TEXT_MODE_COMMAND::info_ = "textmode [normal|simple|columns] - set output modality"; 
const char* const SET_TEXT_MODE_COMMAND::name_ = "textmode";

SET_TEXT_MODE_COMMAND::SET_TEXT_MODE_COMMAND()
{
}

SET_TEXT_MODE_COMMAND::~SET_TEXT_MODE_COMMAND()
{
}

void 
SET_TEXT_MODE_COMMAND::readConfig(string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "set");
    assert(input_line[1] == "textmode");

    bool input_error = true;

    if (input_line.size() == 3)
    {
        if (input_line[2] == "normal")
        {
            textmode_ = LISTENER_CLASS::OM_NORMAL;
            input_error = false;
        }
        else if (input_line[2] == "simple")
        {
            textmode_ = LISTENER_CLASS::OM_SIMPLE;
            input_error = false;
        }
        else if (input_line[2] == "columns")
        {
            textmode_ = LISTENER_CLASS::OM_COLUMNS;
            input_error = false;
        }
    }

    correct_ = !input_error;
}

void 
SET_TEXT_MODE_COMMAND::execute(LISTENER_CLASS* listener) const
{
    listener->setOutputMode(textmode_);
}

bool EXIT_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("exit", createCommand<EXIT_COMMAND>) &&
                                 THE_COMMAND_FACTORY::Instance().Register("q", createCommand<EXIT_COMMAND>) && 
                                 THE_COMMAND_FACTORY::Instance().Register("quit", createCommand<EXIT_COMMAND>);

const char* const EXIT_COMMAND::info_ = "exit/quit - quit program"; 
const char* const EXIT_COMMAND::name_ = "exit";

EXIT_COMMAND::EXIT_COMMAND()
{
}

EXIT_COMMAND::~EXIT_COMMAND()
{
}

void
EXIT_COMMAND::readConfig(string config)
{
    vector<string> input_line = splitLine(config);
    assert(input_line[0] == "exit" || input_line[0] == "quit"|| input_line[0] == "q");

    correct_ = true;
}

void
EXIT_COMMAND::execute(LISTENER_CLASS* listener) const
{
    cout << "Have a nice day!" << endl;
    exit(0);
}

bool LOAD_COMMAND::registered_ = THE_COMMAND_FACTORY::Instance().Register("load", createCommand<LOAD_COMMAND>);
const char* const LOAD_COMMAND::info_ = "load filename - load a new events file";
const char* const LOAD_COMMAND::name_ = "load";

LOAD_COMMAND::LOAD_COMMAND()
{
}

LOAD_COMMAND::~LOAD_COMMAND()
{
}

void
LOAD_COMMAND::readConfig(string config)
{
    bool input_error = true;
    vector<string> input_line = splitLine(config);

    assert(input_line[0] == "exit" || input_line[0] == "quit");

    if (input_line.size()>1)
    {
        events_file_ = input_line[1];
        input_error = false;
    }

    correct_ = !input_error;
}



void
LOAD_COMMAND::execute(LISTENER_CLASS* listener) const
{
    int fd_trace = open(events_file_.c_str(), O_RDONLY);

    if (fd_trace == -1)
    {
        cerr << "Error opening events file " << events_file_ << endl;
        showUsage();
        exit(-1);
    }

    listener->clearEvents();

    DRAL_LISTENER_CONVERTER_CLASS converter(listener);
    DRAL_CLIENT_CLASS client(fd_trace, &converter, 1024*64);
    int result;
    while ((result = client.ProcessNextEvent(true, 1))>0)
    {}

    if (result == -1)
    {
        cout << "An error occurred" << endl;
    }
    close (fd_trace);
}

