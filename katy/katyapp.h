/*
 * Class for main Katy application
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

#ifndef KATYAPP_H
#define KATYAPP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kuniqueapp.h>
#include <kaction.h>

#include <qptrlist.h>

#define katyapp ((KatyApp*)kapp)

class Katy;

typedef QPtrList<Katy> KatyList;
typedef QPtrListIterator<Katy> KatyListIterator;

/**
 * This class serves as the main application class for Katy.
 *
 * @short Main application class
 * @author Gordon Tyler <gordon@doxxx.net>
 */
class KatyApp : public KUniqueApplication {
    Q_OBJECT
public:
    KatyApp();
    ~KatyApp();

    int newInstance();

    Katy *newWindow();
    void removeWindow(Katy *window);
    KatyListIterator windowsIterator();

    QFont readConfig_Font();
    QColor readConfig_NormalForeground();
    QColor readConfig_NormalBackground();
    QColor readConfig_SelectedForeground();
    QColor readConfig_SelectedBackground();
    int readConfig_TabSize();
    bool readConfig_UseSpaces();
    int readConfig_IndentSize();

    void writeConfig_Font(QFont font);
    void writeConfig_NormalForeground(QColor color);
    void writeConfig_NormalBackground(QColor color);
    void writeConfig_SelectedForeground(QColor color);
    void writeConfig_SelectedBackground(QColor color);
    void writeConfig_TabSize(int tabSize);
    void writeConfig_UseSpaces(bool useSpaces);
    void writeConfig_IndentSize(int indentSize);

protected:
    void updateWindowsMenu();

signals:
    void configChanged();

private:
    bool alreadyRestored;
    KatyList windowList;
};

#endif
