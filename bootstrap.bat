@echo off
mkdir build && cd build
cmake -A x64 -Dgtest_force_shared_crt=ON ..
