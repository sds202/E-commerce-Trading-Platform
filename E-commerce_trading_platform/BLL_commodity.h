#ifndef BLL_COMMODITY_H
#define BLL_COMMODITY_H

#include "All.h"
#include "NL_HandleMessage.h"

/**
 * @brief 商品基类，处理商品相关的所有业务逻辑
 */
class Commodity
{
public:
    // 构造析构
    Commodity(); // 默认构造函数
    Commodity(unsigned int id, const QString &name, const QString &type, const QString &description,
              unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
              float discount, float uniDiscount); // 参数构造函数
    virtual ~Commodity() = default;               // 虚析构函数

    // 数据发送方法
    void send_fetchAllInformation(unsigned int id, int page); // 获取商品详细信息
    void updateCommodity(unsigned int id, const QString &name, const QString &type, const QString &description,
                         unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                         double discount, double uniDiscount); // 更新商品信息(double版本)
    void newCommodity(const QString &name, const QString &type, const QString &description,
                      unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                      double discount, double uniDiscount);                                             // 创建新商品(double版本)
    static void setUniDiscount(float uniDiscount, const QString &sourceStoreName, const QString &type); // 设置统一折扣

    // 网络操作
    static void fetchAllInfoOfCommodityFromServer(unsigned int commodityId, const QString &context);           // 从服务器获取商品信息
    static void purchaseFromServer(const QString &buyerName, unsigned int commodityId, unsigned int quantity); // 服务器购买请求
    void updateCommodity(unsigned int id, const QString &name, const QString &type, const QString &description,
                         unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                         float discount, float uniDiscount); // 更新商品信息(float版本)
    void newCommodity(const QString &name, const QString &type, const QString &description,
                      unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                      float discount, float uniDiscount);                                                         // 创建新商品(float版本)
    static void setUniDiscountFromServer(const QString &sourceStoreName, float uniDiscount, const QString &type); // 向服务器设置统一折扣
    void deleteCommodityFromServer();                                                                             // 从服务器删除商品

    // 获取方法
    unsigned int getId() const { return id; }                             // 获取商品ID
    QString getName() const { return name; }                              // 获取商品名称
    QString getType() const { return type; }                              // 获取商品类型
    QString getDescription() const { return description; }                // 获取商品描述
    unsigned long long getOriginalPrice() const { return originalPrice; } // 获取原价(分为单位)
    unsigned int getStock() const { return stock; }                       // 获取库存数量
    QString getSourceStoreName() const { return sourceStoreName; }        // 获取来源商店名
    float getDiscount() const { return discount; }                        // 获取个人折扣
    float getUniDiscount() const { return uniDiscount; }                  // 获取统一折扣

    // 设置方法
    void setStock(unsigned int newCommodityStock); // 设置库存数量

    // 计算价格
    virtual unsigned long long Getprice() const; // 计算最终价格(含折扣)

protected:
    unsigned int id;                  // 商品ID
    QString name;                     // 商品名称
    QString type;                     // 商品类型(Book/Food/Clothing)
    QString description;              // 商品描述
    unsigned long long originalPrice; // 原价(分为单位)
    unsigned int stock;               // 库存数量
    QString sourceStoreName;          // 来源商店名
    float discount;                   // 个人折扣(0-1之间)
    float uniDiscount;                // 统一折扣(0-1之间)
};

/**
 * @brief 书籍类，继承自Commodity
 */
class Book : public Commodity
{
public:
    Book() : Commodity() { type = "Book"; } // 默认构造函数
    ~Book() override = default;             // 析构函数

    Book(unsigned int id, const QString &name, const QString &description,
         unsigned long long originalPrice, unsigned int stock,
         const QString &sourceStoreName, float discount) : Commodity() // 参数构造函数
    {
        this->id = id;
        this->name = name;
        this->type = "Book";
        this->description = description;
        this->originalPrice = originalPrice;
        this->stock = stock;
        this->sourceStoreName = sourceStoreName;
        this->discount = (discount >= 0 && discount <= 1) ? discount : 1; // 折扣范围验证
    }

    unsigned long long Getprice() const override; // 重写价格计算方法

private:
};

/**
 * @brief 食品类，继承自Commodity
 */
class Food : public Commodity
{
public:
    Food() : Commodity() { type = "Food"; } // 默认构造函数
    ~Food() override = default;             // 析构函数

    Food(unsigned int id, const QString &name, const QString &description,
         unsigned long long originalPrice, unsigned int stock,
         const QString &sourceStoreName, float discount) : Commodity() // 参数构造函数
    {
        this->id = id;
        this->name = name;
        this->type = "Food";
        this->description = description;
        this->originalPrice = originalPrice;
        this->stock = stock;
        this->sourceStoreName = sourceStoreName;
        this->discount = (discount >= 0 && discount <= 1) ? discount : 1; // 折扣范围验证
    }

    unsigned long long Getprice() const override; // 重写价格计算方法

private:
};

/**
 * @brief 服装类，继承自Commodity
 */
class Clothing : public Commodity
{
public:
    Clothing() : Commodity() { type = "Clothing"; } // 默认构造函数
    ~Clothing() override = default;                 // 析构函数

    Clothing(unsigned int id, const QString &name, const QString &description,
             unsigned long long originalPrice, unsigned int stock,
             const QString &sourceStoreName, float discount) : Commodity() // 参数构造函数
    {
        this->id = id;
        this->name = name;
        this->type = "Clothing";
        this->description = description;
        this->originalPrice = originalPrice;
        this->stock = stock;
        this->sourceStoreName = sourceStoreName;
        this->discount = (discount >= 0 && discount <= 1) ? discount : 1; // 折扣范围验证
    }

    unsigned long long Getprice() const override; // 重写价格计算方法

private:
};

#endif // BLL_COMMODITY_H
