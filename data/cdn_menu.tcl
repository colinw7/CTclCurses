if {! [info exists pattern]} {
  set pattern CTclCurses
  done
  return
}

catch {exec /home/colinw/dev/script/grep_src -d -f $pattern | sort -u} dirs

set n [llength $dirs]

if       {$n == 0} {
  done
  return
} elseif {$n == 1} {
  done [lindex $dirs 0]
  return
}

proc tlistIndexChangedProc { name } {
}

proc tlistExecProc { name } {
  done [$name get currentText]
}

winop window_title "Directories matching $pattern"

tlabel 0 2 "$n directories match"

set m [tlist 2 2 $dirs]

cls

$m set height 50
