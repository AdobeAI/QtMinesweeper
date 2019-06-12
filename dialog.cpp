#include "dialog.h"
#include <QInputDialog>
#include "ui_playerui.h"

EnterPlayerNameDialog::EnterPlayerNameDialog(QWidget *parent)
    : QDialog (parent)
    , ui(new Ui::EnterPlayerNameDialog)
    , player_name ("No Name Player") {
    ui->setupUi(this);
    setWindowTitle("Login");

    QObject::connect(ui->buttonBox, &QDialogButtonBox::accepted, [this]() {
        player_name = ui->EnterNameEdit->text().toStdString();
        if(player_name == "") {
            player_name = "No Name Player";
        }
        emit getPlayerName(player_name);
    });
}

EnterPlayerNameDialog::~EnterPlayerNameDialog() {
    delete ui;
}
