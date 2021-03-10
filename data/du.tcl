catch {glob *} dirs

set usages {}

foreach dir $dirs {
  lappend usages {........}
}

set t [ttable 2 2 [list $dirs $usages]]

proc ttableIndexChangedProc { name } {
}

proc ttableExecProc { name } {
  done [$name get currentText]
}

set nr [llength $dirs]

set row 0

foreach dir $dirs {
  catch "exec /usr/bin/du -sk $dir" size_name

  set size [lindex $size_name 0]

  $t set item $row 1 $size

  incr row
}
