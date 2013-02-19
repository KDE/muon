/***************************************************************************
 *   Copyright © 2010-2011 Jonathan Thomas <echidnaman@kubuntu.org>        *
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

#include "Application.h"

// Qt includes
#include <QtCore/QFile>
#include <QtCore/QVector>
#include <QtCore/QStringList>

// KDE includes
#include <KIconLoader>
#include <KLocale>
#include <KService>
#include <KServiceGroup>
#include <KDebug>
#include <KToolInvocation>
#include <KIO/Job>
#include <KIO/NetAccess>
#include <KStandardDirs>
#include <KConfigGroup>

// QApt includes
#include <LibQApt/Backend>
#include <LibQApt/Changelog>
#include <LibQApt/Package>

// QJSON includes
#include <qjson/parser.h>

// Libmuon includes
#include "MuonDataSources.h"

Application::Application(const QString& fileName, QApt::Backend* backend)
        : QAptResource(nullptr, backend)
        , m_isTechnical(false)
{
    m_data = desktopContents(fileName);
    m_isTechnical = getField("NoDisplay").toLower() == "true" || !hasField("Exec");
    m_packageName = getField("X-AppInstall-Package");
}

QString Application::name()
{
    QString name;
    if (!m_isTechnical)
        name = i18n(untranslatedName().toUtf8());
    else
        name = untranslatedName();

    if(m_package->isForeignArch())
        name = i18n("%1 (%2)", name, m_package->architecture());
    return name;
}

QString Application::untranslatedName()
{
    QString name = QString::fromUtf8(getField("Name")).trimmed();
    if (name.isEmpty() && package())
        name = m_package->name();

    return name;
}

QString Application::comment()
{
    QString comment = getField("Comment");
    if (comment.isEmpty()) {
        // Sometimes GenericName is used instead of Comment
        comment = getField("GenericName");
        if (comment.isEmpty()) {
            return package()->shortDescription();
        }
    }

    return i18n(comment.toUtf8());
}

QString Application::packageName() const
{
    return m_packageName;
}

QApt::Package *Application::package()
{
    if (!m_package && m_backend) {
        m_package = m_backend->package(packageName());
        emit stateChanged();
    }

    // Packages removed from archive will remain in app-install-data until the
    // next refresh, so we can have valid .desktops with no package
    if (!m_package) {
        m_isValid = false;
    }

    return m_package;
}

QString Application::icon() const
{
    return getField("Icon", "applications-other");
}

QString Application::mimetypes() const
{
    return getField("MimeType");
}

QString Application::menuPath()
{
    QString path;
    QString arrow(QString::fromUtf8(" ➜ "));

    // Take the file name and remove the .desktop ending
    QVector<KService::Ptr> execs = findExecutables();
    if(execs.isEmpty())
        return path;

    KService::Ptr service = execs.first();
    QVector<QPair<QString, QString> > ret;

    if (service) {
        ret = locateApplication(QString(), service->menuId());
    }

    if (!ret.isEmpty()) {
        path.append(QString("<img width=\"16\" heigh=\"16\"src=\"%1\"/>")
                    .arg(KIconLoader::global()->iconPath("kde", KIconLoader::Small)));
        path.append(QString("&nbsp;%1 <img width=\"16\" heigh=\"16\" src=\"%2\"/>&nbsp;%3")
                    .arg(arrow)
                    .arg(KIconLoader::global()->iconPath("applications-other", KIconLoader::Small))
                    .arg(i18n("Applications")));
        for (int i = 0; i < ret.size(); i++) {
            path.append(QString("&nbsp;%1&nbsp;<img width=\"16\" heigh=\"16\" src=\"%2\"/>&nbsp;%3")
                        .arg(arrow)
                        .arg(KIconLoader::global()->iconPath(ret.at(i).second, KIconLoader::Small))
                        .arg(ret.at(i).first));
        }
    }

    return path;
}

QVector<QPair<QString, QString> > Application::locateApplication(const QString &_relPath, const QString &menuId) const
{
    QVector<QPair<QString, QString> > ret;
    KServiceGroup::Ptr root = KServiceGroup::group(_relPath);

    if (!root || !root->isValid()) {
        return ret;
    }

    const KServiceGroup::List list = root->entries(false /* sorted */,
                                                   true /* exclude no display entries */,
                                                   false /* allow separators */);

    for (KServiceGroup::List::ConstIterator it = list.constBegin(); it != list.constEnd(); ++it) {
        const KSycocaEntry::Ptr p = (*it);

        if (p->isType(KST_KService)) {
            const KService::Ptr service = KService::Ptr::staticCast(p);

            if (service->noDisplay()) {
                continue;
            }

            if (service->menuId() == menuId) {
                QPair<QString, QString> pair;
                pair.first  = service->name();
                pair.second = service->icon();
                ret << pair;
                return ret;
            }
        } else if (p->isType(KST_KServiceGroup)) {
            const KServiceGroup::Ptr serviceGroup = KServiceGroup::Ptr::staticCast(p);

            if (serviceGroup->noDisplay() || serviceGroup->childCount() == 0) {
                continue;
            }

            QVector<QPair<QString, QString> > found;
            found = locateApplication(serviceGroup->relPath(), menuId);
            if (!found.isEmpty()) {
                QPair<QString, QString> pair;
                pair.first  = serviceGroup->caption();
                pair.second = serviceGroup->icon();
                ret << pair;
                ret << found;
                return ret;
            }
        } else {
            continue;
        }
    }

    return ret;
}

QString Application::categories()
{
    return getField("Categories");
}

QUrl Application::thumbnailUrl()
{
    QUrl url(package()->controlField(QLatin1String("Thumbnail-Url")));
    if(m_sourceHasScreenshot) {
        url = KUrl(MuonDataSources::screenshotsSource(), "thumbnail/"+packageName());
    }
    return url;
}

QUrl Application::screenshotUrl()
{
    QUrl url(package()->controlField(QLatin1String("Screenshot-Url")));
    if(m_sourceHasScreenshot) {
        url = KUrl(MuonDataSources::screenshotsSource(), "screenshot/"+packageName());
    }
    return url;
}

QString Application::license()
{
    QString component = package()->component();
    if (component == "main" || component == "universe") {
        return i18nc("@info license", "Open Source");
    } else if (component == "restricted") {
        return i18nc("@info license", "Proprietary");
    } else {
        return i18nc("@info license", "Unknown");
    }
}

bool Application::isValid() const
{
    return m_isValid;
}

bool Application::isTechnical() const
{
    return m_isTechnical;
}

QByteArray Application::getField(const char* field, const QByteArray& defaultvalue) const
{
    if(m_data) {
        KConfigGroup group = m_data->group("Desktop Entry");
        return group.readEntry(field, defaultvalue);
    } else
        return defaultvalue;

}

bool Application::hasField(const char* field) const
{
    return m_data && m_data->group("Desktop Entry").hasKey(field);
}

QUrl Application::homepage() const
{
    if(!m_package) return QString();
    return m_package->homepage();
}

QString Application::origin() const
{
    if(!m_package) return QString();
    return m_package->origin();
}

QString Application::longDescription() const
{
    if(!m_package) return QString();
    return m_package->longDescription();
}

QString Application::availableVersion() const
{
    if(!m_package) return QString();
    return m_package->availableVersion();
}

QString Application::installedVersion() const
{
    if(!m_package) return QString();
    return m_package->installedVersion();
}

QString Application::sizeDescription()
{
    if (!isInstalled()) {
        return i18nc("@info app size", "%1 to download, %2 on disk",
                              KGlobal::locale()->formatByteSize(package()->downloadSize()),
                              KGlobal::locale()->formatByteSize(package()->availableInstalledSize()));
    } else {
        return i18nc("@info app size", "%1 on disk",
                              KGlobal::locale()->formatByteSize(package()->currentInstalledSize()));
    }
}

int Application::downloadSize()
{
    return m_package->downloadSize();
}

KSharedConfigPtr Application::desktopContents(const QString& filename)
{
    return KSharedConfig::openConfig(filename, KConfig::SimpleConfig);
}

QVector<KService::Ptr> Application::findExecutables() const
{
    QVector<KService::Ptr> ret;
    foreach (const QString &desktop, m_package->installedFilesList().filter(QRegExp(".+\\.desktop$", Qt::CaseSensitive))) {
        // Important to use serviceByStorageId to ensure we get a service even
        // if the KSycoca database doesn't have our .desktop file yet.
        KService::Ptr service = KService::serviceByStorageId(desktop);
        if (service &&
            service->isApplication() &&
            !service->noDisplay() &&
            !service->exec().isEmpty())
        {
            ret << service;
        }
    }
    return ret;
}

void Application::emitStateChanged()
{
    emit stateChanged();
}

void Application::invokeApplication() const
{
    QVector< KService::Ptr > execs = findExecutables();
    Q_ASSERT(!execs.isEmpty());
    KToolInvocation::startServiceByDesktopPath(execs.first()->desktopEntryPath());
}

bool Application::canExecute() const
{
    return !findExecutables().isEmpty();
}

QString Application::section()
{
    return package()->section();
}

void Application::fetchScreenshots()
{
    bool done = false;
    
    QString dest = KStandardDirs::locate("tmp", "screenshots."+m_packageName);
    //TODO: Make async
    KUrl packageUrl(MuonDataSources::screenshotsSource(), "/json/package/"+m_packageName);
    bool downloadDescriptor = m_sourceHasScreenshot && KIO::NetAccess::download(packageUrl, dest, 0);
    if(downloadDescriptor) {
        QFile f(dest);
        bool b = f.open(QIODevice::ReadOnly);
        Q_ASSERT(b);
        Q_UNUSED(b)
        
        QJson::Parser p;
        bool ok;
        QVariantMap values = p.parse(&f, &ok).toMap();
        if(ok) {
            QVariantList screenshots = values["screenshots"].toList();
            
            QList<QUrl> thumbnailUrls, screenshotUrls;
            foreach(const QVariant& screenshot, screenshots) {
                QVariantMap s = screenshot.toMap();
                thumbnailUrls += s["small_image_url"].toUrl();
                screenshotUrls += s["large_image_url"].toUrl();
            }
            emit screenshotsFetched(thumbnailUrls, screenshotUrls);
            done = true;
        }
    }
    if(!done) {
        QList<QUrl> thumbnails, screenshots;
        if(!thumbnailUrl().isEmpty()) {
            thumbnails += thumbnailUrl();
            screenshots += screenshotUrl();
        }
        emit screenshotsFetched(thumbnails, screenshots);
    }
}

QStringList Application::executables() const
{
    QStringList ret;
    QVector<KService::Ptr> exes = findExecutables();
    foreach(KService::Ptr exe, exes) {
        ret += exe->desktopEntryPath();
    }
    return ret;
}

bool Application::isSecure() const
{
    for (const QString &archive : m_package->archives()) {
        if (archive.contains(QLatin1String("security"))) {
            return true;
        }
    }
    return false;
}
