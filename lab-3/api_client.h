#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QList>
#include "quiz.h"
#include "iquiz_adapter.h"

class ApiClient : public QObject
{
    Q_OBJECT

public:
    static ApiClient &instance();

    // GET /api/quiz/
    void fetchQuizzes();

    // GET /api/quiz/:id/
    void fetchQuiz(int id);

    // POST /api/quiz/
    void createQuiz(const QJsonObject &data);

    // PUT /api/quiz/:id/
    void updateQuiz(int id, const QJsonObject &data);

    // DELETE /api/quiz/:id/
    void deleteQuiz(int id);

signals:
    void quizzesReceived(QList<Quiz> quizzes);
    void quizReceived(Quiz quiz);
    void quizCreated(Quiz quiz);
    void quizUpdated(Quiz quiz);
    void quizDeleted(QString message);
    void errorOccurred(QString error);

private:
    explicit ApiClient(QObject *parent = nullptr);
    ~ApiClient() override;

    ApiClient(const ApiClient &)            = delete;
    ApiClient &operator=(const ApiClient &) = delete;

    QNetworkAccessManager *m_manager;
    IQuizAdapter          *m_adapter;
    QString                m_baseUrl;
};
