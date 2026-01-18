_start:
	
	char msg, "hello world"

	mov amg, hex=msg
	mov msg, b31 amg

	mov amg, 0
	vga msg

	mov ext, code=0
	exec ext
