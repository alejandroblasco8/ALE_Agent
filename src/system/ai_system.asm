include "constants.asm"

SECTION "AI SYSTEM", ROM0

_h_r_shoters: db $07
_h_l_shoters: db $06

_bullet_shoters: db $06, $07
_shuriken_shothers: db $09, $0A

_bullet: db $10, $09, $08, 0, 0
_shuriken: db $10, $0A, $0B, 0, 0

aisys_init_enemies:
    ld b, (MAX_ENTITIES - 1) / 2

    .loop1:
        ld hl, _bullet

        push bc
        call entityman_create
        pop bc

        dec b
        jr nz, .loop1

    ld b, (MAX_ENTITIES - 1) / 2

    .loop2:
        ld hl, _shuriken

        push bc
        call entityman_create
        pop bc

        dec b
        jr nz, .loop2

    ret

; WORK IN PROGRESS
; SHOULD READ THE MAP TO GET THE TILE NUMBER
; USING CONSTANTS SHOULD KNOW WHICH GUN SHOOTS
; IN WHICH DIRECTION
; LAST BYTE OF ENTITY SHOULD INDICATE THE DIRECTION WITH:
; HOR - RIGHT => %00000000
; HOR - LEFT  => %00000001
; VER - UP    => %00000010
; VER - DOWN   => %00000011
; COMPARISON ARE NOT CORRECT RIGHT NOW AND IS
; ONLY CHECKING HORIZONTAL TO BEGIN WITH
aisys_select_dir:
    ; Set HL to the start of the second row, second column
    ld hl, VRAM_START_ADDR

    ; Set counter for number of rows
    ld b, 18

    .row_loop:
        ; Set counter for number of columns
        ld c, 20

    .col_loop:
        ; Read the tile number
        ld a, [hl+]

        ; Check if left horizontal shoter
        ld d, a
        ld a, [_h_l_shoters]
        xor d

        ; Save result and jump to found
        ld e, 0
        jr z, .enemy_found

        ; Check if right horizontal shoter
        ld a, [_h_r_shoters]
        xor d

        ; Save result and jump (conditionally) to not found
        ld e, 1
        jr nz, .enemy_not_found

        .enemy_found
        ; Load bullet
        ld hl, _entities_array + ENTITY_SIZE

        ; Set X Coord
        ld a, c
        ld d, OAM_X_DISPLACEMENT
        add d
        ld [hl], d
        inc hl

        ; Set Y Coord
        ld a, b
        ld d, OAM_Y_DISPLACEMENT
        add d
        ld [hl], d
        inc hl

        ; Jump to specific type of enemy
        ld a, e
        or a

        jr z, .h_r

        .h_l

        ld a, H_L_TILE
        ld [hl+], a
        jr .create_entity

        .h_r

        ld a, H_R_TILE
        ld [hl+], a

        .create_entity

        ; Set attributes
        ld a, 0
        ld [hl], a

        ; Create in OAM
        push bc
        ld hl, _bullet
        call entityman_create
        pop bc

        .enemy_not_found:
        ; Decrement column counter
        dec c
        jr nz, .col_loop

        ; Move to the next row
        ld de, 12
        add hl, de

        ; Decrement row counter
        dec b
        jr nz, .row_loop

ret
