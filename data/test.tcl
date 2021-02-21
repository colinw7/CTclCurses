proc keyPress { key } {
  move 0 0
  cll
  draw_text $key
}

cls
draw_box 10 10 20 50
move 12 12
fgcolor 1
draw_text Hello
move 13 12
fgcolor 2
draw_text World
move 14 12
fgcolor 3
draw_text XXX
move 31 0
