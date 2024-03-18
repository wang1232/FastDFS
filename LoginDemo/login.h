#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);  // explicit 添加之后不能进行隐式类型转换，只能进行显示类型转换
    ~Login();
protected:
    void paintEvent(QPaintEvent *event);

private slots:

    void on_regAccount_clicked();

    void on_regButton_clicked();
    
private:
    Ui::Login *ui;
};

#endif // LOGIN_H
