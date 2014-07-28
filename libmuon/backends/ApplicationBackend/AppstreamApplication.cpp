#include "AppstreamApplication.h"
#include "Application.h"

#include <KStandardDirs>

AppstreamApplication::AppstreamApplication(Appstream::Component comp, QApt::Backend* backend)
    : Application(backend)
    , m_component(comp)
{
    setTechnical(m_component.kind() != Appstream::Component::KindDesktop);
    setPackageName(m_component.packageNames().first().toLatin1());
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
    if(m_component.icon().isEmpty()){
        if(!m_component.iconUrl().toString().isEmpty()){
            QString iconName = m_component.iconUrl().toString().mid(m_component.iconUrl().toString().lastIndexOf('/')+1);
            if(!KGlobal::dirs()->findResource("appicon",iconName).isEmpty()){
                return iconName.left(iconName.lastIndexOf("."));
            }
        }
        return QString("applications-other");
    }
    return m_component.icon();
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
