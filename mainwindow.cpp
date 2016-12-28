#pragma execution_character_set("utf-8")
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindlg.h"
#include "processresultdlg.h"
#include <QtWidgets>
#include <QObject>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    this->image = new QImage();
    statusBar()->showMessage(tr("就绪"));

    QObject::connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(open()));
    QObject::connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(exit()));
    QObject::connect(ui->actionCount,SIGNAL(triggered()),this,SLOT(count()));
    QObject::connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(about()));

}

MainWindow::~MainWindow()
{
    if(this->image != NULL)
    {
        delete this->image;
    }
    delete ui;
}

void MainWindow::count()
{
    if(!this->image->isNull())
    {
        resultDlg.showResultImage(image);
        resultDlg.show();
    }else
    {
        QMessageBox::warning(this,tr("waring"),tr("未打开图片"),QMessageBox::Yes, QMessageBox::Yes);
    }
}


void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(
                this,"open image file",".",
                "Image files (*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;All files (*.*)");
    if (!fileName.isEmpty())
        showImage(fileName);
}

void MainWindow::exit()
{
    if (!(QMessageBox::information(this,tr("退出"),tr("确定退出吗？"),tr("Yes"),tr("No"))))
    {
        app->exit(0);
    }
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("关于Grain Counter"),
            tr("<b>Grain Counter 1.0.0</b> "
               "<br/>Copyright© 2016 李艺健 "
               "<br/>该程序用于大豆等谷物的计数"));
}

void MainWindow::showImage(const QString &fileName)
{
    //显示图片
    if(image->load(fileName))
    {
        statusBar()->showMessage(tr("加载中..."));

        QGraphicsScene *scene = new QGraphicsScene;
        scene->addPixmap(QPixmap::fromImage(*image));
        ui->graphicsView->setScene(scene);
        ui->graphicsView->resize(image->width() + 10, image->height() + 10);
        resize(image->width() + 30,image->height() + 100);
        ui->graphicsView->show();
        //showImage(image);

        statusBar()->showMessage(tr("加载完成"), 2000);
    }else
    {
        statusBar()->showMessage(tr("加载失败"), 2000);
    }
}
