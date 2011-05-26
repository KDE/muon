/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "MainWindow.h"

// Qt includes
#include <QApplication>
#include <QtCore/QStringBuilder>
#include <QtCore/QTimer>
#include <QtGui/QLabel>
#include <QtGui/QShortcut>
#include <QtGui/QSplitter>
#include <QtGui/QStackedWidget>
#include <QtGui/QToolBox>

// KDE includes
#include <KAction>
#include <KActionCollection>
#include <KFileDialog>
#include <KLocale>
#include <KMessageBox>
#include <KStandardAction>
#include <KStandardDirs>
#include <KStatusBar>
#include <KVBox>
#include <Phonon/MediaObject>

// LibQApt includes
#include <LibQApt/Backend>
#include <LibQApt/Config>

// Own includes
#include "config/ManagerSettingsDialog.h"
#include "../libmuon/CommitWidget.h"
#include "../libmuon/DownloadWidget.h"
#include "../libmuon/StatusWidget.h"
#include "FilterWidget.h"
#include "ManagerWidget.h"
#include "ReviewWidget.h"
#include "MuonSettings.h"

MainWindow::MainWindow()
    : MuonMainWindow()
    , m_stack(0)
    , m_settingsDialog(0)
    , m_reviewWidget(0)
    , m_downloadWidget(0)
    , m_commitWidget(0)

{
    initGUI();
    QTimer::singleShot(10, this, SLOT(initObject()));
}

MainWindow::~MainWindow()
{
    MuonSettings::self()->writeConfig();
}

void MainWindow::initGUI()
{
    m_stack = new QStackedWidget;
    setCentralWidget(m_stack);

    m_managerWidget = new ManagerWidget(m_stack);
    connect(this, SIGNAL(backendReady(QApt::Backend *)),
            m_managerWidget, SLOT(setBackend(QApt::Backend *)));

    m_mainWidget = new QSplitter(this);
    m_mainWidget->setOrientation(Qt::Horizontal);
    connect(m_mainWidget, SIGNAL(splitterMoved(int, int)), this, SLOT(saveSplitterSizes()));

    m_filterBox = new FilterWidget(m_stack);
    connect(this, SIGNAL(backendReady(QApt::Backend *)),
            m_filterBox, SLOT(setBackend(QApt::Backend *)));
    connect(m_filterBox, SIGNAL(filterByGroup(const QString &)),
            m_managerWidget, SLOT(filterByGroup(const QString &)));
    connect(m_filterBox, SIGNAL(filterByStatus(const QApt::Package::State)),
            m_managerWidget, SLOT(filterByStatus(const QApt::Package::State)));
    connect(m_filterBox, SIGNAL(filterByOrigin(const QString &)),
            m_managerWidget, SLOT(filterByOrigin(const QString &)));

    m_mainWidget->addWidget(m_filterBox);
    m_mainWidget->addWidget(m_managerWidget);
    loadSplitterSizes();

    m_stack->addWidget(m_mainWidget);
    m_stack->setCurrentWidget(m_mainWidget);

    setupActions();

    m_statusWidget = new StatusWidget(this);
    connect(this, SIGNAL(backendReady(QApt::Backend *)),
            m_statusWidget, SLOT(setBackend(QApt::Backend *)));
    statusBar()->addWidget(m_statusWidget);
    statusBar()->show();
}

void MainWindow::initObject()
{
    MuonMainWindow::initObject();

    loadSettings();

    setActionsEnabled(); //Get initial enabled/disabled state

    m_managerWidget->setFocus();
}

void MainWindow::loadSettings()
{
    m_backend->setUndoRedoCacheSize(MuonSettings::self()->undoStackSize());
}

void MainWindow::loadSplitterSizes()
{
    QList<int> sizes = MuonSettings::self()->splitterSizes();

    if (sizes.isEmpty()) {
        sizes << 115 << (this->width() - 115);
    }
    m_mainWidget->setSizes(sizes);
}

void MainWindow::saveSplitterSizes()
{
    MuonSettings::self()->setSplitterSizes(m_mainWidget->sizes());
    MuonSettings::self()->writeConfig();
}

void MainWindow::setupActions()
{
    MuonMainWindow::setupActions();

    m_loadSelectionsAction = actionCollection()->addAction("open_markings");
    m_loadSelectionsAction->setIcon(KIcon("document-open"));
    m_loadSelectionsAction->setText(i18nc("@action", "Read Markings..."));
    connect(m_loadSelectionsAction, SIGNAL(triggered()), this, SLOT(loadSelections()));

    m_saveSelectionsAction = actionCollection()->addAction("save_markings");
    m_saveSelectionsAction->setIcon(KIcon("document-save-as"));
    m_saveSelectionsAction->setText(i18nc("@action", "Save Markings As..."));
    connect(m_saveSelectionsAction, SIGNAL(triggered()), this, SLOT(saveSelections()));

    m_saveInstalledAction = actionCollection()->addAction("save_package_list");
    m_saveInstalledAction->setIcon(KIcon("document-save-as"));
    m_saveInstalledAction->setText(i18nc("@action", "Save Installed Packages List..."));
    connect(m_saveInstalledAction, SIGNAL(triggered()), this, SLOT(saveInstalledPackagesList()));

    m_safeUpgradeAction = actionCollection()->addAction("safeupgrade");
    m_safeUpgradeAction->setIcon(KIcon("go-up"));
    m_safeUpgradeAction->setText(i18nc("@action Marks upgradeable packages for upgrade", "Cautious Upgrade"));
    connect(m_safeUpgradeAction, SIGNAL(triggered()), this, SLOT(markUpgrade()));

    m_distUpgradeAction = actionCollection()->addAction("fullupgrade");
    m_distUpgradeAction->setIcon(KIcon("go-top"));
    m_distUpgradeAction->setText(i18nc("@action Marks upgradeable packages, including ones that install/remove new things",
                                       "Full Upgrade"));
    connect(m_distUpgradeAction, SIGNAL(triggered()), this, SLOT(markDistUpgrade()));

    m_autoRemoveAction = actionCollection()->addAction("autoremove");
    m_autoRemoveAction->setIcon(KIcon("trash-empty"));
    m_autoRemoveAction->setText(i18nc("@action Marks packages no longer needed for removal",
                                      "Remove Unnecessary Packages"));
    connect(m_autoRemoveAction, SIGNAL(triggered()), this, SLOT(markAutoRemove()));

    m_previewAction = actionCollection()->addAction("preview");
    m_previewAction->setIcon(KIcon("document-preview-archive"));
    m_previewAction->setText(i18nc("@action Takes the user to the preview page", "Preview Changes"));
    connect(m_previewAction, SIGNAL(triggered()), this, SLOT(previewChanges()));

    m_applyAction = actionCollection()->addAction("apply");
    m_applyAction->setIcon(KIcon("dialog-ok-apply"));
    m_applyAction->setText(i18nc("@action Applys the changes a user has made", "Apply Changes"));
    connect(m_applyAction, SIGNAL(triggered()), this, SLOT(startCommit()));
    m_applyAction->setEnabled(isConnected());
    connect(this, SIGNAL(shouldConnect(bool)), m_applyAction, SLOT(setEnabled(bool)));

    KStandardAction::preferences(this, SLOT(editSettings()), actionCollection());

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_M), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(easterEggTriggered()));

    setActionsEnabled(false);

    setupGUI();
}

void MainWindow::markUpgrade()
{
    m_backend->saveCacheState();
    m_backend->markPackagesForUpgrade();

    if (m_backend-> markedPackages().isEmpty()) {
        QString text = i18nc("@label", "Unable to mark upgrades. The "
                             "available upgrades may require new packages to "
                             "be installed or removed. You may wish to try "
                             "a full upgrade by clicking the <interface>Full "
                             " Upgrade</interface> button.");
        QString title = i18nc("@title:window", "Unable to Mark Upgrades");
        KMessageBox::information(this, text, title);
    } else {
        previewChanges();
    }
}

void MainWindow::markDistUpgrade()
{
    m_backend->saveCacheState();
    m_backend->markPackagesForDistUpgrade();
    if (m_backend-> markedPackages().isEmpty()) {
        QString text = i18nc("@label", "Unable to mark upgrades. Some "
                             "upgrades may have unsatisfiable dependencies at "
                             "the moment, or may have been manually held back.");
        QString title = i18nc("@title:window", "Unable to Mark Upgrades");
        KMessageBox::information(this, text, title);
    } else {
        previewChanges();
    }
}

void MainWindow::markAutoRemove()
{
    m_backend->saveCacheState();
    m_backend->markPackagesForAutoRemove();
    previewChanges();
}

void MainWindow::checkForUpdates()
{
    setActionsEnabled(false);
    m_managerWidget->setEnabled(false);
    initDownloadWidget();
    m_backend->updateCache();
}

void MainWindow::errorOccurred(QApt::ErrorCode error, const QVariantMap &details)
{
    Q_UNUSED(details);

    MuonMainWindow::errorOccurred(error, details);

    switch(error) {
    case QApt::UserCancelError:
        if (m_downloadWidget) {
            m_downloadWidget->clear();
        }
    case QApt::AuthError:
        m_managerWidget->setEnabled(true);
        QApplication::restoreOverrideCursor();
        returnFromPreview();
        break;
    default:
        break;
    }
}

void MainWindow::workerEvent(QApt::WorkerEvent event)
{
    MuonMainWindow::workerEvent(event);

    switch (event) {
    case QApt::CacheUpdateStarted:
        if (m_downloadWidget) {
            m_downloadWidget->setHeaderText(i18nc("@info", "<title>Updating software sources</title>"));
            m_stack->setCurrentWidget(m_downloadWidget);
            connect(m_downloadWidget, SIGNAL(cancelDownload()), m_backend, SLOT(cancelDownload()));
        }
        break;
    case QApt::CacheUpdateFinished:
    case QApt::CommitChangesFinished:
        reload();
        returnFromPreview();
        if (m_warningStack.size() > 0) {
            showQueuedWarnings();
            m_warningStack.clear();
        }
        if (m_errorStack.size() > 0) {
            showQueuedErrors();
            m_errorStack.clear();
        }
        break;
    case QApt::PackageDownloadStarted:
        if (m_downloadWidget) {
            m_downloadWidget->setHeaderText(i18nc("@info", "<title>Downloading Packages</title>"));
            m_stack->setCurrentWidget(m_downloadWidget);
            connect(m_downloadWidget, SIGNAL(cancelDownload()), m_backend, SLOT(cancelDownload()));
        }
        QApplication::restoreOverrideCursor();
        break;
    case QApt::CommitChangesStarted:
        if (m_commitWidget) {
            m_commitWidget->setHeaderText(i18nc("@info", "<title>Committing Changes</title>"));
            m_stack->setCurrentWidget(m_commitWidget);
        }
        QApplication::restoreOverrideCursor();
        break;
    case QApt::XapianUpdateStarted:
        m_statusWidget->showXapianProgress();
        connect(m_backend, SIGNAL(xapianUpdateProgress(int)),
                m_statusWidget, SLOT(updateXapianProgress(int)));
        break;
    case QApt::XapianUpdateFinished:
        m_managerWidget->startSearch();
        disconnect(m_backend, SIGNAL(xapianUpdateProgress(int)),
                   m_statusWidget, SLOT(updateXapianProgress(int)));
        m_statusWidget->hideXapianProgress();
        break;
    case QApt::PackageDownloadFinished:
    case QApt::InvalidEvent:
    default:
        break;
    }
}

void MainWindow::previewChanges()
{
    m_reviewWidget = new ReviewWidget(m_stack);
    connect(this, SIGNAL(backendReady(QApt::Backend *)),
            m_reviewWidget, SLOT(setBackend(QApt::Backend *)));
    m_reviewWidget->setBackend(m_backend);
    m_stack->addWidget(m_reviewWidget);

    m_stack->setCurrentWidget(m_reviewWidget);

    m_previewAction->setIcon(KIcon("go-previous"));
    m_previewAction->setText(i18nc("@action:intoolbar Return from the preview page", "Back"));
    disconnect(m_previewAction, SIGNAL(triggered()), this, SLOT(previewChanges()));
    connect(m_previewAction, SIGNAL(triggered()), this, SLOT(returnFromPreview()));
}

void MainWindow::returnFromPreview()
{
    m_stack->setCurrentWidget(m_mainWidget);
    if (m_reviewWidget) {
        m_reviewWidget->deleteLater();
        m_reviewWidget = 0;
    }

    m_previewAction->setIcon(KIcon("document-preview-archive"));
    m_previewAction->setText(i18nc("@action", "Preview Changes"));
    disconnect(m_previewAction, SIGNAL(triggered()), this, SLOT(returnFromPreview()));
    connect(m_previewAction, SIGNAL(triggered()), this, SLOT(previewChanges()));
    // We may not have anything to preview; check.
    setActionsEnabled();
}

void MainWindow::startCommit()
{
    setActionsEnabled(false);
    m_managerWidget->setEnabled(false);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    initDownloadWidget();
    initCommitWidget();
    m_backend->commitChanges();
}

void MainWindow::initDownloadWidget()
{
    if (!m_downloadWidget) {
        m_downloadWidget = new DownloadWidget(this);
        m_stack->addWidget(m_downloadWidget);
        connect(m_backend, SIGNAL(downloadProgress(int, int, int)),
                m_downloadWidget, SLOT(updateDownloadProgress(int, int, int)));
        connect(m_backend, SIGNAL(packageDownloadProgress(const QString &, int, const QString &, double, int)),
                m_downloadWidget, SLOT(updatePackageDownloadProgress(const QString &, int, const QString &, double, int)));
    }
}

void MainWindow::initCommitWidget()
{
    if (!m_commitWidget) {
        m_commitWidget = new CommitWidget(this);
        m_stack->addWidget(m_commitWidget);
        connect(m_backend, SIGNAL(commitProgress(const QString &, int)),
                m_commitWidget, SLOT(updateCommitProgress(const QString &, int)));
    }
}

void MainWindow::reload()
{
    returnFromPreview();
    m_stack->setCurrentWidget(m_mainWidget);

    m_managerWidget->reload();
    if (m_reviewWidget) {
        m_reviewWidget->reload();
    }

    m_statusWidget->updateStatus();
    setActionsEnabled();
    m_managerWidget->setEnabled(true);

    // No need to keep these around in memory.
    if (m_downloadWidget) {
        m_downloadWidget->deleteLater();
        m_downloadWidget = 0;
    }
    if (m_commitWidget) {
        m_commitWidget->deleteLater();
        m_commitWidget = 0;
    }

    if (m_backend->xapianIndexNeedsUpdate()) {
        m_backend->updateXapianIndex();
    }
}

void MainWindow::setActionsEnabled(bool enabled)
{
    MuonMainWindow::setActionsEnabled(enabled);
    if (!enabled) {
        return;
    }

    int upgradeable = m_backend->packageCount(QApt::Package::Upgradeable);
    bool changesPending = m_backend->areChangesMarked();
    int autoRemoveable = m_backend->packageCount(QApt::Package::IsGarbage);

    m_safeUpgradeAction->setEnabled(upgradeable > 0);
    m_distUpgradeAction->setEnabled(upgradeable > 0);
    m_autoRemoveAction->setEnabled(autoRemoveable > 0);
    if (m_stack->currentWidget() == m_reviewWidget) {
        // We always need to be able to get back from review
        m_previewAction->setEnabled(true);
    } else {
        m_previewAction->setEnabled(changesPending);
    }

    if (isConnected()) {
        m_applyAction->setEnabled(changesPending);
    } else {
        m_applyAction->setEnabled(false);
    }

    m_undoAction->setEnabled(!m_backend->isUndoStackEmpty());
    m_redoAction->setEnabled(!m_backend->isRedoStackEmpty());
    m_revertAction->setEnabled(!m_backend->isUndoStackEmpty());

    m_loadSelectionsAction->setEnabled(true);
    m_saveSelectionsAction->setEnabled(changesPending);
    m_saveInstalledAction->setEnabled(true);
    m_softwarePropertiesAction->setEnabled(true);
}

void MainWindow::editSettings()
{
    if (!m_settingsDialog) {
        m_settingsDialog = new ManagerSettingsDialog(this, m_backend->config());
        connect(m_settingsDialog, SIGNAL(finished()), SLOT(closeSettingsDialog()));
        connect(m_settingsDialog, SIGNAL(settingsChanged()), SLOT(loadSettings()));
        m_settingsDialog->show();
    } else {
        m_settingsDialog->raise();
    }
}

void MainWindow::closeSettingsDialog()
{
    m_settingsDialog->deleteLater();
    m_settingsDialog = 0;
}

void MainWindow::easterEggTriggered()
{
    KDialog *dialog = new KDialog(this);
    KVBox *widget = new KVBox(dialog);
    QLabel *label = new QLabel(widget);
    label->setText(i18nc("@label Easter Egg", "This Muon has super cow powers"));
    QLabel *moo = new QLabel(widget);
    moo->setFont(QFont("monospace"));
    moo->setText("             (__)\n"
                 "             (oo)\n"
                 "    /---------\\/\n"
                 "   / | Muuu!!||\n"
                 "  *  ||------||\n"
                 "     ^^      ^^\n");

    dialog->setMainWidget(widget);
    dialog->show();

    QString mooFile = KStandardDirs::locate("appdata", "moo.ogg");
    Phonon::MediaObject *music =
    Phonon::createPlayer(Phonon::MusicCategory,
                         Phonon::MediaSource(mooFile));
    music->play();
}

void MainWindow::revertChanges()
{
    MuonMainWindow::revertChanges();

    if (m_reviewWidget) {
        returnFromPreview();
    }
}

#include "MainWindow.moc"
