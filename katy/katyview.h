#ifndef KATYVIEW_H
#define KATYVIEW_H

#include <qwidget.h>

#include "textdocument.h"

class QPainter;
class KURL;

class TextEditor;

/**
 * This is the main view class for Katy.
 *
 * @short Main view
 * @author Gordon Tyler <gtyler@iafrica.com>
 * @version 0.3
 */
class KatyView : public QWidget
{
    Q_OBJECT
public:
    KatyView(QWidget *parent);
    virtual ~KatyView();

    /**
     * Current document's URL
     */
    KURL currentURL();

    /**
     * Random 'set' function
     */
    void openURL(const KURL& url);

    /**
     * Get the End Of Line type
     */
    TextDocument::EOLType eolType();

    /**
     * Set the End Of Line type
     */
    void setEOLType(TextDocument::EOLType type);

    TextDocument *document();
    TextEditor *editor();

signals:
    /**
     * Change the content of the statusbar
     */
    void signalChangeStatusbar(const QString& text);

    /**
     * Change the content of the caption
     */
    void signalChangeCaption(const QString& text);

private:
    TextDocument *m_document;
    TextEditor *m_editor;
};

#endif // KATYVIEW_H
