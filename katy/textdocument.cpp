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

#include "textdocument.h"

#include <kdebug.h>
#include <kurl.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <qfile.h>
#include <qtextcodec.h>
#include <ktempfile.h>
#include <qtextstream.h>

TextDocument::TextDocument()
{
    m_url = KURL();
    m_eolType = EOL_LF;
    m_loaded = false;
    m_modified = false;
    m_lines.append(TextLine(QString("")));
}

TextDocument::TextDocument(const KURL& url)
{
    openURL(url);
}

TextDocument::~TextDocument()
{
}

KURL TextDocument::url()
{
    return m_url;
}

bool TextDocument::modified()
{
    return m_modified;
}

int TextDocument::lineCount()
{
    return m_lines.count();
}

TextLine TextDocument::line(int line)
{
    return m_lines[line];
}

void TextDocument::setLine(int line, TextLine newLine)
{
    TextLine oldLine = m_lines[line];
    m_lines[line] = newLine;
    setModified(true);
    emit lineChanged(line, oldLine, newLine);
}

TextLineList::ConstIterator TextDocument::lineIterator(int line)
{
    return m_lines.at(line);
}

TextLineList::ConstIterator TextDocument::endLineIterator()
{
    return m_lines.end();
}

TextDocument::EOLType TextDocument::eolType()
{
    return m_eolType;
}

void TextDocument::setEOLType(TextDocument::EOLType type)
{
    m_eolType = type;
    setModified(true);
}

QString TextDocument::text(int startLine, int startColumn, int endLine, int endColumn)
{
    QString text;

    if (startLine == endLine)
    {
        text = m_lines[startLine].text.mid(startColumn, endColumn - startColumn);
    }
    else
    {
        text = m_lines[startLine].text.mid(startColumn) + '\n';
        for (int i = startLine + 1; i < endLine; i++)
            text += m_lines[i].text + '\n';
        text += m_lines[endLine].text.left(endColumn);
    }

    return text;
}

void TextDocument::openURL(const KURL& url)
{
    QString target;

    // download the contents
    if (KIO::NetAccess::download(url, target))
    {
        // load in the file (target is always local)
        if (loadTempFile(target))
        {
            m_url = url;
            m_loaded = true;
            setModified(false);
        }

        // and remove the temp file
        KIO::NetAccess::removeTempFile(target);
    }
}

bool TextDocument::loadTempFile(QString filename)
{
    QFile file(filename);
    if (!file.open(IO_ReadOnly))
    {
        kdError() << "TextDocument::loadTempFile(): Could not open temporary file "  << filename << " for reading" << endl;
        return false;
    }

    QTextCodec *codec = QTextCodec::codecForLocale();
    QTextDecoder *decoder = codec->makeDecoder();

    QStringList lines;
    QString line;
    QChar c;
    EOLType eolType = EOL_Unknown;
    bool foundEOL = false;
    bool mixedEOLType = false;

    while (!file.atEnd())
    {
        char chars[1];

        chars[0] = file.getch();
        QString uc = decoder->toUnicode(chars, 1);
        c = uc[0];


        if (c == '\n')
        {
            if (eolType != EOL_Unknown && eolType != EOL_LF)
                mixedEOLType = true;
            eolType = EOL_LF;
            foundEOL = true;
        }
        else if (c == '\r')
        {
            chars[0] = file.getch();
            QString uc = decoder->toUnicode(chars, 1);
            c = uc[0];

            if (c == '\n')
            {
                if (eolType != EOL_Unknown && eolType != EOL_CRLF)
                    mixedEOLType = true;
                eolType = EOL_CRLF;
                foundEOL = true;
            }
            else
            {
                QCString s = codec->fromUnicode(uc);
                for (int i = s.length() - 1; i >= 0; i--)
                    file.ungetch(s[i]);

                if (eolType != EOL_Unknown && eolType != EOL_CR)
                    mixedEOLType = true;
                eolType = EOL_CR;
                foundEOL = true;
            }
        }

        if (foundEOL)
        {
            lines.append(line);
            line.truncate(0);
            foundEOL = false;
        }
        else
        {
            line += c;
        }
    }

    file.close();

    if (line.length() > 0)
        lines.append(line);

    for (QStringList::Iterator it = lines.begin(); it != lines.end(); ++it)
    {
        TextLine textLine((*it));
        m_lines.append(textLine);
    }

    if (mixedEOLType)
        m_eolType = EOL_Unknown;
    else
        m_eolType = eolType;

    return true;
}

void TextDocument::save()
{
    saveURL(m_url);
}

void TextDocument::saveURL(const KURL& url)
{
    KTempFile tempfile;

    if (tempfile.status() != 0)
    {
        kdError() << "TextDocument::saveURL(): Could not create temporary file" << endl;
        return;
    }

    if (!saveTempFile(tempfile.name()))
        return;

    if (url.isLocalFile())
    {
        if (!KIO::NetAccess::del(url))
        {
            kdError() << "TextDocument::saveURL(): Could not delete destination file " << url.prettyURL() << endl;
            return;
        }

        if (!KIO::NetAccess::copy(KURL(tempfile.name()), url))
        {
            kdError() << "TextDocument::saveURL(): Could not copy temporary file to destination file" << endl;
            return;
        }
    }
    else
    {
        if (!KIO::NetAccess::upload(tempfile.name(), url))
        {
            kdError() << "TextDocument::saveURL(): Could not upload temporary file " << tempfile.name() << " to " << url.prettyURL() << endl;
            return;
        }
    }

    m_url = url;
    setModified(false);

    tempfile.unlink();
}

bool TextDocument::saveTempFile(QString filename)
{
    QFile file(filename);
    if (!file.open(IO_WriteOnly))
    {
        kdError() << "TextDocument::saveURL(): Could not open temporary file "  << filename << " for writing" << endl;
        return false;
    }

    QTextStream textStream(&file);

    TextLineList::Iterator it = m_lines.begin();

    for (int count = 0; it != m_lines.end(); ++it, ++count)
    {
        TextLine textLine = (TextLine)*it;
        textStream << textLine.text;
        if (count < m_lines.count() - 1)
        {
            switch (m_eolType)
            {
                case EOL_CR:
                    textStream << '\r';
                    break;

                case EOL_LF:
                    textStream << '\n';
                    break;

                case EOL_Unknown:
                case EOL_CRLF:
                    textStream << '\r' << '\n';
                    break;
            }
        }
    }

    file.close();

    return true;
}

Position TextDocument::insertText(int line, int column, QString text)
{
    if (text.contains(QChar('\n')))
    {
        QStringList lines = QStringList::split(QChar('\n'), text, TRUE);

        splitLine(line, column);
        insertText(line, column, lines[0]);

        TextLineList insertedLines;
        TextLineList::Iterator it = m_lines.at(line + 1);

        for (int i = 1; i < lines.count() - 1; ++i)
        {
            TextLine textLine(lines[i]);
            insertedLines << textLine;
            m_lines.insert(it, textLine);
        }

        setModified(true);
        emit linesInserted(line + 1, insertedLines);

        insertText(line + lines.count() - 1, 0, lines[lines.count() - 1]);

        return Position(line + lines.count() - 1, lines[lines.count() - 1].length());
    }
    else
    {
        TextLine oldLine = m_lines[line];
        m_lines[line].text.insert(column, text);
        setModified(true);
        emit lineChanged(line, oldLine, m_lines[line]);
        return Position(line, column + text.length());
    }
}

void TextDocument::insertLines(int line, TextLineList newLines, bool after)
{
    TextLineList::Iterator it = m_lines.at(line);

    if (after)
        it++;

    TextLineList::Iterator newIt;

    for (newIt = newLines.begin(); newIt != newLines.end(); ++newIt)
        m_lines.insert(it, (*newIt));

    setModified(true);
    emit linesInserted(line, newLines);
}

void TextDocument::removeText(int startLine, int startColumn, int endLine, int endColumn)
{
    TextLine oldLine;

    if (startLine == endLine)
    {
        oldLine = m_lines[startLine];
        m_lines[startLine].text.remove(startColumn, endColumn - startColumn);
        setModified(true);
        emit lineChanged(startLine, oldLine, m_lines[startLine]);
    }
    else
    {
        oldLine = m_lines[startLine];
        m_lines[startLine].text.truncate(startColumn);
        m_lines[startLine].text += m_lines[endLine].text.mid(endColumn, m_lines[endLine].text.length());
        setModified(true);
        emit lineChanged(startLine, oldLine, m_lines[startLine]);

        removeLines(startLine + 1, endLine - startLine);
    }
}

void TextDocument::removeLines(int line, int count)
{
    TextLineList::Iterator it = m_lines.at(line);

    int i;

    for (i = count; i > 0 && it != m_lines.end(); --i)
    {
        m_lines.remove(it);
        it = m_lines.at(line);
    }

    setModified(true);
    emit linesRemoved(line, count - i);
}

void TextDocument::splitLine(int line, int column)
{
    TextLine oldLine = m_lines[line];
    TextLine newLine(oldLine.text.mid(column));
    oldLine.text.truncate(column);
    setLine(line, oldLine);
    insertLines(line, TextLineList() << newLine, TRUE);
}

void TextDocument::joinLines(int line)
{
    TextLine newLine(m_lines[line].text + m_lines[line + 1].text);
    setLine(line, newLine);
    removeLines(line + 1, 1);
}

void TextDocument::setModified(bool modified)
{
    if (modified)
    {
        m_modified = true;
        emit documentModified();
    }
    else
    {
        m_modified = false;
        emit documentNotModified();
    }
}
