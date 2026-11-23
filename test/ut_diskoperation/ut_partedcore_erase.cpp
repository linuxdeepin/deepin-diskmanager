// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "gtest/gtest.h"

#include "../../service/diskoperation/partedcore.h"
#include "../../service/diskoperation/partition.h"
#include "../../basestruct/commondef.h"

using namespace DiskManager;

/**
 * @brief PartedCore::eraseFilesystemSignatures() 单元测试
 *
 * 该函数在 PMS BUG-383517 修复中新增了 ceilSize() 溢出检测：
 * 当擦除范围的上取整结果无法用 Byte_Value 表示时，必须中止并报告失败，
 * 而不是用截断后的范围继续擦除（否则会遗漏尾随字节）。
 *
 * 说明：函数内部 ranges 为局部常量、且依赖真实块设备才能走到写入
 * 循环，溢出分支在正常输入下不可达。这里覆盖无需真实设备的确定性分支：
 *   - 打开中的 LUKS 加密分区应直接失败早退；
 *   - 设备不存在/无法打开时应安全返回失败而不崩溃。
 */
class ut_partedcore_erase : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ut_partedcore_erase, luks_busy_partition_returns_false)
{
    // 打开中的 LUKS 加密分区不允许在仅擦除签名的步骤中处理，
    // 应直接返回 false 早退（不访问任何设备）。
    PartedCore core;
    Partition partition;
    partition.m_fstype = FS_LUKS;
    partition.m_busy = true;

    EXPECT_FALSE(core.eraseFilesystemSignatures(partition));
}

TEST_F(ut_partedcore_erase, nonexistent_device_returns_false_without_crash)
{
    // 设备路径不存在时 getDevice() 失败，overallSuccess 保持 false，
    // 函数应安全返回 false，既不崩溃也不写入任何数据。
    PartedCore core;
    Partition partition;
    partition.m_fstype = FS_EXT4;
    partition.m_busy = false;
    partition.m_devicePath = "/dev/__ut_nonexistent_device_383517__";
    partition.m_type = TYPE_PRIMARY;
    partition.m_sectorStart = 2048;
    partition.m_sectorEnd = 4096;
    partition.m_sectorSize = 512;

    EXPECT_FALSE(core.eraseFilesystemSignatures(partition));
}

TEST_F(ut_partedcore_erase, unpartitioned_nonexistent_device_returns_false)
{
    // 未分区（虚拟整盘）类型 + 不存在设备：overallSuccess 曾先被置 true，
    // 但 deviceIsOpen 为 false 后仍应整体返回 false。
    PartedCore core;
    Partition partition;
    partition.m_fstype = FS_UNALLOCATED;
    partition.m_busy = false;
    partition.m_devicePath = "/dev/__ut_nonexistent_device_383517__";
    partition.m_type = TYPE_UNPARTITIONED;
    partition.m_sectorSize = 512;
    partition.m_sectorStart = 0;

    EXPECT_FALSE(core.eraseFilesystemSignatures(partition));
}
