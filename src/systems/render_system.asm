include "constants.asm"

SECTION "Render System", ROM0

;; #############################################################################
;; Copy an entity to the OAM
;; 
;; INPUT:
;;  - HL => Pointer to the entity
;;  - BC => Pointer to the OAM address
;;	
;; OUTPUT: None
;;
;; MODIFIES: AF, BC, DE, HL
;; #############################################################################
_copy_entity_to_OAM:
    ; Swap variables
    ld d, h
    ld e, l
    ld h, b
    ld l, c

	ld a, [de]
	inc de
	ld [hl+], a

	ld a, [de]
	inc de
	ld [hl+], a

	ld a, [de]
	inc de
	ld [hl+], a

	ld a, [de]
	inc de
	ld [hl+], a

    ; Preserve the OAM pointer in BC
    ld b, h
    ld c, l

	ret
	
;; #############################################################################
;; Renders the entities by copying their data to the OAM.
;;
;; INPUT: None
;;
;; OUTPUT: None
;;
;; MODIFIES: AF, BC, HL
;; #############################################################################
render_entities:
    ld hl, _entities_array

    ld a, 144
    sub OAM_Y_DISPLACEMENT
    ld [hl+], a

    ld a, 8
    sub OAM_X_DISPLACEMENT
    ld [hl+], a

    ld a, 19
    ld [hl+], a

    ld a, 0
    ld [hl+], a

    ld a, 2
    ld [hl+], a

     ld a, 145
     sub OAM_Y_DISPLACEMENT
     ld [hl+], a

     ld a, 9
     sub OAM_X_DISPLACEMENT
     ld [hl+], a

     ld a, 20
     ld [hl+], a

     ld a, 1
     ld [hl+], a

     ld a, 3
     ld [hl+], a

    ld a, 2
    ld hl, _num_entities
    ld [hl], a

    ld a, 0
    ld bc, OAM_START_ADDR
    ld de, _copy_entity_to_OAM
	call entityman_for_each

	ret
