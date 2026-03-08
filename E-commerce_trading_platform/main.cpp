#include "UIL_mainwindow.h"
#include "All.h"

QTcpSocket tcpSocket;
HandleMessage *m_handleMessage;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1.建立TCP连接
    tcpSocket.connectToHost("127.0.0.1", 8888); // 这里以本地8888端口为例
    if (!tcpSocket.waitForConnected(3000))
    { // 等待3秒
        QMessageBox::critical(nullptr, "连接失败", "无法建立到服务器的TCP连接，程序无法正常工作。");
        // return -1;
    }

    // 2.使用QSS样式表
    QFile file(":/all.qss"); // 替换为你的QSS文件路径
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QLatin1String(file.readAll());
        a.setStyleSheet(styleSheet); // 应用到整个应用程序
        file.close();
        qDebug() << "打开了";
    }
    else
    {
        qWarning() << "无法打开QSS文件";
    }

    // 3.初始化全局信息接收器
    m_handleMessage = new HandleMessage(); // 'this' 作为父对象，确保生命周期管理

    // 4.开启主窗口
    MainWindow w;

    w.show();
    return a.exec();
}
