catch {exec /home/colinw/dev/script/grep_src -d -f $pattern | sort -u} dirs

set n [llength $dirs]

if       {$n == 0} {
  done
  return
} elseif {$n == 1} {
  done [lindex $dirs 0]
  return
}

raw

winop window_title "Directories matching $pattern"

set len 0

foreach dir $dirs {
  set len1 [string length $dir]

  set len [expr {max($len, $len1)}]
}

proc keyPress { key name } {
  #move 40 0
  #cll
  #text "$key $name"

  if       {$name == "down"} {
    if {$::current < [expr {$::n - 1}]} {
      incr ::current
      redraw
    }
  } elseif {$name == "up"} {
    if {$::current > 0} {
      incr ::current -1
      redraw
    }
  } elseif {$name == "return"} {
    set res [lindex $::dirs $::current]
    done $res
  } elseif {$name == "escape"} {
    done
  } else {
    return
  }
}

set row 2
set col 5

set current 0

set row1 1
set col1 1
set row2 [expr {$n + 2}]
set col2 [expr {$len + 6}]

proc redraw { } {
  cls

  #move 39 0
  #cll
  #text "$::current"

  box $::row1 $::col1 $::row2 $::col2

  move [expr $::current + 2] 3

  color 3

  text "->"

  set ::row 2

  foreach dir $::dirs {
    move $::row $::col

    color -1

    text $dir

    incr ::row
  }

  incr ::row

  move $::row $::col

  color 2

  text "$::n directories match"
}

redraw
