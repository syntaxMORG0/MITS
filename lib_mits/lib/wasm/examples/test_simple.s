_start:
    req ftype="wasm", lib.webasm
    
    mov hom, wasm -np page="home"
    
    mov hdr, wasm -ne type="h1" txt="Hello from HTML5!" id="title"
    mov par, wasm -ne type="p" txt="Beautiful webpage built with MITS" id="desc"

