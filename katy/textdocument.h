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
    typedef QValueList<TextLine> TextLineList;

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
    int lineCount();
    TextLine line(int line);
    void setLine(int line, TextLine textLine);
    TextLineList::ConstIterator lineIterator(int line);
    TextLineList::ConstIterator endLineIterator();
    EOLType eolType();
    void setEOLType(EOLType type);
    QString text(int startLine, int startColumn, int endLine, int endColumn);

    void openURL(const KURL& url);

    void insertText(int line, int column, QString text);
    void insertLine(int line, TextLine newLine, bool after=FALSE);
    void removeLine(int line);
    void splitLine(int line, int column);
    void joinLines(int line);

signals:
    void lineChanged(int line, TextLine oldLine, TextLine newLine);
    void lineInserted(int line, TextLine newLine);
    void lineRemoved(int line);

protected:
    bool loadTempFile(QString filename);

protected:
    KURL m_url;
    TextLineList m_lines;
    EOLType m_eolType;
    bool m_loaded;
};

#endif
