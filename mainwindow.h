#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "processresultdlg.h"
#include <QMainWindow>
#include <QImage>
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
    void count();
    void about();
    void exit();
    void Erode();
    void rgbToGray();
    void binarization();
    void histogramEqualization();

private:
    Ui::MainWindow *ui;
    QImage *image;
    QImage resultImage;
    QApplication* app;
    void showHistogram();
    vector<int> histogram;
    vector<int> Histogram(QImage *);
    int chooseThreshold();
    cv::Mat imgToMat(QImage *);
};

#endif // MAINWINDOW_H
