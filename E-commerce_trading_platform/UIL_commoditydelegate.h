#ifndef UIL_COMMODITYDELEGATE_H
#define UIL_COMMODITYDELEGATE_H

#include "All.h"

/**
 * @brief 商品卡片委托类，自定义商品列表项的绘制样式
 */
class CommodityDelegate : public QStyledItemDelegate
{
public:
    CommodityDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {} // 构造函数

    /**
     * @brief 自定义绘制商品卡片
     * @param painter 绘制器
     * @param option 样式选项
     * @param index 数据模型索引
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        painter->save();
        QRect rect = opt.rect;

        // 绘制背景（带圆角）
        painter->setRenderHint(QPainter::Antialiasing); // 开启抗锯齿
        QPainterPath path;
        path.addRoundedRect(rect.adjusted(2, 2, -2, -2), 5, 5); // 圆角矩形路径
        painter->fillPath(path, Qt::white);                     // 填充白色背景
        painter->setPen(QPen(Qt::lightGray, 1));
        painter->drawPath(path); // 绘制圆角边框
        // 根据鼠标状态设置背景色（悬停时变暗）
        painter->fillPath(path, (option.state & QStyle::State_MouseOver) ? QColor("#F4F6F9") : QColor("#FBFBFD"));

        // 提取商品数据
        QString name = index.data(Qt::DisplayRole).toString();             // 商品名称
        QString type = index.data(Qt::UserRole + 1).toString();            // 商品类型
        QString description = index.data(Qt::UserRole + 2).toString();     // 商品描述
        qint64 originalPrice = index.data(Qt::UserRole + 3).toLongLong();  // 原价(分为单位)
        double doubleOriginalPrice = originalPrice / 100;                  // 转换为元显示
        uint stock = index.data(Qt::UserRole + 4).toUInt();                // 库存数量
        QString sourceStoreName = index.data(Qt::UserRole + 5).toString(); // 来源商店
        float discount = index.data(Qt::UserRole + 6).toFloat();           // 折扣

        // 绘制商品信息文本
        int x = rect.x() + 10, y = rect.y() + 25; // 起始坐标
        QFont nameFont = painter->font();

        // 绘制商品名称（较大字体）
        nameFont.setPointSize(13);
        painter->setFont(nameFont);
        painter->setPen(Qt::black);
        painter->drawText(x, y, name);
        painter->setFont(opt.font); // 恢复默认字体

        // 绘制商品详细信息
        y += 20;
        painter->setPen(Qt::black);
        painter->drawText(x, y, QString("类型: %1").arg(type)); // 商品类型
        y += 20;
        painter->setPen(Qt::black);
        painter->drawText(x, y, QString("原价: %1").arg(doubleOriginalPrice)); // 商品价格
        y += 20;
        painter->setPen(Qt::black);
        painter->drawText(x, y, QString("库存: %1").arg(stock)); // 库存数量
        y += 20;
        painter->drawText(x, y, QString("商家: %1").arg(sourceStoreName)); // 来源商家

        // 库存为0时显示"已售罄"标识
        if (stock == 0)
        {
            painter->setPen(Qt::gray);
            painter->setFont(nameFont);
            x += 70;
            y += 75;
            painter->drawText(x, y, QString("已售罄"));
        }

        // 绘制底部状态指示线
        QPainterPath bottomPath;
        QLineF bottomLine(rect.left() + 2, rect.bottom() - 2, rect.right() - 2, rect.bottom() - 2);
        bottomPath.moveTo(bottomLine.p1());
        bottomPath.lineTo(bottomLine.p2());
        painter->setPen(QPen(QColor("#C9CCD1"), 1, Qt::SolidLine, Qt::RoundCap)); // 默认浅灰色圆角线

        if (option.state & QStyle::State_Selected)
        {
            // 选中状态：绘制短的深蓝色线条
            QPainterPath shortPath;
            QLineF shortLine(rect.left() + rect.width() / 4, rect.bottom() - 2, rect.left() + 3 * rect.width() / 4, rect.bottom() - 2);
            shortPath.moveTo(shortLine.p1());
            shortPath.lineTo(shortLine.p2());
            painter->setPen(QPen(QColor("#003E92"), 2, Qt::SolidLine, Qt::RoundCap)); // 深蓝色粗线
            painter->drawPath(shortPath);
        }
        else if (option.state & QStyle::State_MouseOver)
        {
            // 悬停状态：绘制浅灰色线条
            painter->drawPath(bottomPath);
        }

        painter->restore();
    }

    /**
     * @brief 返回商品卡片的固定尺寸
     * @param option 样式选项
     * @param index 数据模型索引
     * @return 卡片尺寸
     */
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        return QSize(150, 200); // 每个卡片固定尺寸：宽150px，高200px
    }
};

#endif // UIL_COMMODITYDELEGATE_H
