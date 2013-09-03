/*
 * Copyright 2013  Lukas Appelhans <l.appelhans@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "AkabeiNotifier.h"

#include <QTimer>
#include <KPluginFactory>
#include <akabeiclientbackend.h>
#include <akabeiconfig.h>
#include <akabeihelpers.h>
#include <akabeidatabase.h>

K_PLUGIN_FACTORY(MuonAkabeiNotifierFactory,
                 registerPlugin<AkabeiNotifier>();
                )
K_EXPORT_PLUGIN(MuonAkabeiNotifierFactory("muon-akabei-notifier"))

const int UPDATE_INTERVAL = 1000 * 60 * 30;//30 min

AkabeiNotifier::AkabeiNotifier(QObject* parent, const QVariantList &)
  : AbstractKDEDModule("akabei", "muondiscover", parent),
    m_timer(new QTimer(this))
{
    m_timer->setInterval(UPDATE_INTERVAL);
    connect(m_timer, SIGNAL(timeout()), SLOT(recheckSystemUpdateNeeded()));
    
    QTimer::singleShot(2 * 60 * 1000, this, SLOT(init()));
}

AkabeiNotifier::~AkabeiNotifier()
{
}

void AkabeiNotifier::init()
{
    connect(AkabeiClient::Backend::instance(), SIGNAL(statusChanged(Akabei::Backend::Status)), SLOT(backendStateChanged(Akabei::Backend::Status)));
    
    /* Used to determine whether debugging prints are to be displayed later */
    Akabei::Config::instance()->setDebug(true);
    
    AkabeiClient::Backend::instance()->initialize();
}

void AkabeiNotifier::backendStateChanged(Akabei::Backend::Status status)
{
    if (status == Akabei::Backend::StatusReady) {
        Akabei::Package::List toBeUpgraded;

        foreach (Akabei::Package *p, Akabei::Backend::instance()->localDatabase()->packages()) {
            Akabei::Package *latest = Akabei::Helpers::latestVersionOfPackage(p->name());

            if (latest && (latest->version() > p->version())) {
                toBeUpgraded.append(latest);
            }
        }
        
        if (!toBeUpgraded.isEmpty()) {
            setSystemUpToDate(false, toBeUpgraded.count());
        } else {
            setSystemUpToDate(true);
        }
        m_timer->start();
    }
}

void AkabeiNotifier::recheckSystemUpdateNeeded()
{
    m_timer->stop();
    AkabeiClient::Backend::instance()->updateDatabase();
}

