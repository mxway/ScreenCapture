# 1.Goal

Our goal is to provide a screen capture tool,the tool should be os independent.

# 2.Build

We use cmake for managing source code and build process.The minimum version required by cmake is 3.11,theoretically,
the later version of cmake should be OK! Besides cmake,you should also install native c/c++ compiler,for windows we support msvc or g++,for linux we support g++ or clang++.The C++ compiler needs to support c++11.

First of all,you should obtain a copy of source code from github,you can clone source by execute

```shell
git clone https://github.com/mxway/ScreenCapture.git
```

Or

```shell
git clone git@github.com:mxway/ScreenCapture.git
```

## 2.1 Compile on linux

First of all,we should install some dependency libraries.We use Xlib to create Window,XRender and Xft for rendering text and image.Besides,when we open file selection dialog,we use magic for recognizing the file's mime type,the magic's source code is included in the project's subdirectory.Magic needs lzma and zlib to recognize real file type.We should install the depedency dev libraries.

**On Ubuntu**

```shell
sudo apt install libX11-dev libxft-dev libxrender-dev libfreetype6-dev liblzma-dev zlib1g-dev
```

**On Centos**

```shell
yum install libX11-devel libXft-devel libXrender-devel freetype-devel xz-devel xz-libs libzstd-devel
```

### 2.1.1 g++

Change current working directory to the root directory of source and execute cmake command.

```shell
cd ScreenCapture && mkdir build && cd build && cmake ../ -DCMAKE_BUILD_TYPE=Release && make -j2
```

Since g++ is the default compiler of my system.The "make" command will use g++ to compile my source code.

### 2.1.2 clang++

If you want to use Clang for compiling source code,execute the following shell script.Make sure Clang has been installed on your system.

```shell
cd ScreenCapture && mkdir build && cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++ && make -j8
```

## 2.2 Compile on Window

### 2.2.1 msvc

I have already installed Visual Studio Community 2022

**NMake**

You could use CMake to generate NMakeFiles,use nmake command to compile the source code.

Click "start menu"==>"Visual Studio 2022"==>"x64 Native Tools Command Prompt for VS 2022".Change working directory to the root of source code.

```shell
cd D:\code\ScreenCapture
mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles"
nmake
```

After compiliation is completed,you should find ScreenCapture.exe in the build directory.

**Visual Studio IDE**

You could use CMake to generate ".sln" project file,and use Visual Studio IDE to compile   the source code.

```shell
cd D:\code\ScreenCapture
mkdir build
cd build
cmake ../
```

After CMake command is executed.You would find "ScreenCapture.sln" file in the build directory,double click the "ScreenCapture.sln" file,then you will open Visual Studio IDE，you can use the IDE to build the ScreenCapture project.

### 2.2.2 g++

To make sure the gcc compiler has been installed on your system.Open the cmd window,and execute the following command

```shell
gcc --version
```

If gcc has beed installed,and the PATH environment variable has beed set properly,you could see some information like below.

```shell
g++ (i686-posix-dwarf-rev0, Built by MinGW-W64 project) 7.3.0
Copyright (C) 2017 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

**Generate Makefile**

To generate Makefile that using g++ as Compiler，you should execute the following command

```shell
cd ScreenCapture
mkdir build
cd build
cmake ../ -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles"
```

**Build**

After generating the Makefile that using g++ as compiler,you could use mingw32-make command to generate executable file.

```shell
mingw32-make
```

After the mingw32-make command is executed,you could find "ScreenCapture.exe" file in the build directory.

# 3.open source software

The open source software and its license we use are as follows

| Name    | License              | URL                                  | Description                                                                                    |
| ------- | -------------------- | ------------------------------------ | ---------------------------------------------------------------------------------------------- |
| stb     | MIT or Public Domain | https://github.com/nothings/stb      | Loading png icon for File selectiondialog on Linux.Saving the screenshot as bmp\|png\|jpg File |
| file    | BSD?                 | https://github.com/file/file         | Recognizing the file's real type,then loading icon for File selection dialog                   |
| lunasvg | MIT                  | https://github.com/sammycage/lunasvg | We develop a file selection dialog box using xlib.The library is used to load svg picture.     |
