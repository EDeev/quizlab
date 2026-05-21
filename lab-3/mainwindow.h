#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include "quiz.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_btnGetAll_clicked();
    void on_btnGetById_clicked();
    void on_btnCreate_clicked();
    void on_btnUpdate_clicked();
    void on_btnDelete_clicked();
    void on_btnClear_clicked();

    void onQuizzesReceived(const QList<Quiz> &quizzes);
    void onQuizReceived(const Quiz &quiz);
    void onQuizCreated(const Quiz &quiz);
    void onQuizUpdated(const Quiz &quiz);
    void onQuizDeleted(const QString &message);
    void onError(const QString &error);

private:
    void setupTable();
    void showQuizzes(const QList<Quiz> &quizzes);
    void showSingleQuiz(const Quiz &q, const QString &operation);
    void appendQuizText(const Quiz &q);
    void setStatus(const QString &msg);

    Ui::MainWindow *ui;
    int m_requestCount = 0;
};

#endif // MAINWINDOW_H
