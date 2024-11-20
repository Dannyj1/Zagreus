FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
ENV CC=clang
ENV CXX=clang++
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
     && apt install lsb-release wget software-properties-common gnupg -y \
     && bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"  \
     && update-alternatives --install /usr/bin/cc cc /usr/bin/clang 100 \
     && update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 100
