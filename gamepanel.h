#ifndef GAMEPANEL_H
#define GAMEPANEL_H

#include <QFrame>
#include <QGridLayout>
#include <memory>
#include <QWidget>

#include "cell.h"

class GamePanel : public QFrame {
    Q_OBJECT
public:
    explicit GamePanel(QWidget *parent = nullptr);

    void init(size_t _width, size_t _height, int _max_mine, bool change_layout);

    void restart();

    int getMaxMine() const {
        return max_mine;
    }

    void showAll();

signals:
    void start(int max_mine);

    void mineCountChanged(int count);

    void moveChanged(int move);

    void stop();

    void gameOver();

    void win();

private:
    size_t game_width, game_height;
    int max_mine, remain_mine, move, normal_cells;
    bool first_dug;
    std::unique_ptr<QGridLayout> layout;
    std::vector<std::vector<std::unique_ptr<Cell>>> cells;

    void randomMine();

    void calcNearMineCount();

    void dug(size_t x, size_t y);

    std::vector<const Cell*> getNearMine(size_t x, size_t y) const;

    void resizeEvent(QResizeEvent*) override;
};

#endif // GAMEPANEL_H
