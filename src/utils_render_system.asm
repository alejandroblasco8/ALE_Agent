SECTION "utils_render_system", ROM0

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