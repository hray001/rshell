rshell
======

Licensing information in LICENSE
---
Source code at http://github.com/hray001/rshell.git
---

Author
---
Harrison Ray

File List
---
```
.:

Makefile

LICENSE

README.md

./src

./tests
```
```
./src:

main.cpp
```
```
./tests:

exec.script
```

Running file
---
Once cloned from github:

1. `cd rshell`
2. Call make
3. `cd` into `bin`
4. Call rshell

Overview
---
Purpose of the shell is to work similarly, though more limited, to the BASH shell.

* Works for execv files
* Uses `exit` to end the program
* Cannot run built-in BASH commads (`cd` and the sort)

BUGS
===
Comment
------
Commenting code too simplistic. Basic commenting will work but trickier caes will not run as intended. EX:

BASH 

`touch hello#world`
 would create a file called `hello#world`

RSHELL

`touch hello#world`
 would create a file called `hello` while world would be considered part of a comment

Connectors
------
The connect `;` works as intended but `&&` and `||` simply execute the commands following them the same way `;` does. 

exit
------
`exit` does not work on a line with multiple commands. `exit` will work but `pwd;exit` would not
size
------
Does not work for an infinitely large command.

LS
---
`-R` does not work properly and no combination of it and other flags will work properly. Works only on current directory with the recursive directories containing other folders. Empty folders cause seg faults.

specifying no directory argument results in nothing being output.

`-l` does not format all nice and pretty when the sizes of the files are different.
