#ifndef PACKAGERESOURCE_H
#define PACKAGERESOURCE_H

#include "QAptResource.h"

namespace QApt {
    class Package;
}

class PackageResource : public QAptResource
{
    Q_OBJECT
public:
    explicit PackageResource(ApplicationBackend *parent, QApt::Backend *backend, QApt::Package *package);

    QApt::Package *package();

    QString name();
    QString comment();
    QString icon() const;
    QString mimetypes() const;
    QString categories();
    QString license();
    QUrl screenshotUrl();
    QUrl thumbnailUrl();
    QList<PackageState> addonsInformation();
    bool isTechnical() const;
    QString packageName() const;
    QUrl homepage() const;
    QString longDescription() const;
    QString installedVersion() const;
    QString availableVersion() const;
    QString sizeDescription();
    QString origin() const;
    int downloadSize();
    QString section();

    QStringList executables() const;
    void invokeApplication() const;
    bool canExecute() const;

    State state();
    void fetchScreenshots();
    void fetchChangelog();

private:
    QString m_packageName;
    
signals:
    
public slots:
    
};

#endif // PACKAGERESOURCE_H
