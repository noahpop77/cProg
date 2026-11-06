# Notes

`gitupdate` - Push github updates in 1 go

# NOB

With the test build of nob this is the test
command that will run:
- The nob rebuild
- Running the nob build executable
- and in turn running our program

```
gcc -Wall -Wextra -o runnob nob.c
```

# Bootstrapping The Program

From here all you need to do to bootstrap your program execution to execution is previx it with the nob binary. 

`./runnob && build/term`

> Even if you change the `nob.c` code, ./runnob will check if there were changes made, rebuild the nob binary, and continue with execution.

# Flags

### --random

This flag makes the program fill in random cells of the screen with a red colored cell 

### --snake

This flag makes the program drag a snake across the screen from left to right and top to bottom
it will eventually loop back around to the top of the screen.
