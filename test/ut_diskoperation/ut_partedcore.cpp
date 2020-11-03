#include <iostream>
#include "gtest/gtest.h"

#include "../../service/diskoperation/partedcore.h"

using namespace DiskManager;

class ut_partedcore : public ::testing::Test
{
protected:
    ut_partedcore()
    {
    }

    virtual ~ut_partedcore()
    {
    }
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(ut_partedcore, test)
{
    PartedCore c;
    int i = c.test();

    EXPECT_EQ(i, 1);
}
