#!/bin/csh -f

set header_type = ""
set table       = ""

while ($#argv > 0)
  if      ("$1" == "-comment_header") then
    set header_type = "comment"
    shift
  else if ("$1" == "-first_line_header") then
    set header_type = "first_line"
    shift
  else
    set table = $1
    shift
  endif
end

/home/colinw/bin/CTclCurses -var header_type=$header_type -var table=$table csv.tcl -loop

exit 0
