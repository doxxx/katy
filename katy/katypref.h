/*
 * Class for Katy Preferences dialog
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

#ifndef KATYPREF_H
#define KATYPREF_H

#include <kdialogbase.h>
#include <qframe.h>

class KatyPrefAppearancePage;
class KatyPrefIndentingPage;

class KatyPreferences : public KDialogBase
{
    Q_OBJECT
public:
    KatyPreferences();

public slots:
    void slotChooseFont();

protected slots:
    void slotChanged();
    void slotApply();
    void slotOk();

protected:
    void initAppearancePage();
    void initIndentingPage();
    void saveAppearancePage();
    void saveIndentingPage();
    QString describeFont(QFont font);

private:
    KatyPrefAppearancePage *m_appearancePage;
    KatyPrefIndentingPage *m_indentingPage;
    bool m_changed;
};

#endif // KATYPREF_H
