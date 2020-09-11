# Hooker component

Hooker leverages eBPF to transparently and safely redirect TCP to another Transport protocols.

## Requirements
* Linux version 5.0
* Clang version >= 6.0

      $  wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
      $  sudo apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-6.0 main"
      $  sudo apt-get update
      $  sudo apt-get install -y clang-6.0
      $  sudo update-alternatives --install /usr/bin/clang clang /usr/bin/clang-6.0 100 
      $  sudo update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-6.0 100 
      $  sudo update-alternatives --install /usr/bin/llc llc /usr/bin/llc-6.0 100

## Build and Execute

      $  sudo make 
      $  sudo ./hooker
