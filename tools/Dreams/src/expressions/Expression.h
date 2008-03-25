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

/**
  * @file  Expression.h
  * @brief

  */

#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

// General includes.
#include <set>
using namespace std;

// Dreams includes.
#include "dDB/DralDBSyntax.h"
#include "dDB/DralDBDefinitions.h"
#include "dDB/DRALTag.h"
#include "dDB/DralDB.h"
#include "dDB/aux/AMemObj.h"
#include "TagValueType.h"
#include "CSLData.h"
#include "DreamsDefs.h"

// Qt includes.
#include <qstring.h>
#include <qptrlist.h>

#define INDENTSTR "    "

//
// Debugging Wrapping Macros
//
#define DODEBUG 1
extern UINT32 rogerLevel;

#ifdef DODEBUG
# define EXPDEBUG(level) if ( rogerLevel >= (level))
#else
# define EXPDEBUG(level) if ( 0 ) 
#endif

// Class forwarding.
class CSLData;

typedef enum
{
  ExpressionNoValue,           ///< Expression type used for "statements" that return no value (IF, SET)
  ExpressionIntegerValue,      ///< Expression evaluates to an Integer result
  ExpressionStringValue,       ///< Expression evaluates to a  String result
  ExpressionFPValue,           ///< Expression evaluates to an FP result
  ExpressionLetterValue,       ///< Expression evaluates to an FP result
  ExpressionShapeValue,         ///< Expression evaluates to an FP result
  ExpressionTypeLast
} ExpressionType;

typedef enum
{
  OperatorUnknown = 0,
  OperatorSetFColor,      ///< Sets cell fill color property
  OperatorSetBColor,      ///< Sets cell border color property
  OperatorSetLColor,      ///< Sets cell letter color property
  OperatorSetLetter,      ///< Sets cell letter property
  OperatorSetShape,       ///< Sets cell shape property
  OperatorSetFont,        ///< Sets cell letter font property
  OperatorSetItalic,      ///< Sets italic font property
  OperatorSetUnderline,   ///< Sets underline font property
  OperatorSetBold,        ///< Sets bold font property
  OperatorEqual,          ///< Integer, FP and String Operands
  OperatorNotEqual,       ///< Integer, FP and String Operands
  OperatorLowerThan,      ///< Integer, FP and String Operands
  OperatorLowerEqual,     ///< Integer, FP and String Operands
  OperatorGreaterThan,    ///< Integer, FP and String Operands
  OperatorGreaterEqual,   ///< Integer, FP and String Operands
  OperatorRegExpMatch,    ///< String operands only
  OperatorLogicalAnd,     ///< Integer operands only
  OperatorLogicalOr,      ///< Integer operands only
  OperatorLogicalNot,     ///< Integer operands only
  OperatorBitwiseAnd,     ///< Integer operands only
  OperatorBitwiseOr,      ///< Integer operands only
  OperatorBitwiseXor,     ///< Integer operands only
  OperatorBitwiseNot,     ///< Integer operands only
  OperatorShiftLeft,      ///< Integer operands only
  OperatorShiftRight,     ///< Integer operands only
  OperatorAdd,            ///< Integer and FP operands only
  OperatorSub,            ///< Integer and FP operands only
  OperatorMul,            ///< Integer and FP operands only
  OperatorDiv,            ///< Integer and FP operands only
  OperatorMod,            ///< Integer operands only
  OperatorUnaryMinus,     ///< Integer and FP operands only
  OperatorITag,           ///< Access Item Tag
  OperatorSTag,           ///< Access Slot Tag
  OperatorCTag,           ///< Access Cycle Tag
  OperatorNTag,           ///< Access Node Tag
  OperatorToDouble,       ///< Converts an Integer into a double
  OperatorToUINT64,      ///< Converts an FP into a UINT32
  OperatorLast
} ExpressionOperatorType;       

class ExpressionToolkit
{
  public:
   static QString        toString(ExpressionOperatorType op);
   static QString        toString(ExpressionType ty);
   static UINT16         toMask(ExpressionOperatorType op);
   static ExpressionType toExpressionType(TagValueType t);
   static bool           operatorMatch(ExpressionOperatorType op, ExpressionType ty);
} ;

/*
 * @struct RuleEvalEnv
 *
 * This class has all the arguments needed in the evaluation of
 * a rule.
 */
struct RuleEvalEnv
{
    TagValueEntry itemTags;  ///< Tags of the item inside the actual slot.
    TagValueEntry slotTags;  ///< Tags of the actual slot.
    TagValueEntry nodeTags;  ///< Tags of the node to which the current slot belongs to.
    TagValueEntry cycleTags; ///< Tags for the current cycle
    INT32 cycle;             ///< Actual evaluation cycle.
    TRACK_ID trackId;        ///< Track Id of the slot that is being evaluated.
    CSLData cslResult;       ///< Variable where the computed data is stored.
} ;

/*
 * @struct RuleEvalEnvDesc
 *
 * This class has all the arguments that are set in a evaluation of a rule.
 */
struct RuleEvalEnvDesc
{
    QString * shape;        ///< Shape evaluation.
    QString * fill_color;   ///< Fill color evaluation.
    QString * border_color; ///< Border color evaluation.
    QString * letter_color; ///< Letter color evaluation.
    QString * letter;       ///< Letter evaluation.
    QString * font;         ///< Font evaluation.
} ;

/**
 * Base class for all the rule evaluation mechanism. This class is resposible
 * of holding all the rules defined in the ADF. It is able to store any kind of
 * exprssion, like if, binary operations, constants values, etc...  Each of
 * this kind of expression will be defineb by each Expression subclass and each
 * of them will implement an different <code>eval</code> method
 */
class Expression : public AMemObj
{
  protected:

    /**
     * Textual description of the expression
     */
    QString desc;

    /** 
     * Type of the underlying expression. Used for typechecking of some
     * expressions
     */
    ExpressionType type;

    /**
     * Stores which elements of the CSLData this node could change when
     * evaluated.
     */
    UINT16 computeMask;

  private:
    /**
     * The following union holds all possible  return values for a given
     * expression: Integer, FP, QString, Char (letter) and Shape.
     * All expressions have accessors for getting the return value in all
     * its possible flavors.
     */
    union {
     UINT64     ivalue;
     INT32      svalue;
     double     fpvalue;
     EventShape shape;
     char       letter;
    } value;

    /**
     * Again, C++ does not allow a constructor for QFont and QColor
     * in the previous union, so we need to keep this one separate!
     * Bummer!
     */
    QFont       font;
    QColor      color;
    bool        transp_color;
    bool        cached_color;

  public:

    Expression(ExpressionType _type) : type(_type)
    { 
     font = QString("");
     value.ivalue = 0;
     value.svalue = 0;
     cached_color = false;
    }

    virtual ~Expression() { }

    /**
     * Performs the type checking of the expression. Avoids incorrect
     * cases like comparing strings with integers.  After the expression
     * tree has been built, the second method invoked on this expression
     * must be "typeCheck" (note that calls to eval may fail if typeCheck
     * has not been invoked previously)
     */ 
    virtual bool typeCheck(QString & error) = 0;

    /**
     * The third method invoked on the expression should be getUsedTags.
     * This method *must* be invoked so that the DataBase can optimize
     * correctly its FlipIterators. Failure to invoke this method may
     * cause the database to skip cycles where the expression might
     * evaluate to a different value This Function appends to the given
     * set the list of slot tags used in the expression.
     */ 
    virtual void getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags) = 0;

    /**
     * Method that walks over the expression tree evaluating it
     * within the context of the given "environment" (RuleEvalEnv arg).
     * The "env" argument contains a CSLData object that must be updated
     * as the tree evaluation proceeds. The actual result of the current
     * expression evaluated is stored in the "value" union above or, for
     * more complex types, in a QString or QFont variable. Because some of
     * this return values take up all available bits in the union, the
     * eval() routine also returns a boolean indicating whether the
     * evaluation was successful/valid at all. BEFORE using an accessor to
     * get the result value, code should check the return value of eval().
     *
     * @param env is a pointer to the actual evaluation data.
     *
     * @return true if the expression has a defined value. Otherwise, the
     *         value is not defined and using any accessor method will
     *         return unpredictable data.
     */
    virtual bool eval(RuleEvalEnv * env) = 0;

    /**
     * Same as above, but used to know which sentences set the different
     *
     * @param env is a pointer to the actual evaluation data.
     * @param desc is a pointer to the actual evaluation description.
     * @param pre is a composed string of the rule description.
     * @param post is a composed string of the rule description.
     *
     * @return true if the expression has a defined value. Otherwise, the
     *         value is not defined and using any accessor method will
     *         return unpredictable data.
     */
    virtual bool eval(RuleEvalEnv * env, RuleEvalEnvDesc * desc, QString * pre, QString * post, INT32 indent) = 0;

    /**
     * Type of this expression
     */
    inline ExpressionType getType(void);

    /*
     * Returns which elements of the CSLData this expression could change
     * if evaluated.
     */
    inline UINT16 getComputeMask(void);

    /*
     * Evaluates the compute mask of the expression.
     */
    virtual UINT16 evalComputeMask(void) = 0;

    /**
     * Integer value resulting of the evaluation of the expression.
     * If the expression has a numeric (boolean or integer)
     * meaning, then it will hold the value (0, <>0 or the actual value)
     * If the expression has an string meaning, then it will hold the index
     * of the string in the string cache.
     */
    inline UINT64     getValue(void) const       { return value.ivalue; }
    inline double     getFPValue(void) const     { return value.fpvalue; }
    inline INT32      getStringValue(void) const { return value.svalue; }
    inline EventShape getShapeValue(void) const  { return value.shape; }
    inline QChar      getLetterValue(void) const { return QChar(value.letter); }
    inline QFont      getFontValue(void) const   { return font; }

    inline QColor     getColorValue(DralDB * draldb, bool * transp)
    {
        //
        // For color attributes, the RHS can be both an Integer or a String.
        // Get the right color to be used below. We abuse somewhat the
        // implementation by ignoring "op" and checking only for the
        // rhsType... because QColor does not break if we feed it a silly
        // value, this evaluation order saves a long if condition.
        //
        if(!cached_color)
        {
            if(getType() == ExpressionStringValue)
            {
                transp_color = (getStringValue() == draldb->addString("transparent"));
                color = QColor(draldb->getString(getStringValue()));
            }
            else
            {
                transp_color = false;
                color = QColor(getValue());
            }
            cached_color = true;
        }
        * transp = transp_color;
        return color;
    }

    /**
     * Function used to set values
     */
    inline void setValue(UINT64 _value)          { value.ivalue = _value; cached_color = false; }
    inline void setFPValue(double _value)        { value.fpvalue = _value; cached_color = false; }
    inline void setStringValue(INT32 _value)     { value.svalue = _value; cached_color = false; }
    inline void setShapeValue(EventShape _value) { value.shape = _value; cached_color = false; }
    inline void setLetterValue(QChar _value)     { value.letter = _value; cached_color = false; }
    inline void setFontValue(QFont _value)       { font = _value; cached_color = false; }

    /**
     * Method to generate a string with the expression tree;
     */
    virtual QString toString(QString indent) = 0;

    /*
     * Functions to get and set the XML description of the rule.
     */
    inline void setDescription(QString d);
    inline QString getRowDescription() const;

    inline UINT32 getLevel(void);
};

ExpressionType
Expression::getType(void)
{
    return type;
}

UINT16
Expression::getComputeMask(void)
{
    return computeMask;
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
/////////
/////////
///////// EXPRESSION LIST
/////////
/////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

/* @typdef ExpressionIterator
 * @brief
 * Just a Expression iterator.
 */
typedef QPtrListIterator<Expression> ExpressionIterator;

/* @typdef ExpressionList
 * @brief
 * An ordered list of expressions
 */
class ExpressionList : public QPtrList<Expression>
{
 public:

    /**
     * Constructor
     */
    ExpressionList() : QPtrList<Expression>() { };

    /**
     * Append an ExpressionList to the end of the list
     */
    void appendList(ExpressionList &elist);

    /** 
     * Returns the bitmask corresponding to the properties that may be
     * modified by this ExpressionList
     */
    UINT16 evalComputeMask();

    /**
     * Returns the list of Tags used by the current expression whenever
     * "eval" is invoked.
     */
    void getUsedTags(set<TAG_ID> * slotTags, set<TAG_ID> * itemTags, set<TAG_ID> * nodeTags, set<TAG_ID> * cycleTags);

    /**
     * Perform type checking on all elements of the ExpressionList
     */
    bool typeCheck(QString & error);

    /**
     * Return String representing all elements of the ExpressionList
     */
    QString toString(QString indent);

    /**
     * Inherited from AMEMObj: return object size
     */
    INT64 getObjSize() const;

    /**
     * Inherited from AMEMObj: return textual explanation of size used by
     * this object
     */
    QString getUsageDescription() const;
};

void
Expression::setDescription(QString d)
{
    desc = d;
}

QString
Expression::getRowDescription() const
{
    return desc;
}

#endif
