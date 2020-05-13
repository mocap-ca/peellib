mkdir debug
cd debug
cmake -H.. -B. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Debug
nmake
nmake install
cd ..
mkdir release
cd release
cmake -H.. -B. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
nmake
nmake install
