#!/bin/csh -f

/home/colinw/bin/CTclCurses -var pattern=$* /home/colinw/dev/script/cdn.tcl -loop -ofile .cdn

exit 0
