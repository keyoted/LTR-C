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
# i  }
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
    --set-compiler compiler  Set the compiler command to be used, if not set 
                             will try to use gcc, clang or the CC environment 
                             variable.

    --transpile              Do not compile, just transpile, should not be used 
                             with set-compiler, by default creates a new file 
                             with the same name as the original and appends 
                             the ".ltr" extension to it.

    --in-place               Transpile without creating a new file, overwrite
                             transpiled file instead.

    --silent                 Don't produce any output to stdout.

    --debug-only             Don't do nothing, just print debug information.
```

### C Compiler Arguments
Any argument that is not understood as a file or a LTR-C option will be assumed to be a native C compiler argument and will be passed verbose to the compiler

### LTR-C Sources
Any valid file paths will be transpiled back to C temporarily and then passed to the native C compiler.