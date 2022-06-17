## Deepin-DiskManager

Deepin-DiskManager  Is A Disk Management Tool For Disk.
View, modify and configure disks and media.

### Dependencies
### Build dependencies

_The **master** branch is current development branch, build dependencies may changes without update README.md, refer to `./debian/control` for a working build depends list_


* gcc-c++
* cmake
* dtkwidget-devel
* qt5-qttools-devel
* qt5-qtbase-devel
* qt5-qttools-devel
* qt5-qtbase-private-devel
* parted-devel
* dde-qt-dbus-factory-devel
* polkit-qt5-1-devel
* smartmontools


### Installation

### Build from source code

1. Make sure you have installed all dependencies.

_Package name may be different between distros, if deepin-music is available from your distro, check the packaging script delivered from your distro is a better idea._

Assume you are using [Deepin](https://distrowatch.com/table.php?distribution=deepin) or other debian-based distro which got deepin-music delivered:

``` shell
$ apt build-dep deepin-diskManager
```

2. Build:

```
$ cd deepin-diskManager
$ mkdir Build
$ cd Build
$ cmake ..
$ make
```

3. Install:

```
$ sudo make install
```

The executable binary file could be found at `/usr/bin/deepin-diskManager`

### Usage

Execute `deepin-diskManager`

### Documentations

 - [Development Documentation](https://linuxdeepin.github.io/)
 - [User Documentation](https://wikidev.uniontech.com/index.php?title=%E9%9F%B3%E4%B9%90) | [用户文档](https://wikidev.uniontech.com/index.php?title=音乐)

### Getting help

 - [Official Forum](https://bbs.deepin.org/)
 - [Developer Center](https://github.com/linuxdeepin/developer-center)
 - [Gitter](https://gitter.im/orgs/linuxdeepin/rooms)
 - [IRC Channel](https://webchat.freenode.net/?channels=deepin)
 - [Wiki](https://wiki.deepin.org/)

### Getting involved

We encourage you to report issues and contribute changes

 - [Contribution guide for developers](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers-en) (English)
 - [开发者代码贡献指南](https://github.com/linuxdeepin/developer-center/wiki/Contribution-Guidelines-for-Developers) (中文)

### License

This project is licensed under [GPLv3](LICENSE) or any later version.