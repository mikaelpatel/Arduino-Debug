# Arduino-Debug

This library provides a simple on-target debugger for Arduino sketches.
Debug command are added directly to the sketch. A debugger command
shell is started on break-points and assertions.

## Install

Download and unzip the Arduino-Debug library into your sketchbook
libraries directory. Rename from Arduino-Debug-master to Debug.

The Debug library and exampls should be found in the Arduino IDE
File>Sketchbook menu.

## Commands
### Sketch Commands
#### ASSERT(cond)
#### BREAKPOINT()
#### BREAK_IF(cond)
#### CHECK_STACK(room)
#### DEBUG_STREAM(dev)
#### OBSERVE_IF(cond,expr)
#### OBSERVE(expr)
#### REGISTER(var)
### Debug Shell Commands
#### ?VARIABLE
#### backtrace
#### commands
#### data
#### go
#### heap
#### help
#### memory
#### quit
#### stack
#### variables
#### where




