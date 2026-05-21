#pragma once

#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include "quiz.h"

class IQuizAdapter
{
public:
    virtual ~IQuizAdapter() = default;

    virtual Quiz toQuiz(const QJsonObject &json) const = 0;

    virtual QList<Quiz> toQuizList(const QJsonArray &json) const = 0;
};
