dir lib\Catch2
cmake -Bbuild -H. -DBUILD_TESTING=OFF
cmake --build build/ --target install