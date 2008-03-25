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
  * @file ClockDomainMgr.h
  */

#ifndef _DRALDB_CLOCKDOMAINMGR_H_
#define _DRALDB_CLOCKDOMAINMGR_H_

// General includes.
#include <set>
#include <vector>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DRALTag.h"

// Qt includes.
#include <qstring.h>

// Class forwarding.
class DralGraph;
class DBListener;

// Defines.
#define MAX_CLOCK_ID 7                   ///< Maximum value of a clock id.
#define DIVISION_SHIFT 1                 ///< Bits shifted to support divisions.
#define BIGGEST_CYCLE (INT32) 2147483647 ///< Maximum cycle.

/*
 * @typedef enum ClockDomainFreqType
 * @brief
 * Different types of formatted output for the clock frequency.
 */
typedef enum ClockDomainFreqType
{
    CLOCK_MHZ, ///< MHz output.
    CLOCK_GHZ  ///< GHz output.
} ;

/*
 * @brief
 * Just an entry of clock id and frequency.
 *
 * @description
 * Implements the functions to access to the fields of the class.
 *
 * @version 0.1
 * @date started at 2004-10-25
 * @author Guillem Sole
 */
class ClockDomainEntry
{
    public:
        ClockDomainEntry();
        ClockDomainEntry(UINT16 _id, UINT64 _freq, UINT16 _divisions, UINT16 _skew, QString _name, UINT16 _internal_clock);
        ~ClockDomainEntry();

        inline UINT16 getId() const;
        inline UINT16 getInternalClock() const;
        inline UINT64 getFreq() const;
        inline UINT64 getInvFreq() const;
        inline UINT16 getDivisions() const;
        inline UINT16 getSkew() const;
        inline QString getName() const;
        inline UINT32 getLCMRatio() const;
        inline float getFastestRatio() const;

        QString getFormattedFreq(ClockDomainFreqType type) const;
        QString toString() const;

    private:
        inline UINT16 getLCMSkew() const;

        inline void setLCMRatio(UINT64 base_freq);
        inline void setFastestRatio(UINT64 base_freq);
        inline void setLCMSkew();
        void dump() const;

        friend class ClockDomainMgr;
        friend class CYCLE;
        friend class DBListener;

    private:
        UINT16 id;             ///< Id of the clock entry.
        UINT64 freq;           ///< Frequency of the entry.
        UINT64 inv_freq;       ///< Inverted frequency of the entry.
        UINT16 divisions;      ///< Number of divisions of the clock.
        UINT16 skew;           ///< Skew of the entry.
        UINT16 lcm_skew;       ///< Skew based on the number of divisions.
        QString name;          ///< Name of the entry.
        UINT16 internal_clock; ///< Internal clock associated. Used to collapse the number of clock ids.
        UINT32 lcm_ratio;      ///< Frequency ratio respect to the LCM.
        float fastest_ratio;   ///< Frequency ratio respect to the fastest entry.
} ;

/* @struct ClockDomainEntryFreqCmp
 * @brief
 * Used to sort clock domain entries.
 */
struct ClockDomainEntryFreqCmp
{
    bool operator()(const ClockDomainEntry * e1, const ClockDomainEntry * e2) const
    {
        return (e1->getFreq() > e2->getFreq());
    }
} ;

/* @struct ClockDomainFreqSet
 * @struct ClockDomainFreqIterator
 * @brief
 * Definition of sets and iterators to sort the clock entries by frequency.
 */
typedef multiset<ClockDomainEntry *, ClockDomainEntryFreqCmp> ClockDomainFreqSet;
typedef multiset<ClockDomainEntry *, ClockDomainEntryFreqCmp>::iterator ClockDomainFreqIterator;

/*
 * @brief
 * This class implements the access to the different clocks domains
 * defined.
 *
 * @description
 * 
 *
 * @version 0.1
 * @date started at 2004-10-25
 * @author Guillem Sole
 */
class ClockDomainMgr
{
    public:
        ClockDomainMgr(DralGraph * _dbgraph);
        ~ClockDomainMgr();

        bool addNewClockDomain(UINT16 id, UINT64 freq, UINT16 divisions, UINT16 skew, const char * name, QString & error);

        inline CLOCK_ID getNumClocks() const;
        inline const ClockDomainEntry * getClockDomain(UINT16 id) const;
        inline bool hasClockDomain(UINT16 id) const;
        inline UINT64 getLCM() const;

        inline ClockDomainFreqIterator getFreqIterator() const;

        inline UINT16 internalToExternalClock(UINT16 clockId) const;

    private:
        void computeRatiosAndLCM();

        UINT64 computeLCM(UINT64 a, UINT64 b);

        friend class DBListener;

    private:
        ClockDomainFreqSet freq_set;         ///< Set of clock entries sorted by frequency.
        vector<ClockDomainEntry *> clocks;   ///< Vector of clocks.
        UINT64 LCM;                          ///< LCM of all the frequencies.
        UINT16 int_to_ext[MAX_CLOCK_ID + 1]; ///< Converts an internal clock id to an external (trace defined) clock id.
        UINT64 int_freq[MAX_CLOCK_ID + 1];   ///< Frequency of the internal clocks.
        INT32 int_skew[MAX_CLOCK_ID + 1];    ///< Skew of the internal clocks.
        UINT16 num_internal_clocks;          ///< Number of defined internal clocks.
        DralGraph * dbgraph;                   ///< Pointer to the DB Graph of the trace.
} ;

/*
 * Some cycle toString mask defines.
 */
#define CYCLE_MASK_CYC (UINT8) 0x01
#define CYCLE_MASK_NUM (UINT8) 0x02
#define CYCLE_MASK_DIV (UINT8) 0x04
#define CYCLE_MASK_CLK (UINT8) 0x08
#define CYCLE_MASK_ALL (UINT8) CYCLE_MASK_CYC | CYCLE_MASK_NUM | CYCLE_MASK_DIV | CYCLE_MASK_CLK

/*
 * @brief
 * This class represents a cycle in the dreams world.
 *
 * @description
 * A cycle is defined by 3 elements:
 *  - clock domain: specifies the frequency and skew of the clock.
 *  - cycle: specifies the cycle within the clock domain.
 *  - division: which division of the cycle.
 *
 * @version 0.1
 * @date started at 2004-12-15
 * @author Guillem Sole
 */
class CYCLE
{
    public:
        CYCLE();
        CYCLE(const ClockDomainEntry * _clock);
        CYCLE(const ClockDomainEntry * _clock, INT32 _cycle, UINT16 _division);

        inline INT64 toLCMCycles() const;
        inline void fromLCMCycles(INT64 cycles);
        QString toString(UINT8 mask) const;
        inline INT64 toPs() const;
        inline void fromPs(INT64 ps);

        inline bool operator==(const CYCLE & cmp) const;
        inline bool operator!=(const CYCLE & cmp) const;
        inline bool operator<(const CYCLE & cmp) const;
        inline bool operator<=(const CYCLE & cmp) const;
        inline bool operator>(const CYCLE & cmp) const;
        inline bool operator>=(const CYCLE & cmp) const;

        inline CYCLE operator+(const CYCLE & cmp) const;
        inline CYCLE operator-(const CYCLE & cmp) const;

    public:
        const ClockDomainEntry * clock; ///< Pointer to the clock domain of this cycle.
        INT32 cycle;                    ///< Cycle within this domain.
        INT8 division;                  ///< Division of the cycle.
} ;

/*
 * Returns the id of the entry.
 *
 * @return the clock id.
 */
UINT16
ClockDomainEntry::getId() const
{
    return id;
}

/*
 * Returns the internal clock id associated to this domain.
 *
 * @return the internal clock id.
 */
UINT16
ClockDomainEntry::getInternalClock() const
{
    return internal_clock;
}

/*
 * Returns the frequency of the entry.
 *
 * @return the frequency.
 */
UINT64
ClockDomainEntry::getFreq() const
{
    return freq;
}

/*
 * Returns the inverted frequency of the entry.
 *
 * @return the inverted frequency.
 */
UINT64
ClockDomainEntry::getInvFreq() const
{
    return inv_freq;
}

/*
 * Returns the divisions of the entry.
 *
 * @return the divisions.
 */
UINT16
ClockDomainEntry::getDivisions() const
{
    return divisions;
}

/*
 * Returns the skew of the entry.
 *
 * @return the skew.
 */
UINT16
ClockDomainEntry::getSkew() const
{
    return skew;
}

/*
 * Returns the skew of the entry in LCM cycles.
 *
 * @return the skew.
 */
UINT16
ClockDomainEntry::getLCMSkew() const
{
    return lcm_skew;
}

/*
 * Returns the name of the entry.
 *
 * @return the name.
 */
QString
ClockDomainEntry::getName() const
{
    return name;
}

/*
 * Returns the frequency ratio (LCM) of the entry.
 *
 * @return the frequency ratio.
 */
UINT32
ClockDomainEntry::getLCMRatio() const
{
    return lcm_ratio;
}

/*
 * Returns the frequency ratio (fastest frequency) of the entry.
 *
 * @return the frequency ratio.
 */
float
ClockDomainEntry::getFastestRatio() const
{
    return fastest_ratio;
}

/*
 * Sets the relative frequency ratio to the LCM frequency.
 *
 * @return void.
 */
void
ClockDomainEntry::setLCMRatio(UINT64 base_freq)
{
    assert(freq!=0);
    lcm_ratio = (UINT32) base_freq / (UINT32) freq;
}

/*
 * Sets the relative frequency ratio to the fastest frequency.
 *
 * @return void.
 */
void
ClockDomainEntry::setFastestRatio(UINT64 base_freq)
{
    assert(freq!=0);
    fastest_ratio = (float) base_freq / (float) freq;
}

/*
 * Sets the number of relative frequency ratio to the fastest frequency.
 *
 * @return void.
 */
void
ClockDomainEntry::setLCMSkew()
{
    lcm_skew = ((skew * getLCMRatio()) << DIVISION_SHIFT) / 100;
}

/*
 * Returns the number of clock domains.
 *
 * @return the number of clocks.
 */
CLOCK_ID
ClockDomainMgr::getNumClocks() const
{
    return (CLOCK_ID) clocks.size();
}

/*
 * Returns the entry of the clock id.
 *
 * @return thc clock.
 */
const ClockDomainEntry *
ClockDomainMgr::getClockDomain(UINT16 id) const
{
    Q_ASSERT(id < clocks.size());
    return clocks[id];
}

/*
 * Returns if the clock id exists.
 *
 * @return if the clock id exists.
 */
bool
ClockDomainMgr::hasClockDomain(UINT16 id) const
{
    return (id < clocks.size());
}

/*
 * Returns the LCM of all the entries.
 *
 * @return the LCM.
 */
UINT64
ClockDomainMgr::getLCM() const
{
    return LCM;
}

/*
 * Returns an iterator to the list of clock entries sorted by
 * frequency.
 *
 * @return the iterator.
 */
ClockDomainFreqIterator
ClockDomainMgr::getFreqIterator() const
{
    return freq_set.begin();
}

/*
 * Converts the internal clockId to a user defined clock id that
 * has the same frequency and skew.
 *
 * @return the clock id.
 */
UINT16
ClockDomainMgr::internalToExternalClock(UINT16 clockId) const
{
    return int_to_ext[clockId];
}

/*
 * Converts the cycle to the LCM frequency cycles.
 *
 * @return the cycles.
 */
INT64
CYCLE::toLCMCycles() const
{
    INT64 ret;

    // Gets the result in phases.
    ret = ((INT64) cycle << DIVISION_SHIFT) + division;

    // Changes it to LCM cycles.
    ret = (ret * clock->getLCMRatio()) + clock->getLCMSkew();
    return ret;
}

/*
 * Gets the cycle represnted by LCM cycles.
 *
 * @return void.
 */
void
CYCLE::fromLCMCycles(INT64 cycles)
{
    // The skew of the clock is substracted.
    cycles -= clock->getLCMSkew();

    // Checks if a non-positive cycle is achieved.
    if(cycles <= 0)
    {
        cycle = 0;
        division = 0;
    }
    else
    {
        INT64 rest;

        cycle = cycles / (clock->getLCMRatio() << DIVISION_SHIFT);
        rest = cycles % (clock->getLCMRatio() << DIVISION_SHIFT);
        if(rest < clock->getLCMRatio())
        {
            division = 0;
        }
        else
        {
            division = 1;
        }
    }
}

/*
 * Returns the cycle in picoseconds.
 *
 * @return the picoseconds.
 */
INT64
CYCLE::toPs() const
{
    INT64 ps;

    ps = ((INT64) cycle * (INT64) 100000 + (INT64) division * (INT64) 50000) / (INT64) clock->getFreq();
    return ps;
}

/*
 * Gets the cycle represnted by LCM cycles.
 *
 * @return void.
 */
void
CYCLE::fromPs(INT64 ps)
{
    INT64 rest;

    cycle = (ps * clock->getFreq()) / 100000;
    rest = (ps * clock->getFreq()) % 100000;

    if(rest >= 50000)
    {
        division = 1;
    }
    else
    {
        division = 0;
    }
}

/*
 * Compares two cycles in LCM cycle granularity
 *
 * @return if the two cycles are equal.
 */
bool
CYCLE::operator==(const CYCLE & cmp) const
{
    return (toLCMCycles() == cmp.toLCMCycles());
}

/*
 * Compares two cycles in LCM cycle granularity
 *
 * @return if the two cycles are different.
 */
bool
CYCLE::operator!=(const CYCLE & cmp) const
{
    return (toLCMCycles() != cmp.toLCMCycles());
}

/*
 * Compares two cycles in LCM cycle granularity
 *
 * @return if the actual cycle is lower.
 */
bool
CYCLE::operator<(const CYCLE & cmp) const
{
    return (toLCMCycles() < cmp.toLCMCycles());
}

/*
 * Compares two cycles in LCM cycle granularity.
 *
 * @return if the actual cycle is lower or equal.
 */
bool
CYCLE::operator<=(const CYCLE & cmp) const
{
    return (toLCMCycles() <= cmp.toLCMCycles());
}

/*
 * Compares two cycles in LCM cycle granularity.
 *
 * @return if the actual cycle is bigger.
 */
bool
CYCLE::operator>(const CYCLE & cmp) const
{
    return (toLCMCycles() > cmp.toLCMCycles());
}

/*
 * Compares two cycles in LCM cycle granularity.
 *
 * @return if the actual cycle is bigger or equal.
 */
bool
CYCLE::operator>=(const CYCLE & cmp) const
{
    return (toLCMCycles() >= cmp.toLCMCycles());
}

/*
 * Adds two cycles and returns a new cycle in the clock domain
 * of the first source.
 *
 * @return the cycle addition.
 */
CYCLE
CYCLE::operator+(const CYCLE & cmp) const
{
    CYCLE ret;

    ret.clock = clock;

    // Checks if the clocks are in the same domain.
    if(clock == cmp.clock)
    {
        // Fast case where the two clocks are from the same domain.
        ret.cycle = cycle + cmp.cycle;
        ret.division = division + cmp.division;

        // Checks if an extra cycle is reached as the addition of the divisions.
        if(ret.division >= clock->getDivisions())
        {
            ret.cycle++;
            ret.division -= clock->getDivisions();
        }
    }
    else
    {
        // Slow case: different domains.
        INT64 cycles;

        // Adds the two LCM cycle conversion. The skew of the second clock is not added.
        cycles = toLCMCycles() + cmp.toLCMCycles() - cmp.clock->getLCMSkew();

        // Creates the cycle and division from the LCM cycle.
        ret.fromLCMCycles(cycles);
    }

    return ret;
}

/*
 * Subs two cycles and returns a new cycle in the clock domain
 * of the first source.
 *
 * @return the cycle substract.
 */
CYCLE
CYCLE::operator-(const CYCLE & cmp) const
{
    CYCLE ret;

    ret.clock = clock;

    // Checks if the clocks are in the same domain.
    if(clock == cmp.clock)
    {
        // Fast case where the two clocks are from the same domain.
        ret.cycle = cycle - cmp.cycle;
        ret.division = division - cmp.division;

        // Checks if an extra cycle is reached as the substraction of the divisions.
        if(ret.division < 0)
        {
            ret.cycle--;
            ret.division += clock->getDivisions();
        }

        // Checks special negative cycle.
        if(ret.cycle < 0)
        {
            ret.cycle = 0;
            ret.division = 0;
        }
    }
    else
    {
        // Slow case: different domains.
        INT64 cycles;

        // Adds the two LCM cycle conversion. The cmp clock skew is not substracted.
        cycles = toLCMCycles() - cmp.toLCMCycles() + cmp.clock->getLCMSkew();

        // Checks special cases.
        if(cycles < 0)
        {
            // We have a negative cycle, so we set a 0.
            ret.cycle = 0;
            ret.division = 0;
        }
        else
        {
            // Creates the cycle and division from the LCM cycle.
            ret.fromLCMCycles(cycles);
        }
    }

    return ret;
}

#endif
