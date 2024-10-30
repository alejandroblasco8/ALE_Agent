include "constants.asm"

SECTION "AI SYSTEM", ROM0

_projectile: db $00, $00, $00, 0, 0, 0

aisys_init_enemies2:
    ld b, (MAX_ENTITIES - 1)

    .loop1:
        ld hl, _projectile

        push bc
        call entityman_create
        pop bc

        dec b
        jr nz, .loop1

aisys_init_enemies:
    ld hl, VRAM_START_ADDR

    ; Gun enemy found counter
    ld d, 0
    push de

    ; Set counter for number of rows
    ld b, 18

    .row_loop:
        ; Set counter for number of columns
        ld c, 20

    .col_loop:
        ; Read the tile number
        ld a, [hl+]
        ld d, a

        ; Check if left horizontal shooter
        ld a, H_L_SHOOTER
        cp d

        ; Save result and jump to found
        ld e, H_L_CODE
        jr z, .enemy_found

        ; Check if right horizontal shooter
        ld a, H_R_SHOOTER
        cp d

        ; Save result and jump to found
        ld a, H_R_CODE
        jr z, .enemy_found

        ; Check if vertical down shooter
        ld a, V_D_SHOOTER
        cp d

        ; Save result and jump to found
        ld a, V_D_CODE
        jr z, .enemy_found

        ; Check if vertical up shooter
        ld a, V_U_SHOOTER
        cp d

        ; Save result and jump to found
        ld a, V_U_CODE
        jp nz, .enemy_not_found

        .enemy_found


        pop de
        inc d
        push de

        ; Save BC for counter
        push bc

        ; Save VRAM pointer
        push hl

        ; Save entity type
        push af

        push de

        ld de, $9800

        ld a, l
        sub e
        ld l, a

        ld a, h
        sbc d
        ld h, a

        ld de, 32
        ld b, 0

        .loop_32
            ld a, l
            sub e
            ld l, a

            ld a, h
            sbc d
            ld h, a

            inc b

            cp 0
            jr nz, .loop_32

            ld a, l
            cp 32
            jr nc, .loop_32

        ld a, b

        sla a
        sla a
        sla a

        ld b, OAM_Y_DISPLACEMENT
        add b

        pop de

        ; Save HL to calculate X
        push hl

        ; Create entity
        ld hl, _projectile

        push de
        push af
        call entityman_create
        pop af
        pop de

        ; Find WRAM position for next bullet
        ld hl, _entities_array
        ld bc, ENTITY_SIZE

        .entity_array_pos_loop:
            add hl, bc

            dec d
            jr nz, .entity_array_pos_loop


        ld [hl+], a

        ; Save VRAM pointer
        ld b, h
        ld c, l

        ; Retrieve decremented HL
        pop hl

        ; Set X Coord
        ld a, l

        sla a
        sla a
        sla a

        ld d, OAM_X_DISPLACEMENT - 8
        add d

        ; Retrieve VRAM pointer
        ld h, b
        ld l, c

        ld [hl+], a

        ; Retrieve entity type
        pop af
        ld e, a

        ; Jump to horizonal left
        cp H_L_CODE
        jr z, .h_l

        ; Jump to horizontal right
        cp H_R_CODE
        jr z, .h_r

        ; Jump to vertical down
        cp V_D_CODE
        jr z, .v_d

        ; Jump to vertical up
        cp V_U_CODE
        jr z, .v_u

        .h_l

        ld a, H_L_TILE
        jr .create_entity

        .h_r

        ld a, H_R_TILE
        jr .create_entity

        .v_d

        ld a, V_D_TILE
        jr .create_entity

        .v_u

        ld a, V_U_TILE

        .create_entity

        ; Load tile
        ld [hl+], a

        ; Set attributes
        ld a, 0
        ld [hl+], a

        ; Set dir
        ld a, e
        ld [hl+], a

        ; Set steps done
        ld a, 0
        ld [hl], a

        ; Retrieve VRAM pointer
        pop hl

        ; Retrieve counter
        pop bc

        .enemy_not_found:
        ; Decrement column counter
        dec c
        jp nz, .col_loop

        ; Move to the next row
        ld de, 12
        add hl, de

        ; Decrement row counter
        dec b
        jp nz, .row_loop

    pop de
    ret

aisys_enemies_shoot::
    ld hl, _entities_array + ENTITY_SIZE - 2
    ld de, ENTITY_SIZE

    ld a, [_num_entities]
    dec a
    cp 0
    ret z

    ld c, a

    .loop:
        add hl, de
        ld a, [hl]

        cp H_L_CODE
        jr z, .h_l

        cp H_R_CODE
        jr z, .h_r

        cp V_D_CODE
        jr z, .v_d

        cp V_U_CODE
        jr z, .v_u

        .h_l:

        inc hl
        inc [hl]

        ld a, l
        sub 4
        ld l, a

        ld a, [hl]
        sub PROJECTILE_SPEED
        ld [hl], a
        ld a, 3
        add l
        ld l, a
        ld a, 0
        adc h
        ld h, a

        jr .next_projectile

        .h_r:

        inc hl
        inc [hl]

        ld a, l
        sub 4
        ld l, a

        ld a, [hl]
        add PROJECTILE_SPEED
        ld [hl], a

        ld a, 3
        add l
        ld l, a
        ld a, 0
        adc h
        ld h, a

        jr .next_projectile

        .v_d:

        inc hl
        inc [hl]

        ld a, l
        sub 5
        ld l, a

        ld a, [hl]
        add PROJECTILE_SPEED
        ld [hl], a

        .no_collision_v_d
        ld a, 4
        add l
        ld l, a
        ld a, 0
        adc h
        ld h, a

        jr .next_projectile

        .v_u:

        inc hl
        inc [hl]

        ld a, l
        sub 5
        ld l, a

        ld a, [hl]
        sub PROJECTILE_SPEED
        ld [hl], a

        ld a, 4
        add l
        ld l, a
        ld a, 0
        adc h
        ld h, a

        .next_projectile:

        dec c
        ret z
        jp .loop

    ret

; a => projectile code
; hl => x or y position depending on a
; if a is horizontal x should be passed
; if a is vertical y should be passed
reset_projectile::
    ; Save entity position
    ld b, h
    ld c, l

    ; Jump to specific type of entity code
    cp H_L_CODE
    jr z, .h_l

    cp H_R_CODE
    jr z, .h_r

    cp V_D_CODE
    jr z, .v_d

    cp V_U_CODE
    jr z, .v_u

    .h_l

    ; Move to steps done propierty
    ld de, 4
    add hl, de

    ; Retrieve steps done
    ld e, [hl]

    ; Reset steps done
    xor a
    ld [hl], a

    ; Reset pointer
    ld h, b
    ld l, c

    ; Reset position
    ld a, [hl]
    add e
    ld [hl], a

    ret

    .h_r

    ; Move to steps done propierty
    ld de, 4
    add hl, de

    ; Retrieve steps done
    ld e, [hl]

    ; Reset steps done
    xor a
    ld [hl], a

    ; Reset pointer
    ld h, b
    ld l, c

    ; Reset position
    ld a, [hl]
    add e
    ld [hl], a

    ret

    .v_d

    ; Move to steps done propierty
    ld de, 5
    add hl, de

    ; Retrieve steps done
    ld e, [hl]

    ; Reset steps done
    xor a
    ld [hl], a

    ; Reset pointer
    ld h, b
    ld l, c

    ; Reset position
    ld a, [hl]
    sub e
    ld [hl], a

    ret

    .v_u

    ; Move to steps done propierty
    ld de, 5
    add hl, de

    ; Retrieve steps done
    ld e, [hl]

    ; Reset steps done
    xor a
    ld [hl], a

    ; Reset pointer
    ld h, b
    ld l, c

    ; Reset position
    ld a, [hl]
    add e
    ld [hl], a

    ret