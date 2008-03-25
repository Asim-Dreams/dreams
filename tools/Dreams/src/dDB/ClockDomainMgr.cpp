// ==================================================
// Copyright (C) 2004-2006 Intel Corporation
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 

/**
  * @file ClockDomainMgr.cpp
  */

// Dreams includes.
#include "dDB/ClockDomainMgr.h"
#include "dDB/dralgraph/DralGraph.h"

/*
 * Default constructor.
 *
 * @return the new object.
 */
ClockDomainEntry::ClockDomainEntry()
{
    id = (UINT16) -1;
    freq = 0;
    inv_freq = 0;
    divisions = 1;
    skew = 0;
    lcm_skew = 0;
    name = "";
    internal_clock = (UINT16) -1;
}

/*
 * Just copies the parameters.
 *
 * @return the new object.
 */
ClockDomainEntry::ClockDomainEntry(UINT16 _id, UINT64 _freq, UINT16 _divisions, UINT16 _skew, QString _name, UINT16 _internal_clock)
{
    id = _id;
    freq = _freq;
    inv_freq = 1 / freq;
    divisions = _divisions;
    skew = _skew;
    lcm_skew = 0;
    name = _name;
    internal_clock = _internal_clock;
}

/*
 * Nothing done.
 *
 * @return destroys the object.
 */
ClockDomainEntry::~ClockDomainEntry()
{
}

/*
 * Returns the frequency of a clock entry in the requested units.
 *
 * @return the formatted frequency.
 */
QString
ClockDomainEntry::getFormattedFreq(ClockDomainFreqType type) const
{
    QString res; ///< Formatted string result.
    switch(type)
    {
        case CLOCK_GHZ:
            res = QString::number((double) freq / 100.0, 'f', 1) + " GHz";
            break;

        case CLOCK_MHZ:
            res = QString::number(freq * 10) + " MHz";
            break;

        default:
            res = "";
            break;
    }
    return res;
}

/*
 * Returns a description of the clock.
 *
 * @return the description.
 */
QString
ClockDomainEntry::toString() const
{
    QString res;

    res = getFormattedFreq(CLOCK_GHZ);
    res += ", " + QString::number(skew) + "% Skew, ";
    if(getDivisions() == 1)
    {
        res += QString::number(1) + " Clock Division";
    }
    else
    {
        res += QString::number(getDivisions()) + " Clock Divisions";
    }
    return res;
}

/*
 * Dumps the content of the clock domain.
 *
 * @return void.
 */
void
ClockDomainEntry::dump() const
{
    printf("This: 0x%X, Id: %i, Freq: %llu, Divisions: %i, Skew: %i, LCMSkew: %i, Name: %s, InternalClock: %i, LCMRatio: %i, FastestRatio: %f\n", (INT32) this, getId(), getFreq(), getDivisions(), getSkew(), getLCMSkew(), getName().latin1(), getInternalClock(), getLCMRatio(), getFastestRatio());
}

/*
 * Creator of the class.
 *
 * @return the new object.
 */
ClockDomainMgr::ClockDomainMgr(DralGraph * _dbgraph)
{
    dbgraph = _dbgraph;
    LCM = 0;
    num_internal_clocks = 0;
}

/*
 * Deletes all the allocated clocks.
 *
 * @return destroys the object.
 */
ClockDomainMgr::~ClockDomainMgr()
{
    // Deletes all the allocated clocks.
    for(UINT32 i = 0; i < clocks.size(); i++)
    {
        delete clocks[i];
    }
}

/*
 * Adds a new clock domain.
 *
 * @return true if the id is not repeated.
 */
bool
ClockDomainMgr::addNewClockDomain(UINT16 id, UINT64 freq, UINT16 divisions, UINT16 skew, const char * name, QString & error)
{
    // Checks that the same clock id is not already defined.
    if(id < clocks.size())
    {
        error = "the clock id " + QString::number(id) + " is defined twice in the trace";
        return false;
    }

    // Checks sequential creation.
    if(id > clocks.size())
    {
        error = "the clock id " + QString::number(id) + " is defined before the clock " + QString::number(clocks.size());
        return false;
    }

    // Checks the number of divisions of this clock.
    if((divisions != 1) && (divisions != 2))
    {
        error = "the clock id " + QString::number(id) + " is defined with " + QString::number(divisions) + " divisions. Only 1 and 2 divisions are supported";
        return false;
    }

    if(freq == 0)
    {
        error = "the freq is defined as 0.0 GHz";
        return false;
    }
    INT32 internal_id = -1; ///< Used to map the external clock domain with an internal clock domain.

    // Tries to find the mapping with an internal clock domain.
    for(UINT32 i = 0; i < num_internal_clocks; i++)
    {
        // Checks if the entry can be mapped to the actual internal clock.
        if((int_freq[i] == freq) && (int_skew[i] == skew))
        {
            internal_id = i;
            break;
        }
    }

    // If no internal clock is found, tries to alloc a new one.
    if(internal_id == -1)
    {
        // Checks that the internal limit is not reached.
        if(num_internal_clocks == (MAX_CLOCK_ID + 1))
        {
            error = "the limit of clock domains is too big due Dreams internal storage limitations";
            return false;
        }

        // Allocates a new internal clock.
        internal_id = num_internal_clocks++;
        int_to_ext[internal_id] = id;
        int_freq[internal_id] = freq;
        int_skew[internal_id] = skew;

        //printf("Allocating new internal clock %i with freq %llu and skew %i\n", internal_id, freq, skew);
    }

    ClockDomainEntry * entry; ///< The clock domain entry that will be inserted.

    // Creates the clock domain.
    entry = new ClockDomainEntry(id, freq, divisions, skew, name, internal_id);

    clocks.push_back(entry);
    freq_set.insert(entry);

    //printf("ClockID %i with freq %llu and skew %i is mapped to internal clock id %i\n", id, freq, skew, internal_id);

    return true;
}

/*
 * Computes the ratios of all the clock entries (respect the
 * fastest one).
 *
 * @return void.
 */
void
ClockDomainMgr::computeRatiosAndLCM()
{
    // If none clock is defined (no multi clock trace).
    if(freq_set.size() == 0)
    {
        QString error; ///< Error string.

        // A default clock is added.
        addNewClockDomain(0, 100, 1, 0, "default clock", error);
        dbgraph->setDefaultClock();
    }

    // Computes the LCM using divisions.
    LCM = 1;
    for(UINT32 i = 0; i < clocks.size(); i++)
    {
        LCM = computeLCM(LCM, clocks[i]->getFreq());
    }

    UINT64 base_freq; ///< Contains the frequency of the fastest clock.
    base_freq = (* freq_set.begin())->getFreq();

    // Computes all the relative frequency ratios.
    for(UINT32 i = 0; i < clocks.size(); i++)
    {
        clocks[i]->setLCMRatio(LCM);
        clocks[i]->setFastestRatio(base_freq);
        clocks[i]->setLCMSkew();
    }

    LCM = LCM << DIVISION_SHIFT;
/*    printf("PhasedLCM: %llu\n", LCM);

    // Dump.
    printf("Dumping clocks:\n");
    for(UINT32 i = 0; i < clocks.size(); i++)
    {
        clocks[i]->dump();
    }
    printf("\n");*/
}

/*
 * Computes the LCM for the 2 given integers.
 *
 * @return the LCM.
 */
UINT64
ClockDomainMgr::computeLCM(UINT64 a, UINT64 b)
{
    UINT64 result;

    result = b;
    while(result % a != 0)
    {
        result += b;
    }
    return result;
}

/*
 * Creator of this class. Sets the default values.
 *
 * @return new object.
 */
CYCLE::CYCLE()
{
    clock = NULL;
    cycle = -1;
    division = 0;
}

/*
 * Creator of this class. Partially initialized.
 *
 * @return new object.
 */
CYCLE::CYCLE(const ClockDomainEntry * _clock)
{
    clock = _clock;
}

/*
 * Creator of this class. Copies the parameters.
 *
 * @return new object.
 */
CYCLE::CYCLE(const ClockDomainEntry * _clock, INT32 _cycle, UINT16 _division)
{
    clock = _clock;
    cycle = _cycle;
    division = _division;
}

/*
 * Returns a string with the cycle.
 *
 * @return the cycle.
 */
QString
CYCLE::toString(UINT8 mask) const
{
    QString ret = "";

    if(mask & CYCLE_MASK_CYC)
    {
        ret += "Cycle ";
    }
    if(mask & CYCLE_MASK_NUM)
    {
        ret += QString::number(cycle) + " ";
    }
    if(mask & CYCLE_MASK_DIV)
    {
        if(division == 0)
        {
            ret += "High ";
        }
        else
        {
            ret += "Low ";
        }
    }
    if(mask & CYCLE_MASK_CLK)
    {
        ret += "@ " + clock->getFormattedFreq(CLOCK_GHZ);
    }
    return ret;
}
