SECTION "Drawing", ROM0

_wait_vblank_start:
    ld a, [$FF44] ;; rLY
    cp 144
    jr nz, _wait_vblank_start
    ret

_switch_off_screen:
    ld a, [$FF40] ;; rLCDC
    res 7, a
    ld [$FF40], a ;; rLCDC
    ret

_switch_on_screen:
    ld a, [$FF40] ;; rLCDC
    set 7, a
    ld [$FF40], a ;; rLCDC
    ret

_get_metatile_addr:
    or a
    ret z

    push hl
    push bc

    ld bc, 4
    ld h, d
    ld l, e

    .loop:
        add hl, bc
        dec a
        jr nz, .loop

    ld d, h
    ld e, l

    pop bc
    pop hl

    ret

_draw_metatile:
    call _get_metatile_addr
    
    ; Draw top-left tile
    ld a, [de]
    ld [hl+], a
    inc de

    ; Draw top-right tile
    ld a, [de]
    ld [hl+], a
    inc de

    ; Move to next row
    ld bc, 30 ; One row minus the 2 already set tiles
    add hl, bc

    ; Draw bottom-left tile
    ld a, [de]
    ld [hl+], a
    inc de

    ; Draw bottom-right tile
    ld a, [de]
    ld [hl], a
    
    ret

start_drawing:
	call _wait_vblank_start
	call _switch_off_screen
	ret

end_drawing:
	call _switch_on_screen
	ret

init_tiles:
    ld de, Tiles
    ld hl, $8000
    ld bc, 128
    call _copy_bc_bytes_de2hl

	ret

draw_map:
    ld hl, $9800
    ld de, Maps
    ld b, 9

	.row_loop:
		push bc
		push hl
		
		ld c, 10

		.col_loop:
			push hl
			push de 
			push bc

			ld a, [de]
			ld de, MetatilesIndex
			call _draw_metatile

			pop bc
			pop de
			pop hl

			inc hl
			inc hl
			inc de

			dec c
			jr nz, .col_loop

		pop hl
		ld bc, 64
		add hl, bc

		pop bc
		dec b
		jr nz, .row_loop
		
    ret
