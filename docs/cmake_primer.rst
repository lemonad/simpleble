.. _cmake-primer:

======================
CMake Fundamentals Primer
======================

This guide introduces the core concepts of CMake for beginners, focusing on the essential commands for configuring and building projects, the difference between building and installing, and how to use installation prefixes. It aims to help you understand CMake's workflow and apply it to any project.

What is CMake?
=============

CMake is a cross-platform tool that automates the process of building software. Instead of writing platform-specific build scripts (e.g., Makefiles or Visual Studio project files), you write a single ``CMakeLists.txt`` file. CMake uses this file to generate build files tailored to your system, making your project portable across Windows, Linux, MacOS, and more.

Prerequisites
============

- **CMake**: Version 3.21 or higher. Download and install from `cmake.org <https://cmake.org/>`_.
- A C++ compiler (e.g., GCC, Clang, MSVC) or another compiler supported by your project.
- A project with a ``CMakeLists.txt`` file (the configuration file for CMake).

Core CMake Workflow
==================

CMake operates in two main phases: **configuration** and **building**. A third optional phase, **installation**, makes the built software available for use. Let’s break these down.

1. Configuring the Project
-------------------------

Configuration is where CMake reads the ``CMakeLists.txt`` file and generates build files (e.g., Makefiles, Ninja files, or IDE project files) based on your system and preferences.

Run the following command from your project’s root directory::

    cmake -S <path-to-source> -B <your-build-directory>

- ``-S <path-to-source>``: Specifies the **source directory**, where the ``CMakeLists.txt`` file is located (e.g., ``/home/user/project``).
- ``-B <your-build-directory>``: Specifies the **build directory** (e.g., ``/home/user/project-build``), where CMake will store generated build files and temporary files.

**Why a separate build directory?** Keeping build files separate from source files avoids cluttering your project and makes it easy to delete build artifacts for a fresh start.

After running this command, CMake creates ``<your-build-directory>`` containing all the files needed to build your project.

2. Building the Project
----------------------

Building compiles the source code into executables, libraries, or other artifacts using the generated build files.

Run::

    cmake --build <your-build-directory> -j4

- ``--build <your-build-directory>``: Tells CMake to build the project in the specified directory.
- ``-j4``: Uses 4 parallel jobs to speed up compilation (adjust based on your CPU cores, e.g., ``-j8`` for 8 cores).

This produces the final output (e.g., ``.exe`` files, ``.a`` or ``.so`` libraries) in ``<your-build-directory>``. At this stage, the project is built but not yet installed.

**Key Point**: Building creates the software in the build directory, but it’s not yet integrated into your system or ready for use by other projects.

3. Installing the Project
------------------------

Installation copies the built files (executables, libraries, headers, etc.) to a location where they can be used by your system or other projects. This is distinct from building because it makes the software accessible outside the build directory.

Run::

    cmake --install <your-build-directory>

This installs files to a default location, typically:

- Linux/MacOS: ``/usr/local`` (e.g., binaries in ``/usr/local/bin``, libraries in ``/usr/local/lib``).
- Windows: ``C:\Program Files\<ProjectName>``.

**Why install?** Installation is necessary if:

- You want to run the program from anywhere on your system (e.g., via the command line).
- You’re building a library that other projects need to link against.
- You want to package the software for distribution.

Customizing the Installation Location with ``CMAKE_INSTALL_PREFIX``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can specify where to install the project using the ``CMAKE_INSTALL_PREFIX`` variable. This is useful for installing to non-standard locations or for testing without affecting system directories.

Set the prefix during installation::

    cmake --install <your-build-directory> --prefix <your-installation-directory>

Or, set it during configuration to make it persistent::

    cmake -S <path-to-source> -B <your-build-directory> -DCMAKE_INSTALL_PREFIX=<your-installation-directory>
    cmake --build <your-build-directory> -j4
    cmake --install <your-build-directory>

Example:

- ``<your-installation-directory>`` could be ``/home/user/my-install`` or ``C:\CustomInstall``.

**Why is a prefix needed?** The prefix defines the root directory for installation, ensuring files are organized predictably (e.g., binaries in ``<your-installation-directory>/bin``, libraries in ``<your-installation-directory>/lib``). Without a prefix, CMake uses a default that may require administrative privileges or conflict with system files.

**Note for Linux/MacOS**: Installing to system directories (e.g., ``/usr/local``) often requires ``sudo``::

    sudo cmake --install <your-build-directory>

Building Shared vs. Static Libraries
===================================

If your project includes libraries, you can choose between **shared** (dynamically linked) and **static** (statically linked) libraries using the ``BUILD_SHARED_LIBS`` variable.

- **Shared Library** (e.g., ``.so`` on Linux, ``.dll`` on Windows)::

    cmake -S <path-to-source> -B <your-build-directory> -DBUILD_SHARED_LIBS=TRUE
    cmake --build <your-build-directory> -j4

- **Static Library** (e.g., ``.a`` on Linux, ``.lib`` on Windows)::

    cmake -S <path-to-source> -B <your-build-directory> -DBUILD_SHARED_LIBS=FALSE
    cmake --build <your-build-directory> -j4

**What’s the difference?**

- **Shared libraries** are loaded at runtime, reducing executable size but requiring the library to be present on the system.
- **Static libraries** are embedded in the executable, increasing its size but making it self-contained.

If ``BUILD_SHARED_LIBS`` is not set, the project’s ``CMakeLists.txt`` determines the default behavior.

Passing Configuration Options
============================

CMake allows customization through variables set with the ``-D`` flag during configuration. These variables control project-specific settings defined in the ``CMakeLists.txt``. For example::

    cmake -S <path-to-source> -B <your-build-directory> -DMY_PROJECT_FEATURE=ON -DMY_LOG_LEVEL=DEBUG

- ``-DMY_PROJECT_FEATURE=ON``: Enables a project-specific feature (if supported).
- ``-DMY_LOG_LEVEL=DEBUG``: Sets a custom log level (if defined).

Check your project’s documentation for available variables. Common ones include:

- ``CMAKE_BUILD_TYPE``: Sets the build type (e.g., ``Debug``, ``Release``)::

    cmake -S <path-to-source> -B <your-build-directory> -DCMAKE_BUILD_TYPE=Release

- ``CMAKE_CXX_STANDARD``: Sets the C++ standard (e.g., ``17`` for C++17)::

    cmake -S <path-to-source> -B <your-build-directory> -DCMAKE_CXX_STANDARD=17

Troubleshooting Tips
===================

- **Verbose Output**: To see detailed build steps, add ``--verbose``::

    cmake --build <your-build-directory> --verbose

- **Clean Build**: To start fresh, delete ``<your-build-directory>`` and re-run configuration::

    rm -rf <your-build-directory>
    cmake -S <path-to-source> -B <your-build-directory>

- **Reconfiguration**: If you change options, re-run the configuration command to update the build files.
- **Check CMake Cache**: The ``<your-build-directory>/CMakeCache.txt`` file stores configuration settings. If issues persist, delete it or the entire build directory.