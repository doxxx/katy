/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include "texteditor.h"

#include <kdebug.h>
#include <kglobalsettings.h>
#include <qpainter.h>

#include "textdocument.h"

TextEditor::TextEditor(QWidget *parent, QString name)
    : QScrollView(parent, name, WNorthWestGravity | WRepaintNoErase | WResizeNoErase)
{
    m_document = NULL;
    m_cursorLine = 0;
    m_cursorColumn = 0;
    m_cursorOn = FALSE;
    m_selectionAnchorLine = -1;
    m_selectionAnchorColumn = -1;
    m_selectionEndLine = -1;
    m_selectionEndColumn = -1;

    setMinimumWidth(500);
    setMinimumHeight(300);

    // Setup the viewport widget
    viewport()->setBackgroundMode(NoBackground); // NoBackground because drawContents paints every pixel
    viewport()->setFont(KGlobalSettings::fixedFont()); // TODO: Make configurable
    viewport()->setFocusPolicy(WheelFocus);
    viewport()->setCursor(ibeamCursor);

    viewport()->setFocus();

    // Start the timer which makes the cursor flash
    m_cursorTimerId = startTimer(250); // TODO: Make configurable
}

TextEditor::~TextEditor()
{
}

TextDocument *TextEditor::document()
{
    return m_document;
}

void TextEditor::setDocument(TextDocument *doc)
{
    m_document = doc;
    m_cursorLine = 0;
    m_cursorColumn = 0;
    m_selectionAnchorLine = -1;
    m_selectionAnchorColumn = -1;
    recalculateDocumentSize();
    viewport()->update();
}

SelectionRange TextEditor::selectionRange()
{
    SelectionRange range;

    if (m_selectionAnchorLine == -1 || m_selectionAnchorColumn == -1 || m_selectionEndLine == -1 || m_selectionEndColumn == -1)
    {
        range.hasSelection = FALSE;
    }
    else if (m_selectionAnchorLine < m_selectionEndLine)
    {
        range.startLine = m_selectionAnchorLine;
        range.startColumn = m_selectionAnchorColumn;
        range.endLine = m_selectionEndLine;
        range.endColumn = m_selectionEndColumn;
        range.hasSelection = TRUE;
    }
    else if (m_selectionAnchorLine > m_selectionEndLine)
    {
        range.startLine = m_selectionEndLine;
        range.startColumn = m_selectionEndColumn;
        range.endLine = m_selectionAnchorLine;
        range.endColumn = m_selectionAnchorColumn;
        range.hasSelection = TRUE;
    }
    else
    {
        range.startLine = range.endLine = m_selectionAnchorLine;

        if (m_selectionAnchorColumn < m_selectionEndColumn)
        {
            range.startColumn = m_selectionAnchorColumn;
            range.endColumn = m_selectionEndColumn;
            range.hasSelection = TRUE;
        }
        else if (m_selectionAnchorColumn > m_selectionEndColumn)
        {
            range.startColumn = m_selectionEndColumn;
            range.endColumn = m_selectionAnchorColumn;
            range.hasSelection = TRUE;
        }
        else
        {
            range.startColumn = range.endColumn = m_selectionAnchorColumn;
            range.hasSelection = FALSE;
        }
    }

    return range;
}

void TextEditor::moveCursorLeft(bool extendSelection)
{
    int cursorColumn = m_cursorColumn,
          cursorLine = m_cursorLine;

    if (cursorColumn > 0)
    {
        cursorColumn--;
    }
    else
    {
        if (cursorLine > 0)
        {
            cursorLine--;
            cursorColumn = m_document->line(cursorLine).text.length();
        }
    }

    if (extendSelection)
        extendSelectionTo(cursorLine, cursorColumn);
    else
        deselect();

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();
}

void TextEditor::moveCursorRight(bool extendSelection)
{
    int cursorColumn = m_cursorColumn,
          cursorLine = m_cursorLine,
          lineLength = m_document->line(cursorLine).text.length();

    if (cursorColumn < lineLength)
    {
        cursorColumn++;
    }
    else
    {
        if (cursorLine < m_document->lineCount() - 1)
        {
            cursorLine++;
            cursorColumn = 0;
        }
    }

    if (extendSelection)
        extendSelectionTo(cursorLine, cursorColumn);
    else
        deselect();

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();
}

void TextEditor::moveCursorUp(bool extendSelection)
{
    int cursorColumn = m_cursorColumn,
          cursorLine = m_cursorLine;

    if (cursorLine > 0)
    {
        cursorLine--;
        int lineLength = m_document->line(cursorLine).text.length();
        if (cursorColumn > lineLength)
            cursorColumn = lineLength;
    }

    if (extendSelection)
        extendSelectionTo(cursorLine, cursorColumn);
    else
        deselect();

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();
}

void TextEditor::moveCursorDown(bool extendSelection)
{
    int cursorColumn = m_cursorColumn,
          cursorLine = m_cursorLine;

    if (cursorLine < m_document->lineCount() - 1)
    {
        cursorLine++;
        int lineLength = m_document->line(cursorLine).text.length();
        if (cursorColumn > lineLength)
            cursorColumn = lineLength;
    }

    if (extendSelection)
        extendSelectionTo(cursorLine, cursorColumn);
    else
        deselect();

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();
}

void TextEditor::deselect()
{
    int topLine = m_selectionAnchorLine < m_selectionEndLine ? m_selectionAnchorLine : m_selectionEndLine;
    int bottomLine = m_selectionAnchorLine > m_selectionEndLine ? m_selectionAnchorLine : m_selectionEndLine;

    topLine = topLine < m_selectionAnchorLine ? topLine : m_selectionAnchorLine;
    topLine = topLine < m_selectionEndLine ? topLine : m_selectionEndLine;
    bottomLine = bottomLine > m_selectionAnchorLine ? bottomLine : m_selectionAnchorLine;
    bottomLine = bottomLine > m_selectionEndLine ? bottomLine : m_selectionEndLine;

    m_selectionAnchorLine = -1;
    m_selectionAnchorColumn = -1;
    m_selectionEndLine = -1;
    m_selectionEndColumn = -1;

    repaintLines(topLine, bottomLine);
}

void TextEditor::recalculateDocumentSize()
{
    if (m_document == NULL)
    {
        resizeContents(0, 0);
        return;
    }

    QFontMetrics fontMetrics = viewport()->fontMetrics();
    int documentWidth = 0;
    int documentHeight = 0;

    // Calculate the document width
    TextDocument::TextLineList::ConstIterator it;
    for (it = m_document->lineIterator(0); it != m_document->endLineIterator(); ++it)
    {
        QString text = (*it).text;
        int width = calculateTextWidth(fontMetrics, text);
        if (width > documentWidth)
            documentWidth = width;
    }

    // Calculate the document height
    documentHeight = m_document->lineCount() * fontMetrics.lineSpacing();

//    documentWidth = QMAX(documentWidth, viewport()->width());

    // Add a single pixel border on all sides
    //documentWidth += 2;
    //documentHeight += 2;

    // Tell the scroll widget the new size of our document
    resizeContents(documentWidth, documentHeight);
}

QRect TextEditor::calculateCursorRect(int cursorLine, int cursorColumn)
{
    QFontMetrics fontMetrics = viewport()->fontMetrics();
    QString text = m_document->line(cursorLine).text;
    int cursorX, cursorX1, cursorX2, cursorY1, cursorY2;

    cursorX = calculateTextWidth(fontMetrics, text, cursorColumn);
    cursorX1 = cursorX - 2;
    cursorX2 = cursorX + 2;
    if (cursorX1 < 0)
        cursorX1 = 0;
    cursorY1 = cursorLine * fontMetrics.lineSpacing();
    cursorY2 = cursorLine * fontMetrics.lineSpacing() + fontMetrics.height();

    return QRect(cursorX1, cursorY1, cursorX2 - cursorX1 + 1, cursorY2 - cursorY1 + 1);
}

void TextEditor::eraseCursor()
{
    m_cursorOn = FALSE;
    repaintContents(calculateCursorRect(m_cursorLine, m_cursorColumn), FALSE);
}

void TextEditor::drawCursor()
{
    m_cursorOn = TRUE;
    repaintContents(calculateCursorRect(m_cursorLine, m_cursorColumn), FALSE);
}

void TextEditor::ensureCursorVisible()
{
    QRect cursorRect = calculateCursorRect(m_cursorLine, m_cursorColumn);
    m_cursorOn = TRUE;

    int scrollX = 0, scrollY = 0;

    if (cursorRect.left() < contentsX())
    {
        while (cursorRect.left() < contentsX() + scrollX)
            scrollX += -100;
    }

    if (cursorRect.right() > contentsX() + visibleWidth() - 1)
    {
        while (cursorRect.right() > contentsX() + visibleWidth() - 1 + scrollX)
            scrollX += 100;
    }

    if (cursorRect.top() < contentsY())
    {
        while (cursorRect.top() < contentsY() + scrollY)
            scrollY += -viewport()->fontMetrics().lineSpacing();
    }

    if (cursorRect.bottom() > contentsY() + visibleHeight() - 1)
    {
        while (cursorRect.bottom() > contentsY() + visibleHeight() - 1 + scrollY)
            scrollY += viewport()->fontMetrics().lineSpacing();
    }

    repaintContents(cursorRect, FALSE);

    if (scrollX != 0 || scrollY != 0)
        scrollBy(scrollX, scrollY);
}

void TextEditor::extendSelectionTo(int line, int column)
{
    if (m_selectionAnchorLine < 0 || m_selectionAnchorColumn < 0)
    {
        m_selectionAnchorLine = m_cursorLine;
        m_selectionAnchorColumn = m_cursorColumn;
        m_selectionEndLine = m_cursorLine;
        m_selectionEndColumn = m_cursorColumn;
    }

    int topLine = m_selectionAnchorLine < m_selectionEndLine ? m_selectionAnchorLine : m_selectionEndLine;
    int bottomLine = m_selectionAnchorLine > m_selectionEndLine ? m_selectionAnchorLine : m_selectionEndLine;

    m_selectionEndLine = line;
    m_selectionEndColumn = column;

    topLine = topLine < m_selectionAnchorLine ? topLine : m_selectionAnchorLine;
    topLine = topLine < m_selectionEndLine ? topLine : m_selectionEndLine;
    bottomLine = bottomLine > m_selectionAnchorLine ? bottomLine : m_selectionAnchorLine;
    bottomLine = bottomLine > m_selectionEndLine ? bottomLine : m_selectionEndLine;

    repaintLines(topLine, bottomLine);
}

void TextEditor::repaintLines(int start, int end)
{
    int lineHeight = viewport()->fontMetrics().lineSpacing();
    int top = lineHeight * start;
    int height = (end - start + 1) * lineHeight;
    QRect rect(0, top, viewport()->width(), height);
    repaintContents(rect, FALSE);
}

int TextEditor::calculateTextWidth(QFontMetrics fontMetrics, QString text, int length)
{
    // TODO: Make tab stop ("fontMetrics.width(' ') * 8") configurable
    return fontMetrics.size(SingleLine | ExpandTabs, text, length, fontMetrics.width(' ') * 8).width();
}

void TextEditor::paintText(QPainter *p, int x, int y, int w, int h, QString text, int length)
{
    QFontMetrics fontMetrics = p->fontMetrics();
    int lineHeight = fontMetrics.lineSpacing();
    QChar c;
    int cx = x;
    QString temp;
    int drawTextLength = (length >= 0) ? length : text.length();

    for (int i = 0; i < drawTextLength; i++)
    {
        c = text[i];
        if (c == QChar(9))
        {
            if (temp.length() > 0)
            {
                int tempWidth = fontMetrics.width(temp);
                p->drawText(cx, y, tempWidth, lineHeight, AlignLeft | AlignTop, temp);
                cx += fontMetrics.width(temp);
                temp.truncate(0);
            }

            temp.fill(' ', 8 - (i % 8));
            int tempWidth = fontMetrics.width(temp);
            p->eraseRect(cx, y, tempWidth, lineHeight);
            cx += tempWidth;
            temp.truncate(0);
        }
        else
        {
            temp += c;
        }
    }
    p->drawText(x, y, w, h, ExpandTabs, text, length);
}

void TextEditor::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
//    kdDebug() << "drawContents " << cx << " " << cy << " " << cw << " " << ch << endl;

    p->setBackgroundMode(OpaqueMode);

    if (m_document != NULL)
    {
        QFontMetrics fontMetrics = p->fontMetrics();
        int lineHeight = fontMetrics.lineSpacing();
        int startLine = cy / lineHeight - 1;
        int endLine = (cy + ch) / lineHeight + 1;
        int viewWidth = viewport()->width();
        SelectionRange selRange = selectionRange();

        kdDebug() << "viewWidth = " << viewWidth << endl;

        if (startLine < 0)
            startLine = 0;

        p->setTabStops(fontMetrics.width(' ') * 8); // TODO: Make configurable

        int i;

        for (i = startLine; i < endLine && i < m_document->lineCount(); ++i)
        {
            QString text = m_document->line(i).text;

            if (selRange.hasSelection)
            {
                if (text.length() > 0)
                {
                    if (i == selRange.startLine && i == selRange.endLine)
                    {
                        p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                        int firstWidth = calculateTextWidth(fontMetrics, text, selRange.startColumn);
                        paintText(p, 0, lineHeight * i, firstWidth, lineHeight, text, selRange.startColumn);

                        p->setPen(KGlobalSettings::highlightedTextColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                        int secondWidth = calculateTextWidth(fontMetrics, text, selRange.endColumn) - firstWidth;
                        paintText(p, firstWidth, lineHeight * i, secondWidth, lineHeight, text.mid(selRange.startColumn, selRange.endColumn - selRange.startColumn));

                        p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                        int thirdWidth = calculateTextWidth(fontMetrics, text) - secondWidth - firstWidth;
                        paintText(p, firstWidth + secondWidth, lineHeight * i, thirdWidth, lineHeight, text.mid(selRange.endColumn));

                        p->eraseRect(firstWidth + secondWidth + thirdWidth, lineHeight * i, viewWidth - firstWidth + secondWidth + thirdWidth, lineHeight);
                    }
                    else if (i == selRange.startLine)
                    {
                        p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                        int firstWidth = calculateTextWidth(fontMetrics, text, selRange.startColumn);
                        paintText(p, 0, lineHeight * i, firstWidth, lineHeight, text, selRange.startColumn);

                        p->setPen(KGlobalSettings::highlightedTextColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                        int secondWidth = calculateTextWidth(fontMetrics, text) - firstWidth;
                        paintText(p, firstWidth, lineHeight * i, secondWidth, lineHeight, text.mid(selRange.startColumn));

                        p->eraseRect(firstWidth + secondWidth, lineHeight * i, viewWidth - firstWidth + secondWidth, lineHeight);
                    }
                    else if (i > selRange.startLine && i < selRange.endLine)
                    {
                        p->setPen(KGlobalSettings::highlightedTextColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                        int textWidth = calculateTextWidth(fontMetrics, text);
                        paintText(p, 0, lineHeight * i, textWidth, lineHeight, text);

                        kdDebug() << "Erasing " << textWidth << ", " << lineHeight * i << ", " << viewWidth - textWidth << ", " << lineHeight << endl;
                        p->eraseRect(textWidth, lineHeight * i, viewWidth - textWidth, lineHeight);
                    }
                    else if (i == selRange.endLine)
                    {
                        p->setPen(KGlobalSettings::highlightedTextColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                        int firstWidth = calculateTextWidth(fontMetrics, text, selRange.endColumn);
                        paintText(p, 0, lineHeight * i, firstWidth, lineHeight, text, selRange.endColumn);

                        p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                        int secondWidth = calculateTextWidth(fontMetrics, text) - firstWidth;
                        paintText(p, firstWidth, lineHeight * i, secondWidth, lineHeight, text.mid(selRange.endColumn));

                        p->eraseRect(firstWidth + secondWidth, lineHeight * i, viewWidth - firstWidth + secondWidth, lineHeight);
                    }
                    else
                    {
                        p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                        p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                        int textWidth = calculateTextWidth(fontMetrics, text);
                        // TODO: Replace this with our own text drawing function, maybe?
                        paintText(p, 0, lineHeight * i, textWidth, lineHeight, text);

                        p->eraseRect(textWidth, lineHeight * i, viewWidth - textWidth, lineHeight);
                    }
                }
                else
                {
                    if (i >= selRange.startLine && i < selRange.endLine)
                        p->setBackgroundColor(KGlobalSettings::highlightColor()); // TODO: Make configurable
                    else
                        p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable

                    p->eraseRect(0, lineHeight * i, viewWidth, lineHeight);
                }
            }
            else
            {
                if (text.length() > 0)
                {
                    p->setPen(KGlobalSettings::textColor()); // TODO: Make configurable
                    p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                    int textWidth = calculateTextWidth(fontMetrics, text);
                    // TODO: Replace this with our own text drawing function, maybe?
                    paintText(p, 0, lineHeight * i, textWidth, lineHeight, text);
                    // p->drawText(1, lineHeight + i * lineHeight, text); // TODO: Replace this with our own text drawing function

                    p->eraseRect(textWidth, lineHeight * i, viewWidth - textWidth, lineHeight);
                }
                else
                {
                    p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
                    p->eraseRect(0, lineHeight * i, viewWidth, lineHeight);
                }
            }
        }

        if (contentsHeight() < viewport()->height())
        {
            p->setBackgroundColor(KGlobalSettings::baseColor()); // TODO: Make configurable
            p->eraseRect(0, contentsHeight(), viewWidth, viewport()->height() - contentsHeight());
        }

        if (m_cursorOn)
        {
            QString text = m_document->line(m_cursorLine).text;
            int cursorX, cursorX1, cursorX2, cursorY1, cursorY2;

            cursorX = calculateTextWidth(fontMetrics, text, m_cursorColumn);
            cursorX1 = cursorX - 2;
            cursorX2 = cursorX + 2;
            cursorY1 = m_cursorLine * lineHeight;
            cursorY2 = m_cursorLine * lineHeight + fontMetrics.height();

            if (cursorX1 < 0)
                cursorX1 = 0;

            p->setPen(QColor(0, 0, 0));
            p->setRasterOp(NotXorROP);
            p->drawLine(cursorX1, cursorY1, cursorX2, cursorY1);
            p->drawLine(cursorX, cursorY1, cursorX, cursorY2);
            p->drawLine(cursorX1, cursorY2, cursorX2, cursorY2);
            p->setRasterOp(CopyROP);
        }
    }
}

void TextEditor::timerEvent(QTimerEvent *event)
{
    if (viewport()->hasFocus())
    {
        m_cursorOn = !m_cursorOn;
        updateContents(calculateCursorRect(m_cursorLine, m_cursorColumn));
    }
}

void TextEditor::keyPressEvent(QKeyEvent *event)
{
    bool shiftPressed = ((event->state() & ShiftButton) == ShiftButton);

    switch (event->key())
    {
        case Key_Left:
            moveCursorLeft(shiftPressed);
            break;

        case Key_Right:
            moveCursorRight(shiftPressed);
            break;

        case Key_Up:
            moveCursorUp(shiftPressed);
            break;

        case Key_Down:
            moveCursorDown(shiftPressed);
            break;

        default:
            event->ignore();
    }
}
