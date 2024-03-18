#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString getMd5(QString path);

private slots:
    void on_selFile_clicked();

    void on_uploadBtn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
