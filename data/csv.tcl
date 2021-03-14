if {$header_type == "header"} {
  catch {exec //home/colinw/bin/CCsvTest -first_line_header -tcl $table} rows
} elseif {$header_type == "comment"} {
  catch {exec //home/colinw/bin/CCsvTest -comment_header -tcl $table} rows
} else {
  catch {exec //home/colinw/bin/CCsvTest -tcl $table} rows
}

if {[llength $rows] == 0} {
  done
}

raw

cls

if {$header_type != ""} {
  set l [ttable -row 2 -col 2 -height 40 -rows $rows -header]
} else {
  set l [ttable -row 2 -col 2 -height 40 -rows $rows]
}

proc ttableIndexChangedProc { name } {
}

proc ttableExecProc { name } {
  done [$name get currentText]
}
