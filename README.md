# Notes

`gitupdate` - Push github updates in 1 go
`./run` - helper script to run program


# NOB

With the test build of nob this is the test
command that will run:
- The nob rebuild
- Running the nob build executable
- and in turn running our program

`gcc -Wall -Wextra -o runnob nob.c && ./runnob && ./build/term`

# Bootstrapping The Program

From here all you need to do to bootstrap your program execution to execution is previx it with the nob binary. 

`./runnob && build/term`

> Even if you change the `nob.c` code, ./runnob will check if there were changes made, rebuild the nob binary, and continue with execution.

# CLI Flags

`no flags`

When no flags are provided it will run its default printout.

---

`-box "text"`

Prints the text inside of a flexible box that will follow the total dimensions of half the height and half the width of the current terminal.

---

`-titlecard "1. Full Metal Achemist Brotherhood" "2. Berserk" "3. Vinland Saga" "4. Bleach" "5. Shaman King" "6. Inuyasha"`

This prints two centered lines and between it your text. Rather than caring only about the FOLLOWNING cli argument, `-titlecard` prints all arguments after it as titlecard items.





### Possible Projects

- Hex viewer for binaries
