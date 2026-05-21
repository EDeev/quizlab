#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "api_client.h"

#include <QPushButton>
#include <QInputDialog>
#include <QDialog>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QColor>
#include <QFont>
#include <QDebug>

static const int COL_ID          = 0;
static const int COL_TITLE       = 1;
static const int COL_AUTHOR      = 2;
static const int COL_TIME        = 3;
static const int COL_PUBLISHED   = 4;
static const int COL_CREATED     = 5;
static const int COL_DESCRIPTION = 6;
static const int COL_COUNT       = 7;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/app.svg"));

    setupTable();

    ui->tabWidget->tabBar()->hide();

    // Кнопки переключения вида
    auto *btnTable = new QPushButton("Таблица", this);
    auto *btnText  = new QPushButton("Текст",   this);

    QString activeStyle   = "QPushButton{background:#1f2937;color:#f9fafb;border:1px solid #4b5563;border-radius:4px;padding:2px 12px;font-weight:bold;}";
    QString inactiveStyle = "QPushButton{background:#111827;color:#6b7280;border:1px solid #374151;border-radius:4px;padding:2px 12px;}";

    btnTable->setStyleSheet(activeStyle);
    btnText->setStyleSheet(inactiveStyle);
    btnTable->setFixedHeight(22);
    btnText->setFixedHeight(22);

    connect(btnTable, &QPushButton::clicked, this, [=]() {
        ui->tabWidget->setCurrentIndex(0);
        btnTable->setStyleSheet(activeStyle);
        btnText->setStyleSheet(inactiveStyle);
    });
    connect(btnText, &QPushButton::clicked, this, [=]() {
        ui->tabWidget->setCurrentIndex(1);
        btnText->setStyleSheet(activeStyle);
        btnTable->setStyleSheet(inactiveStyle);
    });

    statusBar()->addPermanentWidget(btnTable);
    statusBar()->addPermanentWidget(btnText);
    statusBar()->setStyleSheet("QStatusBar::item { border: none; }");

    ApiClient &client = ApiClient::instance();
    connect(&client, &ApiClient::quizzesReceived, this, &MainWindow::onQuizzesReceived);
    connect(&client, &ApiClient::quizReceived,    this, &MainWindow::onQuizReceived);
    connect(&client, &ApiClient::quizCreated,     this, &MainWindow::onQuizCreated);
    connect(&client, &ApiClient::quizUpdated,     this, &MainWindow::onQuizUpdated);
    connect(&client, &ApiClient::quizDeleted,     this, &MainWindow::onQuizDeleted);
    connect(&client, &ApiClient::errorOccurred,   this, &MainWindow::onError);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupTable()
{
    ui->tableWidget->setColumnCount(COL_COUNT);
    ui->tableWidget->setHorizontalHeaderLabels({
        "ID", "Название", "Автор", "Время (мин)", "Опубликован", "Создан", "Описание"
    });

    QHeaderView *header = ui->tableWidget->horizontalHeader();
    header->setSectionResizeMode(COL_ID,          QHeaderView::ResizeToContents);
    header->setSectionResizeMode(COL_TITLE,       QHeaderView::Stretch);
    header->setSectionResizeMode(COL_AUTHOR,      QHeaderView::ResizeToContents);
    header->setSectionResizeMode(COL_TIME,        QHeaderView::ResizeToContents);
    header->setSectionResizeMode(COL_PUBLISHED,   QHeaderView::ResizeToContents);
    header->setSectionResizeMode(COL_CREATED,     QHeaderView::ResizeToContents);
    header->setSectionResizeMode(COL_DESCRIPTION, QHeaderView::Stretch);

    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setWordWrap(true);
    ui->tableWidget->setTextElideMode(Qt::ElideNone);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(52);
    ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

// ── Обработчики кнопок ────────────────────────────────────────────────────────
void MainWindow::on_btnGetAll_clicked()
{
    ++m_requestCount;
    setStatus(QString("GET /api/quiz/…  |  Запросов: %1").arg(m_requestCount));
    ApiClient::instance().fetchQuizzes();
}

void MainWindow::on_btnGetById_clicked()
{
    bool ok;
    int id = QInputDialog::getInt(this, "Get Quiz by ID", "ID теста:", 1, 1, 99999, 1, &ok);
    if (!ok) return;
    ++m_requestCount;
    setStatus(QString("GET /api/quiz/%1/…  |  Запросов: %2").arg(id).arg(m_requestCount));
    ApiClient::instance().fetchQuiz(id);
}

void MainWindow::on_btnCreate_clicked()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Создать тест  —  POST /api/quiz/");
    dlg.setMinimumWidth(400);

    QLineEdit *titleEdit      = new QLineEdit(&dlg);
    QTextEdit *descEdit       = new QTextEdit(&dlg);
    QLineEdit *authorEdit     = new QLineEdit(&dlg);
    QSpinBox  *timeLimitSpin  = new QSpinBox(&dlg);
    QCheckBox *publishedCheck = new QCheckBox("Опубликован", &dlg);

    descEdit->setMaximumHeight(80);
    timeLimitSpin->setRange(1, 600);
    timeLimitSpin->setValue(30);
    timeLimitSpin->setSuffix(" мин");

    {
        auto *layout = new QVBoxLayout();
        auto *form   = new QFormLayout();
        form->addRow("Название:",    titleEdit);
        form->addRow("Описание:",    descEdit);
        form->addRow("Автор:",       authorEdit);
        form->addRow("Время:",       timeLimitSpin);
        form->addRow("",             publishedCheck);
        layout->addLayout(form);
        auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        layout->addWidget(buttons);
        dlg.setLayout(layout);
    }

    if (dlg.exec() != QDialog::Accepted) return;

    QJsonObject data;
    data["title"]        = titleEdit->text();
    data["description"]  = descEdit->toPlainText();
    data["author"]       = authorEdit->text();
    data["time_limit"]   = timeLimitSpin->value();
    data["is_published"] = publishedCheck->isChecked();

    ++m_requestCount;
    setStatus(QString("POST /api/quiz/…  |  Запросов: %1").arg(m_requestCount));
    ApiClient::instance().createQuiz(data);
}

void MainWindow::on_btnUpdate_clicked()
{
    bool ok;
    int id = QInputDialog::getInt(this, "Update Quiz", "ID теста для обновления:", 1, 1, 99999, 1, &ok);
    if (!ok) return;

    QDialog dlg(this);
    dlg.setWindowTitle(QString("Обновить тест ID=%1  —  PUT /api/quiz/%1/").arg(id));
    dlg.setMinimumWidth(400);

    QLineEdit *titleEdit      = new QLineEdit(&dlg);
    QTextEdit *descEdit       = new QTextEdit(&dlg);
    QLineEdit *authorEdit     = new QLineEdit(&dlg);
    QSpinBox  *timeLimitSpin  = new QSpinBox(&dlg);
    QCheckBox *publishedCheck = new QCheckBox("Опубликован", &dlg);

    descEdit->setMaximumHeight(80);
    timeLimitSpin->setRange(1, 600);
    timeLimitSpin->setValue(30);
    timeLimitSpin->setSuffix(" мин");

    {
        auto *layout = new QVBoxLayout();
        auto *form   = new QFormLayout();
        form->addRow("Название:",    titleEdit);
        form->addRow("Описание:",    descEdit);
        form->addRow("Автор:",       authorEdit);
        form->addRow("Время:",       timeLimitSpin);
        form->addRow("",             publishedCheck);
        layout->addLayout(form);
        auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        layout->addWidget(buttons);
        dlg.setLayout(layout);
    }

    if (dlg.exec() != QDialog::Accepted) return;

    QJsonObject data;
    data["title"]        = titleEdit->text();
    data["description"]  = descEdit->toPlainText();
    data["author"]       = authorEdit->text();
    data["time_limit"]   = timeLimitSpin->value();
    data["is_published"] = publishedCheck->isChecked();

    ++m_requestCount;
    setStatus(QString("PUT /api/quiz/%1/…  |  Запросов: %2").arg(id).arg(m_requestCount));
    ApiClient::instance().updateQuiz(id, data);
}

void MainWindow::on_btnDelete_clicked()
{
    bool ok;
    int id = QInputDialog::getInt(this, "Delete Quiz", "ID теста для удаления:", 1, 1, 99999, 1, &ok);
    if (!ok) return;
    ++m_requestCount;
    setStatus(QString("DELETE /api/quiz/%1/…  |  Запросов: %2").arg(id).arg(m_requestCount));
    ApiClient::instance().deleteQuiz(id);
}

void MainWindow::on_btnClear_clicked()
{
    ui->tableWidget->setRowCount(0);
    ui->outputEdit->clear();
    ui->lblStatus->setText("<b>Результат:</b>");
    setStatus("Очищено");
}

// ── Слоты ответов ApiClient ───────────────────────────────────────────────────
void MainWindow::onQuizzesReceived(const QList<Quiz> &quizzes)
{
    showQuizzes(quizzes);

    ui->outputEdit->clear();
    ui->outputEdit->append(QString("<b>Тестов получено: %1</b>").arg(quizzes.size()));
    for (const Quiz &q : quizzes) {
        ui->outputEdit->append("─────────────────────────────");
        appendQuizText(q);
    }

    ui->lblStatus->setText(QString("<b>Результат:</b> получено тестов: <b>%1</b>").arg(quizzes.size()));
    setStatus(QString("Готово — получено %1 тестов  |  Запросов: %2").arg(quizzes.size()).arg(m_requestCount));
    qDebug() << "[GET ALL] count:" << quizzes.size();
}

void MainWindow::onQuizReceived(const Quiz &q)
{
    showSingleQuiz(q, "GET");

    ui->outputEdit->clear();
    ui->outputEdit->append("<b>GET — тест по ID</b>");
    ui->outputEdit->append("─────────────────────────────");
    appendQuizText(q);

    ui->lblStatus->setText(QString("<b>Результат:</b> тест ID=<b>%1</b>").arg(q.id));
    setStatus(QString("Готово — тест ID=%1  |  Запросов: %2").arg(q.id).arg(m_requestCount));
    qDebug() << "[GET] ID:" << q.id << q.title;
}

void MainWindow::onQuizCreated(const Quiz &q)
{
    showSingleQuiz(q, "POST");

    ui->outputEdit->clear();
    ui->outputEdit->append("<b>POST — тест создан</b>");
    ui->outputEdit->append("─────────────────────────────");
    appendQuizText(q);

    ui->lblStatus->setText(QString("<b>Результат:</b> тест создан, ID=<b>%1</b>").arg(q.id));
    setStatus(QString("Тест создан, ID=%1  |  Запросов: %2").arg(q.id).arg(m_requestCount));
    qDebug() << "[CREATE] ID:" << q.id << q.title;
}

void MainWindow::onQuizUpdated(const Quiz &q)
{
    showSingleQuiz(q, "PUT");

    ui->outputEdit->clear();
    ui->outputEdit->append("<b>PUT — тест обновлён</b>");
    ui->outputEdit->append("─────────────────────────────");
    appendQuizText(q);

    ui->lblStatus->setText(QString("<b>Результат:</b> тест обновлён, ID=<b>%1</b>").arg(q.id));
    setStatus(QString("Тест обновлён, ID=%1  |  Запросов: %2").arg(q.id).arg(m_requestCount));
    qDebug() << "[UPDATE] ID:" << q.id << q.title;
}

void MainWindow::onQuizDeleted(const QString &message)
{
    ui->tableWidget->setRowCount(0);
    ui->outputEdit->clear();
    ui->outputEdit->append("<b>DELETE — успешно</b>");
    ui->outputEdit->append(message);

    ui->lblStatus->setText(QString("<b>Результат:</b> <span style='color:#4ade80'>%1</span>").arg(message));
    setStatus(QString("Тест удалён  |  Запросов: %1").arg(m_requestCount));
    qDebug() << "[DELETE]" << message;
}

void MainWindow::onError(const QString &error)
{
    ui->outputEdit->append(
        QString("<span style='color:#f87171'><b>Ошибка:</b> %1</span>").arg(error));
    ui->lblStatus->setText(QString("<b>Результат:</b> <span style='color:#f87171'>Ошибка: %1</span>").arg(error));
    setStatus(QString("Ошибка  |  Запросов: %1").arg(m_requestCount));
    qDebug() << "[ERROR]" << error;
}

// ── Вспомогательные методы ────────────────────────────────────────────────────
void MainWindow::showQuizzes(const QList<Quiz> &quizzes)
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setRowCount(quizzes.size());

    for (int i = 0; i < quizzes.size(); ++i) {
        const Quiz &q = quizzes[i];

        QColor rowColor = q.isPublished ? QColor("#052e16") : QColor("#1f2937");

        auto makeItem = [&](const QString &text) {
            auto *item = new QTableWidgetItem(text);
            item->setBackground(rowColor);
            item->setForeground(QColor("#f9fafb"));
            item->setTextAlignment(Qt::AlignCenter);
            return item;
        };

        ui->tableWidget->setItem(i, COL_ID,          makeItem(QString::number(q.id)));
        ui->tableWidget->setItem(i, COL_TITLE,       makeItem(q.title));
        ui->tableWidget->setItem(i, COL_AUTHOR,      makeItem(q.author));
        ui->tableWidget->setItem(i, COL_TIME,        makeItem(QString::number(q.timeLimit)));
        ui->tableWidget->setItem(i, COL_DESCRIPTION, makeItem(q.description));
        ui->tableWidget->setItem(i, COL_CREATED,     makeItem(q.createdAt.toString("dd.MM.yyyy")));

        auto *pubItem = new QTableWidgetItem(q.isPublished ? "Да" : "Нет");
        pubItem->setBackground(rowColor);
        pubItem->setForeground(q.isPublished ? QColor("#4ade80") : QColor("#9ca3af"));
        pubItem->setTextAlignment(Qt::AlignCenter);
        QFont f = pubItem->font();
        f.setBold(true);
        pubItem->setFont(f);
        ui->tableWidget->setItem(i, COL_PUBLISHED, pubItem);
    }
}

void MainWindow::showSingleQuiz(const Quiz &q, const QString &operation)
{
    ui->tableWidget->setRowCount(1);

    QColor rowColor = QColor("#1f2937");
    if (operation == "POST") rowColor = QColor("#052e16");
    else if (operation == "PUT") rowColor = QColor("#422006");

    auto makeItem = [&](const QString &text) {
        auto *item = new QTableWidgetItem(text);
        item->setBackground(rowColor);
        item->setForeground(QColor("#f9fafb"));
        item->setTextAlignment(Qt::AlignCenter);
        return item;
    };

    ui->tableWidget->setItem(0, COL_ID,          makeItem(QString::number(q.id)));
    ui->tableWidget->setItem(0, COL_TITLE,       makeItem(q.title));
    ui->tableWidget->setItem(0, COL_AUTHOR,      makeItem(q.author));
    ui->tableWidget->setItem(0, COL_TIME,        makeItem(QString::number(q.timeLimit)));
    ui->tableWidget->setItem(0, COL_DESCRIPTION, makeItem(q.description));
    ui->tableWidget->setItem(0, COL_CREATED,     makeItem(q.createdAt.toString("dd.MM.yyyy")));

    auto *pubItem = new QTableWidgetItem(q.isPublished ? "Да" : "Нет");
    pubItem->setBackground(rowColor);
    pubItem->setForeground(q.isPublished ? QColor("#4ade80") : QColor("#9ca3af"));
    pubItem->setTextAlignment(Qt::AlignCenter);
    QFont f = pubItem->font();
    f.setBold(true);
    pubItem->setFont(f);
    ui->tableWidget->setItem(0, COL_PUBLISHED, pubItem);
}

void MainWindow::appendQuizText(const Quiz &q)
{
    ui->outputEdit->append(QString(
        "ID:          %1\n"
        "Название:    %2\n"
        "Описание:    %3\n"
        "Автор:       %4\n"
        "Создан:      %5\n"
        "Время (мин): %6\n"
        "Опубликован: %7"
    ).arg(q.id)
     .arg(q.title)
     .arg(q.description)
     .arg(q.author)
     .arg(q.createdAt.toString(Qt::ISODate))
     .arg(q.timeLimit)
     .arg(q.isPublished ? "Да" : "Нет"));
}

void MainWindow::setStatus(const QString &msg)
{
    statusBar()->showMessage(msg);
}
