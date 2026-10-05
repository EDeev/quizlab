#include "api_client.h"
#include "quiz_json_adapter.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

// Реализация паттерна Синглтон
ApiClient &ApiClient::instance()
{
    static ApiClient s_instance;
    return s_instance;
}

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_adapter(new QuizJsonAdapter())
    // адрес API можно переопределить переменной окружения QUIZ_API_URL
    , m_baseUrl(qEnvironmentVariable("QUIZ_API_URL", "http://localhost:80"))
{
}

ApiClient::~ApiClient()
{
    delete m_adapter;
}

// fetchQuizzes — GET /api/quiz/
void ApiClient::fetchQuizzes()
{
    QNetworkRequest request(QUrl(m_baseUrl + "/api/quiz/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
            return;
        }

        // reply->readAll() — читаем весь буфер HTTP-ответа
        QByteArray    rawData = reply->readAll();
        QJsonDocument doc     = QJsonDocument::fromJson(rawData);

        if (!doc.isArray()) {
            emit errorOccurred("Unexpected JSON format: array expected");
            return;
        }

        QList<Quiz> quizzes = m_adapter->toQuizList(doc.array());
        emit quizzesReceived(quizzes);
    });
}

// fetchQuiz — GET /api/quiz/:id/
void ApiClient::fetchQuiz(int id)
{
    QNetworkRequest request(QUrl(m_baseUrl + "/api/quiz/" + QString::number(id) + "/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = m_manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
            return;
        }

        QByteArray    rawData = reply->readAll();
        QJsonDocument doc     = QJsonDocument::fromJson(rawData);

        if (!doc.isObject()) {
            emit errorOccurred("Unexpected JSON format: object expected");
            return;
        }

        emit quizReceived(m_adapter->toQuiz(doc.object()));
    });
}

// createQuiz — POST /api/quiz/
void ApiClient::createQuiz(const QJsonObject &data)
{
    QNetworkRequest request(QUrl(m_baseUrl + "/api/quiz/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QByteArray body = QJsonDocument(data).toJson(QJsonDocument::Compact);
    QNetworkReply *reply = m_manager->post(request, body);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
            return;
        }
        QByteArray    rawData = reply->readAll();
        QJsonDocument doc     = QJsonDocument::fromJson(rawData);
        if (!doc.isObject()) {
            emit errorOccurred("POST: unexpected JSON format");
            return;
        }
        emit quizCreated(m_adapter->toQuiz(doc.object()));
    });
}

// updateQuiz — PUT /api/quiz/:id/
void ApiClient::updateQuiz(int id, const QJsonObject &data)
{
    QNetworkRequest request(QUrl(m_baseUrl + "/api/quiz/" + QString::number(id) + "/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QByteArray body = QJsonDocument(data).toJson(QJsonDocument::Compact);
    QNetworkReply *reply = m_manager->put(request, body);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
            return;
        }
        QByteArray    rawData = reply->readAll();
        QJsonDocument doc     = QJsonDocument::fromJson(rawData);
        if (!doc.isObject()) {
            emit errorOccurred("PUT: unexpected JSON format");
            return;
        }
        emit quizUpdated(m_adapter->toQuiz(doc.object()));
    });
}

// deleteQuiz — DELETE /api/quiz/:id/
void ApiClient::deleteQuiz(int id)
{
    QNetworkRequest request(QUrl(m_baseUrl + "/api/quiz/" + QString::number(id) + "/"));
    QNetworkReply *reply = m_manager->deleteResource(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
            return;
        }
        QByteArray    rawData = reply->readAll();
        QJsonDocument doc     = QJsonDocument::fromJson(rawData);
        QString message = "Тест удалён";
        if (doc.isObject() && doc.object().contains("detail"))
            message = doc.object()["detail"].toString();
        emit quizDeleted(message);
    });
}
