proc tcheckStateChangedProc { name } {
  if       {$name == $::iv} {
    ttystate set inverse_video [$::iv get checked]
  } elseif {$name == $::ic} {
    ttystate set show_cursor [$::ic get checked]
  } elseif {$name == $::iv} {
    ttystate set blink_cursor [$::ib get checked]
  }
}

raw

winop window_title "Tty State"

set iv [tcheck 2 2]
tlabel 3 6 "Inverse Video"

set ic [tcheck 5 2]
tlabel 6 6 "Show Cursor"

set ib [tcheck 8 2]
tlabel 9 6 "Blink Cursor"

cls
