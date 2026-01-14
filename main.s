_start:
	char rax, "hello, world"
	mov rbx, hex=rax
	mov rax, b31 rbx

	vga rax
	mov rax, 0

	mov ext, code=0
	exec ext
