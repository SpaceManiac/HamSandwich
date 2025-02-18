# Start with Alpine because it's small and works.
FROM alpine:3.19.1
RUN apk add --no-cache bash
SHELL ["/bin/bash", "-c"]

# https://jake-shadle.github.io/xwin
# Run xwin first because it takes a long time.
# Manifest pinned for reproducibility.
COPY manifest_17.json manifest.json
RUN set -euo pipefail; \
	xwin_version="0.5.0"; \
	xwin_prefix="xwin-$xwin_version-x86_64-unknown-linux-musl"; \
	wget -nv "https://github.com/Jake-Shadle/xwin/releases/download/$xwin_version/$xwin_prefix.tar.gz" -O xwin.tar.gz; \
	sha256sum -c <<<'a9b9d2996d0e1050ac837baaec5b05eb42ad9815630f66abf0e5382055e04378 *xwin.tar.gz'; \
	tar xf xwin.tar.gz -C /usr/local/bin --strip-components=1 "$xwin_prefix/xwin"; \
	xwin --manifest manifest.json --arch x86 --accept-license splat --output /xwin --include-debug-libs; \
	rm -rf .xwin-cache /usr/local/bin/xwin xwin.tar.gz manifest.json

# Add the compiler toolchain.
# Use real ninja because samurai doesn't support deps=msvc. no-scripts to suppress the warning.
RUN apk add --no-cache --no-scripts ninja-is-really-ninja
# Clang to cross-compile targeting MSVC ABI.
# llvm for llvm-rc and llvm-mt, lld for lld-link
RUN apk add --no-cache cmake llvm lld clang
# host_tools requirements
RUN apk add --no-cache zlib-dev

# Let the ./container script automatically set the right toolchain.
COPY toolchain.cmake /toolchain.cmake
COPY wrap-mt /usr/local/bin/wrap-mt
ENV CMAKE_TOOLCHAIN_FILE=/toolchain.cmake
