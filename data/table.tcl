cls

set col1 { One Two Three Four Five Six Seven Eight Nine Ten }
set col2 { Un Deux Trois Quatre Cinq Six Sept Huit Neuf Dix }

set l [ttable -row 2 -col 2 -cols [list $col1 $col2]]

proc ttableIndexChangedProc { name } {
}

proc ttableExecProc { name } {
  done [$name get currentText]
}
