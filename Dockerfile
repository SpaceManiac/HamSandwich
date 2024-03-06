# Container for a build environment compatible with the Steam Linux Runtime,
# plus our own additional dependencies.
FROM registry.gitlab.steamos.cloud/steamrt/scout/sdk:0.20220601.1
# NB: Newer SDK versions than this start giving SDL2_mixer a hard dependency
# on libFLAC.so.8, rather than only loading it when a FLAC is accessed.
# Itch builds would then be forced to either include unnecessary .so files or
# build SDL2_mixer from source to work correctly.
# steamrt/soldier spreads the problem to SDL2_image, but it's also not a freely
# available option for Steam deployment so it's irrelevant for now.
SHELL ["/bin/bash", "-c"]

# Install ImageMagick for icon conversion.
# Install unzip for Ninja bootstrap.
RUN apt-get install -y imagemagick unzip

# Install a newer version of CMake from their binary releases.
RUN wget https://github.com/Kitware/CMake/releases/download/v3.21.2/cmake-3.21.2-linux-x86_64.tar.gz -O cmake.tar.gz && \
	sha256sum -c <<<'d5517d949eaa8f10a149ca250e811e1473ee3f6f10935f1f69596a1e184eafc1 *cmake.tar.gz' && \
	tar xf cmake.tar.gz -C /usr/local --strip-components=1 && \
	rm cmake.tar.gz

# Install patchelf for setting SDL2's RPATH to '$ORIGIN' so PNG loading works.
RUN wget https://github.com/NixOS/patchelf/releases/download/0.14.5/patchelf-0.14.5-x86_64.tar.gz -O patchelf.tar.gz && \
	sha256sum -c <<<'514bb05d8f0e41ea0a6cb999041acb6aa386662e9ccdbdfbbfca469fb22d44fa *patchelf.tar.gz' && \
	tar xf patchelf.tar.gz -C /usr/local && \
	rm patchelf.tar.gz

# Skip install-deps.sh to avoid having to code it to actually work inside this.
ENV HSW_NO_INSTALL_DEPS=1 \
	CC=gcc-9 \
	CXX=g++-9

# Volume management is left to container runtime; see `./container`.
