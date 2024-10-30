SECTION "Tiles", ROM0

Tiles8p8::
DB $00,$C3,$3C,$81,$3C,$42,$99,$E7
DB $81,$FF,$C3,$FF,$7E,$FF,$3C,$FF

; Metatile index
MetatilesIndex::
DB $00,$00,$00,$00,$01,$01,$01,$01
DB $02,$02,$02,$02,$03,$03,$03,$03
DB $04,$06,$05,$07,$08,$0A,$09,$0B
DB $0C,$0E,$0D,$7F

; Start of tile array.
Tiles::
; Color 00
DB $00,$00,$00,$00,$00,$00,$00,$00
DB $00,$00,$00,$00,$00,$00,$00,$00

; Color 01
DB $FF,$00,$FF,$00,$FF,$00,$FF,$00
DB $FF,$00,$FF,$00,$FF,$00,$FF,$00

; Color 02
DB $00,$FF,$00,$FF,$00,$FF,$00,$FF
DB $00,$FF,$00,$FF,$00,$FF,$00,$FF

; Color 03
DB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
DB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF

; Unbreakable block
DB $7F,$FF,$08,$F8,$0A,$FA,$0A,$FA
DB $08,$F8,$08,$F8,$7F,$FF,$7F,$FF
DB $7F,$FF,$7F,$FF,$0E,$FF,$0E,$FF
DB $0E,$FF,$0E,$FF,$0E,$FF,$1C,$FF
DB $F0,$FF,$10,$1F,$50,$5F,$50,$5F
DB $10,$1F,$10,$1F,$F0,$FF,$F0,$FF
DB $F0,$FF,$F0,$FF,$70,$FF,$70,$FF
DB $70,$FF,$70,$FF,$70,$FF,$38,$FF

; Brekable block
DB $00,$00,$1F,$00,$3F,$00,$7F,$00
DB $7F,$00,$7F,$00,$7F,$00,$7F,$00
DB $FF,$00,$FF,$00,$FF,$00,$FF,$00
DB $00,$FF,$00,$FF,$FF,$FF,$FF,$FF
DB $F3,$7F,$F3,$7F,$F3,$7F,$F3,$7F
DB $F3,$7F,$F3,$7F,$F3,$7F,$F3,$7F
DB $F3,$7F,$F3,$7F,$F3,$7F,$F3,$7F
DB $03,$FF,$03,$FF,$FF,$FF,$FF,$FF

; Main character
DB $FF,$10,$FF,$10,$FF,$FF,$FF,$04
DB $FF,$04,$FF,$FF,$FF,$20,$FF,$20
DB $FF,$FF,$FF,$04,$FF,$04,$FF,$FF
DB $FF,$10,$FF,$10,$FF,$FF,$FF,$FF
DB $FF,$40,$FF,$40,$FF,$FF,$FF,$04
DB $FF,$04,$FF,$FF,$FF,$20,$FF,$20
DB $FF,$FF,$FF,$08,$FF,$08,$FF,$FF
DB $FF,$20,$FF,$20,$FF,$FF,$FF,$FF

; New game 8x8 tiles
MazeTiles:
DB $00,$00,$00,$00,$00,$00,$00,$00
DB $00,$00,$00,$00,$00,$00,$00,$00
DB $FF,$00,$FF,$00,$FF,$00,$FF,$00
DB $FF,$00,$FF,$00,$FF,$00,$FF,$00
DB $00,$FF,$00,$FF,$00,$FF,$00,$FF
DB $00,$FF,$00,$FF,$00,$FF,$00,$FF
DB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
DB $FF,$FF,$FF,$FF,$FF,$FF,$FF,$FF
DB $00,$00,$3E,$3E,$7F,$7F,$7F,$49
DB $7F,$49,$7F,$77,$3E,$3E,$2A,$2A
DB $BF,$C0,$BF,$C0,$BF,$C0,$BF,$C0
DB $BF,$C0,$BF,$C0,$80,$FF,$FF,$FF
DB $FD,$03,$FD,$03,$FD,$03,$FD,$03
DB $FD,$03,$FD,$03,$01,$FF,$FF,$FF
DB $FF,$FF,$81,$FF,$BD,$C3,$BD,$C3
DB $BD,$C3,$BD,$C3,$81,$FF,$FF,$FF
DB $AA,$AA,$55,$55,$AA,$AA,$55,$55
DB $AA,$AA,$55,$55,$AA,$AA,$55,$55
DB $00,$00,$3E,$3E,$6E,$62,$7E,$62
DB $3E,$3E,$1C,$14,$3E,$2A,$77,$77
DB $00,$00,$7C,$7C,$76,$46,$7E,$46
DB $7C,$7C,$38,$28,$7C,$54,$EE,$EE
DB $00,$00,$18,$18,$3C,$24,$7E,$42
DB $7E,$42,$3C,$24,$18,$18,$00,$00
DB $3C,$3C,$5E,$42,$7E,$42,$7E,$42
DB $7E,$66,$3C,$3C,$18,$18,$00,$00
DB $00,$00,$18,$18,$3C,$3C,$6E,$66
DB $7E,$42,$7E,$42,$7E,$42,$3C,$3C
DB $00,$00,$00,$00,$24,$24,$18,$18
DB $18,$18,$24,$24,$00,$00,$00,$00


CollisionTilesNumbers::
DB $07,$08,$09,$0A,$0C,$0D

CollisionEnemyTilesNumbers::
DB $05,$06,$07,$08