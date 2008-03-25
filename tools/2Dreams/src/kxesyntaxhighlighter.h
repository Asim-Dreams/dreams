/***************************************************************************
                          kxesyntaxhighlighter.h  -  XML Syntax highlighter
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

#ifndef KXESYNTAXHIGHLIGHTER_H
#define KXESYNTAXHIGHLIGHTER_H

#include <qsyntaxhighlighter.h>

class QTextEdit;

/**XML Syntax highlighter
  *@author Lumir Vanek
  */

class KXESyntaxHighlighter : public QSyntaxHighlighter
{
public: 
	KXESyntaxHighlighter(QTextEdit *);
	~KXESyntaxHighlighter();

  int highlightParagraph(const QString &text, int endStateOfLastPara);

  void setColorDefaultText(const QColor color) { m_clrDefaultText = color; }
  void setColorElementNames(const QColor color) { m_clrElementName = color; }
  void setColorAttributeNames(const QColor color) { m_clrAttributeName = color; }
  void setColorAttributeValues(const QColor color) { m_clrAttributeValue = color; }
  void setColorXmlSyntaxChars(const QColor color) { m_clrXmlSyntaxChar = color; }
  void setColorComments(const QColor color) { m_clrComment = color; }
  void setColorSyntaxError(const QColor color) { m_clrSyntaxError = color; }

  const QColor colorDefaultText() const { return m_clrDefaultText; }
  const QColor colorElementNames() const { return m_clrElementName; }
  const QColor colorAttributeNames() const { return m_clrAttributeName; }
  const QColor colorAttributeValues() const { return m_clrAttributeValue; }
  const QColor colorXmlSyntaxChars() const { return m_clrXmlSyntaxChar; }
  const QColor colorComments() const { return m_clrComment; }
  const QColor colorSyntaxError() const { return m_clrSyntaxError; }
  
protected:
  int processDefaultText(int, const QString&);
  
protected:

  QColor m_clrDefaultText;
  QColor m_clrElementName;
  QColor m_clrAttributeName;
  QColor m_clrAttributeValue;
  QColor m_clrXmlSyntaxChar; // < > = "
  QColor m_clrComment;
  QColor m_clrSyntaxError;

  // states for parsing XML
  enum ParserState
	{
    parsingNone = 0,
    expectElementNameOrSlash,
    expectElementName,
    expectAtttributeOrEndOfElement,
		expectEqual,
    expectAttributeValue
	};

  ParserState m_eParserState;
};

#endif
