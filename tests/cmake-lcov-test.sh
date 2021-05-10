#!/bin/bash
export DISPLAY=:0.0
utdir=build-ut
rm -r $utdir
echo "123456" | sudo -S rm -r ../$utdir
mkdir ../$utdir
cd ../$utdir

cmake -DCMAKE_SAFETYTEST_ARG="CMAKE_SAFETYTEST_ARG_ON" ..
make -j4

touch ./tests/utcase.log
echo "123456" | sudo -S ./tests/deepin-diskmanager_app_test --gtest_output=xml:./report/report.xml | tee ./bin/utcase.log

workdir=$(cd ../$(dirname $0)/$utdir; pwd)

mkdir -p report

echo "123456" | sudo -S lcov -d $workdir -c -o ./report/coverage.info

echo "123456" | sudo -S lcov --extract ./report/coverage.info '*/application/*' '*/basestruct/*' -o ./report/coverage.info

echo "123456" | sudo -S lcov --remove ./report/coverage.info '*/tests/*' '*/application/partedproxy/*' -o ./report/coverage.info

echo "123456" | sudo -S genhtml -o ./report ./report/coverage.info

echo "123456" | sudo -S chmod 666 ./asan.log*

echo "123456" | sudo -S chmod 666 ./report/index.html

exit 0
