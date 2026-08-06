// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "gtest/gtest.h"

#include "../../service/diskoperation/DeviceStorage.h"

#include <QFile>
#include <QTemporaryDir>

using namespace DiskManager;

namespace {

class PathGuard
{
public:
    explicit PathGuard(const QString &path)
        : m_oldPath(qgetenv("PATH"))
    {
        qputenv("PATH", QString("%1:%2").arg(path, QString::fromLocal8Bit(m_oldPath)).toLocal8Bit());
    }

    ~PathGuard()
    {
        qputenv("PATH", m_oldPath);
    }

private:
    QByteArray m_oldPath;
};

void writeExecutable(const QString &filePath, const QByteArray &content)
{
    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
    ASSERT_EQ(file.write(content), content.size());
    file.close();
    ASSERT_TRUE(file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner));
}

} // namespace

class ut_devicestorage : public ::testing::Test
{
};

TEST_F(ut_devicestorage, getDiskInfoInterface_usesAttachedToWhenCapacityFollows)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    writeExecutable(dir.filePath("hwinfo"),
                    "#!/bin/sh\n"
                    "printf '%s\\n' '10: None 00.0: 10600 Disk' "
                    "'  Attached to: #1 (UFS 3.1 Controller)' "
                    "'  Capacity: 1 TB (1024626524160 bytes)'\n");
    PathGuard guard(dir.path());

    DeviceStorage storage;
    QString interface;
    QString model("__ut_nonexistent_model__");

    storage.getDiskInfoInterface("/dev/__ut_nonexistent_pms372777_ufs__", interface, model);

    EXPECT_EQ(interface, "UFS");
}

TEST_F(ut_devicestorage, getDiskInfoInterface_ignoresCapacityBytesWhenInterfaceMissing)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    writeExecutable(dir.filePath("hwinfo"),
                    "#!/bin/sh\n"
                    "printf '%s\\n' '  Capacity: 1 TB (1024626524160 bytes)'\n");
    PathGuard guard(dir.path());

    DeviceStorage storage;
    QString interface;
    QString model("__ut_nonexistent_model__");

    storage.getDiskInfoInterface("/dev/__ut_nonexistent_pms372777_capacity__", interface, model);

    EXPECT_TRUE(interface.isEmpty());
}
