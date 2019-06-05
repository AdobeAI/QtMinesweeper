#include<QGridLayout>
#include <iostream>
#include "gamepanel.h"

GamePanel::GamePanel(QWidget *parent)
    : QFrame(parent)
    , game_width(0)
    , game_height(0)
    , max_mine(0)
    , remain_mine(0)
    , move(0)
    , first_dug(true){
    setStyleSheet("#stage { background-color: white; }");
}


void GamePanel::init(size_t _width, size_t _height, int _max_mine, bool change_layout) {
    emit stop();
    if(change_layout) {
        game_width = _width;
        game_height = _height;
        max_mine = _max_mine;

        layout.reset();
        layout.reset(new QGridLayout(this));
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);

        cells.clear();
        cells.resize(game_width);

        for(size_t x = 0 ; x < game_height ; x++) {
            std::vector<std::unique_ptr<Cell>> temp;
            for(size_t y = 0 ; y < game_width ; y++) {
                Cell* cell = new Cell(x, y, this);
                temp.emplace_back(cell);   //能就地通过参数构造对象，不需要拷贝或者移动内存，相比push_back能更好地避免内存的拷贝与移动，使容器插入元素的性能得到进一步提升
                QObject::connect(cell, &Cell::dug, this, &GamePanel::dug);
                //QObject::connect(cell, SIGNAL(dug(int, int)), this, SLOT(onDug(int, int)));
                QObject::connect(cell, &Cell::setFlag, [this]() {});

                layout->addWidget(cell, (int)x, (int)y);
            }
            cells[x] = std::move(temp);   // 搶對方指標納為已用，並設置對方指向null
        }
    }else {
        for(size_t x = 0 ; x < game_height ; x++) {
            for(size_t y = 0 ; y < game_width ; y++) {
                cells[x][y]->reset();
            }
        }
    }
    remain_mine = max_mine;
    move = 0;
    first_dug = true;
    randomMine();
    calcNearMineCount();

    emit start(max_mine);
    emit mineCountChanged(remain_mine);
}

void GamePanel::restart() {
    init(game_width, game_height, max_mine, false);
}

void GamePanel::calcNearMineCount() {
    for(size_t x = 0 ; x < game_height ; x++) {
        for(size_t y = 0 ; y < game_width ; y++) {
            int mine_count = 0;
            for (auto mine : getNearMine(x, y)) {
                if (mine->isMine()) {
                    ++mine_count;
                }
            }
            cells[x][y]->setNearMineCount(static_cast<CellStatus>(mine_count));
        }
    }
}

std::vector<const Cell*> GamePanel::getNearMine(size_t x, size_t y) const {
    std::vector<const Cell*> near_mines;

    if (x && y) {
        near_mines.push_back(cells[x - 1][y - 1].get());
    }

    if (x) {
        near_mines.push_back(cells[x - 1][y].get());
    }

    if (x && y < game_width - 1) {
        near_mines.push_back(cells[x - 1][y + 1].get());
    }

    if (y < game_width - 1) {
        near_mines.push_back(cells[x][y + 1].get());
    }

    if (x < game_height - 1 && y < game_width - 1) {
        near_mines.push_back(cells[x + 1][y + 1].get());
    }

    if (x < game_height - 1) {
        near_mines.push_back(cells[x +1][y].get());
    }

    if (x < game_height - 1 && y) {
        near_mines.push_back(cells[x + 1][y - 1].get());
    }

    if (y) {
        near_mines.push_back(cells[x][y - 1].get());
    }

    return near_mines;
}

void GamePanel::dug(size_t x, size_t y) {
    auto& cell = cells[x][y];
    if(cell->getStatus() == STATUS_FLAG) {
        return;
    }
    if(cell->isDowned()) {
        return;
    }
    if(cell->isMine()) {
        if(first_dug) {
            restart();
            dug(x, y);
        }else {
            cell->setDowned(true);
            showAll();
            emit gameOver();
            return;
        }
    }
    first_dug = false;

    auto is_mine = cell->getStatus() == STATUS_MINE;
    cell->setDowned(true);

    if(!is_mine && cell->getStatus() == STATUS_MINE) {
        --remain_mine;
        emit mineCountChanged(remain_mine);
    }

    if(cell->getStatus() != STATUS_INIT) {
        return;
    }
    cell->setStatus(cell->getNearMineCount());
    if(cell->getStatus() == STATUS_BANK) {
        for(auto mine : getNearMine(x, y)) {
            dug(mine->getX(), mine->getY());
        }
    }
    ++move;
    emit moveChanged(move);
    if(move == ((game_height * game_width) - max_mine)) {
        emit win();
    }
}

void GamePanel::showAll() {
    for (size_t x = 0; x < game_height; ++x) {
        for (size_t y = 0; y < game_width; ++y) {
            auto& mine = cells[x][y];
            if (mine->isMine()) {
                mine->setStatus(STATUS_MINE);
            } else {
                mine->setDowned(true);
                mine->setStatus(mine->getNearMineCount());
            }
        }
    }
}

void GamePanel::randomMine() {

    std::random_device rd;
    std::mt19937_64 gen = std::mt19937_64(rd());

//Fisher–Yates shuffle
    std::vector<bool> random_cell(game_height * game_width);
    for(size_t i = 0 ; i < random_cell.size() ; i++) {
        random_cell[i] = i < max_mine;
        std::cout << random_cell[i] <<std::endl;
    }
    for(size_t i = random_cell.size() - 1 ; i > 0 ; i--) {
        std::uniform_int_distribution<size_t> dis(0, i);
        size_t random_location = dis(gen);
        uint8_t temp = random_cell[i];
        random_cell[i] = random_cell[random_location];
        random_cell[random_location] = temp;
    }
    size_t i = 0;
    for(size_t x = 0 ; x < game_height ; x++) {
        for(size_t y = 0 ; y < game_width ; y++) {
            cells[x][y]->setMine(random_cell[i]);
            i++;
        }
    }
}

void GamePanel::resizeEvent(QResizeEvent*) {
    auto w = width() / game_width;
    auto h = height() / game_height;

    if (w > h) {
        setGeometry(x() + (width() - h * game_width) / 2, y(), h * game_width, height());
    }
    else {
        setGeometry(x(), y() + (height() - w * game_height) / 2, width(), w * game_height);
    }
}
