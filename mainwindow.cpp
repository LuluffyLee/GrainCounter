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

    ui->dockWidget->setFeatures(QDockWidget::DockWidgetMovable);
    ui->dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    ui->dockWidget->setMinimumWidth(200);
    ui->dockWidget->showMinimized();

    QObject::connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(open()));
    QObject::connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(save()));
    QObject::connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(exit()));
    QObject::connect(ui->actionCount,SIGNAL(triggered()),this,SLOT(count()));    
    QObject::connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(about()));
    QObject::connect(ui->actionGray,SIGNAL(triggered()),this,SLOT(rgbToGray()));
    QObject::connect(ui->actionBinarization,SIGNAL(triggered()),this,SLOT(binarization()));
    QObject::connect(ui->actionHistogramEqualization,SIGNAL(triggered()),this,SLOT(histogramEqualization()));
}

MainWindow::~MainWindow()
{
    if(this->image != NULL)
    {
        delete this->image;
    }
    delete ui;
}


void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(
                this,"open image file",".",
                "Image files (*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;All files (*.*)");
    if (!fileName.isEmpty())
    {
        showImage(fileName);
        showHistogram();
        ui->dockWidget->showMinimized();
    }
}

void MainWindow::save()
{
    if(!resultImage.isNull())
    {
        QString fileName = QFileDialog::getSaveFileName(
                    this,"open image file",".",
                    "BMP file (*.bmp);;JPEG file (*.jpg);;PNG file (*.png);;All files (*.*)");
        if (!fileName.isEmpty())
        {
            statusBar()->showMessage(tr("保存中..."));
            resultImage.save(fileName);
            statusBar()->showMessage(tr("保存成功"));
        }
    }else
    {
        QMessageBox::warning(this,tr("waring"),tr("无处理结果"),
                             QMessageBox::Yes, QMessageBox::Yes);
    }

}

//计数
void MainWindow::count()
{
    if(!this->image->isNull())
    {
        QString string = tr("计数结果");
        resultDlg.editLabel(string);
        resultDlg.showResultImage(image);
        resultDlg.show();
    }else
    {
        QMessageBox::warning(this,tr("waring"),tr("未打开图片"),
                             QMessageBox::Yes, QMessageBox::Yes);
    }
}

//二值化
void MainWindow::binarization()
{
    if(!this->image->isNull())
    {
        int width,height;
        width=image->width();
        height=image->height();
        resultImage=QImage(width,height,QImage::Format_ARGB32);

        for(int i=0; i<width; i++){
            for(int j=0;j<height; j++){
                QRgb pixel = image->pixel(i,j);
                int gray = (qGray(pixel)> 100) ? 255 : 0;
                QRgb grayPixel = qRgb(gray,gray,gray);
                resultImage.setPixel(i,j,grayPixel);
            }
        }

        QString string = tr("二值化图像");
        resultDlg.editLabel(string);
        resultDlg.showResultImage(&resultImage);
        resultDlg.show();
    }else
    {
        QMessageBox::warning(this,tr("waring"),tr("未打开图片"),
                             QMessageBox::Yes, QMessageBox::Yes);
    }
    //BinaryArray[i,j] = Convert.ToByte((GrayArray[i,j] > threshold) ? 255 : 0);
}

//灰度图像
void MainWindow::rgbToGray()
{
    if(!this->image->isNull())
    {
        int width,height;
        width=image->width();
        height=image->height();
        resultImage=QImage(width,height,QImage::Format_ARGB32);

        for(int i=0; i<width; i++){
            for(int j=0;j<height; j++){
                QRgb pixel = image->pixel(i,j);
                int gray = qGray(pixel);
                QRgb grayPixel = qRgb(gray,gray,gray);
                resultImage.setPixel(i,j,grayPixel);
            }
        }

        QString string = tr("灰度图像");
        resultDlg.editLabel(string);
        resultDlg.showResultImage(&resultImage);
        resultDlg.show();
    }else
    {
        QMessageBox::warning(this,tr("waring"),tr("未打开图片"),
                             QMessageBox::Yes, QMessageBox::Yes);
    }
}

//直方图均衡化
void MainWindow::histogramEqualization()
{
    statusBar()->showMessage(tr("处理中..."));
    if(!this->image->isNull())
    {
        int i,j;
        int width,height;
        width=image->width();
        height=image->height();
        resultImage=QImage(width,height,QImage::Format_ARGB32);
        QRgb rgb;
        int r[256],g[256],b[256];//原图各个灰度数量的统计
        int rtmp,gtmp,btmp,rj,gj,bj;
        float rPro[256],gPro[256],bPro[256];//原图各个灰度级的概率
        float rTemp[256],gTemp[256],bTemp[256];//均衡化后各个灰度级的概率
        int rJun[256],gJun[256],bJun[256];//均衡化后对应像素的值
        memset(r,0,sizeof(r));
        memset(g,0,sizeof(g));
        memset(b,0,sizeof(b));

        //获取原图各个灰度的数量
        for(i=0;i<width;i++)
        {
            for(j=0;j<height;j++)
            {
                rgb=image->pixel(i,j);
                r[qRed(rgb)]++;
                g[qGreen(rgb)]++;
                b[qBlue(rgb)]++;
            }
        }

        //获取原图各个灰度级的概率
        for(i=0;i<256;i++)
        {
            rPro[i]=(r[i]*1.0)/(width*height);
            gPro[i]=(g[i]*1.0)/(width*height);
            bPro[i]=(b[i]*1.0)/(width*height);
        }

        //均衡化后各个灰度级的概率，同时获取均衡化后对应像素的值
        for(i=0;i<256;i++)
        {
            if(i==0)
            {
                rTemp[0]=rPro[0];
                gTemp[0]=gPro[0];
                bTemp[0]=bPro[0];
            }
            else
            {
                rTemp[i]=rTemp[i-1]+rPro[i];
                gTemp[i]=gTemp[i-1]+gPro[i];
                bTemp[i]=bTemp[i-1]+bPro[i];
            }
            rJun[i]=(int)(255*rTemp[i]+0.5);
            gJun[i]=(int)(255*gTemp[i]+0.5);
            bJun[i]=(int)(255*bTemp[i]+0.5);
        }

        for(i=0;i<width;i++)
        {
            for(j=0;j<height;j++)
            {
                rgb=image->pixel(i,j);
                rtmp=qRed(rgb);
                gtmp=qGreen(rgb);
                btmp=qBlue(rgb);
                rj=rJun[rtmp];
                gj=gJun[gtmp];
                bj=bJun[btmp];
                resultImage.setPixel(i,j,qRgb(rj,gj,bj));
            }
        }

        statusBar()->showMessage(tr("处理完成"));
        QString string = tr("直方图均衡化图像");
        resultDlg.editLabel(string);
        resultDlg.showResultImage(&resultImage);
        resultDlg.show();
    }else
    {
        QMessageBox::warning(this,tr("waring"),tr("未打开图片"),
                             QMessageBox::Yes, QMessageBox::Yes);
    }
}

void MainWindow::exit()
{
    if (!(QMessageBox::information(this,tr("退出"),
                                   tr("确定退出吗？"),tr("Yes"),tr("No"))))
    {
        app->exit(0);
    }
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("关于Grain Counter"),
            tr("<b>Grain Counter 1.0.0</b> "
               "<br/>Copyright© 2016 李艺健 "
               "<br/>该程序可用于简单的图像处理以及大豆等粮食作物的计数"));
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
        resize(image->width() + 235,image->height() + 100);
        ui->graphicsView->show();
        //showImage(image);

        statusBar()->showMessage(tr("加载完成"), 2000);
    }else
    {
        statusBar()->showMessage(tr("加载失败"), 2000);
    }
}

void MainWindow::showHistogram()
{
    if(image->isNull())
    {
        return;
    }
    vector<int> count = Histogram(image);


    histogram.resize(256);
    for (int i=0;i!=256;i++)
    {
        histogram[i] = count[i];
    }

    std::vector<int> sortcount = count;
    std::sort(sortcount.begin(),sortcount.end());
    int maxcount = sortcount[sortcount.size()-1];


    QImage* hist = new QImage(image->width(),image->height(),QImage::Format_RGB888);
    QPainter p(hist);
    hist->fill(qRgb(255,255,255));
    p.translate(0,hist->height());


    //p.drawLine(0,0,100,100);

    int wid=hist->width();
    int hei=hist->height();

    //p.drawLine(10,-10,wid-10,-10);//横轴
    //p.drawLine(10,-10,10,-hei+10);//纵轴

    float xstep = float(wid) / 256;
    float ystep = float(hei-20) / maxcount;

    for (int i=0;i!=256;i++)
    {

        QColor color(i,255-i,0);
        //p.setPen(Qt::blue);
        //p.setBrush(Qt::blue);
        p.setBrush(color);
        p.setPen(color);
        //p.drawLine(QPointF((i+0.5)*xstep,-ystep*count[i]),QPointF(10+(i+1.5)*xstep,-10-ystep*count[i+1]));
        p.drawRect(i*xstep,0,xstep,-ystep*count[i]);
    }
    ui->hist->setPixmap(QPixmap::fromImage(hist->scaled(ui->hist->width(),ui->hist->height(),Qt::KeepAspectRatio)));
}

vector<int> MainWindow::Histogram(QImage* image)
{
    vector<int> hist(256);
    QImage grayImg;
    int width,height;
    width=image->width();
    height=image->height();
    grayImg=QImage(width,height,QImage::Format_ARGB32);

    for(int i=0; i<width; i++){
        for(int j=0;j<height; j++){
            QRgb pixel = image->pixel(i,j);
            int gray = qGray(pixel);
            hist[gray] = hist[gray]+1;
        }
    }
    return hist;
}
