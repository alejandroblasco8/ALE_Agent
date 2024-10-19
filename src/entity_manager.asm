;;
;;  ENTITY MANAGER
;;


def MAX_ENTITIES = 10
def ENTITY_SIZE = 8
def TIPO = 0 ;;Dejar el TIPO como byte 0, NO CAMBIAR
def X = 1 
def Y = 2
def ANCHO = 3
def ALTO = 4
def VELX = 5
def VELY = 6
def COLOR = 7


SECTION "Entity manager data", WRAM0

_num_entities: ds 1
_last_elem_ptr: ds 2
_entity_array: ds MAX_ENTITIES*ENTITY_SIZE


SECTION "Entity manager", ROM0


;;Initializer _num_entities

init_entity_manager::
    ld hl, _num_entities
    xor a
    ld [hl], a

    ld hl, _entity_array
    ld a, h
    ld [_last_elem_ptr], a
    ld a, l
    ld [_last_elem_ptr + 1], a
    
    ret
    
;;OUTPUT
;; HL: Pointer to entity array
entityman_getEntityArray_HL::
    ld hl, _entity_array
    ret

;;OUTPUT
;; A: Pointer to num entities
entityman_getNumEntities_A::
    ld a, [_num_entities]
    ret

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

;;INPUT
;; A: Index
entityman_free_entity::

    ;;Check if the index is inside the array range
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
    ;;Realocate the pointer to the entity_array last element

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



;;INPUT
;; A: Index
entityman_get_by_index::
    ld hl, _entity_array
    ld bc, ENTITY_SIZE
    
    .loop_get_by_index:
        cp 0
        ret z
        dec a
        add hl, bc
        jr .loop_get_by_index


;;INPUT
;; HL: Pointer to the entity
;;  B: Type to check if the entity is or not
;;
;;OUTPUT
;;  Flag Z: Active if entity is of type B

entityman_is_of_type_b::
    ld a, [hl]
    cp b
    ret

;;INPUT
;;  B: Type of the entity to be found

entityman_find_first_by_type::
    
    ld a, [_num_entities]
    ld c, a
    ld hl, _entity_array
    
    .loop_find_first_by_type:
        ;;Checks if there is not more available entities.
        dec c
        ld a, c
        cp 255
        ret z

        call entityman_is_of_type_b
        add hl, de
        jr nz, .loop_find_first_by_type
    
    ret



