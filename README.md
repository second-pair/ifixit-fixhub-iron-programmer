#  iFixit FixHub Iron Programmer
Standalone, offline programmer for the iFixIt FixHub Soldering Iron.

This programmer provides a live-view of your soldering iron and allows you to make configuration changes to it; similar to the official WebSerial interface.   The programmer can be used compoletely offline and without any browser installed.

##  Installing

TODO

---

##  Development
I use 'make' to build this.  I tend to provide 3 build targets:  'linux', 'win', 'arm-linux'.

###  Prerequisites
gcc, pkg-config, gtk4
Optional:  git, make

###  Cloning
git clone URL
cd ifixit-fixhub-iron-programmer
make init

###  Building
make <target>

###  Running
make run-<target>

###  Updating
make update
