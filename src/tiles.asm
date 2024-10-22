SECTION "Tiles", ROM0

Tiles8p8::
DB $00,$C3,$3C,$81,$3C,$42,$99,$E7
DB $81,$FF,$C3,$FF,$7E,$FF,$3C,$FF

; Metatile index
MetatilesIndex::
DB $00,$00,$00,$00,$01,$01,$01,$01
DB $02,$02,$02,$02,$03,$03,$03,$03
DB $04,$06,$05,$07,$08,$0A,$09,$0B
DB $0C,$0E,$0D,$0F

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
DB $0F,$FF,$08,$F8,$0A,$FA,$0A,$FA
DB $08,$F8,$08,$F8,$0F,$FF,$0F,$FF
DB $0F,$FF,$0F,$FF,$0E,$FF,$0E,$FF
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
DB $F3,$0F,$F3,$0F,$F3,$0F,$F3,$0F
DB $F3,$0F,$F3,$0F,$F3,$0F,$F3,$0F
DB $F3,$0F,$F3,$0F,$F3,$0F,$F3,$0F
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
DB $00,$18,$00,$3C,$00,$66,$00,$C3
DB $00,$C3,$00,$66,$00,$3C,$00,$18
DB $FF,$FF,$91,$91,$99,$99,$C3,$C3
DB $D1,$D1,$9D,$9D,$85,$85,$FF,$FF
DB $FF,$00,$FF,$00,$FF,$00,$FF,$00
DB $FF,$00,$FF,$00,$FF,$00,$FF,$00

