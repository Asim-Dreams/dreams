/***************************************************************************
                          kxesyntaxhighlighter.cpp  -  XML Syntax highlighter
                             -------------------
    begin                : Ne pro 14 2003
    copyright            : (C) 2003 by The KXMLEditor Team
    email                : lvanek.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kxesyntaxhighlighter.h"

#include <iostream>

#include <qcolor.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>

#include <qtextedit.h>

// Regular expressions for parsing XML borrowed from:
// http://www.cs.sfu.ca/~cameron/REX.html

KXESyntaxHighlighter::KXESyntaxHighlighter(QTextEdit *textEdit)
  : QSyntaxHighlighter(textEdit)
{
  m_clrDefaultText = QColor("black");
  m_clrElementName = QColor("magenta");
  m_clrAttributeName  = QColor("black");
  m_clrAttributeValue = QColor("blue");
  m_clrXmlSyntaxChar.setRgb(128,128,128); 
  m_clrComment = QColor("green");
  m_clrSyntaxError = QColor("red");
}

KXESyntaxHighlighter::~KXESyntaxHighlighter()
{
}


int KXESyntaxHighlighter::highlightParagraph(const QString& text, int endStateOfLastPara)
{
    //first I format the given line to default so any remaining highlighting is removed (Qt does not do it by itself)
    setFormat(0 , text.length(), QColor(0, 0, 0));

    int iBracketNesting = 0;
    m_eParserState = parsingNone;
    int pos;
    
    for(unsigned int i = 0; i < text.length() - 1; i++)
    {
      switch(text[i])
      {
        case '<':
          iBracketNesting++;

          if(iBracketNesting == 1)
            { setFormat( i, 1, m_clrXmlSyntaxChar );
              m_eParserState = expectElementNameOrSlash;
            }
          else
            setFormat( i, 1, m_clrSyntaxError ); // wrong bracket nesting

          break;

        case '>':
          iBracketNesting--;

          if(iBracketNesting == 0)
            setFormat( i, 1, m_clrXmlSyntaxChar );
          else
            setFormat( i, 1, m_clrSyntaxError ); // wrong bracket nesting

          m_eParserState = parsingNone;
            
          break;

        case '/':

          if(m_eParserState == expectElementNameOrSlash)
          {
            m_eParserState = expectElementName;
            setFormat( i, 1, m_clrXmlSyntaxChar );
          }
          else
          {
            if(m_eParserState == expectAtttributeOrEndOfElement)
              setFormat( i, 1, m_clrXmlSyntaxChar );
            else
              processDefaultText(i, text);  
          }
          
          break;
            
        case '=':
          if(m_eParserState == expectEqual)
          {
            m_eParserState = expectAttributeValue;
            setFormat( i, 1, m_clrXmlSyntaxChar );
          }
          else
          {
            processDefaultText(i, text);  
          }
          break;
            
        case '\"':
          
          if(m_eParserState == expectAttributeValue)
          {
            QRegExp patternAttribute("\"[^<\"]*\"|'[^<']*'"); // search attribute value
            pos=patternAttribute.search(text, i);

            if(pos == (int)i) // attribute value found ?
            {
              int l = patternAttribute.matchedLength();

              setFormat(i, 1, m_clrXmlSyntaxChar );
              setFormat(i+1, l - 2, m_clrAttributeValue);
              setFormat(i+l-1, 1, m_clrXmlSyntaxChar );
              
              i += l - 1; // skip attribute value
              m_eParserState = expectAtttributeOrEndOfElement;
            }
            else
              processDefaultText(i, text);
          }
          else  
            processDefaultText(i, text);
            
          break;

        case '!':
          if(m_eParserState == expectElementNameOrSlash)
          {
            QRegExp patternComment("<!--[^-]*-([^-][^-]*-)*->"); // search comment
            pos=patternComment.search(text, i-1);

            if(pos == (int)i-1) // comment found ?
            {
              int l = patternComment.matchedLength();
              
              setFormat(pos, l, m_clrComment);
              i += l - 2; // skip comment
              m_eParserState = parsingNone;
              iBracketNesting--;
            }
            else
              processDefaultText(i, text);
          }
          else
            processDefaultText(i, text);
            
          break;  

        default:
          int iLenght = processDefaultText(i, text);
          if(iLenght > 0)
            i += iLenght - 1;
          break;
      }
    }

    return endStateOfLastPara;
}

int KXESyntaxHighlighter::processDefaultText(int i, const QString& text)
{
  int l = 0; // length of matched text

  switch(m_eParserState)
  {
    case expectElementNameOrSlash:
    case expectElementName:
      {
        QRegExp patternName("([A-Za-z_:]|[^\\x00-\\x7F])([A-Za-z0-9_:.-]|[^\\x00-\\x7F])*"); // search element name
        int pos=patternName.search(text, i);

        if(pos == i) // found ?
        {
          l = patternName.matchedLength();

          setFormat(pos, l, m_clrElementName);
          m_eParserState = expectAtttributeOrEndOfElement;
        }
        else
          setFormat( i, 1, m_clrDefaultText );
      }  
      break;
      
    case expectAtttributeOrEndOfElement:
      {
        QRegExp patternName("([A-Za-z_:]|[^\\x00-\\x7F])([A-Za-z0-9_:.-]|[^\\x00-\\x7F])*"); // search attribute name
        int pos=patternName.search(text, i);

        if(pos == i) // found ?
        {
          l = patternName.matchedLength();

          setFormat(pos, l, m_clrAttributeName);
          m_eParserState = expectEqual;
        }
        else
          setFormat( i, 1, m_clrDefaultText );
      }
      break;

    default:
      setFormat( i, 1, m_clrDefaultText );
      break;
  }
  return l;
}  

