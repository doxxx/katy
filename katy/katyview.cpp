#include "katyview.h"
#include "textdocument.h"
#include "texteditor.h"

#include <qlayout.h>

#include <kurl.h>

KatyView::KatyView(QWidget *parent)
    : QWidget(parent)
{
    // Setup our layout manager to automatically add our widgets
    QHBoxLayout *top_layout = new QHBoxLayout(this);
    top_layout->setAutoAdd(true);

    // Create our TextEditor widget
    m_editor = new TextEditor(this);
    m_document = new TextDocument();
    m_editor->setDocument(m_document);
}

KatyView::~KatyView()
{
}

KURL KatyView::currentURL()
{
    return m_document->url();
}

void KatyView::openURL(const KURL& url)
{
    if (m_document != NULL)
        delete m_document;

    m_document = new TextDocument(url);
    m_editor->setDocument(m_document);

    emit signalChangeCaption(url.url());
}

TextDocument::EOLType KatyView::eolType()
{
    return m_document->eolType();
}

void KatyView::setEOLType(TextDocument::EOLType type)
{
    m_document->setEOLType(type);
}

TextDocument *KatyView::document()
{
    return m_document;
}

TextEditor *KatyView::editor()
{
    return m_editor;
}

