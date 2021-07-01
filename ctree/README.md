# ctree 
[![Build Status](https://travis-ci.com/CHr15F0x/ctree.svg?branch=master)](https://travis-ci.com/CHr15F0x/ctree) [![codecov](https://codecov.io/gh/CHr15F0x/ctree/branch/master/graph/badge.svg)](https://codecov.io/gh/CHr15F0x/ctree)

### To clone
```
git clone https://github.com/CHr15F0x/ctree
cd ctree
[ctree/] git submodule update --init --recursive
```

### Dependencies
```
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-gfx-dev qtbase5-dev
```

### To build and run tests
```
[ctree/] mkdir build && cd build
[ctree/build] cmake .. && make -j && make test
```

### Verbose test output
```
[ctree/build] make ARGS=-V test
```

### To run render examples
```
[ctree/build] ./qt-render-ctree/qt-render-ctree
[ctree/build] ./sdl-render-ctree/sdl-render-ctree
```

### Platforms
So far tried the following:
- Ubuntu 14.04 && (gcc 4.8.4 || clang 5.0.0)
- Ubuntu 16.04 && (gcc 5.4.0 || clang 3.8.0)
