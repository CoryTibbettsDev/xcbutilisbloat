# xcbutilisbloat library
Utility functions for fonts, monitors and other properties using XCB, fontconfig, and freetype2.
# Dependencies
Package Names Are From Arch Linux Repositories
## Runtime
- freetype2
- fontconfig
- libxcb
- xcb-util
- xcb-util-renderutil
- xcb-util-image
## Build
- pkgconf (pkgconf or pkg-config command)
I did not want to use pkgconf in the Makefile but including freetype2 otherwise is a real pain so thanks for that
# Links I Found Helpful
[Fonts For XCB (Read This!)](https://venam.nixers.net/blog/unix/2018/09/02/fonts-xcb.html)
[Freetype2 Tutorial](https://freetype.org/freetype2/docs/tutorial/step1.html)
[How to write your own code libraries in C](https://www.youtube.com/watch?v=JbHmin2Wtmc)
[tldp.org Program Library HOWTO](https://tldp.org/HOWTO/Program-Library-HOWTO/index.html)
[tldp.org Program Library HOWTO More Examples](https://tldp.org/HOWTO/Program-Library-HOWTO/more-examples.html#AEN288)
[How to Create Static and Shared Libraries](https://stackoverflow.com/questions/27023593/makefile-how-to-create-both-static-and-shared-libraries-in-c)
[Unix C Libraries](https://docencia.ac.upc.edu/FIB/USO/Bibliografia/unix-c-libraries.html)
