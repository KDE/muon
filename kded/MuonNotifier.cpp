/***************************************************************************
 *   Copyright © 2009-2012 Jonathan Thomas <echidnaman@kubuntu.org>        *
 *   Copyright © 2009 Harald Sitter <apachelogger@ubuntu.com>              *
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

#include "MuonNotifier.h"

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTimer>

// KDE includes
#include <KAboutData>
#include <KDirWatch>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KProcess>
#include <KStandardDirs>

// Own includes
#include "distupgradeevent/distupgradeevent.h"
#include "UpdateEvent/UpdateEvent.h"

#include "configwatcher.h"

K_PLUGIN_FACTORY(MuonNotifierFactory,
                 registerPlugin<MuonNotifier>();
                )
K_EXPORT_PLUGIN(MuonNotifierFactory("muon-notifier"))


MuonNotifier::MuonNotifier(QObject* parent, const QList<QVariant>&)
        : KDEDModule(parent)
        , m_distUpgradeEvent(nullptr)
        , m_configWatcher(nullptr)
        , m_checkerProcess(nullptr)
{
    KAboutData aboutData("muon-notifier", "muon-notifier",
                         ki18n("Muon Notification Daemon"),
                         "2.1", ki18n("A Notification Daemon for Muon"),
                         KAboutData::License_GPL,
                         ki18n("(C) 2009-2012 Jonathan Thomas, (C) 2009 Harald Sitter"),
                         KLocalizedString(), "http://kubuntu.org");

    // Lazy init to not cause excessive load when starting kded.
    // Also wait 2 minutes before actually starting the init as update
    // notifications are not immediately relevant after login and there is already
    // enough stuff going on without us. 2 minutes seems like a fair time as
    // the system should have calmed down by then.
    QTimer::singleShot(1000*60*2 /* 2 minutes to start */, this, SLOT(init()));
}

MuonNotifier::~MuonNotifier()
{
    delete m_checkerProcess;
}

void MuonNotifier::init()
{
    m_configWatcher = new ConfigWatcher(this);

    m_distUpgradeEvent = new DistUpgradeEvent(this, "DistUpgrade");
    m_updateEvent = new UpdateEvent(this, "Update");

    if (!m_distUpgradeEvent->isHidden()) {
        KDirWatch *stampDirWatch = new KDirWatch(this);
        stampDirWatch->addFile("/var/lib/update-notifier/dpkg-run-stamp");
        connect(stampDirWatch, SIGNAL(dirty(QString)),
                this, SLOT(distUpgradeEvent()));
        connect(m_configWatcher, SIGNAL(reloadConfigCalled()),
                m_distUpgradeEvent, SLOT(reloadConfig()));

        distUpgradeEvent();
    }

    if (!m_updateEvent->isHidden()) {
        KDirWatch *stampDirWatch = new KDirWatch(this);
        stampDirWatch->addDir("/var/lib/apt/lists/");
        stampDirWatch->addDir("/var/lib/apt/lists/partial/");
        stampDirWatch->addFile("/var/lib/update-notifier/updates-available");
        stampDirWatch->addFile("/var/lib/update-notifier/dpkg-run-stamp");
        connect(stampDirWatch, SIGNAL(dirty(QString)),
                this, SLOT(updateEvent()));
        connect(m_configWatcher, SIGNAL(reloadConfigCalled()),
                m_updateEvent, SLOT(reloadConfig()));

        updateEvent();
    }
}

void MuonNotifier::distUpgradeEvent()
{
    QString checkerFile = KStandardDirs::locate("data", "muon-notifier/releasechecker");
    m_checkerProcess = new KProcess(this);
    connect(m_checkerProcess, SIGNAL(finished(int)),
            this, SLOT(checkUpgradeFinished(int)));
    m_checkerProcess->setProgram(QStringList() << "/usr/bin/python3" << checkerFile);
    m_checkerProcess->start();
}

void MuonNotifier::checkUpgradeFinished(int exitStatus)
{
    if (exitStatus == 0)
        m_distUpgradeEvent->show();

    m_checkerProcess->deleteLater();
    m_checkerProcess = nullptr;
}

void MuonNotifier::updateEvent()
{
    if (QFile::exists("/var/lib/update-notifier/updates-available")) {
        m_updateEvent->getUpdateInfo();
    }
}
