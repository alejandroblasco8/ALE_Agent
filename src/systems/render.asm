include "constants.asm"

SECTION "Render System", ROM0

;; #############################################################################
;; Copy an entity to the OAM
;; 
;; INPUT:
;;  - DE => Pointer to the entity
;;  - HL => Pointer to the OAM address 
;;	
;; OUTPUT: None
;;
;; MODIFIES: AF, C, DE, HL
;; #############################################################################
_copy_entity_to_OAM:
	ld c, BYTES_IN_OAM_OBJECT

	.loop:
		ld a, [de]
		inc de
		ld [hl+], a
	
		dec c
		jr nz, .loop
	
	ret
	
;; #############################################################################
;; Renders the entities by copying their data to the OAM.
;;
;; INPUT: None
;;
;; OUTPUT: None
;;
;; MODIFIES: AF, BC, DE, HL
;; #############################################################################
render_entities:
	ld de, _copy_entity_to_OAM
	call entityman_for_each

	ret
