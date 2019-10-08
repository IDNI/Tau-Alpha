# Tau-Alpha Installation instructions

# Requirements

  * [boost](https://www.boost.org/)

# Building

## 1. Create a build directory

   The recommended way to build Alpha is in a separate build
   directory, for example within the top level of the Alpha package:

    $ mkdir build
    $ cd build

## 2. Configure

    $ cmake ../

   If everything is OK, then this should end with something like:
```
  -- Generating done
  -- Build files have been written to: /home/user/project/Alpha/build
```

   If CMake fails, because it cannot resolve all dependencies, then you
   may help CMake by setting some variables to help CMake locate the
   libraries. This may be done on the command-line using -Dvar=value or
   using the interactive program:

    $ ccmake ../

   or

    $ cmake-gui ../

   The GUI lists all variables that are configurable in Alpha's build
   process.

   The variables specify several build and configuration aspects of Alpha, of
   which the most relevant ones are (there are many more visible in the
   GUI):

   * ENABLE_FRONTEND_EXECUTION=1 - enable run in frontend (default: ON=enabled)
   * DISABLE_BACKEND_EXECUTION=1 - disable run in backend (default: OFF=enabled)

## 3. Building

   To build `alpha` executable run:

    $ make

   If you want to speed up compilation, you may want to use multiple
   threads (e.g. 4):
    $ make -j4

## 4. Install (as user with sufficient permissions):

   // TODO

# Helper build scripts

  | script | description |
  | --- | --- |
  | ./release.sh [<CMAKE_OPTS>] | builds release in build-Release   |
  | ./debug.sh [<CMAKE_OPTS>] | builds debug version in build-Debug |
  | ./build.sh <BUILD_TYPE> [<CMAKE_OPTS>] | builds version according to BUILD_TYPE' can be Debug or Release |
  | ./clean.sh | removes build-* directories |
  | ./devel.sh | runs alpha (Debug)   |
  | ./alpha.sh | runs alpha (Release) |
  | ./alpha-docker.sh | runs alpha (Release) in docker |
