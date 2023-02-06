cd /home/colinw/dev/pics

set files [glob *.jpg]

set n   [llength $files]
set ind 0

set ::iw [timage 0 0 200 100 ""]

proc nextImage { } {
  incr ::ind

  if {$::ind >= $::n} {
    set ::ind 0
  }

  redrawProc
}

proc keyPressProc { args } {
  nextImage
}

proc mousePressProc { args } {
  nextImage
}

proc redrawProc { } {
  cls

  set file [lindex $::files $::ind]

  winop window_title "Image $file"

  $::iw set file $file

  move 51 0
}

redrawProc
