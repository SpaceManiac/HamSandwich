# Container for a build environment compatible with the Steam Linux Runtime,
# plus our own additional dependencies.
FROM registry.gitlab.steamos.cloud/steamrt/sniper/sdk:3.0.20260119.200241
SHELL ["/bin/bash", "-c"]

# Install unzip for Ninja bootstrap.
# Install libfluidsynth-dev for SDL2_mixer build with MIDI support.
# Install patchelf for `install_as_soname`.
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y unzip libfluidsynth-dev clang-19 patchelf

# Skip install-deps.sh to avoid having to code it to actually work inside this.
ENV HSW_NO_INSTALL_DEPS=1 \
	CC=clang-19 \
	CXX=clang++-19

# Volume management is left to container runtime; see `./container`.
