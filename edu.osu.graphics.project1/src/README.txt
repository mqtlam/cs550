========================================
README
CS 550, Assignment #1
Partners: Michael Lam, Behrooz Mahasseni
E-mails: lamm@eecs, mahasseb@eecs
========================================

----------------------------------
Running on Windows **Recommended**
----------------------------------

Using a fresh copy of the Windows MSVC 2012 files from the Resources page,
just copy the attached files (SimpleProgram.cpp and the shader files) into vs2012-example-3.2\SimpleProgram\src.

(Note that the shader files have been unaltered from the original assignment #1 shader files.)

Then compile and run.

Command line arguments takes one argument: the path to the data file

Ex. "cmdline> SimpleProgram Data/noData.txt"

----------------
Running in Linux
----------------

Similar to Windows instructions, except use the linux files from the Resources page.
Also have to comment out the line #define USE_WINDOWS.
But even then, you may possibly have to adjust the header paths and shader paths.
Otherwise it should compile with the correct paths.

Running on the command line is the same as windows: it takes one argument, which is the path to the data file.
