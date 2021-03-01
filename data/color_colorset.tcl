cls

set colorsets {red pink purple deep_purple indigo blue light_blue cyan teal green light_green lime yellow amber orange deep_orange brown grey blue_grey}

set r 1

foreach colorset $colorsets {
  set n 0

  for {set c 1} {$c <= 48} {incr c 3} {
    bgcolor colorset $colorset $n

    move $r $c
    draw_text [format "%02d" $n]

    incr n
  }

  incr r
}

move 24 0
