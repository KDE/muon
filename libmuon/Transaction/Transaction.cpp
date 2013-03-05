/***************************************************************************
 *   Copyright © 2012 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "Transaction.h"

#include "TransactionModel.h"

Transaction::Transaction(QObject *parent, AbstractResource *resource,
                         Role role)
    : Transaction(parent, resource, role, AddonList())
{
}

Transaction::Transaction(QObject *parent, AbstractResource *resource,
                         Role role, AddonList addons)
    : QObject(parent)
    , m_resource(resource)
    , m_role(role)
    , m_status(SetupStatus)
    , m_addons(addons)
    , m_isCancellable(true)
    , m_progress(0)
{
}

AbstractResource *Transaction::resource() const
{
    return m_resource;
}

Transaction::Role Transaction::role() const
{
    return m_role;
}

Transaction::Status Transaction::status() const
{
    return m_status;
}

AddonList Transaction::addons() const
{
    return m_addons;
}

bool Transaction::isCancellable() const
{
    return m_isCancellable;
}

int Transaction::progress() const
{
    return m_progress;
}

void Transaction::setStatus(Status status)
{
    if(m_status != status) {
        m_status = status;
        emit statusChanged(m_status);
    }
}

void Transaction::setCancellable(bool isCancellable)
{
    if(m_isCancellable != isCancellable) {
        m_isCancellable = isCancellable;
        emit cancellableChanged(m_isCancellable);
    }
}

void Transaction::setProgress(int progress)
{
    if(m_progress != progress) {
        m_progress = progress;
        emit progressChanged(m_progress);
    }
}

void Transaction::cancel()
{
    if (!m_isCancellable)
        return;

    TransactionModel::global()->cancelTransaction(this);
}
