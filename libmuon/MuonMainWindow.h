/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef MUONMAINWINDOW_H
#define MUONMAINWINDOW_H

// Qt includes
#include <QtCore/QVariantMap>

// KDE includes
#include <KXmlGuiWindow>
#include <KLocale>

#include "libmuonprivate_export.h"

class KAction;

/**
 * This class serves as a shared Main Window implementation that connects
 * all the various backend bits so that they don't have to be reimplemented
 * in things like an update-centric GUI, etc.
 *
 * @short Main window class
 * @author Jonathan Thomas <echidnaman@kubuntu.org>
 * @version 0.1
 */
class MUONPRIVATE_EXPORT MuonMainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    MuonMainWindow();

    QSize sizeHint() const;
    bool isConnected() const;

Q_SIGNALS:
    void shouldConnect(bool isConnected);
    void actionsEnabledChanged(bool enabled);

protected slots:
    bool queryExit();
    void setupActions();

public slots:
    void easterEggTriggered();
    void setCanExit(bool canExit);
    virtual void setActionsEnabled(bool enabled=true);

private:
    bool m_canExit;
};

#endif
