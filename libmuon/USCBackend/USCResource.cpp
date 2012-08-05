#include "USCResource.h"

#include <QtCore/QStringList>

USCResource::USCResource(const QVariantMap &data)
    : AbstractResource(nullptr)
{
    m_name = data.value("name").toString();
    QString desc = data.value("description").toString();
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
}

AbstractResource::State USCResource::state()
{
    State ret = None;

    return ret;
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

QString USCResource::packageName() const
{
    return m_packageName;
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

QString USCResource::installedVersion() const
{
    // TODO
    return QString();
}

QString USCResource::origin() const
{
    return m_origin;
}

void USCResource::fetchScreenshots()
{
    //TODO
}
