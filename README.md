# xcbutilisbloat library
Utility functions for fonts, monitors and other properties using XCB, fontconfig, and freetype2.
# Dependencies
Package Names Are From Arch Linux Repositories
## Build and Runtime
- freetype2
- fontconfig
- libxcb
## Build
- pkgconf (pkgconf or pkg-config command)
I did not want to use pkgconf in the Makefile but including freetype2 otherwise is a real pain so thanks for that
# Variable and Function Names
Variables and functions that are meant for use in programs including the library are prefixed with xuib and an underscore `xuib_` examples: `xuib_function` or `xuib_variable`<br  />
# Links I Found Helpful
[Fonts For XCB](https://venam.nixers.net/blog/unix/2018/09/02/fonts-xcb.html)<br  />
[Freetype2 Tutorial](https://freetype.org/freetype2/docs/tutorial/step1.html)<br  />
[Version Numbers With Git](https://stackoverflow.com/questions/37814286/how-to-manage-the-version-number-in-git)<br  />
[How to write your own code libraries in C](https://www.youtube.com/watch?v=JbHmin2Wtmc)<br  />
[tldp.org Program Library HOWTO](https://tldp.org/HOWTO/Program-Library-HOWTO/index.html)<br  />
[tldp.org Program Library HOWTO More Examples](https://tldp.org/HOWTO/Program-Library-HOWTO/more-examples.html#AEN288)<br  />
[How to Create Static and Shared Libraries](https://stackoverflow.com/questions/27023593/makefile-how-to-create-both-static-and-shared-libraries-in-c)<br  />
[Unix C Libraries](https://docencia.ac.upc.edu/FIB/USO/Bibliografia/unix-c-libraries.html)<br  />
[Linus' Angry Words on Typdef Structs in C See Reason a.](https://www.kernel.org/doc/html/latest/process/coding-style.html#typedefs)<br  />
