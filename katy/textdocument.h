/*
 * Class for Text Document
 * Copyright (c) by Gordon Tyler <gordon@doxxx.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef TEXTDOCUMENT_H
#define TEXTDOCUMENT_H

#include <qobject.h>
#include <kurl.h>
#include <qvaluelist.h>

class FormatRange
{
public:
    enum FormatType
    {
        FT_Normal,
        FT_Keyword,
        FT_String,
        FT_Comment,
        FT_Number
    };

    FormatType type;
    int begin;
    int length;
};

class TextLine
{
public:
    TextLine()
    {
    }

    TextLine(QString newText)
    {
        text = newText;
    }

public:
    QValueList<FormatRange> formats;
    QString text;
};

typedef QValueList<TextLine> TextLineList;

class Position
{
public:
    Position()
    {
    }

    Position(int newLine, int newColumn)
    {
        line = newLine;
        column = newColumn;
    }

public:
    int line, column;
};

/**
 * This class represents a text document that can be displayed by
 * TextEditor.
 *
 * @short Text document class
 * @author Gordon Tyler <gtyler@iafrica.com>
 * @version 0.3
 */
class TextDocument : public QObject
{
    Q_OBJECT
public:
    enum EOLType
    {
        EOL_Unknown = 0,
        EOL_LF,
        EOL_CR,
        EOL_CRLF
    };


public:
    TextDocument();
    TextDocument(const KURL& url);
    ~TextDocument();

    KURL url();
    bool modified();
    int lineCount();
    TextLine line(int line);
    void setLine(int line, TextLine textLine);
    TextLineList::ConstIterator lineIterator(int line);
    TextLineList::ConstIterator endLineIterator();
    EOLType eolType();
    void setEOLType(EOLType type);
    QString text(int startLine, int startColumn, int endLine, int endColumn);

    void openURL(const KURL& url);
    void save();
    void saveURL(const KURL& url);

    Position insertText(int line, int column, QString text);
    void insertLines(int line, TextLineList newLines, bool after=FALSE);
    void removeText(int startLine, int startColumn, int endLine, int endColumn);
    void removeLines(int line, int count);
    void splitLine(int line, int column);
    void joinLines(int line);

signals:
    void lineChanged(int line, TextLine oldLine, TextLine newLine);
    void linesInserted(int line, TextLineList newLines);
    void linesRemoved(int line, int count);
    void documentModified();
    void documentNotModified();

protected:
    bool loadTempFile(QString filename);
    bool saveTempFile(QString filename);

    void setModified(bool modified);

protected:
    KURL m_url;
    TextLineList m_lines;
    EOLType m_eolType;
    bool m_loaded;
    bool m_modified;
};

#endif
