;;Sistema de colisión

SECTION "Collision system", ROM0




;; ############################################################################
;; Checks the collision between two entities
;;
;; INPUT:
;;    - HL => Pointer to entity collider
;;    - DE => Pointer to entity collided
;;
;; OUTPUT: Flag Z is set if a collision is made
;; ############################################################################

physys_collision_update_one_entity::
    ;;tx = x/4
    ;;ty = y/8
    ;;tw = tilemap_width
    ;;p = tilemap + ty * tw + tx

    ;;tx
    ld a, [hl]
    srl a ;;/
    srl a ;;| A / 8
    srl a ;;\