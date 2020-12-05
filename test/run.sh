SRC=../src
EXEC=lab3
PY=test_generator.py

arg=$1

if [ $# -eq 1 ]; then
    if [ $arg = "clean" ]; then
        cd $SRC
        rm -f cmake_install.cmake CMakeCache.txt
        rm -rf CMakeFiles
        rm -f $EXEC
        rm -f Makefile
        rm -f test*
        exit 0
    fi
fi

touch $SRC/test1.txt $SRC/test2.txt $SRC/test3.txt
python3 $PY 3 > $SRC/test1.txt
python3 $PY 1000 > $SRC/test2.txt
python3 $PY 30000 > $SRC/test3.txt

cd $SRC
cmake CmakeLists.txt
make


array=(`find test*`)
for TEST in $(ls test*)
do
    echo
    tput setaf 5; echo $TEST; tput setaf 7;

    tput setaf 6; printf "single-thread time:"; tput setaf 7;
    time ./$EXEC -f $TEST -t 1 1>/dev/null
    echo
    tput setaf 6; printf "multi-thread time:"; tput setaf 7;
    time ./$EXEC -f $TEST 1>/dev/null
done
