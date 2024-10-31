include "constants.asm"

SECTION "Scenes WRAM", WRAM0

CurrentLevel:
    Ds 1 ; Variable para el nivel actual

SECTION "Scenes", ROM0

player1: db 28, 20, $04, 0, 7, 8

; Datos de notas, cada nota es dos bytes, baja y alta (NR23 y NR24)
notas:
    Db $70, $02  ; Nota 1
    Db $00, $02  ; Nota 2
    Db $70, $01  ; Nota 3
    Db $00, $02  ; Nota 4


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
        call aisys_enemies_shoot
		call check_collisions_enemy_solid

        ld de, _copy_entity_to_OAM
        ld hl, _entities_array
        ld bc, OAM_START_ADDR

        call _wait_vblank_start
        call entityman_for_each

        ldh a, [$00]
        and %00000100 ;UP pressed
        jr z, .pressed
    jp .loop
    
    .pressed:
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

        ld hl, player1
        call entityman_create
        call entityman_getEntityArray_HL
        inc hl
        inc hl
        ld [hl], 0
        call aisys_init_enemies

        call end_drawing
        ret

;; ############################################################################
;; Detects if some button is pressed with a melody
;;
;; INPUT: None
;;
;; OUTPUT: None
;; 
;; MODIfIES: AF, HL, C
;; ############################################################################
_wait_button_with_sound:
; Inicialización del sistema de sonido
    ld hl, $ff10
    ld b, $14   ; Número de registros de sonido para inicializar

    init_sound:
        ld [hl], $00
        INc hl
        dec b
        jr nz, init_sound

    ; configurar los registros NR50, NR51 y NR52 para activar el sonido
    ld a, $77
    ld [$ff24], a  ; NR50, configuración de volumen y panorámica
    ld [$ff25], a  ; NR51, selección de canales
    ld a, $80
    ld [$ff26], a  ; NR52, activar el sistema de sonido

; configurar el canal 2 para una onda cuadrada
    ld a, $81   ; NR21, ciclo de trabajo 50% y longitud de sonido
    ld [$ff16], a
    ld a, $f3   ; NR22, volumen máximo sin cambio automático de volumen
    ld [$ff17], a
    ld a, $00   ; Parte baja del período (NR23)
    ld [$ff18], a
    ld a, $c3   ; NR24, iniciar el sonido con temporizador activo y parte alta del período
    ld [$ff19], a

    ; bucle para reproducir una secuencia de notas
    emepezar_de_nuevo:
        ld hl, notas
        ld c, 4     ; Número de notas a reproducir

    play_loop:

        ldh a, [$00]
        and %00000100 ;UP pressed
        jr z, .pressed_up

        ld a, [hli] ; cargar la próxima parte baja del período
        ld [$ff18], a   ; Establecer la nueva frecuencia (parte baja)
        ld a, [hli] ; cargar parte alta de la frecuencia y control
        or %10000000 ; asegurar que el bit 7 esté en alto para activar el canal
        ld [$ff19], a   ; Iniciar la nota
        call Delay    ; Retardo para mantener la nota sonando
        dec c
        jr nz, play_loop
        jp emepezar_de_nuevo

        .pressed_up
            ret

    ; función de retardo
    Delay:
        ld b, 20   ; Duración del retardo
    delay_loop:
        dec b
        jr nz, delay_loop
        RET