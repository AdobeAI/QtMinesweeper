#pragma once

#include <QDialog>

namespace Ui {
    class EnterPlayerNameDialog;
}

class EnterPlayerNameDialog : public QDialog {
    Q_OBJECT
public:
    explicit EnterPlayerNameDialog(QWidget* parent = nullptr);

    ~EnterPlayerNameDialog();

private:
    std::string player_name;
    Ui::EnterPlayerNameDialog* ui;

signals:
    void getPlayerName(std::string player_name);
};
