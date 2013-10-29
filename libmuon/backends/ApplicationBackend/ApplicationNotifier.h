/***************************************************************************
 *   Copyright © 2013 Lukas Appelhans <l.appelhans@gmx.de>                 *
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
#ifndef APPLICATIONNOTIFIER_H
#define APPLICATIONNOTIFIER_H

#include <resources/AbstractKDEDModule.h>
#include <QVariantList>

class DistUpgradeEvent;
class UpdateEvent;
class KProcess;
class QProcess;

class ApplicationNotifier : public AbstractKDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.muon.application")
public:
    ApplicationNotifier(QObject* parent, const QVariantList &);
    virtual ~ApplicationNotifier();
    
public slots:
    virtual void recheckSystemUpdateNeeded();
    
private slots:
    void checkUpgradeFinished(int exitStatus);
    void distUpgradeEvent();
    void init();
    void parseUpdateInfo();
    
private:
    KProcess *m_checkerProcess;
    QProcess *m_updateCheckerProcess;
    bool m_checkingForUpdates;
};

#endif