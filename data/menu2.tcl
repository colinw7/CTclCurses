cls

proc keyPressProc { args } {
}

proc menuIndexChangedProc { name } {
  set sync [$::check1 get checked]

  if {$sync} {
    if       {$name == $::menu1 || $name == $::check1} {
      $::menu2 set currentInd [$::menu1 get currentInd]
    } elseif {$name == $::menu2} {
      $::menu1 set currentInd [$::menu2 get currentInd]
    }
  }
}

proc menuExecProc { name } {
  set text1 [$::menu1 get currentText]
  set text2 [$::menu2 get currentText]

  done "$text1 = $text2"
}

set strs1 { One Two Three Four Five Six Seven Eight Nine Ten }
set strs2 { Un Deux Trois Quatre Cinq Six Sept Huit Neuf Dix }

set menu1 [menu 2  2 $strs1]
set menu2 [menu 2 22 $strs2]

set check1 [check 15 10]

set label1 [label 16 3 Sync]
