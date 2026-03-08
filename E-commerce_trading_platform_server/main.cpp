#include <QCoreApplication>
#include "All.h"
#include "NL_ConnectionsManager.h"
#include "DAL_UserDAO.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv); // 创建事件循环对象

    // 1.初始化数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setUserName("root");
    db.setPassword("159357");
    db.setDatabaseName("e-commerence_trading_platform");
    if (!db.open())
    {
        qDebug() << db.lastError().text();
    }
    else
    {
        qDebug() << "database connections established!";
    }

    // 2.初始化用户数据文件系统
    if (!UserDAO::initializeFileSystem("data"))
    {
        qDebug() << "Failed to initialize user data file system!";
        return -1;
    }

    qDebug() << "User data file system initialized successfully.";

    ConnectionsManager manager;
    manager.startServer(8888); // 服务器监听端口 8888

    return a.exec();
}
