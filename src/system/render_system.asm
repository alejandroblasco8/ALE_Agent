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
