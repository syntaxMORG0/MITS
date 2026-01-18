_start:
    req ftype="wasm", lib.webasm
    
    mov hom, wasm -np page="home"
    
    mov ttl, wasm -ne type="h1" txt="Hello WASM!" id="title" class="header"
    
    wasm -ae id="title" page="home"
    
    wasm -ns ftype="clang" id="backend" exec => {
        #include <stdio.h>
        
        void main() {
            printf("Hello from C backend!\n");
        }
    }
    
    wasm -ae id="backend" page="home"
    
    wasm -op 8000 -ap page="home"
