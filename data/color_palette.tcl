proc redrawProc { } {
  cls

  set n 0

  for {set r 1} {$r <= 16} {incr r} {
    for {set c 1} {$c <= 64} {incr c 4} {
      # bgcolor palette $n
      fgcolor palette $n

      move $r $c
      draw_text [format "%03d" $n]

      incr n
    }
  }

  move 17 0

  bgcolor -1
}

redrawProc
