#  iFixit FixHub Iron Programmer
Standalone, offline programmer for the iFixIt FixHub Soldering Iron.

This programmer provides a live-view of your soldering iron and allows you to make configuration changes to it; similar to the official WebSerial interface.   The programmer can be used compoletely offline and without any browser installed.

##  Installing

TODO

---

##  Development
I use 'make' to build this.  I tend to provide 3 build targets:  'linux', 'win', 'arm-linux'.  My dev machine is Arch Linux, so the package names will be the ones on the Arch servers.

The programme should compile fine in native linux (make init && make run-linux) with up-to-date tooling.  Cross-compilation is something I struggled to get working some time ago and the way I got it reliable for me and my colleagues was a bit convoluted.  I created a native installation with the prerequisites on 'aarch64' and through MSYS2, the just zipped up the '/usr' folder of each in a subdirectory of '/aarch64' and '/mingw64' respectively.  We the just un-zipped them to our root directories and chowned them to our dev-usernames.  I haven't created these in my own time yet and I'm sure most developers won't want to trust two arbitrary 4GB ZIP files from me, so for now I'll just be uploading the binaries as tagged releases.

If someone wants to help me tool up a less silly way of doing this, I'd be grateful.

###  Prerequisites
gcc, pkg-config, gtk4
Optional:  git, make
Cross:  x86_64-w64-mingw32-gcc aarch64-linux-gnu-gcc

###  Cloning
git clone URL
cd ifixit-fixhub-iron-programmer
make init

###  Building
make \<target\>

###  Building & Running
make run-\<target\>

###  Updating
make update
