include "constants.asm"

SECTION "Scenes WRAM", WRAM0

CurrentLevel:
    Ds 1 ; Variable para el nivel actual


SECTION "Scenes", ROM0

player1: db 28, 20, $04, 0, 7, 8

;; ############################################################################
;; Detects if some button is pressed
;;
;; INPUT: None
;;
;; OUTPUT: None
;; 
;; MODIfIES: af
;; ############################################################################

_wait_button:

    ld a, %11011111
    ld [$FF00], a

    .loop:
        call aisys_enemies_shoot
		call check_collisions_enemy_solid

        ld de, _copy_entity_to_OAM
        ld hl, _entities_array
        ld bc, OAM_START_ADDR

        call _wait_vblank_start
        call entityman_for_each

        ldh a, [$00]
        and %00001000 ;UP pressed
        jr z, .pressed
    jp .loop
    
    .pressed:
        call sound_next_level
        ret

scenes_startscreen::
    call init_entity_manager
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
		ld bc, NUM_TILES*16
		call _copy_bc_bytes_de2hl

	; Mostrar pantalla de inicio

	; Set HL to the start of the second row, second column
	ld hl, $9800

	; Set the tile number we want to use
	ld de, StartScreen

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
    
    ld a, [CurrentLevel]
    ld a, 2
    ld [CurrentLevel], a

    ld hl, OAM_START_ADDR
    ld bc, 160
    call _clear_memory

    ld hl, player1
    call entityman_create
    call entityman_getEntityArray_HL
    inc hl
    inc hl
    ld [hl], 0
    call aisys_init_enemies

	call end_drawing

    call _wait_button

    ret

;; ############################################################################
;; Changes the map to the one given
;;
;; INPUT: DE (map address)
;;
;; OUTPUT: None
;; 
;; MODIfIES: DE, BC, AF, HL
;; ############################################################################
change_map:


    push de

    .init_tiles:
		ld de, MazeTiles
		ld hl, $8000
		ld bc, NUM_TILES*16
		call _copy_bc_bytes_de2hl

    pop de

	.init_map:
		; Set HL to the start of the second row, second column
		ld hl, $9800

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
    
    ld a, [CurrentLevel]

    cp N_MAPS*2
    jr nz, .not_last_map

    call populate_end_score
    ret

    .not_last_map

    add 2
    ld [CurrentLevel], a

    ret

;; ############################################################################
;; Changes the map to the next level
;;
;; OUTPUT: None
;; 
;; MODIfIES: DE, BC, AF, HL
;; ############################################################################

next_level:

    call start_drawing
    ;destruimos las entidades de jugador y enemigos
    ld de, entityman_free_entity
    ld a, 0
    call entityman_for_each

    ld a, [CurrentLevel]
    cp N_MAPS * 2
    jr z, .end_game

    .continue
    ld hl, levels
    ld c, a
    ld b, 0

    add hl, bc         ; Avanzar a la posición del mapa actual

    ld a, [hl+]        ; Cargar la dirección del mapa en DE
    ld e, a
    ld a, [hl]
    ld d, a

    call change_map
    ;; Clear OAM memory
    ld hl, OAM_START_ADDR
    ld bc, 160
    call _clear_memory

    ld hl, player1
    call entityman_create
    call aisys_init_enemies

    call end_drawing

    ret

    .end_game:
        ld hl, levels
        ld c, a
        ld b, 0
        add hl, bc

        ld a, [hl+]
        ld e, a
        ld a, [hl]
        ld d, a

        call change_map
        
        ;; Clear OAM memory
        ld hl, OAM_START_ADDR
        ld bc, 160
        call _clear_memory

        ld hl, player1
        call entityman_create
        call entityman_getEntityArray_HL
        inc hl
        inc hl
        ld [hl], 0
        call aisys_init_enemies

        call end_drawing
        call _wait_button
        ld a, 2
        ld [CurrentLevel], a
        call physys_init_deaths_counter
        call next_level
        ret

;; ############################################################################
;; Initiates de sound
;;
;; INPUT: None
;;
;; OUTPUT: None
;; 
;; MODIfIES: AF, HL, C
;; ############################################################################
init_sound:
    ; Inicialización del sistema de sonido
    ; Desactivar todos los canales primero
    LD A, $00
    LDh [$25], A  ; NR52 - apaga el sonido

    ; Limpiar registros de sonido
    LD HL, $ff10
    LD B, $14   ; Desde $FF10 a $FF23

    clear_sound:
        LD [HL], $00
        inc hl
        DEC B
        JR NZ, clear_sound

    ; Configurar los registros globales de sonido
    LD A, $77
    LDh [$24], A  ; NR50, configuración de volumen
    ld a, $80
    LDh [$26], A  ; NR51, encaminamiento del sonido

    ; Configurar el Canal 2 para una onda cuadrada
    ;Db $70, $02  ; Nota 1
    ;$cf, $f9  
    ;LD A, $70   ; NR21
    ;LDh [$16], A
    ;LD A, $02   ; NR22
    ;LDh [$17], A

    

    ld a, $80
    ldh [$16], a
    ld a, $70
    ldh [$17], a

    ; Initial Volume ; Env dir ; Sweep Space
    ; 7 6 5 4          3    
    ld a, $0d;%00001101 ;$0d
    ;ldh [$18], a

    ;con c3 se escucha poco, con a3 se escucha un sonido muy largo
    ld a, $85;%11000011 ;$c3
    ;ldh [$19], a

    LD A, $ff
    LDh [$25], A  ; NR52, reactiva el sonido

    ret

sound_next_level:
    ld a, $17
    ldh [$10], a
    ld a, $81
    ldh [$11], a
    ld a, $f3
    ldh [$12], a  
    ld a, $73;%00001101 ;$0d
    ldh [$13], a
    ld a, $86;%11000011 ;$c3
    ldh [$14], a

    ret

sound_death:
    ld a, $7c
    ldh [$10], a
    ld a, $81
    ldh [$11], a
    ld a, $f3
    ldh [$12], a  
    ld a, $73;%00001101 ;$0d
    ldh [$13], a
    ld a, $86;%11000011 ;$c3
    ldh [$14], a

    ret

populate_end_score::
    ld hl, SCORE_MAP_POS

    ld a, [DEATHS_COUNTER]
    add NUMBERS_TILES_START
    ld [hl+], a

    ld a, [DEATHS_COUNTER+1]
    add NUMBERS_TILES_START
    ld [hl], a

    ret