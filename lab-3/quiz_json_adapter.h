#pragma once

#include "iquiz_adapter.h"

class QuizJsonAdapter : public IQuizAdapter
{
public:
    Quiz        toQuiz    (const QJsonObject &json) const override;
    QList<Quiz> toQuizList(const QJsonArray  &json) const override;
};
