// BLL_Commodity.h
#ifndef BLL_COMMODITY_H
#define BLL_COMMODITY_H

#include <QVariantMap>
#include "DAL_CommodityDAO.h"
#include "DAL_UserDAO.h"

/**
 * @brief 服务器端商品业务逻辑类
 */
class Commodity
{
public:
    Commodity(); // 构造函数

    // 商品信息操作
    QVariantMap getCommodityDetails(unsigned int commodityId); // 根据商品ID获取详细信息
    QVariantMap updateCommodity(unsigned int id, const QString &name, const QString &type, const QString &description,
                                unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                                double discount, double uniDiscount); // 更新商品信息
    QVariantMap newCommodity(const QString &name, const QString &type, const QString &description,
                             unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                             double discount, double uniDiscount); // 创建新商品

    // 商品管理操作
    QVariantMap setCommodityUniDiscount(const QString &sourceStoreName, const QString &type, float uniDiscount); // 设置统一折扣
    static bool deleteCommodity(unsigned int commodityId, const QString &sourceStoreName);                       // 删除商品

private:
    CommodityDAO m_commodityDAO; // 商品数据访问对象
};

#endif // BLL_COMMODITY_H
