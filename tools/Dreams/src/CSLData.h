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
  * @file  CSLData.h
  */

#ifndef _CSLDATA_H_
#define _CSLDATA_H_

#include "DreamsDefs.h"

#include <qcolor.h>
#include <qfont.h>
#include <qstring.h>

#include "EventShape.h"

typedef struct
{
  double  r, g, b;
  double dr,dg,db;
} rgbFade;


/*
 * @brief
 * Class that just stores the computed CSL information.
 *
 * @descritpion
 * This class is used to store all the different colors and shapes
 * that are computed with the expressions.
 */
class CSLData
{
    protected:
        QColor fill_color;   ///< Computed fill color.
        #define CSL_FILL_COLOR_MASK (UINT16) 0x0001

        QColor border_color; ///< Computed border color.
        #define CSL_BORDER_COLOR_MASK (UINT16) 0x0002

        QColor letter_color; ///< Computed letter color.
        #define CSL_LETTER_COLOR_MASK (UINT16) 0x0004

        EventShape shape;    ///< Computed shape.
        #define CSL_SHAPE_MASK (UINT16) 0x0008

        QChar letter;        ///< Computed letter.
        #define CSL_LETTER_MASK (UINT16) 0x0010

        QFont font;          ///< Computed font.
        #define CSL_FONT_MASK (UINT16) 0x0020

        bool transparency;   ///< True if the computed color is transparent.
        #define CSL_TRANSPARENCY_MASK (UINT16) 0x0040

        UINT8 fill_color_fade;  ///< If not zero, number of cycles over which fading should happen
        #define CSL_FILL_COLOR_FADE_MASK (UINT16) 0x0080

        bool italic;         ///< Computed font italic.
        #define CSL_ITALIC_MASK (UINT16) 0x0100

        bool underline;      ///< Computed font underline.
        #define CSL_UNDERLINE_MASK (UINT16) 0x0200

        bool bold;           ///< Computed font bold.
        #define CSL_BOLD_MASK (UINT16) 0x0400

        UINT16 compute_mask;  ///< This mask holds one bit to indicate if a property of the CSLData has been computed or not. 1 means still to compute, 0 already computed.

        rgbFade fill_fade;
        rgbFade border_fade;
        rgbFade letter_fade;

    public:
        CSLData()
        {
            reset();
        }

        ~CSLData()
        {
        }

        void
        reset()
        {
            shape = EVENT_SHAPE_RECTANGLE;
            letter = ' ';
            transparency = false;
            fill_color_fade = 0;

            letter_color.setRgb(0, 0, 0);
            border_color.setRgb(0, 0, 0);

            // Marks all the elements as not computed.
            compute_mask = 0xFFFF;
            fill_fade.dr   = 0.0; fill_fade.dg   = 0.0; fill_fade.db   = 0.0;
            border_fade.dr = 0.0; border_fade.dg = 0.0; border_fade.db = 0.0;
            letter_fade.dr = 0.0; letter_fade.dg = 0.0; letter_fade.db = 0.0;
            italic = false;
            underline = false;
            bold = false;
        }

        inline UINT16
        getComputeMask()
        {
            return compute_mask;
        }

        bool
        getFillColorSet()
        {
            return (compute_mask & CSL_FILL_COLOR_MASK) == 0;
        }

        bool
        getBorderColorSet()
        {
            return (compute_mask & CSL_BORDER_COLOR_MASK) == 0;
        }

        bool
        getLetterColorSet()
        {
            return (compute_mask & CSL_LETTER_COLOR_MASK) == 0;
        }

        bool
        getShapeSet()
        {
            return (compute_mask & CSL_SHAPE_MASK) == 0;
        }
        
        bool
        getLetterSet()
        {
            return (compute_mask & CSL_LETTER_MASK) == 0;
        }

        bool
        getFontSet()
        {
            return (compute_mask & CSL_FONT_MASK) == 0;
        }

        bool
        getTransparencySet()
        {
            return (compute_mask & CSL_TRANSPARENCY_MASK) == 0;
        }

        bool
        getFillColorFadeSet()
        {
            return (compute_mask & CSL_FILL_COLOR_FADE_MASK) == 0;
        }

        bool
        getItalicSet()
        {
            return (compute_mask & CSL_ITALIC_MASK) == 0;
        }

        bool
        getUnderlineSet()
        {
            return (compute_mask & CSL_UNDERLINE_MASK) == 0;
        }

        bool
        getBoldSet()
        {
            return (compute_mask & CSL_BOLD_MASK) == 0;
        }

        /*
         * Gets the computed fill color.
         *
         * @return color.
         */
        QColor
        getFillColor()
        {
            return fill_color;
        }

        /*
         * Gets the computed border color.
         *
         * @return color.
         */
        QColor
        getBorderColor()
        {
            return border_color;
        }

        /*
         * Gets the computed letter color.
         *
         * @return color.
         */
        QColor
        getLetterColor()
        {
            return letter_color;
        }

        /*
         * Gets the computed shape.
         *
         * @return shape.
         */
        EventShape
        getShape()
        {
            return shape;
        }

        /*
         * Gets the computed letter.
         *
         * @return letter.
         */
        QChar
        getLetter()
        {
            return letter;
        }

        /*
         * Gets the computed font letter.
         *
         * @return font.
         */
        QFont
        getFont()
        {
            return font;
        }

        /*
         * Gets the transparency.
         *
         * @return if the computed color is transparent.
         */
        bool
        getTransparency()
        {
            return transparency;
        }

        /*
         * Returns the fading period for FIll Color
         *
         * @return the fading period for FIll Color
         */
        UINT8
        getFillColorFade()
        {
            return fill_color_fade;
        }

        /*
         * Returns the italic.
         *
         * @return the italic.
         */
        bool
        getItalic()
        {
            return italic;
        }

        /*
         * Returns the underline.
         *
         * @return the underline.
         */
        bool
        getUnderline()
        {
            return underline;
        }

        /*
         * Returns the bold.
         *
         * @return the bold.
         */
        bool
        getBold()
        {
            return bold;
        }

        /*
         * Sets the computed fill color.
         *
         * @return void.
         */
        void
        setFillColor(QColor color)
        {
            compute_mask = compute_mask & ~CSL_FILL_COLOR_MASK;
            fill_color = color;
        }

        /*
         * Sets the fill color fading period
         *
         * @return void.
         */
        void
        setFillColorFade(UINT8 fade)
        {
            compute_mask = compute_mask & ~CSL_FILL_COLOR_FADE_MASK;
            fill_color_fade = fade;
        }

        /*
         * Sets the computed border color.
         *
         * @return void.
         */
        void
        setBorderColor(QColor color)
        {
            compute_mask = compute_mask & ~CSL_BORDER_COLOR_MASK;
            border_color = color;
        }

        /*
         * Sets the computed letter color.
         *
         * @return void.
         */
        void
        setLetterColor(QColor color)
        {
            compute_mask = compute_mask & ~CSL_LETTER_COLOR_MASK;
            letter_color = color;
        }

        /*
         * Sets the computed shape.
         *
         * @return void.
         */
        void
        setShape(EventShape _shape)
        {
            compute_mask = compute_mask & ~CSL_SHAPE_MASK;
            shape = _shape;
        }

        /*
         * Sets the computed letter.
         *
         * @return void.
         */
        void
        setLetter(QChar _letter)
        {
            compute_mask = compute_mask & ~CSL_LETTER_MASK;
            letter = _letter;
        }

        /*
         * Sets the computed font.
         *
         * @return void.
         */
        void
        setFont(QFont _font)
        {
            compute_mask = compute_mask & ~CSL_FONT_MASK;
            font = _font;
        }

        /*
         * Sets the transparency.
         *
         * @return void.
         */
        void
        setTransparency(bool _transparency)
        {
            compute_mask = compute_mask & ~CSL_TRANSPARENCY_MASK;
            transparency = _transparency;
        }

        /*
         * Sets the italic.
         *
         * @return void.
         */
        void
        setItalic(bool _italic)
        {
            compute_mask = compute_mask & ~CSL_ITALIC_MASK;
            italic = _italic;
        }

        /*
         * Sets the underline.
         *
         * @return void.
         */
        void
        setUnderline(bool _underline)
        {
            compute_mask = compute_mask & ~CSL_UNDERLINE_MASK;
            underline = _underline;
        }

        /*
         * Sets the bold.
         *
         * @return void.
         */
        void
        setBold(bool _bold)
        {
            compute_mask = compute_mask & ~CSL_BOLD_MASK;
            bold = _bold;
        }

        bool 
        getDefined(void)
        {
            return (!getTransparency() && getFillColorSet()) || 
                    getBorderColorSet() || getLetterSet();
        }

        void fadeParams(QColor dest, UINT32 s);
        void fadeStep(UINT32 step);

        QString
        toString(void)
        {
            QString res;

            res  = "mask = 0b" + QString::number(compute_mask,2);
            res += " fcolor=" + fill_color.name();
            res += " bcolor=" + border_color.name();
            res += " lcolor=" + letter_color.name();
            res += " shape="  + EventShapeToolkit::toString(shape);
            res += " letter=" + QString(letter);
            res += "\n";

            return res;
        }
} ;

#endif
