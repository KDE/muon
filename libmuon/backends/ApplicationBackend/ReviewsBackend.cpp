/***************************************************************************
 *   Copyright © 2011 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "ReviewsBackend.h"

#include <QtCore/QStringBuilder>
#include <QDebug>

#include <KIO/Job>
#include <KStandardDirs>
#include <KTemporaryFile>
#include <KFilterDev>

#include <LibQApt/Backend>

#include <qjson/parser.h>
#include <qjson/serializer.h>

#include <QtOAuth/interface.h>

#include "ApplicationBackend.h"
#include "QAptResource.h"
#include <ReviewsBackend/Rating.h>
#include <ReviewsBackend/Review.h>
#include <ReviewsBackend/AbstractLoginBackend.h>
#include "UbuntuLoginBackend.h"
#include <MuonDataSources.h>
#include "OAuthSession.h"

ReviewsBackend::ReviewsBackend(QObject *parent)
        : AbstractReviewsBackend(parent)
        , m_aptBackend(nullptr)
        , m_serverBase(MuonDataSources::rnRSource())
{
    QMetaObject::invokeMethod(this, "fetchRatings", Qt::QueuedConnection);
}

void ReviewsBackend::setAptBackend(QApt::Backend *aptBackend)
{
    m_aptBackend = aptBackend;
}

void ReviewsBackend::fetchRatings()
{
    QString ratingsCache = KStandardDirs::locateLocal("data", "libmuon/ratings.txt");
    OAuthSession::global()->refreshConsumerKeys();

    // First, load our old ratings cache in case we don't have net connectivity
    loadRatingsFromFile();

    // Try to fetch the latest ratings from the internet
    KUrl ratingsUrl(m_serverBase, "review-stats/");
    KIO::FileCopyJob *getJob = KIO::file_copy(ratingsUrl,
                               ratingsCache, -1,
                               KIO::Overwrite | KIO::HideProgressInfo);
    connect(getJob, SIGNAL(result(KJob*)), SLOT(ratingsFetched(KJob*)));
}

void ReviewsBackend::ratingsFetched(KJob *job)
{
    if (job->error()) {
        return;
    }

    loadRatingsFromFile();
}

void ReviewsBackend::loadRatingsFromFile()
{
    QString ratingsCache = KStandardDirs::locateLocal("data", "libmuon/ratings.txt");
    QIODevice* dev = KFilterDev::deviceForFile(ratingsCache, "application/x-gzip");

    QJson::Parser parser;
    bool ok = false;
    QVariant ratings = parser.parse(dev, &ok);

    if (!ok) {
        qDebug() << "error while parsing ratings: " << ratingsCache;
        return;
    }

    qDeleteAll(m_ratings);
    m_ratings.clear();
    foreach (const QVariant &data, ratings.toList()) {
        Rating *rating = new Rating(data.toMap());
        if (!rating->ratingCount()) {
            delete rating;
            continue;
        }
        rating->setParent(this);
        m_ratings[rating->packageName()] = rating;
    }
    emit ratingsReady();
}

Rating *ReviewsBackend::ratingForApplication(AbstractResource* app) const
{
    return m_ratings.value(app->packageName());
}

void ReviewsBackend::stopPendingJobs()
{
    for(auto it = m_jobHash.constBegin(); it != m_jobHash.constEnd(); ++it) {
        disconnect(it.key(), SIGNAL(result(KJob*)), this, SLOT(changelogFetched(KJob*)));
    }
    m_jobHash.clear();
}

void ReviewsBackend::fetchReviews(AbstractResource* res, int page)
{
    QAptResource *app = qobject_cast<QAptResource *>(res);

    QString packageName = app->packageName();
    if (packageName.contains(':'))
        packageName = packageName.left(packageName.indexOf(':'));
    QString hashName = packageName + app->name();
    
    // Check our cache before fetching from the 'net
    QList<Review*> revs = m_reviewsCache.value(hashName);
    if (revs.size()>(page*10)) { //there are 10 reviews per page
        emit reviewsReady(app, revs.mid(page*10, 10));
        return;
    }

    QString lang = ApplicationBackend::getLanguage();
    QString origin = app->origin().toLower();

    QString version = QLatin1String("any");
    QString appName = app->name();
    // Replace spaces with %2B for the url
    appName.replace(' ', QLatin1String("%2B"));

    // Figuring out how this damn Django url was put together took more
    // time than figuring out QJson...
    // But that could be because the Ubuntu Software Center (which I used to
    // figure it out) is written in python, so you have to go hunting to where
    // a variable was initially initialized with a primitive to figure out its type.
    KUrl reviewsUrl(m_serverBase, QLatin1String("reviews/filter/") % lang % '/'
            % origin % '/' % QLatin1String("any") % '/' % version % '/' % packageName
            % ';' % appName % '/' % QLatin1String("page") % '/' % QString::number(page));

    KIO::StoredTransferJob* getJob = KIO::storedGet(reviewsUrl, KIO::NoReload, KIO::Overwrite | KIO::HideProgressInfo);
    m_jobHash[getJob] = app;
    connect(getJob, SIGNAL(result(KJob*)),
            this, SLOT(reviewsFetched(KJob*)));
}

static Review* constructReview(const QVariantMap& data)
{
    QString reviewUsername = data.value("reviewer_username").toString();
    QString reviewDisplayName = data.value("reviewer_displayname").toString();
    QString reviewer = reviewDisplayName.isEmpty() ? reviewUsername : reviewDisplayName;
    return new Review(
        data.value("app_name").toString(),
        data.value("package_name").toString(),
        data.value("language").toString(),
        data.value("summary").toString(),
        data.value("review_text").toString(),
        reviewer,
        QDateTime::fromString(data.value("date_created").toString(), "yyyy-MM-dd HH:mm:ss"),
        !data.value("hide").toBool(),
        data.value("id").toULongLong(),
        data.value("rating").toInt() * 2,
        data.value("usefulness_total").toInt(),
        data.value("usefulness_favorable").toInt(),
        data.value("version").toString());
}

void ReviewsBackend::reviewsFetched(KJob *j)
{
    KIO::StoredTransferJob* job = qobject_cast<KIO::StoredTransferJob*>(j);
    QAptResource *app = m_jobHash.take(job);
    if (!app || job->error()) {
        return;
    }

    QJson::Parser parser;
    bool ok = false;
    QVariant reviews = parser.parse(job->data(), &ok);

    if (!ok || !app) {
        return;
    }

    QList<Review *> reviewsList;
    foreach (const QVariant &data, reviews.toList()) {
        reviewsList << constructReview(data.toMap());
    }

    QString packageName = app->packageName();
    if (packageName.contains(':'))
        packageName = packageName.left(packageName.indexOf(':'));
    m_reviewsCache[packageName + app->name()].append(reviewsList);

    emit reviewsReady(app, reviewsList);
}

void ReviewsBackend::submitUsefulness(Review* r, bool useful)
{
    QVariantMap data;
    data["useful"] = useful;

    OAuthPost info(m_serverBase, QString("reviews/%1/recommendations/").arg(r->id()), data);
    OAuthSession::global()->postInformation(info);
}

void ReviewsBackend::submitReview(AbstractResource* application, const QString& summary,
                      const QString& review_text, const QString& rating)
{
    QAptResource *app = qobject_cast<QAptResource *>(application);
    
    QVariantMap data;
    data["app_name"] = app->name();
    data["package_name"] = app->packageName();
    data["summary"] = summary;
    data["version"] = app->package()->version();
    data["review_text"] = review_text;
    data["rating"] = rating;
    data["language"] = ApplicationBackend::getLanguage();
    data["origin"] = app->package()->origin();
    data["distroseries"] = ApplicationBackend::codeName("DISTRIB_CODENAME");
    data["arch_tag"] = app->package()->architecture();
    
    OAuthPost info(m_serverBase, QLatin1String("reviews/"), data);
    OAuthSession::global()->postInformation(info);
}

void ReviewsBackend::deleteReview(Review* r)
{
    OAuthPost info(m_serverBase, QString("reviews/delete/%1/").arg(r->id()),
                   QVariantMap());
    OAuthSession::global()->postInformation(info);
}

void ReviewsBackend::flagReview(Review* r, const QString& reason, const QString& text)
{
    QVariantMap data;
    data["reason"] = reason;
    data["text"] = text;

    OAuthPost info(m_serverBase, QString("reviews/%1/flags/").arg(r->id()), data);
    OAuthSession::global()->postInformation(info);
}

bool ReviewsBackend::isFetching() const
{
    return !m_jobHash.isEmpty();
}

bool ReviewsBackend::hasCredentials() const
{
    return OAuthSession::global()->loginBackend()->hasCredentials();
}

QString ReviewsBackend::userName() const
{
    Q_ASSERT(hasCredentials());
    return OAuthSession::global()->loginBackend()->displayName();
}

void ReviewsBackend::login()
{
    Q_ASSERT(!hasCredentials());
    OAuthSession::global()->loginBackend()->login();
}

void ReviewsBackend::registerAndLogin()
{
    Q_ASSERT(!hasCredentials());
    OAuthSession::global()->loginBackend()->registerAndLogin();
}

void ReviewsBackend::logout()
{
    Q_ASSERT(hasCredentials());
    OAuthSession::global()->loginBackend()->logout();
}
