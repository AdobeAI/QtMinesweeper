#include <QMessageBox>
#include <QShortcut>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "game_record.h"
#include "dialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , debug_mode(false)
    , player_name("No Name Player") {

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    ui->setupUi(this);

    setWindowTitle("MineSweeper");

    ui->player_name->setText(QString::fromStdString(player_name));

    auto cheat_mode_enable = new QShortcut(this);
    cheat_mode_enable->setKey(Qt::CTRL + Qt::Key_G);
    QObject::connect(cheat_mode_enable, reinterpret_cast<void (QShortcut::*)(bool)>(&QShortcut::activated), [this](bool) {
        debug_mode = !debug_mode;
        ui->stage->setDebugMode(debug_mode);
        });
    EnterPlayerNameDialog dialog(this);
    QObject::connect(&dialog, &EnterPlayerNameDialog::getPlayerName, [this](std::string _player_name) {
        player_name = _player_name;
        ui->player_name->setText(QString::fromStdString("Hello " + player_name + " enjoy you game!"));
    });


    auto action1 = new QAction(tr("Beginer"), this);
    QObject::connect(action1, &QAction::triggered, [this]() {
        ui->stage->init(8, 8, 10, true);
        ui->boomsNumber->display(QString::number(10));
        play_level = GAME_LEVEL::BEGINER;
        updateRanking(play_level);
        });

    auto action2 = new QAction(tr("Intermediate"), this);
    QObject::connect(action2, &QAction::triggered, [this]() {
        ui->stage->init(16, 16, 40, true);
        ui->boomsNumber->display(QString::number(40));
        play_level = GAME_LEVEL::INTERMEDIATE;
        updateRanking(play_level);
        });

    auto action3 = new QAction(tr("Export"), this);
    QObject::connect(action3, &QAction::triggered, [this]() {
        ui->stage->init(30, 24, 99, true);
        ui->boomsNumber->display(QString::number(99));
        play_level = GAME_LEVEL::EXPORT;
        updateRanking(play_level);
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

    ui->time->setDigitCount(10);
    ui->time->setMode(QLCDNumber::Dec);
    ui->time->setSegmentStyle(QLCDNumber::Flat);

    base_time = QTime::currentTime();

    ui->time->display("00:00.000");
    QObject::connect(&timer, &QTimer::timeout, [this]() {
        auto curr_time = QTime::currentTime();
        int t = base_time.msecsTo(curr_time);
        QTime show_time(0, 0, 0, 0);
        show_time = show_time.addMSecs(t);
        ui->time->display(show_time.toString("mm:ss.zzz"));
        });

    timer.setTimerType(Qt::PreciseTimer);
    timer.setInterval(10);

    QObject::connect(ui->stage, &GamePanel::start, [this]() {
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
        ui->stage->showAll();
        timer.stop();
        });

    QObject::connect(ui->stage, &GamePanel::win, [this]() {
        setEnmoji(2);
        timer.stop();

        auto curr_time = QTime::currentTime();
        int t = base_time.msecsTo(curr_time);
        QTime show_time(0, 0, 0, 0);
        show_time = show_time.addMSecs(t);
        int second = (show_time.hour()*60*60) + (show_time.minute()*60) + show_time.second();

        InsertRecord(player_name, second, play_level);
        QMessageBox::information(this, "QMinesweeper", tr("You Win!"), QMessageBox::Yes);
        updateRanking(play_level);
        });

    QObject::connect(ui->emojiButton, &QPushButton::pressed, [this]() {
        ui->stage->restart();
    });

    ui->ranking->setShowGrid(true);
    ui->ranking->verticalHeader()->setVisible(false);
    ui->ranking->horizontalHeader()->setStretchLastSection(true);
    //ui->ranking->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->ranking->verticalHeader()->setStretchLastSection(false);

    ui->stage->init(8, 8, 10, true);
    ui->boomsNumber->display(QString::number(10));
    play_level = GAME_LEVEL::BEGINER;
    updateRanking(play_level);

    setStyleSheet("#centralWidget { background-color: white; }");

    dialog.exec();
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

void MainWindow::InsertRecord(std::string _player_name, int _time, int _level) {
    if(_player_name == "No Name Player") {
        return;
    }
    Score s;
    Game_record record;
    if(!record.connectMySQL()) {
        std::cout << "connect faile." << std::endl;
    }
    record.addGameRecord(_player_name, _time, _level);
    record.closeConnect();

}
void MainWindow::updateRanking(int level) {
    while (ui->ranking->rowCount() > 0) {
        ui->ranking->removeRow(0);
    }
    Game_record record;
    if(!record.connectMySQL()) {
        std::cout << "connect faile." << std::endl;
    }
    std::string game_level;
    switch (level) {
        case 1:
            game_level = "Beginer";
            break;
        case 2:
            game_level = "Intermediate";
            break;
        default:
            game_level = "Export";
            break;
    }
    std::map<int, Score> records = record.getGameRecord(level);
    for(auto data : records) {
        auto item_index = ui->ranking->rowCount();

        ui->ranking->insertRow(item_index);
        ui->ranking->setItem(item_index, RANKING::PLAYER, new QTableWidgetItem(QString::fromStdString(data.second.getPlayerName())));
        ui->ranking->setItem(item_index, RANKING::USE_TIME, new QTableWidgetItem(QString::number(data.second.getTime())));
        ui->ranking->setItem(item_index, RANKING::_LEVEL, new QTableWidgetItem(QString::fromStdString(game_level)));
        ui->ranking->item(item_index, RANKING::PLAYER)->setTextAlignment(Qt::AlignCenter);
        ui->ranking->item(item_index, RANKING::USE_TIME)->setTextAlignment(Qt::AlignCenter);
        ui->ranking->item(item_index, RANKING::_LEVEL)->setTextAlignment(Qt::AlignCenter);
    }
    ui->ranking->resizeColumnsToContents();
    ui->ranking->resizeRowsToContents();
    ui->ranking->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    record.closeConnect();

}

MainWindow::~MainWindow()
{
    delete ui;
}
