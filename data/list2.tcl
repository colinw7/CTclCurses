cls

proc keyPressProc { args } {
}

proc tlistIndexChangedProc { name } {
  set sync [$::check1 get checked]

  if {$sync} {
    if       {$name == $::list1 || $name == $::check1} {
      $::list2 set currentInd [$::list1 get currentInd]
    } elseif {$name == $::list2} {
      $::list1 set currentInd [$::list2 get currentInd]
    }
  }
}

proc tlistExecProc { name } {
  set text1 [$::list1 get currentText]
  set text2 [$::list2 get currentText]

  done "$text1 = $text2"
}

proc tcheckStateChangedProc { name } {
}

set strs1 { One Two Three Four Five Six Seven Eight Nine Ten }
set strs2 { Un Deux Trois Quatre Cinq Six Sept Huit Neuf Dix }

set list1 [tlist 2  2 $strs1]
set list2 [tlist 2 22 $strs2]

set check1 [tcheck 15 10]

set label1 [tlabel 16 3 Sync]
