set label1 [label 2 2  "Pos" ]
set label2 [label 2 14 "" ]
set label3 [label 3 2  "Char Size"]
set label4 [label 3 14 ""]
set label5 [label 4 2  "Pixel Size"]
set label6 [label 4 14 ""]

set i1 [input 5 2]

proc redrawProc { } {
  $::label2 set text "$::window_pos"
  $::label4 set text "$::window_char_size"
  $::label6 set text "$::window_pixel_size"

  move 8 0
}

redrawProc
