#pragma once

#include <QString>
#include <QDateTime>

class Quiz
{
public:
    Quiz() = default;

    int id = 0;

    QString title;
    QString description;
    QString author;
    QDateTime createdAt;

    int timeLimit = 0;
    bool isPublished = false;
};
