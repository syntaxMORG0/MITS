_start:
    req ftype="wasm", lib.webasm
    
    mov hom, wasm -np page="home"
    
    mov hdr, wasm -ne type="h1" txt="Welcome to MITS!" id="title" class="header"
    mov par, wasm -ne type="p" txt="This is a test page built with WebAssembly." id="intro"
    mov btn, wasm -ne type="button" txt="Click Me" id="submit" class="btn"
    
    wasm -ae id="title" page="home"
    wasm -ae id="intro" page="home"
    wasm -ae id="submit" page="home"
