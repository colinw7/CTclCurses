catch {exec /home/colinw/dev/script/grep_src -d -f $pattern | sort -u} dirs

set n [llength $dirs]

if       {$n == 0} {
  done
  return
} elseif {$n == 1} {
  done [lindex $dirs 0]
  return
}

proc redrawProc { } {
}

proc keyPressProc { key name } {
}

proc menuIndexChangedProc { name } {
}

proc menuExecProc { name } {
  done $name
}

raw

cls

winop window_title "Directories matching $pattern"

label 0 2 "$n directories match"

set m [menu 2 2 $dirs]

cls

$m set height 20
