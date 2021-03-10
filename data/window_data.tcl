set label1 [tlabel 2 2  "Pos" ]
set label2 [tlabel 2 14 "" ]
set label3 [tlabel 3 2  "Char Size"]
set label4 [tlabel 3 14 ""]
set label5 [tlabel 4 2  "Pixel Size"]
set label6 [tlabel 4 14 ""]

set i1 [tinput 5 2]

proc redrawProc { } {
  $::label2 set text "$::window_pos"
  $::label4 set text "$::window_char_size"
  $::label6 set text "$::window_pixel_size"

  move 8 0
}

redrawProc
