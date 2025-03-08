# Container for a build environment compatible with the Steam Linux Runtime,
# plus our own additional dependencies.
FROM registry.gitlab.steamos.cloud/steamrt/sniper/sdk:3.0.20250210.116596
SHELL ["/bin/bash", "-c"]

# Install unzip for Ninja bootstrap.
# Install libfluidsynth-dev for SDL2_mixer build with MIDI support.
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y unzip libfluidsynth-dev clang-16

# Install patchelf for setting SDL2's RPATH to '$ORIGIN' so PNG loading works.
RUN wget -nv https://github.com/NixOS/patchelf/releases/download/0.14.5/patchelf-0.14.5-x86_64.tar.gz -O patchelf.tar.gz && \
	sha256sum -c <<<'514bb05d8f0e41ea0a6cb999041acb6aa386662e9ccdbdfbbfca469fb22d44fa *patchelf.tar.gz' && \
	tar xf patchelf.tar.gz -C /usr/local && \
	rm patchelf.tar.gz

# Skip install-deps.sh to avoid having to code it to actually work inside this.
ENV HSW_NO_INSTALL_DEPS=1 \
	CC=clang-16 \
	CXX=clang++-16

# Volume management is left to container runtime; see `./container`.
