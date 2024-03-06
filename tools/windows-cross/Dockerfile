# Start with Alpine because it's small and works.
FROM alpine:3.19.1
# Add the compiler toolchain.
# Use LLD because LD mysteriously crashes when linking this project.
RUN apk add --no-cache bash cmake ninja lld i686-mingw-w64-gcc
# host_tools requirements
RUN apk add --no-cache gcompat g++ zlib-dev
# Without this, GCC can't find lld while cross-compiling.
RUN ln -s /usr/bin/ld.lld /usr/i686-w64-mingw32/bin/ld.lld
# Let the ./container script automatically set the right toolchain.
ENV CMAKE_TOOLCHAIN_FILE=tools/windows-cross/mingw32-toolchain.cmake
ENV PATH=/usr/i686-w64-mingw32/bin:$PATH
