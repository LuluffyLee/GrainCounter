#ifndef PROCESSRESULTDLG_H
#define PROCESSRESULTDLG_H

#include <QDialog>
#include <QImage>

namespace Ui {
class ProcessResultDlg;
}

class ProcessResultDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessResultDlg(QWidget *parent = 0);
    ~ProcessResultDlg();

    QImage *resultImage;
    void showResultImage(QImage *);
    void editLabel(const QString);

private:
    Ui::ProcessResultDlg *ui;
};

#endif // PROCESSRESULTDLG_H
