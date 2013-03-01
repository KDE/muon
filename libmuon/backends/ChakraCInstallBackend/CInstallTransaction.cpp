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

#include "CInstallTransaction.h"
#include "CInstallResource.h"
#include <resources/AbstractResourcesBackend.h>
#include <KIO/FileCopyJob>

CInstallTransaction::CInstallTransaction(CInstallResource* resource, Transaction::Role role)
    : Transaction(resource->backend(), resource, role)
{
    resource->setProperty("transaction", qVariantFromValue<QObject*>(this));
    setCancellable(true);
}

CInstallTransaction::~CInstallTransaction()
{
    resource()->setProperty("transaction", 0);
}

void CInstallTransaction::percentChanged(KJob*, ulong percent)
{
    setProgress(percent);
}

void CInstallTransaction::setJob(KIO::FileCopyJob* fcJob)
{
    m_job = fcJob;
    fcJob->setProperty("transaction", qVariantFromValue<QObject*>(this));
    connect(fcJob, SIGNAL(percent(KJob*,ulong)), this, SLOT(percentChanged(KJob*,ulong)));}
