include "constants.asm"

;;
;;  ENTITY MANAGER
;;

SECTION "Entity manager data", WRAM0

_num_entities: ds 1
_last_elem_ptr: ds 2
_entities_array: ds MAX_ENTITIES * ENTITY_SIZE

_blocks_array: ds MAX_BLOCKS * BLOCK_SIZE
_num_blocks: ds 1

_for_each_func_pointer: DS 4


SECTION "Entity manager", ROM0


;; ############################################################################
;; Initialize entity_manager
;;
;; MODIFIES: AF, HL
;; ############################################################################

init_entity_manager::
    ld hl, _num_entities
    xor a
    ld [hl], a

    ld hl, _entities_array
    ld a, h
    ld [_last_elem_ptr], a
    ld a, l
    ld [_last_elem_ptr + 1], a

    ret

;; ############################################################################
;; Return entities_array
;;
;; OUTPUT:
;;  -  HL => Pointer to entity array
;;
;; MODIFIES: A
;; ############################################################################

entityman_getEntityArray_HL::
    ld hl, _entities_array
    ret

;; ############################################################################
;; Return num_entities
;;
;; OUTPUT:
;;  -  A => Pointer to num entities
;;
;; MODIFIES: A
;; ############################################################################

entityman_getNumEntities_A::
    ld a, [_num_entities]
    ret

;; ############################################################################
;; Create new entity
;;
;; INPUT:
;;  -  HL => Pointer to entity initializer bytes
;;
;; MODIFIES: AF, BC, HL, DE
;; ############################################################################

;;INPUT
;; HL: Pointer to entity initializer bytes

entityman_create::
    ld a, [_last_elem_ptr]
    ld d, a
    ld a, [_last_elem_ptr + 1]
    ld e, a
    ld c, ENTITY_SIZE

    ;;Save new entity data
    .create_entity_loop:

        ld a, [hl+]
        ld [de], a
        inc de
        dec c

        jr nz, .create_entity_loop

    ;;Inc the number of entities
    ld a, [_num_entities]
    inc a
    ld [_num_entities], a

    ;;Update last element pointer due to the creation of the new entity
    ld a, [_last_elem_ptr]
    ld h, a
    ld a, [_last_elem_ptr + 1]
    ld l, a
    ld bc, ENTITY_SIZE
    add hl, bc
    ld a, h
    ld [_last_elem_ptr], a
    ld a, l
    ld [_last_elem_ptr + 1], a

    ret

;; ############################################################################
;; Delete entity associated to A index
;;
;; INPUT:
;;  -  A => Entity index + 1
;;
;; MODIFIES: AF, BC, HL, DE
;; ############################################################################

entityman_free_entity::

    ;;Check if the index is inside the array range
    dec a
    ld b, a
    ld a, [_num_entities]
    ld c, a
    ld a, b
    cp c
    jr c, .is_inside
    ret

    .is_inside:

    push af
    ld a, [_num_entities]
    dec a
    call entityman_get_by_index
    ld d, h
    ld e, l

    pop af
    call entityman_get_by_index
    ld c, ENTITY_SIZE

    ;;Check if the entity to delete is the last one
    ld a, l
    cp e
    jr nz, .loop_free_entity

    ld a, h
    cp d
    jr z, .is_last

    .loop_free_entity:
        ld a, [de]
        ld [hl+], a
        inc de
        dec c
        jr nz, .loop_free_entity


    .is_last:
    ;;Realocate the pointer to the entities_array last element

    ld c, ENTITY_SIZE
    ld a, [_last_elem_ptr]
    ld h, a
    ld a, [_last_elem_ptr + 1]
    ld l, a

    .loop_last_elem:
        dec hl
        dec c
        jr nz, .loop_last_elem

    ld a, h
    ld [_last_elem_ptr], a
    ld a, l
    ld [_last_elem_ptr + 1], a

    ;;Decrements the number of entities

    ld a, [_num_entities]
    dec a
    ld [_num_entities], a

    ret

;; ############################################################################
;; Checks if the entity is of type B
;;
;; INPUT:
;;  -  A => Entity index
;;
;; OUTPUT:
;;  - HL => Points to the entity associated to A index
;;
;; MODIFIES: HL, BC, AF
;; ############################################################################

entityman_get_by_index::
    ld hl, _entities_array
    ld bc, ENTITY_SIZE

    .loop_get_by_index:
        cp 0
        ret z
        dec a
        add hl, bc
        jr .loop_get_by_index


;; ############################################################################
;; Checks if the entity is of type B
;;
;; INPUT:
;;  - HL => Pointer to the entity
;;  -  B => Type to check if the entity is or not
;;
;; OUTPUT:
;;  - Flag Z => Active if entity is of type B
;;
;; MODIFIES: AF
;; ############################################################################

entityman_is_of_type_b::
    ld de, TYPE
    add hl, de
    ld a, [hl]
    cp b
    ret

;; ############################################################################
;; Find first entity that matches the type
;;
;; INPUT:
;;	- B => Type of the entity to be found
;;
;; OUTPUT:
;;  - HL => Points to the first type b entity
;;
;; MODIFIES: AF, C, HL
;; ############################################################################

entityman_find_first_by_type::

    ld a, [_num_entities]
    ld c, a
    ld hl, _entities_array

    .loop_find_first_by_type:
        ;;Checks if there is not more available entities.
        dec c
        ld a, c
        cp 255
        ret z
        push hl
        call entityman_is_of_type_b
        pop hl
        ret z
        add hl, de
        jp .loop_find_first_by_type

    ret

;; ############################################################################
;; Performs an operation on all the entities in the array:
;;
;; INPUT:
;;	- A => 0 to use entities array, 1 to use block arrays
;;	- DE => Pointer to a function to be performed on all the entities.
;;
;; OUTPUT: None
;;
;; MODIFIES: AF, BC, HL
;;
;; BEWARE! Registers AF and HL will be restored after the applied function call.
;; #############################################################################

entityman_for_each::
    ; Construct callable pointer to the function
    ld hl, _for_each_func_pointer

    ; Add call operation code
    ld a, $CD
    ld [hl+], a

    ; Add low byte of the address
    ld [hl], e
    inc hl

    ; Add high byte of the address
    ld [hl], d
    inc hl

    ; Add ret operation code
    ld a, $C9
    ld [hl], a

    ld hl, _entities_array
    ld a, [_num_entities]

    ; Execute the function for every entity
    .loop_for_each:
        push af
        push hl

        call _for_each_func_pointer

        pop hl
        pop af

        push bc

        ld bc, ENTITY_SIZE
        add hl, bc

        pop bc

        dec a
        jr nz, .loop_for_each

    ret

;; ############################################################################
;; Update the entity associated to the index A
;;
;; INPUT:
;;	- HL  => Pointer to entity update bytes
;;	- A => Index
;;
;; OUTPUT: None
;;
;; MODIFIES: AF, C, DE, HL
;; ############################################################################

entityman_update::

    ld d, h
    ld e, l
    ld c, ENTITY_SIZE

    call entityman_get_by_index

    .loop_update:
        ld a, [de]
        ld [hl+], a
        inc de
        dec c
        jr nz, .loop_update

    ret
