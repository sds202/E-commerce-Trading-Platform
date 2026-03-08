#ifndef DAL_COMMODITYDAO_H
#define DAL_COMMODITYDAO_H

#include "All.h"

/**
 * @brief 商品数据访问对象，负责商品数据的持久化操作
 */
class CommodityDAO
{
public:
    // 商品查询方法
    bool FetchAllCommodities(const QString &storeName, QJsonArray &commoditiesArray); // 获取指定商店的所有商品
    QVariantMap fetchCommodityDetailsById(unsigned int commodityId);                  // 根据商品ID获取详细信息

    // 库存管理方法
    bool getCommodityStock(unsigned int commodityId, unsigned int *outStock);                                        // 获取商品库存数量
    bool updateCommodityStock(unsigned int commodityId, qint64 quantityChange, unsigned int *outNewStock = nullptr); // 更新商品库存(正数增加，负数减少)

    // 商品信息管理方法
    bool updateCommodity(unsigned int id, const QString &name, const QString &type, const QString &description,
                         unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                         float discount, float uniDiscount); // 更新商品信息
    bool newCommodity(const QString &name, const QString &type, const QString &description,
                      unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                      float discount, float uniDiscount);                           // 创建新商品
    bool deleteCommodity(unsigned int commodityId, const QString &sourceStoreName); // 删除商品

    // 折扣管理方法
    bool updateCommodityUniDiscount(const QString &sourceStoreName, const QString &type, float uniDiscount); // 更新商品统一折扣
};

#endif // DAL_COMMODITYDAO_H
