;; Entity Structure
;;
;; struct Entity {
;; u8 cmps, type; // u8: unsigned 8-bit
;; u8 x, w;
;; u8 y, h;
;; u8 tile, attrs;
;; };
;;
DEF E_cmps = 0 ;; Byte 0: Components
DEF E_type = 1 ;; Byte 1: Type
DEF E_x = 2 ;; Byte 2: Pos X
DEF E_w = 3 ;; Byte 3: Width
DEF E_y = 4 ;; Byte 4: Pos Y
DEF E_h = 5 ;; Byte 5: Height
DEF E_tile = 6 ;; Byte 6: Tile Number
DEF E_attr = 7 ;; Byte 7: Attributes

;; Interval Stucture
;;
;; struct Interval {
;; u8 x, w; // u8 = unsigned 8-bit
;; };
;;
DEF I_x = 0 ;; Byte 0: Interval.x
DEF I_w = 1 ;; Byte 1: Interval.y

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SECTION "Collision calculi", WRAM0 ;; DATA

;; Space for two Intervals (To check if they overlap)
;; struct Interval_check {
;; Interval I1, I2;
;; };
;;
intervals:
I1: DS 2 ;; Interval 1: [X, W]
I2: DS 2 ;; Interval 2: [X, W]

SECTION "C4", ROM0

; DE = DE + BC (without using HL)
ld a, e
add c
ld e, a
ld a, d
adc b 
ld d, a

; HL = 5 * HL
ld d, h
ld e, l
add hl, de
add hl, hl
add hl, de

; DE = DE / 4
srl d
rl e
sra d
rl e

; BC = BC - DE
ld a, c
sub e
ld c, a
ld a, b
sbc d
ld b, a

; BC = 6 * BC (without using HL)
sla c
rl b

ld d, b
ld e, c

sla c
rl b

ld a, c
add e
ld c, a
ld a, b
adc d
ld b, a

; EJERCICIO 2

sys_collisions_check_interval_overlap:
	; X1
	ld a, [hl]
	inc hl
	; W1
	ld b, [hl]
	inc hl
	; X2
	ld c, [hl]

	add b
	dec a
	sub c

	jr nc, .check_case_2
	
	scf
	ret
	
	.check_case_2:
	; X2
	ld a, c
	ld c, [hl]
	
	add c
	dec a

	dec hl
	dec hl
	ld c, [hl]

	sub c

	ret nc
	
	scf
	ret

; EJERCICIO 3


sys_collisions_check_if_two_entities_overlap:
	push hl

	; Copiar X1 Y W1 en I1

	; HL = E1.x
	ld bc, E_x
	add hl, bc

	; I1.x = E1.x
	ld a, [hl]
	ld [I1 + I_x], a

	; HL = E1
	pop hl

	; HL = E1.w
	ld bc, E_w
	add hl, bc

	; I1.w = E1.w
	ld a, [hl]
	ld [I1 + I_w], a

	; Copiar X2 Y W2 en I2

	push hl

	; HL = E2
	ld h, d
	ld l, e

	; HL = E2.x
	ld bc, E_w
	add hl, bc

	; I2.x = E2.x
	ld a, [hl]
	ld [I2 + I_w], a

	; HL = E2
	ld h, d
	ld l, e

	; HL = E2.w
	ld bc, E_w
	add hl, bc

	; I2.w = E2.w
	ld a, [hl]
	ld [I2 + I_w], a

	; Check X collisions
	ld hl, intervals
	call sys_collisions_check_interval_overlap

	ret c

	; Copiar Y1 y H2 en I1
	pop hl
	push hl

	; HL = E1.y
	ld bc, E_y
	add hl, bc

	; I1.y = E1.y
	ld a, [hl]
	ld [I1 + I_x], a

	; HL = E1
	pop hl

	; HL = E1.h
	ld bc, E_h
	add hl, bc
	
	; I1.h = E1.h	
	ld a, [hl]
	ld [I1 + I_w], a

	; Copiar Y2 y H2 en I2
	ld h, d
	ld l, e
	
	; HL = E2
	ld bc, E_h
	add hl, bc
	
	; I2.y = E2.y
	ld a, [hl]
	ld [I2 + E_h], a

	; HL = E2
	ld h, d
	ld l, e

	; HL = E2.h
	ld bc, E_h
	add hl, bc

	; I2.h = E2.h
	ld a, [hl]
	ld [I2 + I_w], a

	; Check Y collisions
	ld hl, intervals
	call sys_collisions_check_interval_overlap

	ret
