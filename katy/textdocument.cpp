#include "textdocument.h"

#include <kdebug.h>
#include <kurl.h>
#include <kio/netaccess.h>
#include <qfile.h>
#include <qtextcodec.h>

TextDocument::TextDocument()
{
    m_url = KURL();
    m_eolType = EOL_LF;
    m_loaded = false;
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
        }

        // and remove the temp file
        KIO::NetAccess::removeTempFile(target);
    }
}

bool TextDocument::loadTempFile(QString filename)
{
    QFile file(filename);
    if (!file.open(IO_ReadOnly))
        return false;

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

        emit linesInserted(line + 1, insertedLines);

        insertText(line + lines.count() - 1, 0, lines[lines.count() - 1]);

        return Position(line + lines.count() - 1, lines[lines.count() - 1].length());
    }
    else
    {
        TextLine oldLine = m_lines[line];
        m_lines[line].text.insert(column, text);
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

    emit linesInserted(line, newLines);
}

void TextDocument::removeText(int startLine, int startColumn, int endLine, int endColumn)
{
    TextLine oldLine;

    if (startLine == endLine)
    {
        oldLine = m_lines[startLine];
        m_lines[startLine].text.remove(startColumn, endColumn - startColumn);
        emit lineChanged(startLine, oldLine, m_lines[startLine]);
    }
    else
    {
        oldLine = m_lines[startLine];
        m_lines[startLine].text.truncate(startColumn);
        m_lines[startLine].text += m_lines[endLine].text.mid(endColumn, m_lines[endLine].text.length());
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

