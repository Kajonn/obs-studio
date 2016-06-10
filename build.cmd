mkdir build64
cd build64
del CMakeCache.txt
cmake -DOBS_DEP_TYPE=win64 -DDEPS_PATH="C:\dev\github\kajonn\obs-studio\obs-dependencies2013" -DCOPY_DEPENDENCIES=TRUE -DQTDIR="C:/Qt/Qt5.6.0/5.6/msvc2013_64" -G "Visual Studio 12 Win64" ../
cd ..
mkdir build32
cd build32
del CMakeCache.txt
cmake -DOBS_DEP_TYPE=win32  -DDEPS_PATH="C:\dev\obs-dependencies2013" -DCOPY_DEPENDENCIES=TRUE -DQTDIR="C:/Qt/Qt5.6.0_32/5.6/msvc2013" -G "Visual Studio 12" ../
cd ..
cmake --build build64 --config Release --target PACKAGE
cmake --build build32 --config Release --target PACKAGE