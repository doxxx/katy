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
#include <qregexp.h>

#define STAT_TIMER_INTERVAL 3000

TextDocument::TextDocument()
{
    m_url = KURL();
    m_eolType = EOL_LF;
    m_loaded = FALSE;
    m_modified = FALSE;
    m_lines.append(TextLine(QString("")));
    m_statTimerId = -1;
    m_lastModifiedTime = -1;
}

TextDocument::TextDocument(const KURL& url)
{
    TextDocument();

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

void TextDocument::setModified(bool modified)
{
    if (modified)
    {
        m_modified = true;
        emit documentModified();
    }
    else
    {
        m_modified = FALSE;
        emit documentNotModified();
    }
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
            setModified(FALSE);

            // start external change detection
            KIO::StatJob *job = KIO::stat(m_url, FALSE);
            connect(job, SIGNAL(result(KIO::Job*)), SLOT(slotStatJobResult(KIO::Job*)));
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
        return FALSE;
    }

    QTextCodec *codec = QTextCodec::codecForLocale();
    QTextDecoder *decoder = codec->makeDecoder();

    QStringList lines;
    QString line;
    QChar c;
    EOLType eolType = EOL_Unknown;
    bool foundEOL = FALSE;
    bool mixedEOLType = FALSE;

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
            foundEOL = FALSE;
        }
        else
        {
            line += c;
        }
    }

    file.close();

    if (line.length() > 0)
        lines.append(line);

    m_lines.clear();

    for (QStringList::Iterator it = lines.begin(); it != lines.end(); ++it)
    {
        TextLine textLine((*it));
        m_lines.append(textLine);
    }

    if (mixedEOLType)
        m_eolType = EOL_Unknown;
    else
        m_eolType = eolType;

    return TRUE;
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
        if (KIO::NetAccess::exists(url))
        {
            if (!KIO::NetAccess::del(url))
            {
                kdError() << "TextDocument::saveURL(): Could not delete destination file " << url.prettyURL() << endl;
                return;
            }
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
    setModified(FALSE);

    tempfile.unlink();

    m_lastModifiedTime = -1;
}

bool TextDocument::saveTempFile(QString filename)
{
    QFile file(filename);
    if (!file.open(IO_WriteOnly))
    {
        kdError() << "TextDocument::saveURL(): Could not open temporary file "  << filename << " for writing" << endl;
        return FALSE;
    }

    QTextStream textStream(&file);

    TextLineList::Iterator it = m_lines.begin();

    for (uint count = 0; it != m_lines.end(); ++it, ++count)
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

    return TRUE;
}

DocumentPosition TextDocument::insertText(int line, int column, QString text)
{
    if (text.contains(QChar('\n')))
    {
        QStringList lines = QStringList::split(QChar('\n'), text, TRUE);

        splitLine(line, column);
        insertText(line, column, lines[0]);

        TextLineList insertedLines;
        TextLineList::Iterator it = m_lines.at(line + 1);

        for (uint i = 1; i < lines.count() - 1; ++i)
        {
            TextLine textLine(lines[i]);
            insertedLines << textLine;
            m_lines.insert(it, textLine);
        }

        setModified(TRUE);
        emit linesInserted(line + 1, insertedLines);

        insertText(line + lines.count() - 1, 0, lines[lines.count() - 1]);

        return DocumentPosition(line + lines.count() - 1, lines[lines.count() - 1].length());
    }
    else
    {
        TextLine oldLine = m_lines[line];
        m_lines[line].text.insert(column, text);
        setModified(TRUE);
        emit lineChanged(line, oldLine, m_lines[line]);
        return DocumentPosition(line, column + text.length());
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

    setModified(TRUE);
    emit linesInserted(line, newLines);
}

void TextDocument::removeText(int startLine, int startColumn, int endLine, int endColumn)
{
    TextLine oldLine;

    if (startLine == endLine)
    {
        oldLine = m_lines[startLine];
        m_lines[startLine].text.remove(startColumn, endColumn - startColumn);
        setModified(TRUE);
        emit lineChanged(startLine, oldLine, m_lines[startLine]);
    }
    else
    {
        oldLine = m_lines[startLine];
        m_lines[startLine].text.truncate(startColumn);
        m_lines[startLine].text += m_lines[endLine].text.mid(endColumn, m_lines[endLine].text.length());
        setModified(TRUE);
        emit lineChanged(startLine, oldLine, m_lines[startLine]);

        removeLines(startLine + 1, endLine - startLine);
    }
}

void TextDocument::removeLines(int startLine, int count)
{
    TextLineList::Iterator it = m_lines.at(startLine);

    int i;

    for (i = 0; i < count && it != m_lines.end(); ++i)
    {
        m_lines.remove(it);
        it = m_lines.at(startLine);
    }

    setModified(TRUE);
    emit linesRemoved(startLine, count - i);
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

DocumentPosition TextDocument::insertTab(int line, int column, bool useSpaces, int indentSize)
{
    DocumentPosition retPos(line, column);
    QString spaces;
    spaces.fill(' ', indentSize);

    TextLine newLine = m_lines[line];
    if (useSpaces)
    {
        newLine.text.insert(column, spaces);
        retPos.column += spaces.length();
    }
    else
    {
        newLine.text.insert(column, '\t');
        retPos.column++;
    }

    setLine(line, newLine);

    return retPos;
}

void TextDocument::indentLines(int startLine, int count, bool useSpaces, int indentSize)
{
    TextLineList::Iterator it = m_lines.at(startLine);

    int i;
    QString spaces;

    spaces.fill(' ', indentSize);

    for (i = 0; i < count && it != m_lines.end(); ++i, ++it)
    {
        TextLine newLine = *it;
        if (useSpaces)
            newLine.text.prepend(spaces);
        else
            newLine.text.prepend('\t');
        setLine(startLine + i, newLine);
    }
}

void TextDocument::unindentLines(int startLine, int count, bool useSpaces, int indentSize)
{
    kdError() << "Not implemented" << endl;
}

void TextDocument::tabsToSpaces(int numberOfSpaces, bool leadingTabsOnly)
{
    TextLineList::Iterator it = m_lines.at(0);

    QString spaces;

    spaces.fill(' ', numberOfSpaces);

    for (int lineIndex = 0; it != m_lines.end(); ++it, ++lineIndex)
    {
        TextLine newLine = *it;
        for (uint i = 0; i < newLine.text.length(); i++)
        {
            QChar c = newLine.text[i];
            if (c == QChar(9))
            {
                newLine.text.replace(i, 1, spaces);
                i += numberOfSpaces - 1;
            }
            else if (!c.isSpace() && leadingTabsOnly)
            {
                break;
            }
        }
        setLine(lineIndex, newLine);
    }
}

void TextDocument::spacesToTabs(int numberOfSpaces, bool leadingSpacesOnly)
{
    TextLineList::Iterator it = m_lines.at(0);

    QString spaces;

    spaces.fill(' ', numberOfSpaces);

    for (int lineIndex = 0; it != m_lines.end(); ++it, ++lineIndex)
    {
        TextLine newLine = *it;
        for (uint i = 0; i < newLine.text.length(); i++)
        {
            QChar c = newLine.text[i];
            if (c.isSpace() && newLine.text.mid(i, numberOfSpaces) == spaces)
            {
                newLine.text.replace(i, numberOfSpaces, QString("\t"));
            }
            else if (!c.isSpace() && leadingSpacesOnly)
            {
                break;
            }
        }
        setLine(lineIndex, newLine);
    }
}

DocumentRange TextDocument::findText(QString text, DocumentPosition start, int flags)
{
    bool backward = (flags & Backward) == Backward;
    bool caseSensitive = (flags & CaseSensitive) == CaseSensitive;
    bool regularExpression = (flags & RegularExpression) == RegularExpression;
    QRegExp regExp;

    if (regularExpression)
    {
        regExp.setCaseSensitive(caseSensitive);
        regExp.setPattern(text);
    }

    TextLineList::Iterator it = m_lines.at(start.line);

    if (backward)
    {
        for (int lineIndex = start.line; it != m_lines.begin(); --it, --lineIndex)
        {
            QString lineText = (*it).text;

            int charIndex;
            int matchLen;

            if (lineIndex == start.line)
            {
                if (regularExpression)
                {
                    charIndex = lineText.findRev(regExp, start.column);
                    if (charIndex >= 0)
                        regExp.match(lineText, charIndex, &matchLen);
                }
                else
                {
                    charIndex = lineText.findRev(text, start.column, caseSensitive);
                }
            }
            else
            {
                if (regularExpression)
                {
                    charIndex = lineText.findRev(regExp, -1);
                    if (charIndex >= 0)
                        regExp.match(lineText, charIndex, &matchLen);
                }
                else
                {
                    charIndex = lineText.findRev(text, -1, caseSensitive);
                }
            }

            if (charIndex >= 0)
            {
                return DocumentRange(lineIndex, charIndex, lineIndex, charIndex + text.length());
            }
        }
    }
    else
    {
        for (int lineIndex = start.line; it != m_lines.end(); ++it, ++lineIndex)
        {
            QString lineText = (*it).text;

            int charIndex;
            int matchLen = text.length();

            if (lineIndex == start.line)
            {
                if (regularExpression)
                    charIndex = regExp.match(lineText, start.column, &matchLen);
                else
                    charIndex = lineText.find(text, start.column, caseSensitive);
            }
            else
            {
                if (regularExpression)
                    charIndex = regExp.match(lineText, 0, &matchLen);
                else
                    charIndex = lineText.find(text, 0, caseSensitive);
            }

            if (charIndex >= 0)
            {
                return DocumentRange(lineIndex, charIndex, lineIndex, charIndex + matchLen);
            }
        }
    }

    return DocumentRange();
}

void TextDocument::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_statTimerId)
    {
        killTimer(m_statTimerId);
        KIO::StatJob *job = KIO::stat(m_url, FALSE);
        connect(job, SIGNAL(result(KIO::Job*)), SLOT(slotStatJobResult(KIO::Job*)));
    }
}

void TextDocument::slotStatJobResult(KIO::Job *job)
{
    KIO::StatJob *statJob = (KIO::StatJob*)job;

    if (statJob->error() == 0)
    {
        const KIO::UDSEntry& entry = statJob->statResult();
        KIO::UDSEntry::ConstIterator it;
        for (it = entry.begin(); it != entry.end(); ++it)
        {
            KIO::UDSAtom atom = *it;
            if ((atom.m_uds & KIO::UDS_MODIFICATION_TIME) == KIO::UDS_MODIFICATION_TIME)
            {
                if (m_lastModifiedTime < 0)
                {
                    m_lastModifiedTime = atom.m_long;
                    break;
                }
                else if (atom.m_long > m_lastModifiedTime)
                {
                    emit documentExternallyChanged();
                    m_lastModifiedTime = atom.m_long;
                    break;
                }
            }
        }
    }

    m_statTimerId = startTimer(STAT_TIMER_INTERVAL);
}

