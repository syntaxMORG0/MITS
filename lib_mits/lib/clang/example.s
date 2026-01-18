_main:
    ; comming soon! WebAssembly needs to be finished first.
    req ftype="clang", lib.clang

    clang -c id="main" exec => {
        #include <stdio.h>

        void main() {
            printf("Hello, MITS World!\n");
        }
    }

    clang run id="main"

    mov ext, code=0
    exec ext