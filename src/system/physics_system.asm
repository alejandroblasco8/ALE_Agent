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
    call physys_check_collision_down
    pop hl
    ret z
    push hl
    call physys_check_collision_up
    pop hl
    ret z

    .update:
    ld a, b
    ld [hl+], a
    ld a, c
    ld [hl], a


    ret


physys_check_collision_down:

    ld a, b
    sub 16
    add HEIGHT
    and a, %11111000
    ld l, a
    ld h, 0
    
    add hl, hl ; position * 16
    add hl, hl ; position * 32

    push hl


    ld a, c
    sub 8
    srl a ; a / 2
    srl a ; a / 4
    srl a ; a / 8


    ld d, 0
    ld e, a
    add hl, de


    ld de, $9800
    add hl, de

    ld a, [hl]
    call check_collision_solid_blocks
    pop hl
    ret z

    .right:

    ld a, c
    sub 8
    add WIDTH
    srl a ; a / 2
    srl a ; a / 4
    srl a ; a / 8


    ld d, 0
    ld e, a
    add hl, de


    ld de, $9800
    add hl, de

    ld a, [hl]
    call check_collision_solid_blocks
    ret


physys_check_collision_up:

    ld a, b
    sub 15
    and a, %11111000
    ld l, a
    ld h, 0
    
    add hl, hl ; position * 16
    add hl, hl ; position * 32
    push hl

    .up_right:
    ld a, c
    sub 8
    add WIDTH
    srl a ; a / 2
    srl a ; a / 4
    srl a ; a / 8

    ld e, a
    ld d, 0
    add hl, de

    ld de, $9800
    add hl, de

    ld a, [hl]
    call check_collision_solid_blocks
    pop hl
    ret z

    .up_left:

    ld a, c
    sub 8
    srl a ; a / 2
    srl a ; a / 4
    srl a ; a / 8

    ld e, a
    ld d, 0
    add hl, de

    ld de, $9800
    add hl, de

    ld a, [hl]
    call check_collision_solid_blocks
    ret


check_collision_solid_blocks:
    ld hl, CollisionTilesNumbers
    ld d, NUM_COLLISION_TILES
    ld e, a

    .loop:

        ld a, [hl+]
        cp e
        ret z
        dec d
        jr nz, .loop
    
    add a, 1
    cp 0
    ret