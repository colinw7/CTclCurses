cls

set i1 [tinput 2 2]
set b1 [tbox   6 2 12 7]
set l1 [tlabel 20 2]
set l2 [tlabel 21 2]
set l3 [tlabel 22 2]

$i1 set text "green"

proc tinputExecProc { name } {
  $::b1 set fill [$::i1 get text]

  set rgb [$::b1 get fill_rgb]
  set hsv [$::b1 get fill_hsv]

  $::l1 set text [format "Hex : #%02x%02x%02x" [lindex $rgb 0] [lindex $rgb 1] [lindex $rgb 2]]
  $::l2 set text [format "RGB : %d %d %d" [lindex $rgb 0] [lindex $rgb 1] [lindex $rgb 2]]
  $::l3 set text [format "HSV : %d %d %d" [lindex $hsv 0] [lindex $hsv 1] [lindex $hsv 2]]
}

tinputExecProc ""
