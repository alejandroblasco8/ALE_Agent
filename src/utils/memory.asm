SECTION "Memory", ROM0

_copy_bc_bytes_de2hl:
	.loop:
		ld a, [de]
		ld [hl+], a

		inc de
		dec bc

	    	ld a, b
	    	or c
	jr nz, .loop
ret

_clear_memory:
    ld a, b
    or c
    ret z

    .loop:
        xor a

        ld [hl+], a
        dec bc
        ld a, b
        or c
    jr nz, .loop

    ret
