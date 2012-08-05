#ifndef USCRESOURCE_H
#define USCRESOURCE_H

#include <QtCore/QVariantMap>

#include <resources/AbstractResource.h>

class USCResource : public AbstractResource
{
    Q_OBJECT
public:
    explicit USCResource(const QVariantMap &data);

    AbstractResource::State state();
    QString name();
    QString icon() const;
    QString comment();
    QString packageName() const;
    QString categories();
    QUrl homepage() const;
    QUrl thumbnailUrl();
    QUrl screenshotUrl();
    QString license();
    QString longDescription() const;
    QList<PackageState> addonsInformation() { return QList<PackageState>(); }
    QString sizeDescription() { return QString(); }
    QString availableVersion() const;
    QString installedVersion() const;
    QString origin() const;
    QString section() { return QString(); }
    void fetchScreenshots();

private:
    QString m_name;
    QString m_comment;
    QString m_longDesc;
    QString m_packageName;
    QString m_categories;
    QString m_homepage;
    QUrl m_screenshotUrl;
    QList<QUrl> m_screenshotUrls;
    QString m_license;
    QString m_version;
    QString m_origin;
};

#endif // USCRESOURCE_H
