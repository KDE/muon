/***************************************************************************
 *   Copyright © 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
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

#ifndef ABSTRACTBACKENDUPDATER_H
#define ABSTRACTBACKENDUPDATER_H

#include <QObject>
#include "libmuonprivate_export.h"

class QIcon;
class MUONPRIVATE_EXPORT AbstractBackendUpdater : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    public:
        explicit AbstractBackendUpdater(QObject* parent = 0);
        virtual void start() = 0;
        virtual bool hasUpdates() const = 0;
        virtual qreal progress() const = 0;
        
        /** proposed ETA in milliseconds */
        virtual long unsigned int remainingTime() const = 0;

    signals:
        void progressChanged(qreal progress);
        void message(const QIcon& icon, const QString& msg);
        void updatesFinnished();
        void remainingTimeChanged();
};

#endif // ABSTRACTBACKENDUPDATER_H