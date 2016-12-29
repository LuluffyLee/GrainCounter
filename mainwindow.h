#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "processresultdlg.h"
#include "math.h"
#include <QMainWindow>
#include <QImage>
//#include <QDebug>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;

namespace Ui {
class MainWindow;
class QMenu;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    //MainWindow();
    ProcessResultDlg resultDlg;
    void showImage(const QString &fileName);
    ~MainWindow();

private slots:
    void open();
    void save();
    void autoCount();
    void manualCount();
    void about();
    void exit();
    void sharpen();
    void blur();
    void rgbToGray();
    void manualBinarization();
    void otsuBinarization();
    void histogramEqualization();

private:
    Ui::MainWindow *ui;
    QImage *image;
    QImage resultImage;
    QApplication* app;
    void showHistogram();
    vector<int> histogram;
    vector<int> Histogram(QImage *);
    int chooseThresholdbyOtsu();
    void getSizeContours(vector<vector<cv::Point>> &);
    cv::Mat QImageToMat(QImage &);
    QImage MatToQImage(cv::Mat &);
};

#endif // MAINWINDOW_H
