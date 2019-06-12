#ifndef CELL_H
#define CELL_H

#include <QFrame>

enum CellStatus{
    STATUS_BANK = 0,	//挖开的空白方块
    STATUS_NUM1,
    STATUS_NUM2,
    STATUS_NUM3,
    STATUS_NUM4,
    STATUS_NUM5,
    STATUS_NUM6,
    STATUS_NUM7,
    STATUS_NUM8,

    STATUS_INIT,		//初始化的状态
    STATUS_FLAG,		//标记
    STATUS_MINE,        //雷
};

class Cell : public QFrame {
    Q_OBJECT
public:
    explicit Cell(size_t x, size_t y, QWidget *parent = nullptr);

    CellStatus getStatus() const;

    void setStatus(CellStatus _status);

    bool isMine() const;

    bool isDowned() const;

    bool isFlag() const;

    void setMine(bool _isMine);

    void setNearMineCount(CellStatus _near_mine_count);

    void setDowned(bool downed);

    CellStatus getNearMineCount() const;

    size_t getX() const;

    size_t getY() const;

    void reset();

    void setDebugMode(bool);


private:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;

private:
    size_t x, y;
    CellStatus status, near_mine_count;
    bool is_Mine, downed, debug_mode;
    QPixmap mine_img, flag_img;

signals:
    void dug(size_t x, size_t y);

    void setFlag();
};

#endif // CELL_H
