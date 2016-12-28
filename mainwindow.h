#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "processresultdlg.h"
#include <QMainWindow>
#include <QImage>

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

    void showImage(const QString &fileName);
    ~MainWindow();

    ProcessResultDlg resultDlg;

private slots:
    void open();
    void save();
    void count();
    void about();
    void exit();
    void rgbToGray();
    void histogramEqualization();

private:
    Ui::MainWindow *ui;
    QImage *image;
    QPixmap *resultPixmap;
    QApplication* app;
    //ProcessResultDlg resultDlg;
    //QImage *MainWindow::greyScale(QImage *origin)();
};

#endif // MAINWINDOW_H
