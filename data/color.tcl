cls

set i1 [input 2 2]
set b1 [box   6 2 12 7]
set l1 [label 20 2]
set l2 [label 21 2]
set l3 [label 22 2]

$i1 set text "green"

proc inputExecProc { name } {
  $::b1 set fill [$::i1 get text]

  set rgb [$::b1 get fill_rgb]
  set hsv [$::b1 get fill_hsv]

  $::l1 set text [format "Hex : #%02x%02x%02x" [lindex $rgb 0] [lindex $rgb 1] [lindex $rgb 2]]
  $::l2 set text [format "RGB : %d %d %d" [lindex $rgb 0] [lindex $rgb 1] [lindex $rgb 2]]
  $::l3 set text [format "HSV : %d %d %d" [lindex $hsv 0] [lindex $hsv 1] [lindex $hsv 2]]
}

inputExecProc ""
