#!/bin/sh
DEP=$1
shift
cmake .. -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk -DCMAKE_BUILD_TYPE=Release -DUSER_INCLUDE_PATH="/Users/trent/code/libs/mac/include/SDL2;/Users/trent/code/libs/mac/include;/Users/trent/code/tgui4/include" -DUSER_LIBRARY_PATH="/Users/trent/code/libs/mac/lib;/Users/trent/code/tgui4/build" -DCMAKE_OSX_DEPLOYMENT_TARGET=$DEP -DCMAKE_MODULE_PATH=/Users/trent/code/dog-o/tools/batch_files/cmake/macos -DSHARED=off $*
