cls

set strs { One Two Three Four Five Six Seven Eight Nine Ten }

set l [tlist 2 2 $strs]

proc tlistIndexChangedProc { name } {
}

proc tlistExecProc { name } {
  done [$name get currentText]
}
