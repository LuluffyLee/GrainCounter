#pragma execution_character_set("utf-8")
#include "logindlg.h"
#include "ui_logindlg.h"
#include <QMessageBox>

LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDlg)
{
    ui->setupUi(this);
    //ui->pwdLineEdit->setPlaceholderText(tr("请输入密码"));
    //ui->userLineEdit->setPlaceholderText(tr("请输入用户名"));
}

LoginDlg::~LoginDlg()
{
    delete ui;
}

void LoginDlg::on_loginPushBtn_clicked()
{
    if(ui->userLineEdit->text().trimmed() == tr("liyijian") &&
              ui->pwdLineEdit->text() == tr("123456"))
       {
          accept();
       } else {
        QMessageBox::warning(this, tr("Waring"),
                                     tr("user name or password error!"),
                                     QMessageBox::Yes);
        ui->userLineEdit->clear();
        ui->pwdLineEdit->clear();
        ui->userLineEdit->setFocus();
       }
}
