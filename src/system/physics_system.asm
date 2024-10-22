include "constants.asm"

SECTION "Physics system", ROM0


;; ############################################################################
;; Update the entity position
;;
;; INPUT:
;;    - a => keys pressed (0000DOWNUPLEFTRIGHT)
;;    - b => Y pos
;;    - c => X pos
;;
;; OUTPUT: None
;;
;; MODIFIES: BC
;; ############################################################################

physys_update_pos::

    ;;move down
    ld d, a
    and %00001000
    jr nz, .not_down_move
    inc B

    .not_down_move:
    ;;move up
    ld a, d
    and %00000100
    jr nz, .not_up_move
    dec B

    .not_up_move:
    ;;move left
    ld a, d
    and %00000010
    jr nz, .not_left_move
    dec C

    .not_left_move:
    ;;move right
    ld a, d
    and %00000001
    jr nz, .not_right_move
    inc C

    .not_right_move:
    ret




;; ############################################################################
;; Update the position of the player entity
;;
;; OUTPUT: None
;;
;; MODIFIES: AF, HL
;; ############################################################################

physys_move_player::

    ld a, 0
    call entityman_get_by_index

    push hl
    ld a, [hl+]
    ld b, a
    ld a, [hl]
    ld c, a
    
    ldh a, [$00]

    call physys_update_pos

    pop hl
    ld a, b
    ld [hl+], a
    ld a, c
    ld [hl], a

    ret
    
