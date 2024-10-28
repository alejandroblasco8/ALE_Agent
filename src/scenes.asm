SEcTION "Scenes", ROM0

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
        ldh a, [$00]
        and %00000100 ;UP pressed
        ;jp .loop
        jr z, .pressed
    jp .loop
    
    .pressed:
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

game_over:
    ret

