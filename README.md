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

First of all,we should install some dependency libraries.We use Xlib to create Window,XRender and Xft for rendering text and image.Besides,when we open file selection dialog,we use magic for recognizing the file's mime type,the magic's source code is included in the project's subdirectory.Magic needs lzma and zlib to recognize real file type.We should install the depedency dev libraries.

**On Ubuntu**

```shell
sudo apt install libX11-dev libxft-dev libxrender-dev libfreetype6-dev liblzma-dev zlib1g-dev
```

**On Centos**

```shell
yum install libX11-devel libXft-devel libXrender-devel freetype-devel xz-devel xz-libs libzstd-devel
```

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

# 3.open source software

The open source software and its license we use are as follows

| Name    | License              | URL                                  | Description                                                                                  |
| ------- | -------------------- | ------------------------------------ | -------------------------------------------------------------------------------------------- |
| stb     | MIT or Public Domain | https://github.com/nothings/stb      | Loading png icon for File choose dialog on Linux.Saving the screenshot as bmp\|png\|jpg File |
| file    | BSD?                 | https://github.com/file/file         | Recognizing the file's real type,then loading icon for File choose dialog                    |
| lunasvg | MIT                  | https://github.com/sammycage/lunasvg | We develop a file selection dialog box using xlib.The library is used to load svg picture.   |
