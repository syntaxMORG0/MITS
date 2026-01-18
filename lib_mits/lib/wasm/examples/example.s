_start:
    req ftype="wasm", lib.webasm

    wasm -np page="home"
    
    wasm -ne type="h1" txt="Welcome to MITS WebAssembly!" class="header" id="title"
    wasm -ne type="p" txt="This is a beautiful webpage built with assembly language." id="desc" class="intro"
    wasm -ne type="button" txt="Click Me" id="btn" class="btn-primary"
    wasm -ne type="div" id="main" class="container"

    wasm -op 8000 -ap page="home"
