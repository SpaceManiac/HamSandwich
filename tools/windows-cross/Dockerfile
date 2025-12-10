# Start with Debian Slim. We need glibc binary compat to run itch.io's butler.
FROM debian:13.2-slim
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
	--mount=type=cache,target=/var/lib/apt,sharing=locked \
	apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y \
	wget ca-certificates

# https://jake-shadle.github.io/xwin
# Run xwin first because it takes a long time.
# Manifest pinned for reproducibility.
COPY manifest_17.json manifest.json
RUN set -euo pipefail; \
	xwin_version="0.5.0"; \
	xwin_prefix="xwin-$xwin_version-x86_64-unknown-linux-musl"; \
	wget -nv "https://github.com/Jake-Shadle/xwin/releases/download/$xwin_version/$xwin_prefix.tar.gz" -O xwin.tar.gz; \
	echo 'a9b9d2996d0e1050ac837baaec5b05eb42ad9815630f66abf0e5382055e04378 *xwin.tar.gz' | sha256sum -c -; \
	tar xf xwin.tar.gz -C /usr/local/bin --strip-components=1 "$xwin_prefix/xwin"; \
	xwin --manifest manifest.json --arch x86 --accept-license splat --output /xwin --include-debug-libs; \
	rm -rf .xwin-cache /usr/local/bin/xwin xwin.tar.gz manifest.json

# Add the compiler toolchain.
# clang to cross-compile targeting MSVC ABI. clang-tools for clang-cl
# llvm for llvm-rc and llvm-mt, lld for lld-link
# zlib1g-dev for host_tools
# make for dev convenience.
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
	--mount=type=cache,target=/var/lib/apt,sharing=locked \
	apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y \
	make cmake ninja-build \
	clang clang-tools lld llvm \
	zlib1g-dev

# Let the ./container script automatically set the right toolchain.
COPY toolchain.cmake /toolchain.cmake
COPY wrap-mt /usr/local/bin/wrap-mt
ENV CMAKE_TOOLCHAIN_FILE=/toolchain.cmake \
	HSW_NO_INSTALL_DEPS=1 \
	CMAKE=cmake \
	os=windows
