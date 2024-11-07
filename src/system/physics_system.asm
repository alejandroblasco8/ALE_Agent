include "constants.asm"

SECTION "Physics system locations", WRAM0

_UP_DOWN: ds 4
DEATHS_COUNTER: ds 2

SECTION "Physics system", ROM0

physys_init_deaths_counter::
    xor a
    ld [DEATHS_COUNTER], a
    ld [DEATHS_COUNTER + 1], a
    ret

; HL => VRAM
; OUT => BC (YX)
get_y_x_coords_from_vram:
    ld de, VRAM_START_ADDR

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

    ; Save Y Coord
    ld b, a

    ; Set X Coord
    ld a, l

    sla a
    sla a
    sla a

    ld d, OAM_X_DISPLACEMENT - 8
    add d

    ; Save X Coord
    ld c, a

    ret

get_vram_tile_from_y_x_coords::

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

    ld a, %11101111
    ld [$FF00], a
    
    ldh a, [$00]

    call physys_update_pos
    call physys_check_collision_down
    pop hl
    ret z   

    .update:
    ld a, b
    ld [hl+], a
    ld a, c
    ld [hl], a

    ret


physys_check_collision_down::

    ;;DOWN
    ld a, b
    sub 17
    add HEIGHT
    and a, %11111000
    ld l, a
    ld h, 0
    
    add hl, hl ; position * 16
    add hl, hl ; position * 32

    ld a, h
    ld[_UP_DOWN], a
    ld a, l
    ld[_UP_DOWN+1], a

    ;;UP
    ld a, b
    sub 15
    and a, %11111000
    ld l, a
    ld h, 0
    
    add hl, hl ; position * 16
    add hl, hl ; position * 32

    ld a, h
    ld[_UP_DOWN+2], a
    ld a, l
    ld[_UP_DOWN+3], a

    ;;UP-LEFT
    ld a, c
    sub 7
    srl a ; a / 2
    srl a ; a / 4
    srl a ; a / 8


    ld d, 0
    ld e, a
    add hl, de
    push de


    ld de, $9800
    add hl, de

    ld a, [hl]
    call check_collisions_player_block
    pop de
    ret z

    ;;DOWN-LEFT
    ld a, [_UP_DOWN]
    ld h, a
    ld a, [_UP_DOWN+1]
    ld l, a

    add hl, de
    ld de, $9800
    add hl, de

    ld a, [hl]
    call check_collisions_player_block
    ret z

    ld a, e
    cp END_LEVEL_TILE
    call check_goal_entry
    ret z

    ;;DOWN-RIGHT
    ld a, [_UP_DOWN]
    ld h, a
    ld a, [_UP_DOWN+1]
    ld l, a

    ld a, c
    sub 8
    add WIDTH
    srl a ; a / 2
    srl a ; a / 4
    srl a ; a / 8


    ld d, 0
    ld e, a
    push de
    add hl, de

    ld de, $9800
    add hl, de

    ld a, [hl]
    call check_collisions_player_block
    pop de
    ret z

    ;;UP-RIGHT
    ld a, [_UP_DOWN+2]
    ld h, a
    ld a, [_UP_DOWN+3]
    ld l, a

    add hl, de
    ld de, $9800
    add hl, de

    ld a, [hl]
    call check_collisions_player_block
    ret 
    
check_collisions_player_block::
    ld hl, CollisionTilesNumbers
    ld d, NUM_COLLISION_TILES
    ld e, a

    .collision_solid_loop:

        ld a, [hl+]
        cp e
        ret z
        dec d
        jr nz, .collision_solid_loop
    
    .continue:
        or 1
        ret

check_collisions_enemy_block::
    ld hl, CollisionEnemyTilesNumbers
    ld d, NUM_COLLISION_ENEMY_TILES
    ld e, a

    .collision_solid_loop:

        ld a, [hl+]
        cp e
        ret z
        dec d
        jr nz, .collision_solid_loop
    
    .continue:
        or 1
        ret

    
check_collisions_player_enemy::
    call entityman_getEntityArray_HL

    ld a, [hl+]
    ld b, a ;;B -> Y
    ld a, [hl-]
    ld c, a ;;C -> X
    ld de, ENTITY_SIZE + 1
    add hl, de

    call entityman_getNumEntities_A
    dec a
    jr z, .continue
    ld d, a
    ld e, ENTITY_SIZE + 1

    .collision_enemy_loop:
        ld a, [hl]
        cp c
        call .collision_axis_y
        jr c, .next_axis
        jr nz, .collision_enemy_next

        .next_axis:
        dec hl
        ld a, [hl]
        cp b
        call .collision_axis_x
        jr c, .reset_pos
        jr z, .reset_pos
    
    .collision_enemy_next:
        dec hl
        dec d
        jr z, .continue
        ld a, d
        ld d, 0
        add hl, de
        ld d, a
        jr .collision_enemy_loop

    .collision_axis_x:
        jr nc, .x_not_greater
        ld a, b
        sub [hl]
        inc hl
        jr .diff_less_than
        .x_not_greater:
            sub b
            jr .diff_less_than

    .collision_axis_y:
        jr nc, .y_not_greater
        ld a, c
        sub [hl]
        jr .diff_less_than
        .y_not_greater:
            sub c
    
    .diff_less_than:
        cp 4
        ret

    .reset_pos:
        ld a, 0
        call entityman_get_by_index
        ld a, 28
        ld [hl+], a
        ld [hl], 20
        call sound_death

        ld a, [DEATHS_COUNTER + 1]
        ld b, a
        ld a, [DEATHS_COUNTER]
        cp 9
        jr nz, .is_lower_than_99
        ld a, b
        cp 9
        jr nz, .is_lower_than_99
        jp .reset_continue

        .is_lower_than_99
        xor a
        ld a, [DEATHS_COUNTER + 1]
        inc a
        daa
        cp $10
        jr nz, .not_greater_than_10
        xor a
        ld [DEATHS_COUNTER + 1], a
        xor a
        ld a, [DEATHS_COUNTER]
        inc a
        daa
        ld [DEATHS_COUNTER], a
        jp .reset_continue

        .not_greater_than_10
        ld [DEATHS_COUNTER + 1], a

        .reset_continue
        xor a
        ret
    
    .continue:
        or 1
        ret


check_goal_entry::
    ret nz

    ld a,0
    call entityman_get_by_index
    
    call sound_next_level
    call next_level
    
    xor a
    ret

    

check_collisions_enemy_solid::
    ld hl, _entities_array + ENTITY_SIZE - 3

    ld a, [_num_entities]
    dec a
    cp 0
    ret z

    .loop:
        ld bc, ENTITY_SIZE
        add hl, bc
        push hl

        ld a, [hl+]

        ; Load collision coordinate
        ld b, [hl]

        ; Move to X coord
        dec hl
        dec hl
        dec hl
        dec hl

        cp H_L_SHOOTER
        jr z, .check_h_l

        cp H_R_SHOOTER
        jr z, .check_h_r

        cp V_D_SHOOTER
        jr z, .check_v_d

        cp V_U_SHOOTER
        jr z, .check_v_u

        .check_h_l:
            ; Get current coord
            ld a, [hl]
            cp b
            jr nc, .next_enemy

            ld a, H_L_SHOOTER
            push de
            call reset_projectile
            pop de

            jr .next_enemy

        .check_h_r:
             ; Get current coord
             ld a, [hl]
             cp b
             jr c, .next_enemy

             ld a, H_R_SHOOTER
             push de
             call reset_projectile
             pop de

            jr .next_enemy

        .check_v_d:

            dec hl

            ; get current coord
            ld a, [hl]
            cp b
            jr c, .next_enemy

            ld a, V_D_SHOOTER
            push de
            call reset_projectile
            pop de

            jr .next_enemy

        .check_v_u:

            dec hl

            ; get current coord
            ld a, [hl]
            cp b
            jr nc, .next_enemy

            ld a, V_U_SHOOTER
            push de
            call reset_projectile
            pop de

        .next_enemy:
            pop hl

            dec e
            ret z
            jp .loop
    
    ret