#include "BLL_commodity.h"

Commodity::Commodity()
{
    // 构造函数，这里可以进行一些初始化，比如设置数据库连接（如果DAO需要）
}

QVariantMap Commodity::getCommodityDetails(unsigned int commodityId)
{
    qDebug() << "BLL: Fetching commodity details for ID:" << commodityId;
    return m_commodityDAO.fetchCommodityDetailsById(commodityId);
}

QVariantMap Commodity::updateCommodity(unsigned int id, const QString &name, const QString &type, const QString &description,
                                       unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                                       double discount, double uniDiscount)
{
    QVariantMap result;

    // 直接调用 DAL 更新数据库
    bool success = m_commodityDAO.updateCommodity(id, name, type, description, originalPrice, stock, sourceStoreName, discount, uniDiscount);

    if (success)
    {
        result["status"] = "success";
        result["message"] = "Commodity updated successfully!";
    }
    else
    {
        result["status"] = "error";
        result["message"] = "Failed to update commodity in database. (Check ID or data)";
    }
    return result;
}

// 在 BLL_Commodity.cpp 的 newCommodity 方法中修改
QVariantMap Commodity::newCommodity(const QString &name, const QString &type, const QString &description,
                                    unsigned long long originalPrice, unsigned int stock,
                                    const QString &sourceStoreName, double discount, double uniDiscount)
{
    QVariantMap result;
    result["status"] = "error";
    result["message"] = "Unknown error.";

    try
    {
        // 如果未指定统一折扣或为默认值，从商家的用户文件中获取统一折扣
        float merchantUniDiscount = static_cast<float>(uniDiscount);
        if (uniDiscount <= 0 || uniDiscount == 1.0)
        {
            if (UserDAO::getUserUniDiscount(sourceStoreName, type, &merchantUniDiscount))
            {
                qDebug() << "Commodity::newCommodity: Using merchant uniDiscount" << merchantUniDiscount
                         << "for store" << sourceStoreName << "type" << type;
            }
            else
            {
                merchantUniDiscount = 1.0f; // 默认值
                qDebug() << "Commodity::newCommodity: Using default uniDiscount 1.0 for" << sourceStoreName;
            }
        }

        CommodityDAO commodityDAO;
        bool success = commodityDAO.newCommodity(name, type, description, originalPrice, stock, sourceStoreName, discount, merchantUniDiscount);

        if (success)
        {
            result["status"] = "success";
            result["message"] = QString("商品添加成功！使用商家统一折扣：%1").arg(merchantUniDiscount);
        }
        else
        {
            result["message"] = "商品添加失败";
        }
    }
    catch (const std::exception &e)
    {
        result["message"] = QString("添加商品时发生错误: %1").arg(e.what());
        qDebug() << "Commodity::newCommodity exception:" << e.what();
    }

    return result;
}

QVariantMap Commodity::setCommodityUniDiscount(const QString &sourceStoreName, const QString &type, float uniDiscount)
{
    QVariantMap result;
    result["status"] = "error";
    result["message"] = "Unknown error.";

    // 可以在这里添加简单的业务验证，例如 uniDiscount 的范围
    if (uniDiscount <= 0 || uniDiscount > 1.0)
    {
        result["message"] = "Universal discount must be between (0, 1].";
        return result;
    }
    if (sourceStoreName.isEmpty() || type.isEmpty())
    {
        result["message"] = "Source store name and commodity type cannot be empty.";
        return result;
    }

    bool success = m_commodityDAO.updateCommodityUniDiscount(sourceStoreName, type, uniDiscount);

    if (success)
    {
        result["status"] = "success";
        result["message"] = "Universal discount updated successfully!";
    }
    else
    {
        result["message"] = "Failed to update universal discount in database.";
    }
    return result;
}

bool Commodity::deleteCommodity(unsigned int commodityId, const QString &sourceStoreName)
{
    qDebug() << "CommodityServer: Processing delete commodity request. ID:" << commodityId
             << ", Store:" << sourceStoreName;

    // 参数验证
    if (commodityId == 0 || sourceStoreName.isEmpty())
    {
        qDebug() << "CommodityServer: Invalid parameters for delete commodity";
        return false;
    }

    // 调用DAO层删除商品
    CommodityDAO dao;
    bool success = dao.deleteCommodity(commodityId, sourceStoreName);

    if (success)
    {
        qDebug() << "CommodityServer: Commodity deleted successfully. ID:" << commodityId;
    }
    else
    {
        qDebug() << "CommodityServer: Failed to delete commodity. ID:" << commodityId;
    }

    return success;
}
