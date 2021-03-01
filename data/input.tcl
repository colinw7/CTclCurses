cls

set i1 [input 2 2]
set l1 [label 6 2]

$i1 set text "One Two Three Four Five Six Seven Eight"

proc inputExecProc { name } {
  $::l1 set text [$::i1 get text]
}
