include "constants.asm"

SECTION "Scenes WRAM", WRAM0

CurrentLevel:
    Ds 1 ; Variable para el nivel actual


SECTION "Scenes", ROM0

player1: db 28, 20, $04, 0, 7, 8

; Datos de notas, cada nota es dos bytes, baja y alta (NR23 y NR24)
notas:
    Db $0d, $85  ; Nota 1
    Db $0d, $83  ; Nota 2
    Db $0d, $85  ; Nota 1
    Db $0d, $83  ; Nota 2
    ;Db $70, $01  ; Nota 3
    ;Db $00, $02  ; Nota 4
    ;Db $20, $30  ; Nota 4
    ;Db $20, $30  ; Nota 4
    ;Db $60, $13  ; Nota 4
MusicDelay:
    DB 5

CurrentNote:
    Db 0

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

    .loop:
        ldh a, [$00]
        and %00000100 ;UP pressed
        ;jp .loop
        jr z, .pressed
    jp .loop
    
    .pressed:
        ret

scenes_startscreen::
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
		ld bc, 15*16
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

	call end_drawing

    call _wait_button

    call init_entity_manager

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
		ld bc, 15*16
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

        call aisys_init_enemies

        call end_drawing
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


;; ############################################################################
;; Next note
;;
;; INPUT: None
;;
;; OUTPUT: None
;; 
;; MODIfIES: AF, HL, BC, DE
;; ############################################################################
next_note:
    ld hl, MusicDelay
    ld a, [hl]
    dec a
    ret nz

    ld a, 5
    ld [hl], a

    ld hl, CurrentNote
    ld c, [hl]     ; Número de nota
    ld b, $00

    
    ; Calculamos la nota actual
    add hl, bc

    push hl

    inc c
    ld a, c
    cp 4; numero total de notas
    jr z, .reset_song

    .continue:

    ; Guardamos en memoria cual seria la siguiente nota
    ld hl, CurrentNote
    ld [hl], c

    pop hl

    ld a, $00
    ldh [$25], A  ; NR52 - apaga el sonido

    LD A, [HL+]  ; Cargar parte alta del período y banderas de control (NR24)
    LDh [$18], A
    LD A, [HL+]  ; Cargar parte baja del período (NR23)
    or %10000000 ; asegurar que el bit 7 esté en alto para activar el canal
    LDh [$19], A
    ret

    LD A, $ff
    LDh [$25], A  ; NR52, reactiva el sonido

    .reset_song
        ld hl, CurrentNote
        ld [hl], 0     ; Volvemos al principio
        jp .continue


sound_next_level:
    ld a, $77
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