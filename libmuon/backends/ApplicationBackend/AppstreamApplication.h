#ifndef APPSTREAMAPPLICATION_H
#define APPSTREAMAPPLICATION_H

#include "Application.h"
#include <AppstreamQt/database.h>

class Application;

class AppstreamApplication : public Application
{
public:
    explicit AppstreamApplication(Appstream::Component comp, QApt::Backend *backend);
    QString untranslatedName();
    QString comment();
    QString icon() const;
    QStringList mimetypes() const;
    QStringList categories();
private:
    Appstream::Component m_component;
};

#endif