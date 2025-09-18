
if [ -d "cmake" ]; then
    rm -rf "cmake"
fi

mkdir -p cmake

cp CMakeLists.txt cmake/
cp include/* cmake/
cp src/* cmake/
cp protos/* cmake/


cd cmake


if [ -d "build" ]; then
    rm -rf "build"
fi

mkdir -p build

cd build

cmake .. -DCMAKE_PREFIX_PATH="/opt/homebrew/opt/qt"

make -j4
cd ..
cd ..
