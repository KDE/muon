/***************************************************************************
 *   Copyright © 2009 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "distupgradeevent.h"

#include <KDebug>
#include <KProcess>
#include <KStandardDirs>

DistUpgradeEvent::DistUpgradeEvent(QObject* parent, QString name)
        : Event(parent, name)
{
}

void DistUpgradeEvent::show()
{
    QString icon = "system-software-update";
    QString text(i18nc("Notification when a new version of Kubuntu is available",
                       "A new version of Kubuntu is available"));
    QStringList actions;
    actions << i18nc("Start the upgrade", "Upgrade");
    actions << i18nc("Button to dismiss this notification once", "Ignore for now");
    actions << i18nc("Button to make this notification never show up again",
                     "Never show again");

    Event::show(icon, text, actions);
}

void DistUpgradeEvent::run()
{
    KProcess *proc = new KProcess(this);
    QStringList arguments;
    QString kdesudo = KStandardDirs::findExe("kdesudo");
    QString upgrader = QString("do-release-upgrade -m desktop -f DistUpgradeViewKDE");

    arguments << kdesudo << upgrader;
    proc->setProgram(arguments);
    proc->start();

    connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
    Event::run();
}