#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog> // 选择文件对话框
#include <QMessageBox>
// 进行http传输的模板
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QFileInfo> // 获取文件属性信息
#include <QCryptographicHash>// md5
#include "logininstance.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始化进度条
    ui->progressBar->setRange(0,100);
    ui->progressBar->setValue(0);

    // 登录成功 -> 设置当前用户的名字
    LoginInstance* ins = LoginInstance::getInstance();
    ins->setUserName("Tom");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getMd5(QString path)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    // 1. 添加数据
    QFile file(path);
    file.open(QFile::ReadOnly);
    hash.addData(&file);
    // 2. 数据运算 —> 结果
    QByteArray res =  hash.result().toHex();
    // 3. return result
    return res;
}


void MainWindow::on_selFile_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"打开文件","d:\\");
    if( path.isEmpty() )
    {
        QMessageBox::warning(this,"Warning","文件路径为空");
        return;
    }
    ui->filePath->setText(path);
}


void MainWindow::on_uploadBtn_clicked()
{
    // 1. 创建networkmanage对象
    QNetworkAccessManager* pManage = new QNetworkAccessManager(this);
    // 2. 发送数据
    QNetworkRequest request;
    request.setUrl(QUrl("http://192.168.198.131:80/upload"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"multipart/form-data");

    //post 数据块
    QFileInfo info(ui->filePath->text());
    QHttpPart part;
    QString disp = QString("form-data; user=\"%1\"; filename=\"%2\"; md5=\"%3\"; size=%4").arg(LoginInstance::getInstance()->getName()).arg(info.fileName()).arg(getMd5(ui->filePath->text())).arg(info.size());
    QString sufix = info.suffix();
    part.setHeader(QNetworkRequest::ContentDispositionHeader,disp);
    // 堆空间，发送完成在释放
    QFile *file  = new QFile(ui->filePath->text());
    file->open(QFile::ReadOnly);
    part.setBodyDevice(file);
    // 传输的数据块的格式
    /*
     * json配置文件
     * {
     *      ".mp3":"audio/mp3",
     *      ".avi":"video/avi",
     * }
    */
    part.setHeader(QNetworkRequest::ContentTypeHeader,"xxx");
    QHttpMultiPart * multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType,this);
    multiPart->append(part);

    ui->recordMessage->append("开始向服务器传送数据……");
    // post 是非阻塞函数
    QNetworkReply* reply =  pManage->post(request,multiPart /*QHttpMultiPart *multiPart*/);
    connect(reply,&QNetworkReply::readyRead,this,[=](){
       // 3. 接受数据
        QByteArray all = reply->readAll();
       // 4. 格式解析-> 纯文本
       // 5. 判断是否上传成功了
         ui->recordMessage->append("服务器响应数据：" + all);
        qDebug() << disp;
        // 释放内存
        // delete multiPart;
        // delete file;

         /*
          * 释放new出来的堆空间——保证数据上传完毕
          * 1. 释放multiPart
          * 2. 释放file 记得先close file
          * 3. 释放reply
        */
         multiPart->deleteLater(); // deleteLater当用完时候在释放，和delete一样，比delete更加安全
         file->close();
         file->deleteLater();
         reply->deleteLater();
    });

    // 进度条
    connect(reply,&QNetworkReply::uploadProgress,this,[=](qint64 bytesSent, qint64 bytesTotal){
        // 更新进度条
        qDebug() << "当前进度：" << bytesSent << "总数：" << bytesTotal;
        if(bytesSent > 0)
            ui->progressBar->setValue(bytesSent * 100 / bytesTotal );
    });
}

