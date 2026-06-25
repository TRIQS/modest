# See ../triqs/packaging for other options
FROM flatironinstitute/triqs:unstable-ubuntu-clang

# git-lfs is required when Enable_LFS_Tests=ON pulls reference data in test/c++/ref_data_lfs
RUN apt-get update && apt-get install -y --no-install-recommends git-lfs && rm -rf /var/lib/apt/lists/*
