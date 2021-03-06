/*
 * Class for Text Editor widget
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

#include "katyapp.h"
#include "texteditor.h"
#include "textdocument.h"

#include <kdebug.h>
#include <kglobalsettings.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qclipboard.h>

TextEditor::TextEditor(QWidget *parent, QString name)
        : QScrollView(parent, name, WNorthWestGravity | WRepaintNoErase | WResizeNoErase) {
    m_document = NULL;
    m_cursorLine = 0;
    m_cursorColumn = 0;
    m_cursorOn = FALSE;
    m_selectionAnchorLine = -1;
    m_selectionAnchorColumn = -1;
    m_selectionEndLine = -1;
    m_selectionEndColumn = -1;

    setMinimumWidth(100);
    setMinimumHeight(100);

    // Setup the viewport widget
    viewport()->setBackgroundMode(NoBackground); // NoBackground because drawContents paints every pixel
    viewport()->setFocusPolicy(WheelFocus);
    viewport()->setCursor(ibeamCursor);

    // Set the font
    viewport()->setFont(katyapp->readConfig_Font());
    verticalScrollBar()->setLineStep(viewport()->fontMetrics().lineSpacing());

    viewport()->setFocus();

    // Start the timer which makes the cursor flash
    // TODO: Make configurable (flash enabled && flash speed)
    m_cursorTimerId = startTimer(250);
    if (m_cursorTimerId == 0) {
        kdWarning() << "Could not start timer for cursor flashing" << endl;
        m_cursorOn = TRUE;
    }

    connect(katyapp, SIGNAL(configChanged()), SLOT(refresh()));
}

TextEditor::~TextEditor() {
    if (m_cursorTimerId != 0) {
        killTimer(m_cursorTimerId);
    }
}

void TextEditor::setDocument(TextDocument *doc) {
    m_document = doc;

    connect(m_document, SIGNAL(lineChanged(int, TextLine, TextLine)), SLOT(document_lineChanged(int, TextLine, TextLine)));
    connect(m_document, SIGNAL(linesInserted(int, TextLineList)), SLOT(document_linesInserted(int, TextLineList)));
    connect(m_document, SIGNAL(linesRemoved(int, int)), SLOT(document_linesRemoved(int, int)));

    resetView();
}

SelectionRange TextEditor::selectionRange() {
    SelectionRange range;

    if (m_selectionAnchorLine == -1 || m_selectionAnchorColumn == -1 || m_selectionEndLine == -1 || m_selectionEndColumn == -1) {
        range.hasSelection = FALSE;
    } else if (m_selectionAnchorLine < m_selectionEndLine) {
        range.startLine = m_selectionAnchorLine;
        range.startColumn = m_selectionAnchorColumn;
        range.endLine = m_selectionEndLine;
        range.endColumn = m_selectionEndColumn;
        range.hasSelection = TRUE;
    } else if (m_selectionAnchorLine > m_selectionEndLine) {
        range.startLine = m_selectionEndLine;
        range.startColumn = m_selectionEndColumn;
        range.endLine = m_selectionAnchorLine;
        range.endColumn = m_selectionAnchorColumn;
        range.hasSelection = TRUE;
    } else {
        range.startLine = range.endLine = m_selectionAnchorLine;

        if (m_selectionAnchorColumn < m_selectionEndColumn) {
            range.startColumn = m_selectionAnchorColumn;
            range.endColumn = m_selectionEndColumn;
            range.hasSelection = TRUE;
        } else if (m_selectionAnchorColumn > m_selectionEndColumn) {
            range.startColumn = m_selectionEndColumn;
            range.endColumn = m_selectionAnchorColumn;
            range.hasSelection = TRUE;
        } else {
            range.startColumn = range.endColumn = m_selectionAnchorColumn;
            range.hasSelection = FALSE;
        }
    }

    return range;
}

QString TextEditor::selectedText() {
    SelectionRange range = selectionRange();

    if (!range.hasSelection) {
        return NULL;
    }

    return m_document->text(range.startLine, range.startColumn, range.endLine, range.endColumn);
}

void TextEditor::setSelectedText(QString newText) {
    deleteSelection();
    eraseCursor();
    DocumentPosition pos = m_document->insertText(m_cursorLine, m_cursorColumn, newText);
    m_cursorLine = pos.line;
    m_cursorColumn = pos.column;
    ensureCursorVisible();
}

void TextEditor::moveCursorTo(int line, int column, bool extendSelection ) {
    int cursorColumn = column,
                       cursorLine = line;

    if (cursorLine < 0) {
        cursorLine = 0;
    } else if (cursorLine >= m_document->lineCount()) {
        cursorLine = m_document->lineCount() - 1;
    }

    int lineLength = m_document->line(cursorLine).text.length();

    if (cursorColumn < 0) {
        cursorColumn = 0;
    } else if (cursorColumn > lineLength) {
        cursorColumn = lineLength;
    }

    if (extendSelection) {
        extendSelectionTo(cursorLine, cursorColumn);
    } else {
        deselect();
    }

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::moveCursorLeft(bool extendSelection) {
    int cursorColumn = m_cursorColumn,
                       cursorLine = m_cursorLine;

    if (cursorColumn > 0) {
        cursorColumn--;
    } else {
        if (cursorLine > 0) {
            cursorLine--;
            cursorColumn = m_document->line(cursorLine).text.length();
        }
    }

    if (extendSelection) {
        extendSelectionTo(cursorLine, cursorColumn);
    } else {
        deselect();
    }

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::moveCursorRight(bool extendSelection) {
    int cursorColumn = m_cursorColumn,
                       cursorLine = m_cursorLine,
                                    lineLength = m_document->line(cursorLine).text.length();

    if (cursorColumn < lineLength) {
        cursorColumn++;
    } else {
        if (cursorLine < m_document->lineCount() - 1) {
            cursorLine++;
            cursorColumn = 0;
        }
    }

    if (extendSelection) {
        extendSelectionTo(cursorLine, cursorColumn);
    } else {
        deselect();
    }

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::moveCursorWordLeft(bool extendSelection) {
    int cursorColumn = m_cursorColumn,
                       cursorLine = m_cursorLine;

    QString text = m_document->line(cursorLine).text;

    if (cursorColumn == 0 || text.length() == 0) {
        if (cursorLine > 0) {
            cursorLine--;
            text = m_document->line(cursorLine).text;
            cursorColumn = text.length();
        } else {
            return;
        }
    } else {
        DocumentPosition wordStart = m_document->findWordStart(DocumentPosition(cursorLine, cursorColumn));
        cursorColumn = wordStart.column;
        cursorLine = wordStart.line;
    }

    if (extendSelection) {
        extendSelectionTo(cursorLine, cursorColumn);
    } else {
        deselect();
    }

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::moveCursorWordRight(bool extendSelection) {
    int cursorColumn = m_cursorColumn,
                       cursorLine = m_cursorLine;

    QString text = m_document->line(cursorLine).text;

    if (cursorColumn == text.length()) {
        if (cursorLine < m_document->lineCount() - 1) {
            cursorLine++;
            cursorColumn = 0;
        } else {
            return;
        }
    } else {
        DocumentPosition wordStart = m_document->findWordStart(DocumentPosition(cursorLine, cursorColumn), FALSE);
        cursorColumn = wordStart.column;
        cursorLine = wordStart.line;
    }

    if (extendSelection) {
        extendSelectionTo(cursorLine, cursorColumn);
    } else {
        deselect();
    }

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::moveCursorUp(bool extendSelection) {
    int cursorColumn = m_cursorColumn,
                       cursorLine = m_cursorLine;

    if (cursorLine > 0) {
        cursorLine--;
        int lineLength = m_document->line(cursorLine).text.length();
        if (cursorColumn > lineLength)
            cursorColumn = lineLength;
    }

    if (extendSelection) {
        extendSelectionTo(cursorLine, cursorColumn);
    } else {
        deselect();
    }

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::moveCursorDown(bool extendSelection) {
    int cursorColumn = m_cursorColumn,
                       cursorLine = m_cursorLine;

    if (cursorLine < m_document->lineCount() - 1) {
        cursorLine++;
        int lineLength = m_document->line(cursorLine).text.length();
        if (cursorColumn > lineLength)
            cursorColumn = lineLength;
    }

    if (extendSelection) {
        extendSelectionTo(cursorLine, cursorColumn);
    } else {
        deselect();
    }

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::moveCursorPageUp(bool extendSelection ) {
    int cursorColumn = m_cursorColumn,
                       cursorLine = m_cursorLine,
                                    pageHeightLines = viewport()->height() / viewport()->fontMetrics().lineSpacing();

    if (cursorLine >= pageHeightLines) {
        cursorLine -= pageHeightLines;
    } else {
        cursorLine = 0;
    }

    int lineLength = m_document->line(cursorLine).text.length();
    if (cursorColumn > lineLength) {
        cursorColumn = lineLength;
    }

    if (extendSelection) {
        extendSelectionTo(cursorLine, cursorColumn);
    } else {
        deselect();
    }

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    scrollBy(0, -(pageHeightLines * viewport()->fontMetrics().lineSpacing()));

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::moveCursorPageDown(bool extendSelection ) {
    int cursorColumn = m_cursorColumn,
                       cursorLine = m_cursorLine,
                                    pageHeightLines = viewport()->height() / viewport()->fontMetrics().lineSpacing();

    if (cursorLine < m_document->lineCount() - pageHeightLines) {
        cursorLine += pageHeightLines;
    } else {
        cursorLine = m_document->lineCount() - 1;
    }

    int lineLength = m_document->line(cursorLine).text.length();
    if (cursorColumn > lineLength) {
        cursorColumn = lineLength;
    }

    if (extendSelection) {
        extendSelectionTo(cursorLine, cursorColumn);
    } else {
        deselect();
    }

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    scrollBy(0, pageHeightLines * viewport()->fontMetrics().lineSpacing());

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::moveCursorHome(bool extendSelection) {
    int cursorColumn = 0,
                       cursorLine = m_cursorLine;

    if (extendSelection) {
        extendSelectionTo(cursorLine, cursorColumn);
    } else {
        deselect();
    }

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::moveCursorEnd(bool extendSelection) {
    int cursorColumn = m_document->line(m_cursorLine).text.length(),
                       cursorLine = m_cursorLine;

    if (extendSelection) {
        extendSelectionTo(cursorLine, cursorColumn);
    } else {
        deselect();
    }

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::moveCursorDocumentStart(bool extendSelection ) {
    int cursorColumn = 0,
                       cursorLine = 0;

    if (extendSelection) {
        extendSelectionTo(cursorLine, cursorColumn);
    } else {
        deselect();
    }

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::moveCursorDocumentEnd(bool extendSelection ) {
    int cursorColumn = m_document->line(m_document->lineCount() - 1).text.length(),
                       cursorLine = m_document->lineCount() - 1;

    if (extendSelection) {
        extendSelectionTo(cursorLine, cursorColumn);
    } else {
        deselect();
    }

    eraseCursor();

    m_cursorColumn = cursorColumn;
    m_cursorLine = cursorLine;

    ensureCursorVisible();

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::deselect() {
    int topLine = m_selectionAnchorLine < m_selectionEndLine ? m_selectionAnchorLine : m_selectionEndLine;
    int bottomLine = m_selectionAnchorLine > m_selectionEndLine ? m_selectionAnchorLine : m_selectionEndLine;

    m_selectionAnchorLine = -1;
    m_selectionAnchorColumn = -1;
    m_selectionEndLine = -1;
    m_selectionEndColumn = -1;

    repaintLines(topLine, bottomLine);
}

void TextEditor::resetView() {
    m_cursorLine = 0;
    m_cursorColumn = 0;
    m_selectionAnchorLine = -1;
    m_selectionAnchorColumn = -1;

    recalculateDocumentSize();

    setContentsPos(0, 0);

    viewport()->update();

    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::cut() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(selectedText());
    deleteSelection();
}

void TextEditor::copy() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(selectedText());
}

void TextEditor::paste() {
    QClipboard *clipboard = QApplication::clipboard();
    QString text = clipboard->text();

    deleteSelection();

    eraseCursor();
    DocumentPosition pos = m_document->insertText(m_cursorLine, m_cursorColumn, text);
    m_cursorLine = pos.line;
    m_cursorColumn = pos.column;
    ensureCursorVisible();
    emit cursorMoved(m_cursorLine, m_cursorColumn);
}

void TextEditor::selectAll() {
    m_selectionAnchorColumn = m_selectionAnchorLine = 0;
    m_selectionEndLine = m_document->lineCount() - 1;
    m_selectionEndColumn = m_document->line(m_document->lineCount() - 1).text.length();
    repaintLines(0, m_document->lineCount() - 1);
}

void TextEditor::refresh() {
    viewport()->setFont(katyapp->readConfig_Font());
    recalculateDocumentSize();
    ensureCursorVisible();
    viewport()->update();
}

void TextEditor::document_lineChanged(int line, TextLine oldLine, TextLine newLine) {
    recalculateDocumentSize(oldLine.text, newLine.text);
    repaintLines(line, line);
}

void TextEditor::document_linesInserted(int line, TextLineList newLines) {
    if (newLines.count() == 0) {
        recalculateDocumentSize(newLines[0].text);
    } else {
        recalculateDocumentSize();
    }
    repaintLines(line, m_document->lineCount() - 1);
}

void TextEditor::document_linesRemoved(int line, int count) {
    recalculateDocumentSize();
    repaintLines(line, m_document->lineCount() - 1);
}

void TextEditor::recalculateDocumentSize() {
    if (m_document == NULL) {
        resizeContents(0, 0);
        return;
    }

    QFontMetrics fontMetrics = viewport()->fontMetrics();
    int documentWidth = 0;
    int documentHeight = 0;

    // Calculate the document width
    TextLineList::ConstIterator it;
    for (it = m_document->lineIterator(0); it != m_document->endLineIterator(); ++it) {
        QString text = (*it).text;
        int width = calculateTextWidth(fontMetrics, text);
        if (width > documentWidth) {
            documentWidth = width;
        }
    }

    // Calculate the document height
    documentHeight = m_document->lineCount() * fontMetrics.lineSpacing();

    // Tell the scroll widget the new size of our document
    resizeContents(documentWidth, documentHeight);
}

void TextEditor::recalculateDocumentSize(QString newLineText) {
    if (m_document == NULL) {
        resizeContents(0, 0);
        return;
    }

    QFontMetrics fontMetrics = viewport()->fontMetrics();
    int documentHeight = contentsHeight() + fontMetrics.lineSpacing();
    int documentWidth = contentsWidth();
    int newLineWidth = calculateTextWidth(fontMetrics, newLineText);

    if (newLineWidth > documentWidth) {
        resizeContents(newLineWidth, documentHeight);
    } else { 
        resizeContents(documentWidth, documentHeight);
    }
}
void TextEditor::recalculateDocumentSize(QString oldLineText, QString newLineText) {
    if (m_document == NULL) {
        resizeContents(0, 0);
        return;
    }

    QFontMetrics fontMetrics = viewport()->fontMetrics();
    int documentWidth = contentsWidth();
    int oldLineWidth = calculateTextWidth(fontMetrics, oldLineText);
    int newLineWidth = calculateTextWidth(fontMetrics, newLineText);

    if (newLineWidth > oldLineWidth && newLineWidth > documentWidth) {
        resizeContents(newLineWidth, contentsHeight());
    } else if (newLineWidth < oldLineWidth) {
        recalculateDocumentSize();
    }
}

QRect TextEditor::calculateCursorRect(int cursorLine, int cursorColumn, int *cursorMiddleX) {
    QFontMetrics fontMetrics = viewport()->fontMetrics();
    QString text = m_document->line(cursorLine).text;
    int cursorX, cursorX1, cursorX2, cursorY1, cursorY2;

    cursorX = calculateTextWidth(fontMetrics, text, cursorColumn);
    cursorX1 = cursorX - 2;
    cursorX2 = cursorX + 2;
    if (cursorX1 < 0) {
        cursorX1 = 0;
    }
    cursorY1 = cursorLine * fontMetrics.lineSpacing();
    cursorY2 = cursorLine * fontMetrics.lineSpacing() + fontMetrics.height();

    if (cursorMiddleX != NULL) {
        *cursorMiddleX = cursorX;
    }

    return QRect(cursorX1, cursorY1, cursorX2 - cursorX1 + 1, cursorY2 - cursorY1 + 1);
}

void TextEditor::eraseCursor() {
    m_cursorOn = FALSE;
    repaintContents(calculateCursorRect(m_cursorLine, m_cursorColumn), FALSE);
}

void TextEditor::drawCursor() {
    m_cursorOn = TRUE;
    repaintContents(calculateCursorRect(m_cursorLine, m_cursorColumn), FALSE);
}

void TextEditor::ensureCursorVisible() {
    QRect cursorRect = calculateCursorRect(m_cursorLine, m_cursorColumn);
    m_cursorOn = TRUE;

    int scrollX = 0, scrollY = 0;

    if (cursorRect.left() < contentsX()) {
        while (cursorRect.left() < contentsX() + scrollX) {
            scrollX += -100;
        }
    }

    if (cursorRect.right() > contentsX() + visibleWidth() - 1) {
        while (cursorRect.right() > contentsX() + visibleWidth() - 1 + scrollX) {
            scrollX += 100;
        }
    }

    if (cursorRect.top() < contentsY()) {
        while (cursorRect.top() < contentsY() + scrollY) {
            scrollY += -viewport()->fontMetrics().lineSpacing();
        }
    }

    if (cursorRect.bottom() > contentsY() + visibleHeight() - 1) {
        while (cursorRect.bottom() > contentsY() + visibleHeight() - 1 + scrollY) {
            scrollY += viewport()->fontMetrics().lineSpacing();
        }
    }

    repaintContents(cursorRect, FALSE);

    if (scrollX != 0 || scrollY != 0) {
        scrollBy(scrollX, scrollY);
    }
}

void TextEditor::extendSelectionTo(int line, int column) {
    int topLine, bottomLine;
    int leftCol, rightCol;

    if (m_selectionAnchorLine < 0) {
        m_selectionAnchorLine = m_selectionEndLine = m_cursorLine;
        m_selectionAnchorColumn = m_selectionEndColumn = m_cursorColumn;
    }

    topLine = bottomLine = m_selectionEndLine;
    leftCol = rightCol = m_selectionEndColumn;

    m_selectionEndLine = line;
    m_selectionEndColumn = column;

    topLine = topLine < m_selectionEndLine ? topLine : m_selectionEndLine;
    bottomLine = bottomLine > m_selectionEndLine ? bottomLine : m_selectionEndLine;
    leftCol = leftCol < m_selectionEndColumn ? leftCol : m_selectionEndColumn;
    rightCol = rightCol > m_selectionEndColumn ? rightCol : m_selectionEndColumn;

    if (topLine != bottomLine) {
        repaintLines(topLine, bottomLine);
    } else {
        QFontMetrics fontMetrics = viewport()->fontMetrics();
        QString text = m_document->line(topLine).text;
        int lineHeight = fontMetrics.lineSpacing();
        int top = lineHeight * topLine;
        int left = calculateTextWidth(fontMetrics, text, leftCol);
        int right = calculateTextWidth(fontMetrics, text, rightCol);
        int width = right - left + 1;
        QRect rect(left, top, width, lineHeight);
        repaintContents(rect, FALSE);
    }
}

void TextEditor::repaintLines(int start, int end) {
    int lineHeight = viewport()->fontMetrics().lineSpacing();
    int top = lineHeight * start;
    int height = (end - start + 1) * lineHeight;
    QRect rect(0, top, QMAX(contentsWidth(), visibleWidth()), height);
    repaintContents(rect, FALSE);
}

int TextEditor::calculateTextWidth(QFontMetrics fontMetrics, QString text, int length) {
    QChar c;
    int cx = 0;
    QString temp;
    int drawTextLength = (length >= 0) ? length : text.length();
    int tabSize = katyapp->readConfig_TabSize();
    int column = 0;

    for (int i = 0; i < drawTextLength; i++) {
        c = text[i];
        if (c == QChar(9)) {
            if (temp.length() > 0) {
                int tempWidth = fontMetrics.width(temp);
                cx += tempWidth;
                temp.truncate(0);
            }

            int tabWidth = fontMetrics.width(' ') * (tabSize - (column % tabSize));
            cx += tabWidth;
            column += tabSize - (column % tabSize);
        } else {
            temp += c;
            column++;
        }
    }

    if (temp.length() > 0) {
        int tempWidth = fontMetrics.width(temp);
        cx += tempWidth;
    }

    return cx;
}

void TextEditor::paintText(QPainter *p, int x, int y, QString text, int start, int end) {
    QFontMetrics fontMetrics = p->fontMetrics();
    int lineHeight = fontMetrics.lineSpacing();
    QChar c;
    int cx = x;
    QString temp;
    int stop = (end >= 0) ? end : text.length();
    int tabSize = katyapp->readConfig_TabSize();
    int column = 0;
    int i;

    // first calculate the starting column for later tab size calculation
    for (i = 0;  i < start; i++) {
        c = text[i];
        if (c == QChar(9)) {
            column += tabSize - (column % tabSize);
        } else {
            column++;
        }
    }

    // draw characters
    for (i = start; i < stop; i++) {
        c = text[i];
        if (c == QChar(9)) {
            if (temp.length() > 0) {
                int tempWidth = fontMetrics.width(temp);
                p->eraseRect(cx, y, tempWidth, lineHeight);
                p->drawText(cx, y, tempWidth, lineHeight, AlignLeft | AlignTop, temp);
                cx += tempWidth;
                temp.truncate(0);
            }

            int tabWidth = fontMetrics.width(' ') * (tabSize - (column % tabSize));
            p->eraseRect(cx, y, tabWidth, lineHeight);
            cx += tabWidth;
            column += tabSize - (column % tabSize);
        } else {
            temp += c;
            column++;
        }
    }

    if (temp.length() > 0) {
        int tempWidth = fontMetrics.width(temp);
        p->eraseRect(cx, y, tempWidth, lineHeight);
        p->drawText(cx, y, tempWidth, lineHeight, AlignLeft | AlignTop, temp);
    }
}

void TextEditor::pointToLineColumn(QPoint p, int &line, int &column) {
    QFontMetrics fontMetrics = viewport()->fontMetrics();

    line = p.y() / fontMetrics.lineSpacing();

    if (line < 0) {
        line = 0;
        column = 0;
        return;
    }

    if (line >= m_document->lineCount()) {
        line = m_document->lineCount() - 1;
        column = m_document->line(line).text.length();
        return;
    }

    QString text = m_document->line(line).text;

    for (column = 0; column < text.length(); column++) {
        int x = calculateTextWidth(fontMetrics, text, column);
        if (p.x() <= x + fontMetrics.width(text[column]) / 2)
            break;
    }
}

bool TextEditor::deleteSelection() {
    SelectionRange range = selectionRange();
    if (!range.hasSelection) {
        return FALSE;
    }

    deselect();

    eraseCursor();
    m_document->removeText(range.startLine, range.startColumn, range.endLine, range.endColumn);
    m_cursorLine = range.startLine;
    m_cursorColumn = range.startColumn;
    ensureCursorVisible();
    emit cursorMoved(m_cursorLine, m_cursorColumn);

    return TRUE;
}

bool TextEditor::textIsPrint(QString text) {
    for (uint i = 0; i < text.length(); i++) {
        if (!text[i].isPrint()) {
            return FALSE;
        }
    }
    return TRUE;
}

void TextEditor::drawContents(QPainter *p, int cx, int cy, int cw, int ch) {
    p->setBackgroundMode(OpaqueMode);

    if (m_document != NULL) {
        QFontMetrics fontMetrics = p->fontMetrics();
        int lineHeight = fontMetrics.lineSpacing();
        int startLine = cy / lineHeight - 1;
        int endLine = (cy + ch) / lineHeight + 1;
        int maxPaintWidth = QMAX(contentsWidth(), viewport()->width());
        SelectionRange selRange = selectionRange();
        int tabSize = katyapp->readConfig_TabSize();
        QColor normalForeground = katyapp->readConfig_NormalForeground();
        QColor normalBackground = katyapp->readConfig_NormalBackground();
        QColor selectedForeground = katyapp->readConfig_SelectedForeground();
        QColor selectedBackground = katyapp->readConfig_SelectedBackground();

        if (startLine < 0) {
            startLine = 0;
        }

        p->setTabStops(fontMetrics.width(' ') * tabSize);

        int i;

        for (i = startLine; i < endLine && i < m_document->lineCount(); ++i) {
            QString text = m_document->line(i).text;

            if (selRange.hasSelection) {
                if (text.length() > 0) {
                    if (i == selRange.startLine && i == selRange.endLine) {
                        p->setPen(normalForeground);
                        p->setBackgroundColor(normalBackground);
                        int firstWidth = calculateTextWidth(fontMetrics, text, selRange.startColumn);
                        paintText(p, 0, lineHeight * i, text, 0, selRange.startColumn);

                        p->setPen(selectedForeground);
                        p->setBackgroundColor(selectedBackground);
                        int secondWidth = calculateTextWidth(fontMetrics, text, selRange.endColumn) - firstWidth;
                        paintText(p, firstWidth, lineHeight * i, text, selRange.startColumn, selRange.endColumn);

                        p->setPen(normalForeground);
                        p->setBackgroundColor(normalBackground);
                        int thirdWidth = calculateTextWidth(fontMetrics, text) - secondWidth - firstWidth;
                        paintText(p, firstWidth + secondWidth, lineHeight * i, text, selRange.endColumn);

                        p->eraseRect(firstWidth + secondWidth + thirdWidth, lineHeight * i, maxPaintWidth - firstWidth + secondWidth + thirdWidth, lineHeight);
                    } else if (i == selRange.startLine) {
                        p->setPen(normalForeground);
                        p->setBackgroundColor(normalBackground);
                        int firstWidth = calculateTextWidth(fontMetrics, text, selRange.startColumn);
                        paintText(p, 0, lineHeight * i, text, 0, selRange.startColumn);

                        p->setPen(selectedForeground);
                        p->setBackgroundColor(selectedBackground);
                        int secondWidth = calculateTextWidth(fontMetrics, text) - firstWidth;
                        paintText(p, firstWidth, lineHeight * i, text, selRange.startColumn);

                        p->eraseRect(firstWidth + secondWidth, lineHeight * i, maxPaintWidth - firstWidth + secondWidth, lineHeight);
                    } else if (i > selRange.startLine && i < selRange.endLine) {
                        p->setPen(selectedForeground);
                        p->setBackgroundColor(selectedBackground);
                        int textWidth = calculateTextWidth(fontMetrics, text);
                        paintText(p, 0, lineHeight * i, text);

                        p->eraseRect(textWidth, lineHeight * i, maxPaintWidth - textWidth, lineHeight);
                    } else if (i == selRange.endLine) {
                        p->setPen(selectedForeground);
                        p->setBackgroundColor(selectedBackground);
                        int firstWidth = calculateTextWidth(fontMetrics, text, selRange.endColumn);
                        paintText(p, 0, lineHeight * i, text, 0, selRange.endColumn);

                        p->setPen(normalForeground);
                        p->setBackgroundColor(normalBackground);
                        int secondWidth = calculateTextWidth(fontMetrics, text) - firstWidth;
                        paintText(p, firstWidth, lineHeight * i, text, selRange.endColumn);

                        p->eraseRect(firstWidth + secondWidth, lineHeight * i, maxPaintWidth - firstWidth + secondWidth, lineHeight);
                    } else {
                        p->setPen(normalForeground);
                        p->setBackgroundColor(normalBackground);
                        int textWidth = calculateTextWidth(fontMetrics, text);
                        paintText(p, 0, lineHeight * i, text);

                        p->eraseRect(textWidth, lineHeight * i, maxPaintWidth - textWidth, lineHeight);
                    }
                } else {
                    if (i >= selRange.startLine && i < selRange.endLine)
                        p->setBackgroundColor(selectedBackground);
                    else
                        p->setBackgroundColor(normalBackground);

                    p->eraseRect(0, lineHeight * i, maxPaintWidth, lineHeight);
                }
            } else {
                if (text.length() > 0) {
                    p->setPen(normalForeground);
                    p->setBackgroundColor(normalBackground);
                    int textWidth = calculateTextWidth(fontMetrics, text);
                    paintText(p, 0, lineHeight * i, text);

                    p->eraseRect(textWidth, lineHeight * i, maxPaintWidth - textWidth, lineHeight);
                } else {
                    p->setBackgroundColor(normalBackground);
                    p->eraseRect(0, lineHeight * i, maxPaintWidth, lineHeight);
                }
            }
        }

        if (contentsHeight() < viewport()->height()) {
            p->setBackgroundColor(normalBackground);
            p->eraseRect(0, contentsHeight(), maxPaintWidth, viewport()->height() - contentsHeight());
        }

        if (m_cursorOn) {
            int cursorMiddleX;
            QRect cursorRect = calculateCursorRect(m_cursorLine, m_cursorColumn, &cursorMiddleX);

            p->setPen(QColor(0, 0, 0));
            p->setRasterOp(NotXorROP);
            p->drawLine(cursorRect.left(), cursorRect.top(), cursorRect.right(), cursorRect.top());
            p->drawLine(cursorMiddleX, cursorRect.top(), cursorMiddleX, cursorRect.bottom());
            p->drawLine(cursorRect.left(), cursorRect.bottom(), cursorRect.right(), cursorRect.bottom());
            p->setRasterOp(CopyROP);
        }
    }
}

void TextEditor::timerEvent(QTimerEvent *event) {
    if (viewport()->hasFocus()) {
        m_cursorOn = !m_cursorOn;
        updateContents(calculateCursorRect(m_cursorLine, m_cursorColumn));
    }
}

void TextEditor::keyPressEvent(QKeyEvent *event) {
    bool shiftPressed = ((event->state() & ShiftButton) == ShiftButton);
    bool controlPressed = ((event->state() & ControlButton) == ControlButton);

    switch (event->key()) {
    case Key_Left:
        if (controlPressed) {
            moveCursorWordLeft(shiftPressed);
        } else {
            moveCursorLeft(shiftPressed);
        }
        break;

    case Key_Right:
        if (controlPressed) {
            moveCursorWordRight(shiftPressed);
        } else {
            moveCursorRight(shiftPressed);
        }
        break;

    case Key_Up:
        moveCursorUp(shiftPressed);
        break;

    case Key_Down:
        moveCursorDown(shiftPressed);
        break;

    case Key_PageUp:
        moveCursorPageUp(shiftPressed);
        break;

    case Key_PageDown:
        moveCursorPageDown(shiftPressed);
        break;

    case Key_Home:
        if (controlPressed) {
            moveCursorDocumentStart(shiftPressed);
        } else {
            moveCursorHome(shiftPressed);
        }
        break;

    case Key_End:
        if (controlPressed) {
            moveCursorDocumentEnd(shiftPressed);
        } else {
            moveCursorEnd(shiftPressed);
        }
        break;

    case Key_Enter:
    case Key_Return:
        deleteSelection();
        eraseCursor();
        m_document->splitLine(m_cursorLine, m_cursorColumn);
        m_cursorColumn = 0;
        m_cursorLine++;
        ensureCursorVisible();
        emit cursorMoved(m_cursorLine, m_cursorColumn);
        break;

    case Key_Backspace:
        if (!deleteSelection()) {
            if (controlPressed) {
                moveCursorWordLeft(TRUE);
                deleteSelection();
            } else {
                if (m_cursorColumn > 0) {
                    eraseCursor();
                    TextLine line = m_document->line(m_cursorLine);
                    line.text.remove(--m_cursorColumn, 1);
                    m_document->setLine(m_cursorLine, line);
                    ensureCursorVisible();
                    emit cursorMoved(m_cursorLine, m_cursorColumn);
                } else if (m_cursorLine > 0) {
                    eraseCursor();
                    TextLine line = m_document->line(m_cursorLine - 1);
                    m_cursorLine--;
                    m_cursorColumn = line.text.length();
                    m_document->joinLines(m_cursorLine);
                    ensureCursorVisible();
                    emit cursorMoved(m_cursorLine, m_cursorColumn);
                }
            }
        }
        break;

    case Key_Delete:
        if (!deleteSelection()) {
            if (m_cursorColumn == m_document->line(m_cursorLine).text.length()) {
                eraseCursor();
                m_document->joinLines(m_cursorLine);
                ensureCursorVisible();
            } else {
                eraseCursor();
                TextLine line = m_document->line(m_cursorLine);
                line.text.remove(m_cursorColumn, 1);
                m_document->setLine(m_cursorLine, line);
                ensureCursorVisible();
            }
        }
        break;

    case Key_Tab: {
            SelectionRange selRange = selectionRange();
            if (selRange.hasSelection) {
                m_document->indentLines(selRange.startLine, selRange.endLine - selRange.startLine + 1, katyapp->readConfig_UseSpaces(), katyapp->readConfig_IndentSize());
            } else {
                DocumentPosition newPos = m_document->insertTab(m_cursorLine, m_cursorColumn, katyapp->readConfig_UseSpaces(), katyapp->readConfig_IndentSize());
                moveCursorTo(newPos.line, newPos.column);
            }
            break;
        }

    default:
        if (event->text().length() > 0) {
            if (textIsPrint(event->text())) {
                deleteSelection();
                eraseCursor();
                DocumentPosition pos = m_document->insertText(m_cursorLine, m_cursorColumn, event->text());
                m_cursorLine = pos.line;
                m_cursorColumn = pos.column;
                ensureCursorVisible();
                emit cursorMoved(m_cursorLine, m_cursorColumn);
            }
        } else {
            event->ignore();
        }
    }
}

void TextEditor::contentsMousePressEvent(QMouseEvent *event) {
    int line, col;

    pointToLineColumn(event->pos(), line, col);

    bool leftButton = ((event->button() & LeftButton) == LeftButton);
    bool shiftPressed = ((event->state() & ShiftButton) == ShiftButton);

    if (leftButton) {
        moveCursorTo(line, col, shiftPressed);
    }
}

void TextEditor::contentsMouseMoveEvent(QMouseEvent *event) {
    int line, col;

    pointToLineColumn(event->pos(), line, col);

    bool leftButton = ((event->state() & LeftButton) == LeftButton);

    if (leftButton) {
        moveCursorTo(line, col, TRUE);
    }
}

void TextEditor::contentsMouseDoubleClickEvent(QMouseEvent *event) {
    int line, col;

    pointToLineColumn(event->pos(), line, col);

    bool leftButton = ((event->button() & LeftButton) == LeftButton);

    if (leftButton) {
        DocumentPosition wordStart = m_document->findWordStart(DocumentPosition(line, col));
        DocumentPosition wordEnd = m_document->findWordEnd(wordStart);
        moveCursorTo(wordStart.line, wordStart.column);
        moveCursorTo(wordEnd.line, wordEnd.column, TRUE);
    }
}

#include "texteditor.moc"
