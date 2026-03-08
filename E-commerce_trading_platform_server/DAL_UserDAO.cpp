#include "DAL_UserDAO.h"

// 静态成员变量初始化
QString UserDAO::dataFilePath;
unsigned int UserDAO::nextUserId = 1;

bool UserDAO::initializeFileSystem(const QString &dataDirectory)
{
    // 确保数据目录存在
    QDir dir;
    if (!dir.exists(dataDirectory))
    {
        if (!dir.mkpath(dataDirectory))
        {
            qDebug() << "UserDAO: Failed to create data directory:" << dataDirectory;
            return false;
        }
    }

    // 设置数据文件路径
    dataFilePath = dataDirectory + "/users.json";

    // 确保数据文件存在
    if (!ensureDataFileExists())
    {
        return false;
    }

    // 加载现有用户数据以确定下一个用户ID
    QList<UserData> users;
    if (loadUsersFromFile(users))
    {
        nextUserId = 1;
        for (const UserData &user : users)
        {
            if (user.id >= nextUserId)
            {
                nextUserId = user.id + 1;
            }
        }
        qDebug() << "UserDAO: File system initialized. Next user ID:" << nextUserId;
        return true;
    }

    return false;
}

bool UserDAO::ensureDataFileExists()
{
    QFile file(dataFilePath);
    if (!file.exists())
    {
        // 创建空的用户数据文件
        if (file.open(QIODevice::WriteOnly))
        {
            QJsonObject rootObj;
            rootObj["nextUserId"] = 1;
            rootObj["users"] = QJsonArray();

            QJsonDocument doc(rootObj);
            file.write(doc.toJson());
            file.close();
            qDebug() << "UserDAO: Created new user data file:" << dataFilePath;
            return true;
        }
        else
        {
            qDebug() << "UserDAO: Failed to create user data file:" << dataFilePath;
            return false;
        }
    }
    return true;
}

bool UserDAO::loadUsersFromFile(QList<UserData> &users)
{

    QFile file(dataFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "UserDAO: Failed to open file for reading:" << dataFilePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "UserDAO: JSON parse error:" << error.errorString();
        return false;
    }

    QJsonObject rootObj = doc.object();
    QJsonArray usersArray = rootObj["users"].toArray();

    users.clear();
    for (const QJsonValue &value : usersArray)
    {
        if (value.isObject())
        {
            users.append(UserData::fromJson(value.toObject()));
        }
    }

    return true;
}

bool UserDAO::saveUsersToFile(const QList<UserData> &users)
{

    QFile file(dataFilePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "UserDAO: Failed to open file for writing:" << dataFilePath;
        return false;
    }

    QJsonArray usersArray;
    for (const UserData &user : users)
    {
        usersArray.append(user.toJson());
    }

    QJsonObject rootObj;
    rootObj["nextUserId"] = static_cast<qint64>(nextUserId);
    rootObj["users"] = usersArray;

    QJsonDocument doc(rootObj);
    file.write(doc.toJson());
    file.close();

    return true;
}

unsigned int UserDAO::getNextUserId()
{
    return nextUserId++;
}

LoginErrorType UserDAO::userLogin(const QString &username, const QString &password, QString *outType, unsigned long long *outBalance)
{
    QList<UserData> users;
    if (!loadUsersFromFile(users))
    {
        qDebug() << "UserDAO: Login failed - could not load user data";
        return LoginErrorType::FileError;
    }

    for (const UserData &user : users)
    {
        if (user.name == username)
        {
            if (user.password == password)
            {
                if (outType)
                    *outType = user.type;
                if (outBalance)
                    *outBalance = user.balance;
                qDebug() << "UserDAO: Login successful for user:" << username;
                return LoginErrorType::NoError;
            }
            else
            {
                qDebug() << "UserDAO: Wrong password for user:" << username;
                return LoginErrorType::WrongPassword;
            }
        }
    }

    qDebug() << "UserDAO: Username does not exist:" << username;
    return LoginErrorType::UsernameInexists;
}

RegisterErrorType UserDAO::userRegister(const QString &username, const QString &password, const QString &type)
{
    QList<UserData> users;
    if (!loadUsersFromFile(users))
    {
        qDebug() << "UserDAO: Register failed - could not load user data";
        return RegisterErrorType::FileError;
    }

    // 检查用户名是否已存在
    for (const UserData &user : users)
    {
        if (user.name == username)
        {
            qDebug() << "UserDAO: Username already exists:" << username;
            return RegisterErrorType::UsernameAlreadyExists;
        }
    }

    // 创建新用户
    UserData newUser(getNextUserId(), username, password, 0, type, "");
    users.append(newUser);

    // 保存到文件
    if (!saveUsersToFile(users))
    {
        qDebug() << "UserDAO: Register failed - could not save user data";
        return RegisterErrorType::FileError;
    }

    qDebug() << "UserDAO: User registered successfully:" << username << "with ID:" << newUser.id;
    return RegisterErrorType::NoError;
}

bool UserDAO::getUserIdAndBalanceByUsername(const QString &username, unsigned int *outId, unsigned long long *outBalance)
{
    QList<UserData> users;
    if (!loadUsersFromFile(users))
    {
        qDebug() << "UserDAO: Failed to get user ID/balance - could not load user data";
        return false;
    }

    for (const UserData &user : users)
    {
        if (user.name == username)
        {
            if (outId)
                *outId = user.id;
            if (outBalance)
                *outBalance = user.balance;
            return true;
        }
    }

    qDebug() << "UserDAO: User with username" << username << "not found.";
    return false;
}

bool UserDAO::updateUserBalance(unsigned int userId, qint64 amount, unsigned long long *outNewBalance)
{
    QList<UserData> users;
    if (!loadUsersFromFile(users))
    {
        qDebug() << "UserDAO: Failed to update balance - could not load user data";
        return false;
    }

    bool userFound = false;
    for (UserData &user : users)
    {
        if (user.id == userId)
        {
            // 检查余额是否足够（如果是扣款操作）
            if (amount < 0 && static_cast<unsigned long long>(-amount) > user.balance)
            {
                qDebug() << "UserDAO: Insufficient balance for user ID:" << userId;
                return false;
            }

            user.balance = static_cast<unsigned long long>(static_cast<qint64>(user.balance) + amount);
            if (outNewBalance)
                *outNewBalance = user.balance;
            userFound = true;
            break;
        }
    }

    if (!userFound)
    {
        qDebug() << "UserDAO: User with ID" << userId << "not found for balance update.";
        return false;
    }

    // 保存更新后的数据
    if (!saveUsersToFile(users))
    {
        qDebug() << "UserDAO: Failed to save updated balance";
        return false;
    }

    qDebug() << "UserDAO: Balance updated successfully for user ID:" << userId << "amount:" << amount;
    return true;
}

bool UserDAO::updateUserPassword(const QString &username, const QString &newPassword)
{
    QList<UserData> users;
    if (!loadUsersFromFile(users))
    {
        qDebug() << "UserDAO: Failed to update password - could not load user data";
        return false;
    }

    bool userFound = false;
    for (UserData &user : users)
    {
        if (user.name == username)
        {
            user.password = newPassword;
            userFound = true;
            break;
        }
    }

    if (!userFound)
    {
        qDebug() << "UserDAO: User with username" << username << "not found for password update.";
        return false;
    }

    // 保存更新后的数据
    if (!saveUsersToFile(users))
    {
        qDebug() << "UserDAO: Failed to save updated password";
        return false;
    }

    qDebug() << "UserDAO: Password updated successfully for user:" << username;
    return true;
}

unsigned int UserDAO::getUserIdByUsername(const QString &username)
{
    QList<UserData> users;
    if (!loadUsersFromFile(users))
    {
        qDebug() << "UserDAO: Failed to get user ID - could not load user data";
        return 0;
    }

    for (const UserData &user : users)
    {
        if (user.name == username)
        {
            return user.id;
        }
    }

    qDebug() << "UserDAO: Failed to get user ID for username" << username;
    return 0; // 0 表示未找到或查询失败
}

bool UserDAO::userExists(const QString &username)
{
    QList<UserData> users;
    if (!loadUsersFromFile(users))
    {
        return false;
    }

    for (const UserData &user : users)
    {
        if (user.name == username)
        {
            return true;
        }
    }
    return false;
}

UserData UserDAO::getUserByUsername(const QString &username)
{
    QList<UserData> users;
    if (loadUsersFromFile(users))
    {
        for (const UserData &user : users)
        {
            if (user.name == username)
            {
                return user;
            }
        }
    }
    return UserData(); // 返回默认构造的用户数据
}

UserData UserDAO::getUserById(unsigned int userId)
{
    QList<UserData> users;
    if (loadUsersFromFile(users))
    {
        for (const UserData &user : users)
        {
            if (user.id == userId)
            {
                return user;
            }
        }
    }
    return UserData(); // 返回默认构造的用户数据
}

QList<UserData> UserDAO::getAllUsers()
{
    QList<UserData> users;
    loadUsersFromFile(users);
    return users;
}

// 统一折扣管理方法
bool UserDAO::getUserUniDiscount(const QString &username, const QString &type, float *outUniDiscount)
{
    QList<UserData> users;
    if (!loadUsersFromFile(users))
    {
        qDebug() << "UserDAO: Failed to load user data for discount retrieval";
        return false;
    }

    for (const UserData &user : users)
    {
        if (user.name == username)
        {
            if (user.uniDiscounts.contains(type))
            {
                if (outUniDiscount)
                    *outUniDiscount = user.uniDiscounts[type];
                return true;
            }
            else
            {
                qDebug() << "UserDAO: Discount type" << type << "not found for user:" << username;
                return false;
            }
        }
    }

    qDebug() << "UserDAO: User not found for discount retrieval:" << username;
    return false;
}

bool UserDAO::setUserUniDiscount(const QString &username, const QString &type, float uniDiscount)
{
    QList<UserData> users;
    if (!loadUsersFromFile(users))
    {
        qDebug() << "UserDAO: Failed to load user data for discount update";
        return false;
    }

    bool userFound = false;
    for (UserData &user : users)
    {
        if (user.name == username)
        {
            user.uniDiscounts[type] = uniDiscount;
            userFound = true;
            break;
        }
    }

    if (!userFound)
    {
        qDebug() << "UserDAO: User not found for discount update:" << username;
        return false;
    }

    // 保存更新后的数据
    if (!saveUsersToFile(users))
    {
        qDebug() << "UserDAO: Failed to save updated user data after discount update";
        return false;
    }

    qDebug() << "UserDAO: Discount updated successfully for user:" << username << "type:" << type << "discount:" << uniDiscount;
    return true;
}

QMap<QString, float> UserDAO::getUserAllUniDiscounts(const QString &username)
{
    QList<UserData> users;
    QMap<QString, float> discounts;

    if (!loadUsersFromFile(users))
    {
        qDebug() << "UserDAO: Failed to load user data for all discount retrieval";
        return discounts;
    }

    for (const UserData &user : users)
    {
        if (user.name == username)
        {
            discounts = user.uniDiscounts;
            return discounts;
        }
    }

    qDebug() << "UserDAO: User not found for all discount retrieval:" << username;
    return discounts;
}
