#include "QAptResource.h"

// Qt includes
#include <QFile>

// LibQApt includes
#include <LibQApt/Config>

// Own includes
#include "ApplicationBackend.h"

QAptResource::QAptResource(ApplicationBackend *parent,
                           QApt::Backend *backend,
                           QApt::Package *package)
    : AbstractResource(parent)
    , m_backend(backend)
    , m_package(package)
    , m_isValid(true)
{
}

bool QAptResource::isValid() const
{
    return m_isValid;
}

void QAptResource::clearPackage()
{
    m_package = 0;
}

QApt::PackageList QAptResource::addons()
{
    QApt::PackageList addons;

    QApt::Package *pkg = package();
    if (!pkg)
        return addons;

    QStringList tempList;
    // Only add recommends or suggests to the list if they aren't already going to be
    // installed
    if (!m_backend->config()->readEntry("APT::Install-Recommends", true)) {
        tempList << m_package->recommendsList();
    }
    if (!m_backend->config()->readEntry("APT::Install-Suggests", false)) {
        tempList << m_package->suggestsList();
    }
    tempList << m_package->enhancedByList();

    QStringList languagePackages;
    QFile l10nFilterFile("/usr/share/language-selector/data/pkg_depends");

    if (l10nFilterFile.open(QFile::ReadOnly)) {
        QString contents = l10nFilterFile.readAll();

        foreach (const QString &line, contents.split('\n')) {
            if (line.startsWith(QLatin1Char('#'))) {
                continue;
            }
            languagePackages << line.split(':').last();
        }

        languagePackages.removeAll("");
    }

    for (const QString &addon : tempList) {
        bool shouldShow = true;
        QApt::Package *package = m_backend->package(addon);

        if (!package || QString(package->section()).contains("lib") || addons.contains(package)) {
            continue;
        }

        foreach (const QString &langpack, languagePackages) {
            if (addon.contains(langpack)) {
                shouldShow = false;
                break;
            }
        }

        if (shouldShow) {
            addons << package;
        }
    }

    return addons;
}
