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

TextDocument::TextLineList::ConstIterator TextDocument::lineIterator(int line)
{
    return m_lines.at(line);
}

TextDocument::TextLineList::ConstIterator TextDocument::endLineIterator()
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

void TextDocument::insertText(int line, int column, QString text)
{*******************************************************************************
    QStringList lines = QStringList::split(QChar('\n'), text, TRUE);
    if (lines.count() == 1)
    {

    }
    else
    {
        splitLine(line, column);
        insertText(line, column,
        for (QStringList::Iterator it = lines.begin(); it != lines.end(); ++it)
        {
        }
    }
}

void TextDocument::insertLine(int line, TextLine newLine, bool after)
{
    TextDocument::TextLineList::Iterator it = m_lines.at(line);

    if (after)
        it++;

    m_lines.insert(it, newLine);

    emit lineInserted(line, newLine);
}

void TextDocument::removeLine(int line)
{
    TextDocument::TextLineList::Iterator it = m_lines.at(line);

    m_lines.remove(it);

    emit lineRemoved(line);
}

void TextDocument::splitLine(int line, int column)
{
    TextLine oldLine = m_lines[line];
    TextLine newLine(oldLine.text.mid(column));
    oldLine.text.truncate(column);
    setLine(line, oldLine);
    insertLine(line, newLine, TRUE);
}

void TextDocument::joinLines(int line)
{
    TextLine newLine(m_lines[line].text + m_lines[line + 1].text);
    setLine(line, newLine);
    removeLine(line + 1);
}

