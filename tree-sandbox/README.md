# tree-sandbox [![Build Status](https://travis-ci.com/CHr15F0x/tree-sandbox.svg?token=hWa4W6EnvgUqM3Wa4JzH&branch=master)](https://travis-ci.com/CHr15F0x/tree-sandbox) [![codecov](https://codecov.io/gh/CHr15F0x/tree-sandbox/branch/master/graph/badge.svg)](https://codecov.io/gh/CHr15F0x/tree-sandbox)

### To build and run tests
```
[tree-sandbox/] mkdir build && cd build
[tree-sandbox/build] cmake .. && make -j && make test
```

### Verbose test output
```
[tree-sandbox/build] make ARGS=-V test
```

### To run render examples
```
[tree-sandbox/build] ./qt-render-tree/qt-render-tree
[tree-sandbox/build] ./qt-render-ctree/qt-render-ctree
```

### Platforms
So far tried the following:
- Ubuntu 14.04 && (gcc 4.8.4 || clang 5.0.0)
- Ubuntu 16.04 && (gcc 5.4.0 || clang 3.8.0)
