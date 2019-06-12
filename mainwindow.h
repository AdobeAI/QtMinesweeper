#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QTableWidget>
#include <QString>

namespace Ui {
class MainWindow;
}

enum RANKING {
    PLAYER,
    USE_TIME,
    _LEVEL
};

enum GAME_LEVEL {
    BEGINER = 1,
    INTERMEDIATE,
    EXPORT
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    void setEnmoji(int i);
    void updateRanking(int level);

    void InsertRecord(std::string _player_name, int _time, int _level);

    std::string player_name;

    QTimer timer;
    QTime base_time;
    std::vector<QIcon> enmoji;
    bool debug_mode;
    int play_level;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
