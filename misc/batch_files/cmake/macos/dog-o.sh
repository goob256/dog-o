#!/bin/sh
DEP=$1
shift
cmake .. -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.14.sdk -DCMAKE_BUILD_TYPE=Release -DUSER_INCLUDE_PATH="/Users/trent/code/shim2/external/SDL2_ttf-2.0.14;/Users/trent/code/libs/mac/include/SDL2;/Users/trent/code/libs/mac/include;/Users/trent/code/tgui4/include;/Users/trent/code/shim2/include;/Users/trent/code/wedge2/include" -DUSER_LIBRARY_PATH="/Users/trent/code/libs/mac/lib;/Users/trent/code/tgui4/build;/Users/trent/code/shim2/build;/Users/trent/code/wedge2/build" -DCMAKE_OSX_DEPLOYMENT_TARGET=$DEP -DCMAKE_MODULE_PATH=/Users/trent/code/dog-o/tools/batch_files/cmake/macos $*
