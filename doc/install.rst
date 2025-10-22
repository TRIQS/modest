.. highlight:: bash

.. _install:

Install modest
**************

Compiling modest from source
============================

.. note:: To guarantee reproducibility in scientific calculations we strongly recommend the use of a stable `release <https://github.com/TRIQS/triqs/releases>`_ of both TRIQS and its applications.

Prerequisites
-------------

#. The :ref:`TRIQS <triqslibs:welcome>` library, see :ref:`TRIQS installation instruction <triqslibs:triqs_install>`.
   In the following, we assume that TRIQS is installed in the directory ``path_to_triqs``.

Installation steps
------------------

#. Download the source code of the latest stable version by cloning the ``TRIQS/modest`` repository from GitHub::

     $ git clone https://github.com/TRIQS/modest modest.src

#. Create and move to a new directory where you will compile the code::

     $ mkdir modest.build && cd modest.build

#. Ensure that your shell contains the TRIQS environment variables by sourcing the ``triqsvars.sh`` file from your TRIQS installation::

     $ source path_to_triqs/share/triqs/triqsvars.sh

#. In the build directory call cmake, including any additional custom CMake options, see below::

     $ cmake ../modest.src

#. Compile the code, run the tests and install the application::

     $ make
     $ make test
     $ make install

Version compatibility
---------------------

Keep in mind that the version of ``modest`` must be compatible with your TRIQS library version,
see :ref:`TRIQS website <triqslibs:versions>`.
In particular the Major and Minor Version numbers have to be the same.
To use a particular version, go into the directory with the sources, and look at all available versions::

     $ cd modest.src && git tag

Checkout the version of the code that you want::

     $ git checkout 2.1.0

and follow steps 2 to 4 above to compile the code.

Custom CMake options
--------------------

The compilation of ``modest`` can be configured using CMake-options::

    cmake ../modest.src -DOPTION1=value1 -DOPTION2=value2 ...

+-----------------------------------------------------------------+-----------------------------------------------+
| Options                                                         | Syntax                                        |
+=================================================================+===============================================+
| Specify an installation path other than path_to_triqs           | -DCMAKE_INSTALL_PREFIX=path_to_modest         |
+-----------------------------------------------------------------+-----------------------------------------------+
| Build in Debugging Mode                                         | -DCMAKE_BUILD_TYPE=Debug                      |
+-----------------------------------------------------------------+-----------------------------------------------+
| Disable testing (not recommended)                               | -DBuild_Tests=OFF                             |
+-----------------------------------------------------------------+-----------------------------------------------+
| Build the documentation                                         | -DBuild_Documentation=ON                      |
+-----------------------------------------------------------------+-----------------------------------------------+
| Enable tests requiring Git LFS data files (~68MB)               | -DEnable_LFS_Tests=ON                         |
+-----------------------------------------------------------------+-----------------------------------------------+

Git LFS Tests (Optional)
-------------------------

Some tests require large data files (~68MB) stored with Git Large File Storage (LFS). These include:

* C++ tests using reference data in ``test/c++/ref_data_lfs/``
* Wannier90 converter tests in ``test/python/dft_tools/wannier90/``

These tests are **disabled by default**. To enable them:

#. Install Git LFS (one-time setup)::

     # On Ubuntu/Debian
     $ sudo apt install git-lfs

     # On macOS
     $ brew install git-lfs

     # Or download from https://git-lfs.github.com/

#. Initialize Git LFS and pull test data::

     $ cd modest.src
     $ git lfs install
     $ git lfs pull

#. Enable LFS tests in CMake configuration::

     $ cmake ../modest.src -DEnable_LFS_Tests=ON

.. note:: If you enable ``-DEnable_LFS_Tests=ON`` without completing steps 1-2, CMake will fail with clear instructions on what's missing.
