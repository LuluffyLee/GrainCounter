#include "processresultdlg.h"
#include "ui_processresultdlg.h"
#include <QGraphicsScene>
#include <QPixmap>

ProcessResultDlg::ProcessResultDlg(QWidget *parent):
    QDialog(parent),
    ui(new Ui::ProcessResultDlg)
{
    ui->setupUi(this);
    this->resultImage = new QImage();
}

ProcessResultDlg::~ProcessResultDlg()
{
    delete ui;
    if(this->resultImage != NULL)
    {
        delete resultImage;
    }
}

void ProcessResultDlg::showResultImage(QImage* resultImage)
{
    QGraphicsScene *scene = new QGraphicsScene;
    scene->addPixmap(QPixmap::fromImage(*resultImage));
    ui->resultGraphicsView->setScene(scene);
    ui->resultGraphicsView->resize(resultImage->width() + 10, resultImage->height() + 10);
    resize(resultImage->width() + 30,resultImage->height() + 50);
    ui->resultGraphicsView->show();
    setWindowTitle(tr("处理结果"));
}

void ProcessResultDlg::editLabel(const QString string)
{
    ui->label->setText(string); //写内容
}
