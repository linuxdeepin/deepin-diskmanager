// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "logger.h"
#include "ddlog.h"
#include <QLoggingCategory>
#include <QObject>

#ifdef DTKCORE_CLASS_DConfigFile
#include <DConfig>
#endif
DCORE_USE_NAMESPACE

// 分类定义：主程序 / 后台服务 / 公共库 basestruct 各自独立分类，
// 规则 "org.deepin.dde.diskmanager*.debug" 通配三者（* 可跨 '.' 匹配）
Q_LOGGING_CATEGORY(appLog, "org.deepin.dde.diskmanager")
Q_LOGGING_CATEGORY(serviceLog, "org.deepin.dde.diskmanager.service")
Q_LOGGING_CATEGORY(baseLog, "org.deepin.dde.diskmanager.basestruct")

MLogger::MLogger(QObject *parent)
    : QObject(parent), m_rules("")
#ifdef DTKCORE_CLASS_DConfigFile
    , m_config(nullptr)
#endif
{
    QByteArray logRules = qgetenv("QT_LOGGING_RULES");
    // qunsetenv 之前一定不要有任何日志打印，否则取消环境变量设置不会生效（大坑）
    qunsetenv("QT_LOGGING_RULES");

    // set env
    m_rules = logRules;

#ifdef DTKCORE_CLASS_DConfigFile
    // set dconfig
    m_config = DConfig::create("org.deepin.dde.diskmanager", "org.deepin.dde.diskmanager");
    if (m_config) {
        logRules = m_config->value("log_rules").toByteArray();
        appendRules(logRules);
        setRules(m_rules);

        // 监听 dconfig 变更，实现调试模式运行中动态调整
        connect(m_config, &DConfig::valueChanged, this, [this](const QString &key) {
            if (key == "log_rules") {
                setRules(m_config->value(key).toByteArray());
            }
        });
    } else {
        // dconfig meta 未安装时退回环境变量规则，保证基础日志能力可用
        setRules(m_rules);
    }
#else
    setRules(m_rules);
#endif
}

MLogger::~MLogger()
{
#ifdef DTKCORE_CLASS_DConfigFile
    if (m_config) {
        m_config->deleteLater();
    }
#endif
}

void MLogger::setRules(const QString &rules)
{
    auto tmpRules = rules;
    // Qt 限制：setFilterRules 的分隔符必须是 '\n'，不能是 ';'
    m_rules = tmpRules.replace(";", "\n");
    QLoggingCategory::setFilterRules(m_rules);
}

void MLogger::appendRules(const QString &rules)
{
    QString tmpRules = rules;
    tmpRules = tmpRules.replace(";", "\n");
    auto tmplist = tmpRules.split('\n');
    for (int i = 0; i < tmplist.count(); i++)
        if (m_rules.contains(tmplist.at(i))) {
            tmplist.removeAt(i);
            i--;
        }
    if (tmplist.isEmpty())
        return;
    m_rules.isEmpty() ? m_rules = tmplist.join("\n")
                      : m_rules += "\n" + tmplist.join("\n");
}
