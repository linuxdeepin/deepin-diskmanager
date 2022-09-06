# diskManager
Disk Utility is a useful tool to help you better manage your disks through disk partitioning, disk data administration, and health management. 
磁盘管理器是一款管理磁盘的工具，可帮助用户进行磁盘分区管理、磁盘数据管理及磁盘健康管理。

## Dependencies
* smartmontools
* exfat-fuse
* Qt5 (>= 5.3)
* Qt5-X11extras

### Build dependencies

* cmake
* qt5
* dtk


## Installation
sudo apt-get install deepin-diskmanager

### Build from source code

1. Make sure you have installed all dependencies.

2. Build:
```
$ cd deepin-diskmanager
$ mkdir Build
$ cd Build
$ qmake ..
$ make
```

3. Install:
```
$ sudo make install
```

When install complete, the executable binary file is placed into `/usr/bin/deepin-diskmanager`.

## Getting help

Any usage issues can ask for help via
* [Gitter](https://gitter.im/orgs/linuxdeepin/rooms)
* [IRC channel](https://webchat.freenode.net/?channels=deepin)
* [WiKi](https://wiki.deepin.org)
* [官方论坛](https://bbs.deepin.org)
* [开发者中心](https://github.com/linuxdeepin/dde-session-shell)

## Getting involved

We encourage you to report issues and contribute changes

* [开发者代码贡献指南](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers)


## License
This project is licensed under GPLv3.
