# LTR-C
Left to Right C

## Description of the problem
Regular *C*, as we all know, is too convenient and easy to write, this repo tries to address this problem by making it harder to write.

This new solution should be compatible with the existing C building pipeline.

## The solution
The solution to this problem is trivial actually. All you need to do is change the writing direction.

Regular C is written in a **regressive sad top-to-bottom manner**, LTR-C is written ***left-to-write***.

The tool featured on this repo is a transpiller, it is however able to call on your favorite C compiler in order to fit in with your favorite build system.

### Regular, sad C example
```c
#include <stdio.h>

int main() {
    printf("Hello World!\n");
}
```
As you can see, regular C is boring and trivial.

### Improved LTR-C example
```ltr-c
# i }
i n
n t
c
l mp
u ar
d ii
e nn
  (t
< )f
s  (
t {"
d  H
i  e
o  l
.  l
h  o
>
   W
   o
   r
   l
   d
   !
   \
   n
   "
   )
   ;
```
As it should be evident this is *clearly* the definitive way to write C.

### Migrating to LTR-C
When you decide you're ready to convert your C source files to LTR-C simply use the command:
```bash
ltr_c --transpile --in-place [c_source_files]
```

Then replace `CC` in your source files:
```bash
CC="ltr_c"
```
**IMPORTANT NOTE:** If you have a compiler other than *gcc* or *clang* you may need to specify the compiler:
```bash
CC="ltr_c --set-compiler YOUR_COMPILER_HERE"
```

## Usage
```
ltr_c [options] [ c_compiler_args... | ltr_c_sources... ]
```
### Options
```
    --version                Display version number and quit.
    -v

    --help                   Display help dialog and quit.
    -h

    --set-compiler compiler  Set the compiler command to be used, if not set
                             will try to use gcc, clang or the CC environment
                             variable.

    --transpile              Do not compile, just transpile, should not be used
                             with set-compiler, by default creates a new file
                             with the same name as the original and appends
                             the ".ltr" extension to it.

    --in-place               Transpile without creating a new file, overwrite
                             transpiled file instead, to be used with transpile.

    --silent                 Don't produce any output to stdout.

    --debug-only             Don't do nothing, just print debug information.

    --b4 command             Puts 'command' as an argument before every file.
```

### C Compiler Arguments
Any argument that is not understood as a file or a LTR-C option will be assumed to be a native C compiler argument and will be passed verbose to the compiler

### Compilation Sources
Any valid file paths ending with `.c`, `.C`, `.ltr`, `.LTR`, `.LTR_C` or `.ltr_c` will be transpiled back to C temporarily and then passed to the native C compiler.

**NOTE:** A `.c` file extension is not guaranteed for the temporary file so you may need to add some commands before your file names. If you wanted to use gcc, for example, you'd need `-x c` before every file name (if you don't set-compiler this is done automatically):
```bash
ltr_c --set-compiler gcc --b4 "-x c" file1.ltr file2.ltr
```

### Transpillation Sources
Any valid file paths ending with `.c` or `.C` will be transpiled into a `.ltr` file or transpiled in-place if the corresponding option is set.

## Building
Building ltr_c is easy, just compile `ltr_c.cpp` with a c++ compiler that supports the c++17 standard.

If you're a unix user you can also use `make build` to build ltr_c automatically and `make install` to install the binary executables and the man pages.