#include <QPainter>
#include <QMouseEvent>
#include "cell.h"

Cell::Cell(size_t _x, size_t _y, QWidget *parent)
    : QFrame(parent)
    , x(_x)
    , y(_y)
    , status(STATUS_INIT)
    , near_mine_count(STATUS_BANK)
    , is_Mine(false)
    , downed(false) {

    setFrameStyle(QFrame::NoFrame);
    setObjectName("cell");
}

void Cell::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); //反鋸齒

    auto rc = rect();

    if (downed) {
        painter.fillRect(0, 0, width() - 1, height() - 1, Qt::lightGray);
        if (status >= STATUS_NUM1 && status <= STATUS_NUM8) {
            static QString path_table[] = {
                "",
                ":/resources/Resources/imgs/num1.png",
                ":/resources/Resources/imgs/num2.png",
                ":/resources/Resources/imgs/num3.png",
                ":/resources/Resources/imgs/num4.png",
                ":/resources/Resources/imgs/num5.png",
                ":/resources/Resources/imgs/num6.png",
                ":/resources/Resources/imgs/num7.png",
                ":/resources/Resources/imgs/num8.png",
            };
            painter.drawPixmap(rc, QPixmap(path_table[status]));
        }
    }

    if (status == STATUS_FLAG) {
        if (flag_img.isNull()) {
            flag_img = QPixmap(":/resources/Resources/imgs/flag.png");
        }
        painter.drawPixmap(rc, flag_img);
    }

    /*painter.setPen(QPen(QBrush(Qt::gray), 1));
    painter.drawRoundRect(0, 0, width() - 1, height() - 1, 2, 2);*/

    if (status == STATUS_MINE) {
        painter.drawPixmap(rc, mine_img);
    }
    else if (status == STATUS_BANK) {
        painter.drawPixmap(rc, QPixmap(":/resources/Resources/imgs/bank.png"));
    }
    else if (status == STATUS_INIT) {
        painter.drawPixmap(rc, QPixmap(":/resources/Resources/imgs/init.png"));
    }
}

void Cell::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit dug(x, y);
    }
    else if (event->button() == Qt::RightButton) {
        if (status == STATUS_INIT) {
            setStatus(STATUS_FLAG);
            emit setFlag();
        }
        else if (status == STATUS_FLAG) {
            setStatus(STATUS_INIT);
        }
    }
}
void Cell::reset() {
    status = STATUS_INIT;
    near_mine_count = STATUS_BANK;
    is_Mine = false;
    downed = false;
}

CellStatus Cell::getStatus() const {
    return status;
}

void Cell::setStatus(CellStatus _status) {
    status = _status;
    update();
}

bool Cell::isMine() const {
    return is_Mine;
}

void Cell::setMine(bool _isMine) {
    is_Mine = _isMine;
    if (is_Mine) {
        mine_img = QPixmap(":/resources/Resources/imgs/mine.png");
    }
    else {
        mine_img = QPixmap();
    }
}

void Cell::setNearMineCount(CellStatus _near_mine_count) {
    near_mine_count = _near_mine_count;
}

void Cell::setDowned(bool _downed) {
    if (status == STATUS_FLAG || isDowned()) {
        return;
    }
    downed = _downed;
    if (is_Mine) {
        setStatus(STATUS_MINE);
    }
}

bool Cell::isDowned() const {
    return downed;
}

CellStatus Cell::getNearMineCount() const {
    return near_mine_count;
}

size_t Cell::getX() const {
    return x;
}

size_t Cell::getY() const {
    return y;
}

bool Cell::isFlag() const {
    return status == STATUS_FLAG;
}
