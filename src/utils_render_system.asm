SECTION "utils_render_system", ROM0

;; Copy memory
;; de = Address to be copied from
;; hl = Address to be copied to
;; bc = Number of bytes to be copied
;; Modifies: af, bc, de, hl
mem_copy:
    ld a, [de]
    ld [hl], a

    inc de
    inc hl
    dec bc

    ld a, b
    or c
    jr nz, mem_copy
    ret

wait_vblank_start:
    ld a, [$FF44] ;; rLY
    cp 144
    jr nz, wait_vblank_start
    ret

switch_off_screen:
    ld a, [$FF40] ;; rLCDC
    res 7, a
    ld [$FF40], a ;; rLCDC
    ret

switch_on_screen:
    ld a, [$FF40] ;; rLCDC
    set 7, a
    ld [$FF40], a ;; rLCDC
    ret

start_print_to_screen:
    call wait_vblank_start
    call switch_off_screen
    ret

end_print_to_screen:
    call switch_on_screen
    ret

;;----------------------------------------------
;; Sets all the bytes of an array of bytes in
;; memory to a given value-
;; WARNING: Maximum of 255 bytes per array
;; INPUT:
;; - HL: Start address of the array in memory
;; - B: Size of the address in bytes
;; - A: Value to be set for each byte
;; MODIFIES: F, B, HL
;;

memset_mini:
    .loop
        ld [hl+], a ;; Write next byte and HL++
        dec b ;; Counter--
        jr nz, .loop ;; If (Counter != 0) repeat
        ret ;; Return to caller


;;----------------------------------------------
;; Clear the OAM (sets all bytes to 0’s)
;; Uses memset_mini.
;; WARNING: It will only work on VBLANK/HBLANK
;; MODIFIES: AF, B, HL
;;
clear_OAM:
    ld hl, $FE00 ;; HL -> OAM Start Address
    ld b, 160 ;; B = Total bytes of the OAM 40 sprites * 4 bytes por sprite
    xor a ;; A = 0
    call memset_mini ;; Set all to 0’s
    ret ;; Return to caller


;;----------------------------------------------
;; Sets all three palettes (BGP, OBP1, OBP2)
;; to the default palette (3-2-1-0, Black, Dark
;; grey, Light grey, White).
;; MODIFIES: A
;;
set_default_palettes:
    ld a, $E4 ;; A = Std.Palette 3-2-1-0
    ld [$FF47], a ;; Set BGP Palette
    ld [$FF48], a ;; Set OB1 Palette
    ld [$FF49], a ;; Set OB2 Palette
    ret ;; Return to caller

;;----------------------------------------------
;; Makes objects (sprites) visible and sets
;; them to be of size 8x16.
;; MODIFIES: A
;;
setup_OAM_objects:
    ld a, [$FF40] ;; A = Read rLCDC
    or %00000110 ;; Set bits 1 and 2
    ld [$FF40], a ;; Write rLCDC with bits changed
    ret ;; Return to caller


;;----------------------------------------------
;; Copy one row of a tilemap from ROM to VRAM.
;; WARNING: A row will be 255 bytes at most
;; INPUT:
;; - DE: Start address of the row in ROM
;; - HL: Start address of the destination in VRAM
;; - C: Size of the row in bytes
;; MODIFIES: AF, B, DE, HL
;;
copy_tilemap_row_de2hl:
    .loop
        ld a, [de] ;; A = Next byte read from ROM
        ld [hl+], a ;; Write the byte to VRAM, and HL++
        inc de ;; DE++, DE->Next byte to be read
        dec c ;; Decrement counter
        jr nz, .loop ;; If (Counter!=0) next iteration
        ret ;; Return to caller

;;----------------------------------------------
;; Copy tilemap from ROM to VRAM. It copies the
;; tilemap ROW by ROW.
;; Uses copy_tilemap_row_de2hl
;; INPUT:
;; - DE: Start address of the source tilemap in ROM
;; - HL: Start address of the destination in VRAM
;; - BC: Size of a ROW in bytes (Max 255. B=0)
;; - A: Number of ROWs
;; MODIFIES: AF, BC, DE, HL
;;
copy_tilemap_row_by_row:
    .loop
        ;; 1. Save values in all registers in the stack.
        ;; All will be destroyed when copying one row.
        push AF ;; Save Number of ROWs
        push BC ;; Save Size of ROW in bytes
        push DE ;; Save Next ROM source tilemap address
        push HL ;; Save Next VRAM dest. tilemap address

        call copy_tilemap_row_de2hl ;; Copy 1 Row. Destroys register values.

        ;; Update HL Pointer: Point to next VRAM tilemap ROW
        pop HL ;; HL=Restore VRAM Next tilemap address
        ld de, 20 ;; DE= Size of a tilemap ROW in VRAM
        ADD HL, DE  ;; HL+=DE. HL->Next tilemap ROW in VRAM

        ;; Update DE Pointer: Point to address of next ROW
        ;; of the source tilemap in ROM
        pop DE ;; DE= Restore source tilemap Next address
        pop BC ;; BC= Restore size of a ROW in bytes
        push HL ;; Save HL (We need HL for calculations)
        ld h, b ;; /
        ld l, c ;; |
        add hl, de  ;; | DE = BC + DE
        ld d, h ;; | DE -> Next row in ROM
        ld e, l ;; \

        pop HL ;; Restore HL

        ;; Finally, restore counter and decrease it as we
        ;; have copied another ROW of the tilemap
        pop AF ;; Restore Counter (Number of ROWs left)
        dec A ;; Decrase counter
        jr nz, .loop ;; If not zero, continue with next row
        ret ;; Return to caller
