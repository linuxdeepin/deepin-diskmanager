// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#ifndef DDLOG_H
#define DDLOG_H

#include <QLoggingCategory>

// 全项目统一的日志分类声明，便于 DConfig(org.deepin.dde.diskmanager) 按应用统一过滤。
// 分类名必须以 org.deepin. 前缀、与 desktop 名(deepin-diskmanager)对应，
// 调试模式(deepin-debug-config)按 "org.deepin.dde.diskmanager*.debug" 等通配规则分级开关。

// 主程序 deepin-diskmanager（GUI 前端进程）
Q_DECLARE_LOGGING_CATEGORY(appLog)

// 后台服务 deepin-diskmanager-service（systemd/D-Bus 激活的 root 守护进程）
Q_DECLARE_LOGGING_CATEGORY(serviceLog)

// 公共基础库 basestruct（被主程序与后台服务共同链接，在两个进程中都会输出）
Q_DECLARE_LOGGING_CATEGORY(baseLog)

#endif   // DDLOG_H
