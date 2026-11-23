// SPDX-FileCopyrightText: 2022 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-only

#include "gtest/gtest.h"

#include <climits>

#include "../../basestruct/utils.h"
#include "../../basestruct/commondef.h"

/**
 * @brief 尺寸工具函数（formatSize / floorSize / ceilSize）单元测试
 *
 * 覆盖 PMS BUG-383517 修复的场景：
 *  - formatSize() 的 sectors * sectorSize 乘法溢出
 *  - floorSize()/ceilSize() 的 roundingSize 零值/负值除零
 *  - ceilSize() 分子加法溢出及「向上取整结果不可表示」的失败上报
 */
class ut_utils : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

/* -------------------- formatSize 正常单位换算 -------------------- */

TEST_F(ut_utils, formatSize_byte_unit)
{
    // 小于 1 KiB 的走 B 分支
    EXPECT_EQ(Utils::formatSize(0, 512), QString("0.00 B"));
    EXPECT_EQ(Utils::formatSize(1, 512), QString("512.00 B"));
}

TEST_F(ut_utils, formatSize_kib_unit)
{
    // 512 字节扇区，2 扇区 = 1024 字节 = 1 KiB
    EXPECT_EQ(Utils::formatSize(2, 512), QString("1.00 KiB"));
}

TEST_F(ut_utils, formatSize_mib_unit)
{
    // 2048 扇区 * 512 = 1 MiB
    EXPECT_EQ(Utils::formatSize(2048, 512), QString("1.00 MiB"));
}

TEST_F(ut_utils, formatSize_gib_unit)
{
    // 2097152 扇区 * 512 = 1 GiB
    EXPECT_EQ(Utils::formatSize(2097152, 512), QString("1.00 GiB"));
}

TEST_F(ut_utils, formatSize_tib_unit)
{
    // 2147483648 扇区 * 512 = 1 TiB
    EXPECT_EQ(Utils::formatSize(2147483648LL, 512), QString("1.00 TiB"));
}

TEST_F(ut_utils, formatSize_overflow_no_crash)
{
    // sectors * sectorSize 溢出时不能崩溃/未定义行为；
    // 结果饱和到 LLONG_MAX 后落入 TiB 分支
    QString res = Utils::formatSize(LLONG_MAX, 512);
    EXPECT_FALSE(res.isEmpty());
    EXPECT_TRUE(res.endsWith("TiB"));
}

/* -------------------- floorSize -------------------- */

TEST_F(ut_utils, floorSize_normal)
{
    EXPECT_EQ(Utils::floorSize(13, 4), 12);     // 向下取整到 4 的倍数
    EXPECT_EQ(Utils::floorSize(16, 4), 16);     // 已是倍数保持不变
    EXPECT_EQ(Utils::floorSize(4099, 4096), 4096);
    EXPECT_EQ(Utils::floorSize(0, 4096), 0);
}

TEST_F(ut_utils, floorSize_zero_rounding)
{
    // roundingSize 为 0 时不应 SIGFPE，直接返回原值
    EXPECT_EQ(Utils::floorSize(12345, 0), 12345);
    // 负值同样安全返回
    EXPECT_EQ(Utils::floorSize(12345, -4), 12345);
}

/* -------------------- ceilSize -------------------- */

TEST_F(ut_utils, ceilSize_normal)
{
    bool overflow = true;
    EXPECT_EQ(Utils::ceilSize(13, 4, &overflow), 16);   // 向上取整到 4 的倍数
    EXPECT_FALSE(overflow);
    EXPECT_EQ(Utils::ceilSize(16, 4, &overflow), 16);   // 已是倍数保持不变
    EXPECT_FALSE(overflow);
    EXPECT_EQ(Utils::ceilSize(4097, 4096, &overflow), 8192);
    EXPECT_FALSE(overflow);
    EXPECT_EQ(Utils::ceilSize(0, 4096, &overflow), 0);
    EXPECT_FALSE(overflow);
}

TEST_F(ut_utils, ceilSize_zero_rounding)
{
    // roundingSize 为 0 时不应 SIGFPE，直接返回原值
    bool overflow = true;
    EXPECT_EQ(Utils::ceilSize(12345, 0, &overflow), 12345);
    EXPECT_FALSE(overflow);
    // 负值同样安全返回
    EXPECT_EQ(Utils::ceilSize(12345, -4, &overflow), 12345);
    EXPECT_FALSE(overflow);
}

TEST_F(ut_utils, ceilSize_no_overflow_default_param)
{
    // 不传 overflow 出参时仍可正常使用（默认参数）
    EXPECT_EQ(Utils::ceilSize(13, 4), 16);
}

TEST_F(ut_utils, ceilSize_overflow_reports_failure)
{
    // 分子加法溢出：value 接近 LLONG_MAX 且 roundingSize 不整除 LLONG_MAX。
    // 应报告 overflow=true 并返回饱和值 LLONG_MAX，而不是向下取整到更小倍数。
    bool overflow = false;
    Byte_Value res = Utils::ceilSize(LLONG_MAX, 4, &overflow);
    EXPECT_TRUE(overflow);
    EXPECT_EQ(res, LLONG_MAX);
}

TEST_F(ut_utils, ceilSize_boundary_no_overflow)
{
    // LLONG_MAX-1 加 1 恰好等于 LLONG_MAX，不构成溢出
    bool overflow = true;
    Byte_Value res = Utils::ceilSize(LLONG_MAX - 1, 2, &overflow);
    EXPECT_FALSE(overflow);
    EXPECT_EQ(res, LLONG_MAX - 1);  // (LLONG_MAX-1) 是 2 的倍数
}
