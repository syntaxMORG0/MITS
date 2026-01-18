_start:
    req ftype="wasm", lib.webasm
    
    wasm -np page="home"
    wasm -ne type="h1" txt="Hello" id="title"
    wasm -ne type="p" txt="World" id="desc"
    wasm -op 8000 -ap page="home"
