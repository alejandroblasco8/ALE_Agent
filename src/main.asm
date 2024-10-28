include "constants.asm"
;;----------LICENSE NOTICE-------------------------------------------------------------------------------------------------------;;
;;  This file is part of GBTelera: A Gameboy Development Framework                                                               ;;
;;  Copyright (C) 2024 ronaldo / Cheesetea / ByteRealms (@FranGallegoBR)                                                         ;;
;;                                                                                                                               ;;
;; Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation    ;;
;; files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy,    ;;
;; modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the         ;;
;; Softwareis furnished to do so, subject to the following conditions:                                                           ;;
;;                                                                                                                               ;;
;; The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.;;
;;                                                                                                                               ;;
;; THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE          ;;
;; WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR         ;;
;; COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   ;;
;; ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                         ;;
;;-------------------------------------------------------------------------------------------------------------------------------;;

SECTION "Entry point", ROM0[$250]


player1: db 28, 20, $01, 0, 7, 8
player1: db 28, 20, $01, 0, 7, 8


main::
	call start_drawing

 ; Load default palette
	ld a, %11100100
	ld [$FF47], a
	ld [$FF48], a
	ld [$FF49], a

   ld a, [$FF40]    ; Cargar el valor actual del registro LCDC
   or %00000010     ; Habilitar sprites (bit 1 de LCDC)
   ld [$FF40], a    ; Guardar el nuevo valor en LCDC

	.init_tiles:
		ld de, MazeTiles
		ld hl, $8000
		ld bc, 12*16
		call _copy_bc_bytes_de2hl

	; Mostrar pantalla de inicio

	; Set HL to the start of the second row, second column
	ld hl, $9800

	; Set the tile number we want to use
	ld de, PantallaInicio

	; Set counter for number of rows
	ld b, 18
	.row_loop1:
		; Set counter for number of columns
		ld c, 20

	.col_loop1:
		; Write the tile number to VRAM
		ld a, [de]
		inc de

		ld [hl+], a

		; Decrement column counter
		dec c
		jr nz, .col_loop1

		; Restore HL and move to the next row
		push de

		ld de, 12
		add hl, de

		pop de

		; Decrement row counter
		dec b
		jr nz, .row_loop1


	call end_drawing

	call _wait_button

	call start_drawing

	.init_map:
		; Set HL to the start of the second row, second column
		ld hl, $9800

		; Set the tile number we want to use
		ld de, MazeMap01

		; Set counter for number of rows
		ld b, 18

		.row_loop:
			; Set counter for number of columns
			ld c, 20

		.col_loop:
			; Write the tile number to VRAM
			ld a, [de]
			inc de

			ld [hl+], a

			; Decrement column counter
			dec c
			jr nz, .col_loop

			; Restore HL and move to the next row
			push de

			ld de, 12
			add hl, de

			pop de

			; Decrement row counter
			dec b
			jr nz, .row_loop


    ; Clear OAM memory
     ld hl, OAM_START_ADDR
     ld bc, 160
     call _clear_memory

   ; Initialize the entity manager
    call init_entity_manager

    ; Create the player entity
    ld hl, player1
    call entityman_create

    ; Create the enemies entities
    call aisys_init_enemies

    call end_drawing

    .loop
        ; No need for wait for vblank
        ld de, _copy_entity_to_OAM
        ld hl, _entities_array
        ld bc, OAM_START_ADDR

        call _wait_vblank_start

        call entityman_for_each
        call physys_move_player

        ; No need for wait for vblank
        call aisys_enemies_shoot
        call check_enemy_collisions
    jp .loop

	call game_over

   	di     ;; Disable Interrupts
   	halt   ;; Halt the CPU (stop procesing here)
