rm -rf build
mkdir build
cd build
# Windows: Install CYGWIN with GCC, Make and CMAKE.
#This worked on windows, we might need to modify the linux version
#cmake .. -G"Unix Makefiles" -DCMAKE_CONFIGURATION_TYPES="Debug;Release"
cmake .. -G"Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug
make VERBOSE=1
