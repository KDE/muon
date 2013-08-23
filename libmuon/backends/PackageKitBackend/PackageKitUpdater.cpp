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
#include "PackageKitUpdater.h"

#include <PackageKit/packagekit-qt2/Daemon>
#include <QSet>

#include <KLocale>
#include <KMessageBox>
#include <KDebug>

PackageKitUpdater::PackageKitUpdater(PackageKitBackend * parent)
  : AbstractBackendUpdater(parent),
    m_transaction(0),
    m_backend(parent),
    m_speed(0),
    m_remainingTime(0),
    m_percentage(0)
{
    connect(m_backend, SIGNAL(reloadFinished()),
            this, SLOT(reloadFinished()));
}

PackageKitUpdater::~PackageKitUpdater()
{
    delete m_transaction;
}

void PackageKitUpdater::reloadFinished()
{
}

void PackageKitUpdater::prepare()
{
    kDebug();
    m_toUpgrade = m_backend->upgradeablePackages();
    
    if (m_transaction)
        m_transaction->deleteLater();
    
    m_transaction = new PackageKit::Transaction(this);
    connect(m_transaction, SIGNAL(finished(PackageKit::Transaction::Exit,uint)), SLOT(finished(PackageKit::Transaction::Exit,uint)));
    connect(m_transaction, SIGNAL(changed()), this, SLOT(backendChanged()));
    connect(m_transaction, SIGNAL(errorCode(PackageKit::Transaction::Error,QString)), this, SLOT(errorFound(PackageKit::Transaction::Error,QString)));
    connect(m_transaction, SIGNAL(mediaChangeRequired(PackageKit::Transaction::MediaType,QString,QString)),
            this, SLOT(mediaChange(PackageKit::Transaction::MediaType,QString,QString)));
    connect(m_transaction, SIGNAL(requireRestart(PackageKit::Transaction::Restart,QString)),
            this, SLOT(requireRestard(PackageKit::Transaction::Restart,QString)));
    connect(m_transaction, SIGNAL(eulaRequired(QString, QString, QString, QString)), SLOT(eulaRequired(QString, QString, QString, QString)));

}

void PackageKitUpdater::finished(PackageKit::Transaction::Exit exit, uint )
{
    kDebug() << "EXIT" << exit;
    if (exit == PackageKit::Transaction::ExitEulaRequired)
        return;
    if (exit == PackageKit::Transaction::ExitSuccess && m_transaction->role() == PackageKit::Transaction::RoleAcceptEula) {
        prepare();
        start();
        return;
    }
    m_isProgressing = false;
    emit progressingChanged(m_isProgressing);
    m_backend->populateInstalledCache();
}

void PackageKitUpdater::backendChanged()
{
    kDebug();
    if (m_isCancelable != m_transaction->allowCancel()) {
        m_isCancelable = m_transaction->allowCancel();
        emit cancelableChanged(m_isCancelable);
    }
    
    if (m_status != m_transaction->status()) {
        m_status = m_transaction->status();
        switch (m_status) {
            case PackageKit::Transaction::StatusWait:
                m_statusMessage = i18n("Waiting..");
                m_statusDetail = i18n("We are waiting for something.");
                break;
            case PackageKit::Transaction::StatusSetup:
                m_statusMessage = i18n("Setup...");
                m_statusDetail = i18n("Setting up transaction...");
                break;
            case PackageKit::Transaction::StatusRunning:
                m_statusMessage = i18n("Processing...");
                m_statusDetail = i18n("The transaction is currently working...");
                break;
            case PackageKit::Transaction::StatusRemove:
                m_statusMessage = i18n("Remove...");
                m_statusDetail = i18n("The transaction is currently removing packages...");
                break;
            case PackageKit::Transaction::StatusDownload:
                m_statusMessage = i18n("Downloading...");
                m_statusDetail = i18n("The transaction is currently downloading packages...");
                break;
            case PackageKit::Transaction::StatusInstall:
                m_statusMessage = i18n("Installing...");
                m_statusDetail = i18n("The transactions is currently installing packages...");
                break;
            case PackageKit::Transaction::StatusUpdate:
                m_statusMessage = i18n("Updating...");
                m_statusDetail = i18n("The transaction is currently updating packages...");
                break;
            case PackageKit::Transaction::StatusCleanup:
                m_statusMessage = i18n("Cleaning up...");
                m_statusDetail = i18n("The transaction is currently cleaning up...");
                break;
            //case PackageKit::Transaction::StatusObsolete,
            case PackageKit::Transaction::StatusDepResolve:
                m_statusMessage = i18n("Resolving dependencies...");
                m_statusDetail = i18n("The transaction is currently resolving the dependencies of the packages it will install...");
                break;
            case PackageKit::Transaction::StatusSigCheck:
                m_statusMessage = i18n("Chacking signatures...");
                m_statusDetail = i18n("The transaction is currently checking the signatures of the packages...");
                break;
            case PackageKit::Transaction::StatusTestCommit:
                m_statusMessage = i18n("Test committing...");
                m_statusDetail = i18n("The transaction is currently testing the commit of this set of packages...");
                break;
            case PackageKit::Transaction::StatusCommit:
                m_statusMessage = i18n("Committing...");
                m_statusDetail = i18n("The transaction is currently committing its set of packages...");
                break;
        //StatusRequest,
            case PackageKit::Transaction::StatusFinished:
                m_statusMessage = i18n("Finished.");
                m_statusDetail = i18n("The transaction has finished!");
                m_lastUpdate = QDateTime::currentDateTime();
                break;
            case PackageKit::Transaction::StatusCancel:
                m_statusMessage = i18n("Cancelled.");
                m_statusDetail = i18n("The transaction was cancelled");
                break;
            case PackageKit::Transaction::StatusWaitingForLock:
                m_statusMessage = i18n("Waiting for lock...");
                m_statusDetail = i18n("The transaction is currently waiting for the lock...");
                break;
            case PackageKit::Transaction::StatusWaitingForAuth:
                m_statusMessage = i18n("Waiting for authorization...");
                m_statusDetail = i18n("Waiting for the user to authorize the transaction...");
                break;
        //StatusScanProcessList,
        //StatusCheckExecutableFiles,
        //StatusCheckLibraries,
            case PackageKit::Transaction::StatusCopyFiles:
                m_statusMessage = i18n("Copying files...");
                m_statusDetail = i18n("The transaction is currently copying files...");
                break;
            case PackageKit::Transaction::StatusUnknown:
            default:
                m_statusDetail = i18n("PackageKit does not tell us a useful status right now! Its status is %1.", m_transaction->status());
                m_statusMessage = i18n("Unknown Status");
                break;
        };
        emit statusMessageChanged(m_statusMessage);
        emit statusDetailChanged(m_statusDetail);
    }
    
    if (m_speed != m_transaction->speed()) {
        m_speed = m_transaction->speed();
        emit downloadSpeedChanged(m_speed);
    }
    
    if (m_remainingTime != m_transaction->remainingTime()) {
        m_remainingTime = m_transaction->remainingTime();
        emit remainingTimeChanged();
    }
    
    if (m_percentage != m_transaction->percentage()) {
        m_percentage = m_transaction->percentage();
        emit progressChanged(m_percentage);
    }
}

bool PackageKitUpdater::hasUpdates() const
{
    return m_backend->allUpdatesCount() > 0;
}

qreal PackageKitUpdater::progress() const
{
    return m_percentage;
}

/** proposed ETA in milliseconds */
long unsigned int PackageKitUpdater::remainingTime() const
{
    return m_remainingTime;
}

void PackageKitUpdater::removeResources(const QList<AbstractResource*>& apps)
{
    for (AbstractResource * app : apps) {
        m_toUpgrade.removeAll(app);
    }
}

void PackageKitUpdater::addResources(const QList<AbstractResource*>& apps)
{
    m_toUpgrade << apps;
}

QList<AbstractResource*> PackageKitUpdater::toUpdate() const
{
    return m_toUpgrade;
}

QDateTime PackageKitUpdater::lastUpdate() const
{
    return m_lastUpdate;
}

bool PackageKitUpdater::isAllMarked() const
{
    return m_toUpgrade.count() >= m_backend->allUpdatesCount();
}

bool PackageKitUpdater::isCancelable() const
{
    return m_isCancelable;
}

bool PackageKitUpdater::isProgressing() const
{
    return m_isProgressing;
}

QString PackageKitUpdater::statusMessage() const
{
    return m_statusMessage;
}

QString PackageKitUpdater::statusDetail() const
{
    return m_statusDetail;
}

quint64 PackageKitUpdater::downloadSpeed() const
{
    return m_speed;
}

QList<QAction*> PackageKitUpdater::messageActions() const
{
    return QList<QAction*>();
}

void PackageKitUpdater::cancel()
{
    m_transaction->cancel();
}

void PackageKitUpdater::start()
{
    kDebug() << m_toUpgrade.count();
    QSet<QString> m_packageIds;
    for (AbstractResource * res : m_toUpgrade) {
        PackageKitResource * app = qobject_cast<PackageKitResource*>(res);
        m_packageIds.insert(app->availablePackageId());
        qDebug() << "Upgrade" << app->availablePackageId() << app->installedPackageId();
    }
    m_transaction->installPackages(m_packageIds.toList());
}

void PackageKitUpdater::errorFound(PackageKit::Transaction::Error err, const QString& error)
{
    kDebug() << "ERROR" << error;
    if (err == PackageKit::Transaction::ErrorNoLicenseAgreement)
        return;
    KMessageBox::error(0, error, PackageKitBackend::errorMessage(err));
}

void PackageKitUpdater::mediaChange(PackageKit::Transaction::MediaType media, const QString& type, const QString& text)
{
    Q_UNUSED(media)
    KMessageBox::information(0, text, i18n("Media Change of type '%1' is requested.", type));
}

void PackageKitUpdater::requireRestard(PackageKit::Transaction::Restart restart, const QString& p)
{
    QString message;
    switch (restart) {
        case PackageKit::Transaction::RestartApplication:
            message = i18n("'%1' was changed and suggests to be restarted.", PackageKit::Daemon::packageName(p));
            break;
        case PackageKit::Transaction::RestartSession:
            message = i18n("A change by '%1' suggests your session to be restarted.", PackageKit::Daemon::packageName(p));
            break;
        case PackageKit::Transaction::RestartSecuritySession:
            message = i18n("'%1' was updated for security reasons, a restart of the session is recommended.", PackageKit::Daemon::packageName(p));
            break;
        case PackageKit::Transaction::RestartSecuritySystem:
            message = i18n("'%1' was updated for security reasons, a restart of the system is recommended.", PackageKit::Daemon::packageName(p));
            break;
        case PackageKit::Transaction::RestartSystem:
        case PackageKit::Transaction::RestartUnknown:
        case PackageKit::Transaction::RestartNone:
        default:
            message = i18n("A change by '%1' suggests your system to be rebooted.", PackageKit::Daemon::packageName(p));
            break;
    };
    KMessageBox::information(0, message);
}

void PackageKitUpdater::eulaRequired(const QString& eulaID, const QString& packageID, const QString& vendor, const QString& licenseAgreement)
{
    kDebug();
    int ret = KMessageBox::questionYesNo(0, i18n("The package %1 and its vendor %2 require that you accept their license:\n %3", 
                                                 PackageKit::Daemon::packageName(packageID), vendor, licenseAgreement),
                                            i18n("%1 requires user to accept its license!", PackageKit::Daemon::packageName(packageID)));
    if (ret == KMessageBox::Yes) {
        m_transaction->acceptEula(eulaID);
    } else {
        finished(PackageKit::Transaction::ExitCancelled, 0);
    }
}
