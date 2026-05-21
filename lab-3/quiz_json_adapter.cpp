#include "quiz_json_adapter.h"
#include <QJsonValue>

// toQuiz — десериализация одного объекта
Quiz QuizJsonAdapter::toQuiz(const QJsonObject &json) const
{
    Quiz q;

    q.id = json["id"].toInt();

    q.title       = json["title"].toString();
    q.description = json["description"].toString();
    q.author      = json["author"].toString();

    q.createdAt = QDateTime::fromString(json["created_at"].toString(),
                                        Qt::ISODateWithMs);

    q.timeLimit = json["time_limit"].toInt();

    q.isPublished = json["is_published"].toBool();

    return q;
}

// toQuizList — десериализация массива объектов
QList<Quiz> QuizJsonAdapter::toQuizList(const QJsonArray &json) const
{
    QList<Quiz> list;

    list.reserve(json.size());

    for (const QJsonValue &value : json) {
        if (value.isObject()) {
            list.append(toQuiz(value.toObject()));
        }
    }

    return list;
}
