#ifndef DAL_USERDAO_H
#define DAL_USERDAO_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

// 登录错误类型枚举
enum class LoginErrorType
{
    NoError,
    UsernameInexists,
    WrongPassword,
    FileError
};

// 注册错误类型枚举
enum class RegisterErrorType
{
    NoError,
    UsernameAlreadyExists,
    FileError
};

/**
 * @brief 用户数据结构
 */
struct UserData
{
    unsigned int id;            // 用户ID
    QString name;               // 用户名
    QString password;           // 密码
    unsigned long long balance; // 余额(分为单位)
    QString type;               // 用户类型(Customer/Merchant)
    QString storeName;          // 商店名称(商家专用)

    QMap<QString, float> uniDiscounts; // 统一折扣设置(商品类型->折扣值)

    // 构造函数
    UserData() : id(0), balance(0)
    {
        // 初始化默认统一折扣
        uniDiscounts["Clothing"] = 1.0f;
        uniDiscounts["Book"] = 1.0f;
        uniDiscounts["Food"] = 1.0f;
    }

    UserData(unsigned int id, const QString &name, const QString &password,
             unsigned long long balance, const QString &type, const QString &storeName = "")
        : id(id), name(name), password(password), balance(balance), type(type), storeName(storeName)
    {
        // 初始化默认统一折扣
        uniDiscounts["Clothing"] = 1.0f;
        uniDiscounts["Book"] = 1.0f;
        uniDiscounts["Food"] = 1.0f;
    }

    // JSON序列化
    QJsonObject toJson() const
    {
        QJsonObject obj;
        obj["id"] = static_cast<qint64>(id);
        obj["name"] = name;
        obj["password"] = password;
        obj["balance"] = static_cast<qint64>(balance);
        obj["type"] = type;
        obj["storeName"] = storeName;

        // 保存统一折扣
        QJsonObject uniDiscountObj;
        for (auto it = uniDiscounts.constBegin(); it != uniDiscounts.constEnd(); ++it)
        {
            uniDiscountObj[it.key()] = static_cast<double>(it.value());
        }
        obj["uniDiscounts"] = uniDiscountObj;

        return obj;
    }

    // JSON反序列化
    static UserData fromJson(const QJsonObject &obj)
    {
        UserData user;
        user.id = static_cast<unsigned int>(obj["id"].toInteger());
        user.name = obj["name"].toString();
        user.password = obj["password"].toString();
        user.balance = static_cast<unsigned long long>(obj["balance"].toInteger());
        user.type = obj["type"].toString();
        user.storeName = obj["storeName"].toString();

        // 加载统一折扣
        QJsonObject uniDiscountObj = obj["uniDiscounts"].toObject();
        if (!uniDiscountObj.isEmpty())
        {
            for (auto it = uniDiscountObj.constBegin(); it != uniDiscountObj.constEnd(); ++it)
            {
                user.uniDiscounts[it.key()] = static_cast<float>(it.value().toDouble());
            }
        }
        else
        {
            // 如果没有统一折扣数据，使用默认值
            user.uniDiscounts["Clothing"] = 1.0f;
            user.uniDiscounts["Book"] = 1.0f;
            user.uniDiscounts["Food"] = 1.0f;
        }

        return user;
    }
};

/**
 * @brief 用户数据访问对象，负责用户数据的持久化操作
 */
class UserDAO
{
private:
    static QString dataFilePath;    // 数据文件路径
    static unsigned int nextUserId; // 下一个用户ID
    static QMutex fileMutex;        // 线程安全保护

    // 私有文件操作方法
    static bool loadUsersFromFile(QList<UserData> &users);     // 从文件加载用户数据
    static bool saveUsersToFile(const QList<UserData> &users); // 保存用户数据到文件
    static bool ensureDataFileExists();                        // 确保数据文件存在
    static unsigned int getNextUserId();                       // 获取下一个用户ID

public:
    // 初始化文件系统
    static bool initializeFileSystem(const QString &dataDirectory = "data"); // 初始化数据目录

    // 用户认证相关方法
    static LoginErrorType userLogin(const QString &username, const QString &password, QString *outType, unsigned long long *outBalance); // 用户登录验证
    static RegisterErrorType userRegister(const QString &username, const QString &password, const QString &type);                        // 用户注册
    static bool updateUserPassword(const QString &username, const QString &newPassword);                                                 // 更新用户密码

    // 用户信息查询方法
    static bool getUserIdAndBalanceByUsername(const QString &username, unsigned int *outId, unsigned long long *outBalance); // 根据用户名获取ID和余额
    static unsigned int getUserIdByUsername(const QString &username);                                                        // 根据用户名获取用户ID
    static bool userExists(const QString &username);                                                                         // 检查用户是否存在
    static UserData getUserByUsername(const QString &username);                                                              // 根据用户名获取用户数据
    static UserData getUserById(unsigned int userId);                                                                        // 根据用户ID获取用户数据
    static QList<UserData> getAllUsers();                                                                                    // 获取所有用户数据

    // 余额管理方法
    static bool updateUserBalance(unsigned int userId, qint64 amount, unsigned long long *outNewBalance); // 更新用户余额

    // 统一折扣管理方法
    static bool getUserUniDiscount(const QString &username, const QString &type, float *outUniDiscount); // 获取用户统一折扣
    static bool setUserUniDiscount(const QString &username, const QString &type, float uniDiscount);     // 设置用户统一折扣
    static QMap<QString, float> getUserAllUniDiscounts(const QString &username);                         // 获取用户所有统一折扣
};

#endif // DAL_USERDAO_H
