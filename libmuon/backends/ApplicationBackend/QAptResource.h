#ifndef QAPTRESOURCE_H
#define QAPTRESOURCE_H

#include <LibQApt/Globals>

// Libmuon includes
#include "resources/AbstractResource.h"

namespace QApt {
    class Backend;
    class Package;
}

class ApplicationBackend;

class QAptResource : public AbstractResource
{
    Q_OBJECT
public:
    explicit QAptResource(ApplicationBackend *parent, QApt::Backend *backend, QApt::Package *package = nullptr);

    virtual QApt::Package *package() = 0;
    bool isValid() const;
    void clearPackage();
    QApt::PackageList addons();

protected:
    QApt::Backend *m_backend;
    QApt::Package *m_package;
    bool m_isValid;

signals:
    
public slots:
    
};

#endif // QAPTRESOURCE_H
