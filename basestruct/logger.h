// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#ifndef BASESTRUCT_LOGGER_H
#define BASESTRUCT_LOGGER_H

#include <QObject>
#include <dtkcore_global.h>

#ifdef DTKCORE_CLASS_DConfigFile
DCORE_BEGIN_NAMESPACE
class DConfig;
DCORE_END_NAMESPACE
#endif

/**
 * @brief 动态日志规则监听器
 *
 * 参考 deepin-reader 的 MLogger 实现：
 * 1. 读取环境变量 QT_LOGGING_RULES 后立即 qunsetenv（必须在任何日志打印之前，
 *    否则取消环境变量设置不生效），保证外部注入的规则仍然有效；
 * 2. 读取 DConfig(org.deepin.dde.diskmanager) 的 log_rules 默认规则；
 * 3. 监听 DConfig valueChanged，实现调试模式运行中动态调整日志级别。
 *
 * 使用方式：在 main() 中、Application 实例化之前创建一次（先于任何日志打印）：
 *     MLogger logger;
 */
class MLogger : public QObject
{
    Q_OBJECT
public:
    explicit MLogger(QObject *parent = nullptr);
    ~MLogger();

    inline QString rules() const { return m_rules; }
    void setRules(const QString &rules);

private:
    void appendRules(const QString &rules);

private:
    QString m_rules;
#ifdef DTKCORE_CLASS_DConfigFile
    Dtk::Core::DConfig *m_config;
#endif
};

#endif   // BASESTRUCT_LOGGER_H
