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

    // attributes
    KURL url();
    int lineCount();
    TextLine line(int line);
    TextLineList::ConstIterator lineIterator(int line);
    TextLineList::ConstIterator endLineIterator();
    EOLType eolType();
    void setEOLType(EOLType type);

    // operations
    void openURL(const KURL& url);

protected:
    bool loadTempFile(QString filename);

protected:
    KURL m_url;
    TextLineList m_lines;
    EOLType m_eolType;
    bool m_loaded;
};

#endif
