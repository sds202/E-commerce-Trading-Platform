#include "BLL_user.h"

unsigned long long User::getBalance() const
{
    return balance;
}

QString User::getType() const
{
    return type;
}

QString User::getName() const
{
    return name;
}

QString User::getPassword() const
{
    return password;
}

User::User()
    : name(""), password(""), balance(0), type("")
{
}

LoginErrorType User::userLogin(const QString &username, const QString &password)
{
    UserDAO userdao;
    LoginErrorType rtype = userdao.userLogin(username, password, &this->type, &this->balance);

    if (rtype == LoginErrorType::NoError)
    {
        this->name = username;
        this->password = password;
    }

    return rtype;
}

RegisterErrorType User::userRegister(const QString &username, const QString &password, const QString &type)
{
    UserDAO userdao;
    RegisterErrorType rtype = userdao.userRegister(username, password, type);

    if (rtype == RegisterErrorType::NoError)
    {
        this->name = username;
        this->password = password;
        this->balance = 0;
        this->type = type;
    }

    return rtype;
}

QVariantMap User::makePurchase(const QString &buyerUsername, unsigned int commodityId, unsigned int purchaseQuantity, const QString &sellerStoreName, unsigned long long totalPrice)
{
    QVariantMap result;
    result["status"] = "error"; // 默认失败
    result["message"] = "Unknown error.";

    if (purchaseQuantity == 0 || totalPrice == 0)
    {
        result["message"] = "Purchase quantity or total price cannot be zero.";
        return result;
    }

    QSqlDatabase db = QSqlDatabase::database(); // 获取默认数据库连接
    if (!db.isValid())
    {
        qDebug() << "BLL: Database connection is invalid.";
        result["message"] = "Database connection error.";
        return result;
    }

    // 1. 获取买家信息
    unsigned int buyerUserId = 0;
    unsigned long long buyerCurrentBalance = 0;
    if (!m_userDAO.getUserIdAndBalanceByUsername(buyerUsername, &buyerUserId, &buyerCurrentBalance))
    {
        result["message"] = "Buyer user not found or database error.";
        return result;
    }

    // 2. 获取卖家信息 (通过 storeName 查找)
    unsigned int sellerUserId = 0;
    unsigned long long sellerCurrentBalance = 0;
    if (!m_userDAO.getUserIdAndBalanceByUsername(sellerStoreName, &sellerUserId, &sellerCurrentBalance))
    {
        result["message"] = "Seller store not found or database error.";
        return result;
    }

    // 3. 获取商品信息 (简单验证商品是否存在和获取库存)
    unsigned int commodityCurrentStock = 0;
    if (!m_commodityDAO.getCommodityStock(commodityId, &commodityCurrentStock))
    {
        result["message"] = "Commodity not found or database error.";
        return result;
    }

    // 5. 验证余额和库存
    if (buyerCurrentBalance < totalPrice)
    {
        result["message"] = QString("Insufficient balance. Your balance: %1, Required: %2")
                                .arg(buyerCurrentBalance / 100.0, 0, 'f', 2)
                                .arg(totalPrice / 100.0, 0, 'f', 2);
        return result;
    }
    if (commodityCurrentStock < purchaseQuantity)
    {
        result["message"] = QString("Insufficient stock for commodity. Available: %1, Requested: %2")
                                .arg(commodityCurrentStock)
                                .arg(purchaseQuantity);
        return result;
    }

    // 4. 简单的价格验证：从数据库获取商品的当前价格
    QSqlQuery query(db);
    query.prepare("SELECT originalPrice, discount, uniDiscount FROM commodity_table WHERE id = :commodityId AND sourceStoreName = :storeName");
    query.bindValue(":commodityId", commodityId);
    query.bindValue(":storeName", sellerStoreName);

    if (!query.exec() || !query.next())
    {
        result["message"] = "Cannot get commodity price or commodity not belongs to seller.";
        return result;
    }

    // 计算服务端的实际总价
    unsigned long long originalPrice = query.value("originalPrice").toULongLong();
    float discount = query.value("discount").toFloat();
    float uniDiscount = query.value("uniDiscount").toFloat();

    unsigned long long serverTotalPrice = static_cast<unsigned long long>(originalPrice * discount * uniDiscount) * purchaseQuantity;

    // 验证价格是否一致
    if (totalPrice != serverTotalPrice)
    {
        result["message"] = QString("Price mismatch! Expected: %1, Provided: %2")
                                .arg(static_cast<double>(serverTotalPrice) / 100.0, 0, 'f', 2)
                                .arg(static_cast<double>(totalPrice) / 100.0, 0, 'f', 2);
        qDebug() << "BLL: Price mismatch for commodity" << commodityId << "Expected:" << serverTotalPrice << "Got:" << totalPrice;
        return result;
    }

    // 6. 开始事务
    if (!db.transaction())
    {
        qDebug() << "BLL: Failed to start transaction:" << db.lastError().text();
        result["message"] = "Database transaction error.";
        return result;
    }

    bool allOperationsSuccess = true;
    unsigned long long newBuyerBalance = 0;
    unsigned long long newBuyerBalance_merchant = 0;
    unsigned int newCommodityStock = 0;

    try
    {
        // 7. 扣除买家余额
        if (!m_userDAO.updateUserBalance(buyerUserId, -static_cast<qint64>(totalPrice), &newBuyerBalance))
        {
            qDebug() << "BLL: Failed to debit buyer balance.";
            allOperationsSuccess = false;
        }

        // 8. 增加卖家余额
        if (allOperationsSuccess)
        {
            if (!m_userDAO.updateUserBalance(sellerUserId, static_cast<qint64>(totalPrice), &newBuyerBalance_merchant))
            {
                qDebug() << "BLL: Failed to credit seller balance.";
                allOperationsSuccess = false;
            }
        }

        // 9. 减少商品库存
        if (allOperationsSuccess)
        {
            if (!m_commodityDAO.updateCommodityStock(commodityId, -static_cast<qint64>(purchaseQuantity), &newCommodityStock))
            {
                qDebug() << "BLL: Failed to decrease commodity stock.";
                allOperationsSuccess = false;
            }
        }

        // 10. 提交或回滚事务
        if (allOperationsSuccess)
        {
            if (db.commit())
            {
                result["status"] = "success";
                result["message"] = "Purchase successful!";
                result["newBuyerBalance"] = newBuyerBalance;
                result["newCommodityStock"] = newCommodityStock;
                qDebug() << "BLL: Purchase committed for buyer" << buyerUsername;
            }
            else
            {
                qDebug() << "BLL: Failed to commit transaction:" << db.lastError().text();
                result["message"] = "Database commit error.";
                db.rollback();
            }
        }
        else
        {
            db.rollback();
            qDebug() << "BLL: Purchase rolled back due to an error.";
            if (result["message"].toString() == "Unknown error.")
            {
                result["message"] = "An error occurred during purchase, transaction rolled back.";
            }
        }
    }
    catch (const std::exception &e)
    {
        qDebug() << "BLL: Exception during purchase:" << e.what();
        db.rollback();
        result["message"] = "An unexpected error occurred during purchase.";
    }
    catch (...)
    {
        qDebug() << "BLL: Unknown exception during purchase.";
        db.rollback();
        result["message"] = "An unexpected error occurred during purchase.";
    }

    return result;
}

QVariantMap User::changePassword(const QString &username, const QString &newPassword)
{
    QVariantMap result;
    result["status"] = "error";
    result["message"] = "Unknown error.";

    // 可以在这里添加简单的业务验证，例如密码长度等，但根据你的要求，这里只做最简单的转发
    if (username.isEmpty() || newPassword.isEmpty())
    {
        result["message"] = "Username and new password cannot be empty.";
        return result;
    }

    bool success = m_userDAO.updateUserPassword(username, newPassword);

    if (success)
    {
        result["status"] = "success";
        result["message"] = "Password updated successfully!";
    }
    else
    {
        result["message"] = "Failed to update password in database. User might not exist.";
    }
    return result;
}
QVariantMap User::rechargeBalance(const QString &username, unsigned long long amount)
{
    QVariantMap result;
    result["status"] = "error";
    result["message"] = "Unknown error.";

    if (username.isEmpty())
    {
        result["message"] = "Username cannot be empty.";
        return result;
    }
    if (amount <= 0)
    {
        result["message"] = "Recharge amount must be positive.";
        return result;
    }

    // 1. 根据用户名获取用户ID
    unsigned int userId = m_userDAO.getUserIdByUsername(username);
    if (userId == 0)
    {
        result["message"] = "User not found.";
        return result;
    }

    // 2. 调用 updateUserBalance 更新余额
    unsigned long long newBalance = 0;
    // 注意：updateUserBalance 的 amount 参数是 qint64。
    // 如果 amount 是 unsigned long long，需要确保它不会溢出 qint64 的正值范围。
    // 对于充值，amount 总是正数，所以直接 static_cast<qint64> 是安全的。
    bool success = m_userDAO.updateUserBalance(userId, static_cast<qint64>(amount), &newBalance);

    if (success)
    {
        result["status"] = "success";
        result["message"] = "Recharge successful!";
        result["newBalance"] = static_cast<qint64>(newBalance); // 返回新余额给客户端
    }
    else
    {
        result["message"] = "Failed to recharge balance.";
    }
    return result;
}
