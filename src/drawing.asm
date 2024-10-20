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
    ld bc, 16 * 16
    call _copy_bc_bytes_de2hl

	ld de, Tiles8p8
	ld hl, $87F0
    ld bc, 16
    call _copy_bc_bytes_de2hl

	ret

draw_map:
    ld hl, $9821
    ld b, 8

	.row_loop:
		push bc
		push hl
		
		ld c, 9

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

; Draw a tile to the borders of the screen
; Input:
;   DE - Address of the tile to draw
draw_map_borders:
    ; Start at the top-left corner of the screen
    ld hl, $9800

    ; Draw top row
    ld b, 20
    .top_row:
        ld [hl+], a
        dec b
        jr nz, .top_row

    ; Draw left and right borders
    ld hl, $9820
    ld b, 16

    .side_borders:
		; Draw the left border
        ld [hl], a

        ; Move to right border
        ld de, 19
        add hl, de

		; Draw the right border
        ld [hl], a

        ; Move to next row
        ld de, 13
        add hl, de

        dec b
        jr nz, .side_borders
	
    ; Draw bottom row
	ld hl, $9A20
    ld b, 20

    .bottom_row:
        ld [hl+], a
        dec b
        jr nz, .bottom_row

    ret

; Fill the visible screen (excluding borders) with tile $7D
init_map:
    ; Set HL to the start of the second row, second column
    ld hl, $9821  ; $9800 + 32 + 1

    ; Set the tile number we want to use
    ld a, $7E

    ; Set counter for number of rows
    ld b, 16

.row_loop:
    ; Set counter for number of columns
    ld c, 18

.col_loop:
    ; Write the tile number to VRAM
    ld [hl+], a

    ; Decrement column counter
    dec c
    jr nz, .col_loop

    ; Restore HL and move to the next row
    ld de, 14
    add hl, de

    ; Decrement row counter
    dec b
    jr nz, .row_loop

    ret
