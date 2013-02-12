/***************************************************************************
 *   Copyright © 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
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

#ifndef PKTRANSACTION_H
#define PKTRANSACTION_H

#include <Transaction/Transaction.h>
#include <PackageKit/packagekit-qt2/Transaction>

class PKTransaction : public Transaction
{
    Q_OBJECT
    public:
        explicit PKTransaction(AbstractResource* app, TransactionAction action, PackageKit::Transaction* pktrans);
        PackageKit::Transaction* transaction();

    private slots:
        void cleanup(PackageKit::Transaction::Exit, uint);
        void errorFound(PackageKit::Transaction::Error err, const QString& error);
        void mediaChange(PackageKit::Transaction::MediaType media, const QString& type, const QString& text);
        void requireRestard(PackageKit::Package::Restart restart, const PackageKit::Package& p);

    private:
        PackageKit::Transaction* m_trans;
};

#endif // PKTRANSACTION_H