include "constants.asm"

SECTION "Render System", ROM0

;; ############################################################################
;; Renders the entities by copying their data to the OAM.
;;
;; INPUT:
;;	- B  => Number of entities to render
;;	- DE => Pointer to the start of entity array
;;
;; OUTPUT: None
;;
;; MODIFIES: AF, BC, DE, HL
;; ############################################################################
render_entities:
	ld hl, OAM_START_ADDR

	.entities_loop:
		ld c, BYTES_IN_OAM_OBJECT

		; Load the entity BYTES_IN_OAM_OBJECT bytes in the OAM
		.entity_loop
			ld a, [de]
			inc de
			ld [hl+], a

			dec c
			jr nz, .entity_loop

		dec hl

		; Check if there are any extra attributes
		ld a, NOT_OAM_EXTRA_ATTRIBUTES 
		or a
		jr z, .no_extra_attributes

		.extra_attributes_loop:
			inc de
			dec a
			jr nz, .extra_attributes_loop

		.no_extra_attributes:
		; Render the next entity
		dec b
		jr nz, .entities_loop

	ret
