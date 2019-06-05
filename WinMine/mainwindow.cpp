#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    ui->setupUi(this);

    setWindowTitle("MineSweeper");

    auto action1 = new QAction(tr("Beginer"), this);
    QObject::connect(action1, &QAction::triggered, [this]() {
        ui->stage->init(8, 8, 10, true);
        });

    auto action2 = new QAction(tr("Intermediate"), this);
    QObject::connect(action2, &QAction::triggered, [this]() {
        ui->stage->init(16, 16, 40, true);
        });

    auto action3 = new QAction(tr("Export"), this);
    QObject::connect(action3, &QAction::triggered, [this]() {
        ui->stage->init(30, 24, 99, true);
        });

    auto level_menu = new QMenu(tr("New Game"));
    level_menu->addAction(action1);
    level_menu->addAction(action2);
    level_menu->addAction(action3);
    ui->menuBar->addAction(level_menu->menuAction());

    enmoji.push_back(QIcon(":/resources/Resources/imgs/emoji_0.png"));
    enmoji.push_back(QIcon(":/resources/Resources/imgs/emoji_1.png"));
    enmoji.push_back(QIcon(":/resources/Resources/imgs/emoji_2.png"));

    ui->emojiButton->setStyleSheet("QPushButton#emojiButton { border: none; }");
    ui->emojiButton->setIconSize(ui->emojiButton->size());
    setEnmoji(1);

    ui->time->setDigitCount(5);
    ui->time->setMode(QLCDNumber::Dec);
    ui->time->setSegmentStyle(QLCDNumber::Flat);

    base_time = QTime::currentTime();

    ui->time->display("00:00");
    QObject::connect(&timer, &QTimer::timeout, [this]() {
        auto curr_time = QTime::currentTime();
        int t = base_time.msecsTo(curr_time);
        QTime show_time(0, 0, 0, 0);
        show_time = show_time.addMSecs(t);
        ui->time->display(show_time.toString("mm:ss"));
        });

    timer.setInterval(1000);

    QObject::connect(ui->stage, &GamePanel::start, [this](int max_mine) {
        ui->boomsNumber->display(QString::number(max_mine));
        base_time = QTime::currentTime();
        timer.start();
        setEnmoji(1);
        });

    QObject::connect(ui->stage, &GamePanel::moveChanged, [this](int move) {
        ui->moveNumber->display(QString::number(move));
        });

    QObject::connect(ui->stage, &GamePanel::mineCountChanged, [this](int mine_count) {
        ui->boomsNumber->display(QString::number(mine_count));
        if (mine_count <= ui->stage->getMaxMine() / 4) {
            setEnmoji(2);
        }
        else if (mine_count <= ui->stage->getMaxMine() / 3) {
            setEnmoji(1);
        }
        });

    QObject::connect(ui->stage, &GamePanel::stop, [this]() {
        ui->moveNumber->display(QString::number(0));
        timer.stop();
        });

    QObject::connect(ui->stage, &GamePanel::gameOver, [this]() {
        setEnmoji(0);
        //QMessageBox::information(this, "QMinesweeper", tr("Game Over"), QMessageBox::Yes);
        ui->stage->showAll();
        timer.stop();
        //ui->stage->restart();
        });

    QObject::connect(ui->stage, &GamePanel::win, [this]() {
        setEnmoji(2);
        timer.stop();
        QMessageBox::information(this, "QMinesweeper", tr("You Win!"), QMessageBox::Yes);
        });

    QObject::connect(ui->emojiButton, &QPushButton::pressed, [this]() {
        ui->stage->restart();
    });

    ui->stage->init(8, 8, 10, true);

    setStyleSheet("#centralWidget { background-color: white; }");
}



void MainWindow::setEnmoji(int i) {
    switch (i) {
    case 0:
        ui->emojiButton->setIcon(enmoji[0]);
        break;
    case 1:
        ui->emojiButton->setIcon(enmoji[1]);
        break;
    case 2:
        ui->emojiButton->setIcon(enmoji[2]);
        break;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
