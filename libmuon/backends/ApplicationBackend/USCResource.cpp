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

#include "USCResource.h"

// Qt includes
#include <QtCore/QStringList>

// KDE includes
#include <KGlobal>
#include <KLocale>

// LibQApt includes
#include <LibQApt/Backend>

USCResource::USCResource(ApplicationBackend *parent,
                         QApt::Backend *backend, const QVariantMap &data)
    : QAptResource(parent, backend)
{
    m_name = data.value("name").toString();
    QString desc = data.value("description").toString();
    desc.replace(QLatin1String("\r\n"), QLatin1String("<br>"));
    QStringList descSplit = desc.split('\n');

    m_comment = descSplit.takeFirst();
    for (const QString &descLine : descSplit)
        m_longDesc += descLine;

    m_packageName = data.value("package_name").toString();

    m_categories = data.value("categories").toString();
    m_homepage = data.value("website").toString();
    m_screenshotUrl = data.value("screenshot_url").toUrl();
    m_license = data.value("license").toString();
    m_version = data.value("version").toString();
    m_origin = data.value("channel").toString();

    QList<QVariant> variantList = data.value("screenshot_urls").toList();
    for (const QVariant &variant : variantList)
        m_screenshotUrls += variant.toUrl();

    m_price = data.value("price").toString();
    m_archiveId = data.value("archive_id").toString();
    m_size = data.value("binary_filesize").toUInt();
}

QApt::Package *USCResource::package()
{
    if (!m_package && m_backend) {
        m_package = m_backend->package(m_packageName);
        emit stateChanged();
    }

    return m_package;
}

QString USCResource::name()
{
    return m_name;
}

QString USCResource::icon() const
{
    return QLatin1String("applications-other");
}

QString USCResource::comment()
{
    return m_comment;
}

QString USCResource::longDescription() const
{
    return m_longDesc;
}

QString USCResource::sizeDescription()
{
    return i18nc("@info app size", "%1 on disk",
                 KGlobal::locale()->formatByteSize(m_size));
}

QString USCResource::packageName() const
{
    return m_packageName;
}

AbstractResource::State USCResource::state()
{
    if (package())
        return QAptResource::state();

    // FIXME: could be purchased, but we don't have deb_line, etc
    return AbstractResource::NeedsPurchase;
}

QString USCResource::categories()
{
    return m_categories;
}

QUrl USCResource::homepage() const
{
    return m_homepage;
}

QUrl USCResource::thumbnailUrl()
{
    // Thumbnail is the same as screenshot in USC, but needs scaled down
    return m_screenshotUrl;
}

QUrl USCResource::screenshotUrl()
{
    return m_screenshotUrl;
}

QString USCResource::license()
{
    return m_license;
}

QString USCResource::availableVersion() const
{
    return m_version;
}

QString USCResource::origin() const
{
    return m_origin;
}

QString USCResource::archiveId() const
{
    return m_archiveId;
}

int USCResource::downloadSize()
{
    if (m_package)
        return m_package->downloadSize();

    return 0;
}

QString USCResource::section()
{
    if (m_package)
        return m_package->section();

    return QString();
}

QString USCResource::price() const
{
    return m_price;
}

void USCResource::fetchScreenshots()
{
    emit screenshotsFetched(m_screenshotUrls, m_screenshotUrls);
}
