/*
 * api_client.h — HTTP-клиент, реализующий паттерн «Синглтон»
 *
 * ╔══════════════════════════════════════════════════════════════════╗
 * ║                 ПАТТЕРН «СИНГЛТОН» (Singleton)                  ║
 * ╠══════════════════════════════════════════════════════════════════╣
 * ║  Задача: гарантировать, что в приложении существует ровно       ║
 * ║  ОДИН экземпляр QNetworkAccessManager. Создание нескольких      ║
 * ║  менеджеров приводит к дублированию соединений и cookie-хранилищ.║
 * ║                                                                  ║
 * ║  Реализация (Meyers' Singleton):                                 ║
 * ║  • статическая локальная переменная в instance()                ║
 * ║  • конструктор объявлен private                                 ║
 * ║  • копирующий конструктор и operator= удалены (= delete)        ║
 * ╚══════════════════════════════════════════════════════════════════╝
 *
 * ApiClient также является «клиентом» паттерна «Адаптер»:
 * он хранит указатель на IQuizAdapter и использует его
 * для преобразования ответов сервера в Quiz-объекты,
 * не зная о деталях реализации парсинга.
 *
 * Коммуникация с вызывающим кодом — через механизм сигналов Qt:
 *   quizzesReceived — список тестов успешно получен и десериализован
 *   quizReceived    — один тест успешно получен и десериализован
 *   quizCreated     — тест создан (POST)
 *   quizUpdated     — тест обновлён (PUT)
 *   quizDeleted     — тест удалён (DELETE)
 *   errorOccurred   — произошла сетевая ошибка или ошибка парсинга
 */

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
