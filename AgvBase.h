/*!
 * @file AgvBase
 * @brief 描述AGV基本属性信息与功能的文件
 * @date 2019-10-16
 * @author Fankaiyu
 * @version 2.0
 */
#ifndef AGVBASE_H
#define AGVBASE_H

#include <QObject>
#include <chrono>

/*!
 * @brief 描述AGV类型信息的结构体
 * @date 2019-10-16
 */
struct AgvType
{
    std::string m_brand;     /*!< 品牌 */
    std::string m_version;   /*!< 型号 */
    unsigned char m_type;    /*!< 类型 */
    float m_maxSpeed;        /*!< 最大速度:单位(m/min) */
    float m_maxWeight;       /*!< 最大载重量:单位(kg) */
};

/*!
 * @class AgvBase
 * @brief 描述AGV基本属性信息与功能的类
 * @date 2019-10-16
 */
class AgvBase : public QObject
{
    Q_OBJECT
public:
    explicit AgvBase(QObject *parent = nullptr);

protected:
    AgvType m_type;                         /*!< 类型 */
    unsigned short m_id;                    /*!< 编号 */

protected:
    unsigned char m_mode;                   /*!< 模式 */
    unsigned char m_status;                 /*!< 状态 */
    char m_speed;                           /*!< 速度:单位(%),正向移动式速度为正数，反向移动时速度为负数 */
    unsigned char m_battery;                /*!< 电量:单位(%) */
    unsigned char m_cargo;                  /*!< 载货数量 */

protected:
    unsigned char m_action;                 /*!< 动作 */
    unsigned char m_actStatus;              /*!< 动作状态 */
    std::chrono::steady_clock m_actCount;   /*!< 动作计时器 */

signals:

public slots:
};

/*! @brief 描述AGV功能的枚举 */
enum AgvAbility
{
    Transfer = 1,   /*!< 移载式AGV */
    Lifting,        /*!< 举升式AGV */
    Pull,           /*!< 牵引式AGV */
    Submersible,    /*!< 潜入式AGV */
    Arm,            /*!< 机械臂式AGV */
    Fork,           /*!< 叉车式AGV */
};

#endif // AGVBASE_H
