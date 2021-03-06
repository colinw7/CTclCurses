cls

set iv [check 2 2]
label 3 6 "Inverse Video"

set ic [check 5 2]
label 6 6 "Show Cursor"

set ib [check 8 2]
label 9 6 "Blink Cursor"

proc checkStateChangedProc { name } {
  if       {$name == $::iv} {
    ttystate set inverse_video [$::iv get checked]
  } elseif {$name == $::ic} {
    ttystate set show_cursor [$::ic get checked]
  } elseif {$name == $::iv} {
    ttystate set blink_cursor [$::ib get checked]
  }
}
