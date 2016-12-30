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
    QObject::connect(ui->actionAutoCount,SIGNAL(triggered()),this,SLOT(autoCount()));
    QObject::connect(ui->actionManualCount,SIGNAL(triggered()),this,SLOT(manualCount()));
    QObject::connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(about()));
    QObject::connect(ui->actionGray,SIGNAL(triggered()),this,SLOT(rgbToGray()));
    QObject::connect(ui->actionSharpen,SIGNAL(triggered()),this,SLOT(sharpen()));
    QObject::connect(ui->actionBlur,SIGNAL(triggered()),this,SLOT(blur()));
    QObject::connect(ui->actionManual,SIGNAL(triggered()),this,SLOT(manualBinarization()));
    QObject::connect(ui->actionOtsu,SIGNAL(triggered()),this,SLOT(otsuBinarization()));
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
                this,"打开图片",".",
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
                    this,"保存处理结果",".",
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

//QImageToMat
cv::Mat MainWindow:: QImageToMat(QImage& image)
{
    cv::Mat mat;
    //cv::cvtColor(mat, mat, CV_BGR2RGB);
        //qDebug() << image.format();
        switch(image.format())
        {
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4, (uchar*)image.bits(), image.bytesPerLine());
            break;
        case QImage::Format_RGB888:
            mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
            cv::cvtColor(mat, mat, CV_BGR2RGB);
            break;
        case QImage::Format_Indexed8:
            mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
            break;
        }
    cv::Mat dst =mat;
    cvtColor(mat,dst,CV_BGR2GRAY);
    return dst;
}

//MatToQImage
QImage MainWindow::MatToQImage(cv::Mat& InputMat)
{
    cv::Mat TmpMat;
    // convert the color space to RGB
    if (InputMat.channels() == 1)
    {
        cv::cvtColor(InputMat, TmpMat, CV_GRAY2RGB);
    }else
    {
        cv::cvtColor(InputMat, TmpMat, CV_BGR2RGB);
    }
    // construct the QImage using the data of the mat, while do not copy the data
    QImage Result = QImage((const uchar*)(TmpMat.data),
                           TmpMat.cols, TmpMat.rows,
                           TmpMat.cols*TmpMat.channels(),//非常重要，不然结果会扭曲
                           QImage::Format_RGB888);
    // deep copy the data from mat to QImage
    Result.bits();
    return Result;
}

//auto阈值计数
void MainWindow::autoCount()
{
    if(!this->image->isNull())
    {
        statusBar()->showMessage(tr("处理中..."));
        //todo：count
        cv::Mat resultImage = QImageToMat(*image);
        cv::Mat imageShold;
        threshold(resultImage, imageShold, chooseThresholdbyOtsu(), 255, CV_THRESH_BINARY);   // 必须进行二值化
        vector<vector<cv::Point>> contours;
        //CV_CHAIN_APPROX_NONE  获取每个轮廓每个像素点
        findContours(imageShold, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));
        getSizeContours(contours);


        drawContours(imageShold,contours,-1,cv::Scalar(125),2);//画出边缘

        //end count
        statusBar()->showMessage(tr("处理完成"));
        QString string =tr("计数结果：");
        string.append(QString::number(contours.size()));
        resultDlg.editLabel(string);
        resultDlg.showResultImage(&MatToQImage(imageShold));
        resultDlg.show();
    }else
    {
        QMessageBox::warning(this,tr("waring"),tr("未打开图片"),
                             QMessageBox::Yes, QMessageBox::Yes);
    }
}

//manual阈值计数
void MainWindow::manualCount()
{
    if(!this->image->isNull())
    {
        statusBar()->showMessage(tr("处理中..."));
        //todo：count
        cv::Mat resultImage = QImageToMat(*image);
        cv::Mat imageShold;
        threshold(resultImage, imageShold, ui->lcdThreshold->value(), 255, CV_THRESH_BINARY);   // 必须进行二值化

        medianBlur(imageShold,imageShold,3);//中值滤波

        vector<vector<cv::Point>> contours;
        //CV_CHAIN_APPROX_NONE  获取每个轮廓每个像素点
        findContours(imageShold, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, cvPoint(0, 0));
        getSizeContours(contours);

        drawContours(imageShold,contours,-1,cv::Scalar(125),2);//画出边缘

        //end count
        statusBar()->showMessage(tr("处理完成"));
        QString string =tr("计数结果：");
        string.append(QString::number(contours.size()));
        resultDlg.editLabel(string);
        resultDlg.showResultImage(&MatToQImage(imageShold));
        resultDlg.show();
    }else
    {
        QMessageBox::warning(this,tr("waring"),tr("未打开图片"),
                             QMessageBox::Yes, QMessageBox::Yes);
    }
}

// 移除过小或过大的轮廓
void MainWindow::getSizeContours(vector<vector<cv::Point>> &contours)
{
    int cmin = 10;   // 最小轮廓长度
    int cmax = 1000;   // 最大轮廓长度
    vector<vector<cv::Point>>::const_iterator itc = contours.begin();
    while (itc != contours.end())
    {
        if ((itc->size()) < cmin
            || (itc->size()) > cmax
            )
        {
            itc = contours.erase(itc);
        }
        else ++itc;
    }
}

//manual二值化
void MainWindow::manualBinarization()
{
    if(!this->image->isNull())
    {
        statusBar()->showMessage(tr("处理中..."));
        int width,height;
        width=image->width();
        height=image->height();
        resultImage=QImage(width,height,QImage::Format_ARGB32);

        for(int i=0; i<width; i++){
            for(int j=0;j<height; j++){
                QRgb pixel = image->pixel(i,j);
                int gray = (qGray(pixel)> ui->lcdThreshold->value()) ? 255 : 0;
                //int gray = (qGray(pixel)> chooseThreshold()) ? 255 : 0;
                QRgb grayPixel = qRgb(gray,gray,gray);
                resultImage.setPixel(i,j,grayPixel);
            }
        }
        statusBar()->showMessage(tr("处理完成"));
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

//otsu二值化
void MainWindow::otsuBinarization()
{
    if(!this->image->isNull())
    {
        statusBar()->showMessage(tr("处理中..."));
        int width,height;
        width=image->width();
        height=image->height();
        resultImage=QImage(width,height,QImage::Format_ARGB32);
        int threshold=0;
        threshold=chooseThresholdbyOtsu();
        for(int i=0; i<width; i++){
            for(int j=0;j<height; j++){
                QRgb pixel = image->pixel(i,j);
                //int gray = (qGray(pixel)> ui->lcdThreshold->value()) ? 255 : 0;
                int gray = (qGray(pixel)>threshold ) ? 255 : 0;
                QRgb grayPixel = qRgb(gray,gray,gray);
                resultImage.setPixel(i,j,grayPixel);
                //BinaryArray[i,j]=gray;
            }
        }
        statusBar()->showMessage(tr("处理完成"));

        QString string = tr("自适应二值化图像,阈值为：");
        string.append(QString::number(threshold));
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
    if(image->isGrayscale())
    {
        QMessageBox::warning(this,tr("waring"),tr("已经是灰度图像"),
                             QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    if(!this->image->isNull())
    {
        statusBar()->showMessage(tr("处理中..."));
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
        statusBar()->showMessage(tr("处理完成"));
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
    if(!this->image->isNull())
    {
        statusBar()->showMessage(tr("处理中..."));
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

//显示直方图
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

    QImage* hist = new QImage(ui->hist->geometry().width(),ui->hist->geometry().height(),QImage::Format_RGB888);
    QPainter p(hist);
    hist->fill(qRgb(255,255,255));
    p.translate(0,hist->height());

    //p.drawLine(0,0,100,100);

    int wid=hist->width();
    int hei=hist->height();
    //ui->hist->geometry().width()
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

int MainWindow::chooseThresholdbyOtsu()
{
    double omega[256];//到第k个灰度值的总像素数
    double mu[256];//到第k个灰度值的总灰度和

    omega[0] = (double)histogram[0];
    mu[0] = 0;
    for(int i = 1; i < 256; i++)
    {
        omega[i] = omega[i-1] +(double) histogram[i]; //累积分布函数
        mu[i] = mu[i-1] + i *(double) histogram[i];
    }
    double mean = mu[255]/omega[255];// 灰度平均值
    double max = 0;
    int k_max = 0;
    for(int k = 1; k < 255; k++)
    {
        double PA = omega[k]/omega[255]; // A类所占的比例
        double PB = 1 - omega[k]/omega[255]; //B类所占的比例
        double value = 0;
        if( fabs(PA) > 0.001 && fabs(PB) > 0.001)
        {
            double MA = mu[k] / omega[k]; //A 类的灰度均值
            double MB = (mu[255] - mu[k]) /(omega[255]-omega[k]);//B类灰度均值
            value = PA * (MA - mean) * (MA - mean) + PB * (MB - mean) * (MB - mean);//类间方差

            if (value > max)
            {
                max = value;
                k_max = k;
            }
        }
        //qDebug() <<k << " " << histogram[k] << " " << value<<k_max;
    }
    ui->lcdThreshold->display(k_max);
    ui->hSliderThreshold->setValue(k_max);
    return k_max;
}

//锐化
void MainWindow::sharpen(){
    if(!this->image->isNull())
    {
        statusBar()->showMessage(tr("处理中..."));
        QImage * newImage = new QImage(* image);

        int kernel [3][3]= {{0,-1,0},
                            {-1,5,-1},
                            {0,-1,0}};
        int kernelSize = 3;
        int sumKernel = 1;
        int r,g,b;
        QColor color;

        for(int x=kernelSize/2; x<newImage->width()-(kernelSize/2); x++){
            for(int y=kernelSize/2; y<newImage->height()-(kernelSize/2); y++){

                r = 0;
                g = 0;
                b = 0;

                for(int i = -kernelSize/2; i<= kernelSize/2; i++){
                    for(int j = -kernelSize/2; j<= kernelSize/2; j++){
                        color = QColor(image->pixel(x+i, y+j));
                        r += color.red()*kernel[kernelSize/2+i][kernelSize/2+j];
                        g += color.green()*kernel[kernelSize/2+i][kernelSize/2+j];
                        b += color.blue()*kernel[kernelSize/2+i][kernelSize/2+j];
                    }
                }

                r = qBound(0, r/sumKernel, 255);
                g = qBound(0, g/sumKernel, 255);
                b = qBound(0, b/sumKernel, 255);

                newImage->setPixel(x,y, qRgb(r,g,b));
            }
        }
        statusBar()->showMessage(tr("处理完成"));
        QString string = tr("锐化图像");
        resultDlg.editLabel(string);
        resultDlg.showResultImage(newImage);
        resultDlg.show();}else
    {
        QMessageBox::warning(this,tr("waring"),tr("未打开图片"),
                             QMessageBox::Yes, QMessageBox::Yes);
    }
}

//模糊
void MainWindow::blur(){
    if(!this->image->isNull())
    {
        statusBar()->showMessage(tr("处理中..."));
        QImage * newImage = new QImage(* image);

        int kernel [5][5]= {{0,0,1,0,0},
                            {0,1,3,1,0},
                            {1,3,7,3,1},
                            {0,1,3,1,0},
                            {0,0,1,0,0}};
        int kernelSize = 5;
        int sumKernel = 27;
        int r,g,b;
        QColor color;

        for(int x=kernelSize/2; x<newImage->width()-(kernelSize/2); x++){
            for(int y=kernelSize/2; y<newImage->height()-(kernelSize/2); y++){

                r = 0;
                g = 0;
                b = 0;

                for(int i = -kernelSize/2; i<= kernelSize/2; i++){
                    for(int j = -kernelSize/2; j<= kernelSize/2; j++){
                        color = QColor(image->pixel(x+i, y+j));
                        r += color.red()*kernel[kernelSize/2+i][kernelSize/2+j];
                        g += color.green()*kernel[kernelSize/2+i][kernelSize/2+j];
                        b += color.blue()*kernel[kernelSize/2+i][kernelSize/2+j];
                    }
                }

                r = qBound(0, r/sumKernel, 255);
                g = qBound(0, g/sumKernel, 255);
                b = qBound(0, b/sumKernel, 255);

                newImage->setPixel(x,y, qRgb(r,g,b));

            }
        }
        statusBar()->showMessage(tr("处理完成"));
        QString string = tr("模糊图像");
        resultDlg.editLabel(string);
        resultDlg.showResultImage(newImage);
        resultDlg.show();}else
    {
        QMessageBox::warning(this,tr("waring"),tr("未打开图片"),
                             QMessageBox::Yes, QMessageBox::Yes);
    }
}


//高斯滤波
        //src:输入图像
        //dst:输出图像
        //Size(5,5)模板大小，为奇数
        //x方向方差
        //Y方向方差
//        GaussianBlur(src,dst,Size(5,5),0,0);
//        imwrite("gauss.jpg",dst);

        //中值滤波
        //src:输入图像
        //dst::输出图像
        //模板宽度，为奇数
//        medianBlur(src,dst,3);
//        imwrite("med.jpg",dst);

        //均值滤波
        //src:输入图像
        //dst:输出图像
        //模板大小
        //Point(-1,-1):被平滑点位置，为负值取核中心
//        blur(src,dst,Size(3,3),Point(-1,-1));
//        imwrite("mean.jpg",dst);

        //双边滤波
        //src:输入图像
        //dst:输入图像
        //滤波模板半径
        //颜色空间标准差
        //坐标空间标准差
//        bilateralFilter(src,dst,5,10.0,2.0);//这里滤波没什么效果，不明白
