# See ../triqs/packaging for other options
FROM flatironinstitute/triqs:unstable-ubuntu-clang
ARG APPNAME=triqs_modest

# Install here missing dependencies, e.g.
RUN apt-get update && apt-get install -y \
    clang-19 \
    llvm-19-dev \
    libclang-19-dev \
    libomp-19-dev \
    libzstd-dev \
    doxygen

COPY --chown=build . $SRC/$APPNAME
RUN mkdir $BUILD/$APPNAME && chown build $BUILD/$APPNAME

RUN git clone https://github.com/flatironinstitute/clair --branch unstable $SRC/clair && \
    mkdir $BUILD/clair && \
    CXX="clang++-19" CXXFLAGS="" CPLUS_INCLUDE_PATH="" cmake -S $SRC/clair -B $BUILD/clair -DBuild_Tests=OFF -DCMAKE_INSTALL_PREFIX=$INSTALL && \
    cmake --build $BUILD/clair && cmake --install $BUILD/clair

ENV OMP_NUM_THREADS=1

ARG BUILD_ID
ARG CMAKE_ARGS
USER build
WORKDIR $BUILD/$APPNAME
RUN cmake $SRC/$APPNAME -DTRIQS_ROOT=${INSTALL} $CMAKE_ARGS && make -j4 || make -j1 VERBOSE=1
USER root
RUN make install
