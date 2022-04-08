Virtual Transport Layer (VTL)
===
Virtual Transport Layer, a protocol deployment management system aiming
(1) to dynamically deploy protocol mechanisms/components, in order
(2) to ensure optimal data moving between end-points.

I am spending time bettering the implementation. Until now the focus has been
on the correctness rather than performance. I will keep correctness, but I will
improve performance, especially in light of practical experience with
applications. Suggestions are welcome.

![](files/vtl.png)

Notice
---
VTL and its components have been tested on Ubuntu 20.04 LTS.

Step 1: Compile and install VTL's customized linux kernel
---

``` shell
git clone https://github.com/elfadel/vtl_kern.git linux-vtl
cd linux-vtl
cp -v /boot/config-$(uname -r) .config
make olddefconfig
scripts/config --disable SYSTEM_TRUSTED_KEYS
make -j $(nproc)
sudo make INSTALL_MOD_STRIP=1 modules_install
sudo make headers_install
sudo make install
```
Reboot, and make sure to boot on linux v5.3.5+.

Step 2: Configure eBPF/XDP environment
---

``` shell
sudo bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
LLVM_VERSION=14 # match the actual version of the clang installed on the system
sudo update-alternatives --install /usr/bin/clang clang "/usr/bin/clang-$LLVM_VERSION" 100
sudo update-alternatives --install /usr/bin/clang++ clang++ "/usr/bin/clang++-$LLVM_VERSION" 100
sudo update-alternatives --install /usr/bin/llc llc "/usr/bin/llc-$LLVM_VERSION" 100
sudo update-alternatives --install /usr/bin/llvm-mc llvm-mc "/usr/bin/llvm-mc-$LLVM_VERSION" 50
```

Step 3: Compile and build VTL
---

``` shell
git clone --recurse-submodules https://github.com/elfadel/vtl.git
sudo apt install libpcap-dev gcc-multilib

# configure nDPI deps
cd deps/nDPI
sudo apt ins	tall -y autoconf libtool libjson-c-dev
./autogen.sh
./configure
make
cd ../..

# compile VTL
mkdir bin
make
```

Test and run  Hooker:
---
\> Hooker contains two programs:
* The daemon in /hooker folder, that is created with a make call. It is _user prog_ part of Hooker.
* The _kernel prog_ part that contains mainly two eBPF progs: _**sock_ops**_, to monitor specific 
or a set of TCP applications, and _**sk_msg**_, to redirect the monitored applications' data.

\> First: build and launch (deploy) kernel prog part:
```
cd bin
sudo ./vtl_ui

# Follow the instructions and make choices.
```

\> Second: start user daemon
```
$ cd hooker
$ make 
$ sudo ./hk-daemon <IPPROTO> <HOST_MODE>
```

*IPPROTO:* this argument is an integer and is used to indicate which protocol should be used to replace TCP. 
	Indeed, VTL allows the user to manually fix the substitution protocol by specifying one of the supported IP 
	protocol (list below) or let this task to VTL by specified 253 (default) as the argument value.

List of substitution protocol supported by VTL and their corresponding IP number:
Protocol |    IP number [[IANA](https://bit.ly/3nehetj)]   
:---------: | :---------------------------------------------:
UDP    |             17            
UDPLite  |             136           
DCCP   |             33            
SCTP   |             132           
QUIC   | 142 (conventional in VTL) 
VTL    | 253 (conventional in VTL) 

*HOST_MODE:* a string to indicate if the hooker daemon will hook either a TCP client or a TCP server.


\> Finally, just run your TCP applications.