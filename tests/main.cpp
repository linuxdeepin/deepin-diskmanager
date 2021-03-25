#include "gtest/gtest.h"
#include <QApplication>
#include "cusapplication.h"

int main(int argc, char *argv[])
{
//    qputenv("QT_QPA_PLATFORM", "offscreen");
    CusApplication a(argc, argv);

    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    return ret;
}

