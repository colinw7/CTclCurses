cls

set i1 [tinput 2 2]
set l1 [tlabel 6 2]

$i1 set text "One Two Three Four Five Six Seven Eight"

proc tinputExecProc { name } {
  $::l1 set text [$::i1 get text]
}
