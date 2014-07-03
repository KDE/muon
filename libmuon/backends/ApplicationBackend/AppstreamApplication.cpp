#include "AppstreamApplication.h"
#include "Application.h"

AppstreamApplication::AppstreamApplication(Appstream::Component comp, QApt::Backend* backend)
    : Application(backend)
    , m_component(comp)
{
    setTechnical(m_component.kind() != Appstream::Component::KindDesktop);
    setPackageName(m_component.packageName().toLatin1());
}

QString AppstreamApplication::untranslatedName()
{
    return m_component.name();
}

QString AppstreamApplication::comment()
{
    return m_component.summary();
}

QString AppstreamApplication::icon() const
{
    //TODO return icon path from cache
    return QString();
}

QStringList AppstreamApplication::mimetypes() const
{
    QStringList mimeValues;
    foreach(const Appstream::Provides &provides,  m_component.provides(Appstream::Provides::KindMimetype)){
        mimeValues << provides.value();
    }
    return mimeValues;
}

QStringList AppstreamApplication::categories()
{
    return m_component.categories();
}
