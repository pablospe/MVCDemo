This is a demo implementation of some of the aspects in Farbman et. al. Coordinates for Instant Image Cloning SIGGRAPH 2009 paper.

This is an effort to make work this code on QT5.

#### Dependencies
Tested on Ubuntu 13.10:

    > sudo apt-get install libqt5opengl5-dev libcgal-dev

  * Note: `Qt4` can be used but compiling with `qmake` instead of `cmake`.
  * Note 2: for Windows, `CGal` has to be added in .pro by hand.

## How to compile with cmake?
    > mkdir build
    > cd build
    > cmake ..
    > make

## How to compile with qmake?
    > mkdir build
    > cd build
    > qmake ..
    > make

Or using QtCreator.

Note: 'README.txt.original' is the original README.txt provided by author.
