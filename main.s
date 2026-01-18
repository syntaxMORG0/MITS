_start:
	char prm, "Anngi navnet ditt!"
	mov msg, hex=prm
	mov prm, b31 msg

	vga prm
	mov prm, 0
	mov msg, 0

	rdl nam
	mov nbm, hex=nam

	char msg, "Velkommen "
	mov amg, hex=msg

	addr rax, amg + nbm

	mov acm, b31 rax
	vga acm
	
	mov ext, code=0
	exec ext
