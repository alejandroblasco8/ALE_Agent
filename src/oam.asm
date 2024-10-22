INCLUDE "constants.asm"

SECTION "OAM", ROM0

load_tile_in_oam:
	ld hl, OAM_START_ADDR

	ld a, 90 - OAM_Y_DISPLACEMENT
	ld [hl+], a

	ld a, 80 - OAM_X_DISPLACEMENT
	ld [hl+], a

	ld a, 6
	ld [hl+], a

	ld a, 0
	ld [hl], a
	
	ret
