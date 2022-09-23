
SRC_FILES=$(find src -name "*.c" -o -name "*.cc" -o -name "*.m" -type f)
HDR_FILES=$(find include -name "*.h" -type f)
TEST_FILES=$(find test -name "*.c" -o -name "*.cc" -o -name "*.h" -type f)

for file in "$SRC_FILES $HDR_FILES $TEST_FILES";
do
  clang-format -i $file;
done
