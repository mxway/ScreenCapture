# 1.Goal

Our goal is to provide a screen capture tool,the tool should be os independent.

# 2.Build

We use cmake for managing source code and build process.The minimum version required by cmake is 3.11,theoretically,
the later version of cmake should be OK! Besides cmake,you should also install native c/c++ compiler,for windows we
support msvc or gcc,for linux we support gcc or clang.

First of all,you should obtain a copy of source code from github,you can clone source by execute

```shell
git clone https://github.com/mxway/ScreenCapture.git
```

Or

```shell
git clone git@github.com:mxway/ScreenCapture.git
```

## 2.1 Compile on linux

### 2.1.1 gcc

Change current working directory to the root directory of source and execute cmake command.

```shell
cd ScreenCapture && mkdir build && cd build && cmake ../ -DCMAKE_BUILD_TYPE=Release && make -j2
```

Since gcc is the default compiler of my system.The "make" command will use gcc to compile my source code.

### 2.1.2 clang

## 2.2 Compile on Window

### 2.2.1 msvc

### 2.2.2 gcc
