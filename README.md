# Arduino-Debug

This library provides a simple on-target debugger for Arduino sketches.
Debug command are added directly to the sketch. A debugger command
shell is started on break-points and assertions.
![screenshot](https://dl.dropboxusercontent.com/u/993383/Cosa/screenshots/Screenshot%20from%202016-02-07%2021%3A48%3A33.png)

## Install

Download and unzip the Arduino-Debug library into your sketchbook
libraries directory. Rename from Arduino-Debug-master to Arduino-Debug.

The Debug library and examples should be found in the Arduino IDE
File>Examples menu.

## Commands

### Sketch Commands
Command | Description
--------|------------
ASSERT(cond) | Check assert condition. If false the debug shell is called. The sketch cannot continue.
BREAKPOINT() | The debug shell is called.
BREAK_IF(cond) | The debug shell is called if the condition is true.
CHECK_STACK(room) | Check that there is room (bytes) on the stack. If false the debug shell is called.
DEBUG_STREAM(dev) | Use the given stream device for debug session. Typically Serial.
OBSERVE(expr) | Print the expression to the debug stream.
OBSERVE_IF(cond,expr) | Print the expression to the debug stream if the condition is true.
REGISTER(var) | Register a variable for access from the debug shell.

### Debug Shell Commands
Command | Description
--------|------------
?VARIABLE | Print variable address and value.
@VARIABLE | Print pointer variable address and reference value.
backtrace | Print simple call-stack.
commands | Print list of commands (see also help).
data | Print contents of data area, i.e. global variables.
go | Leave debug shell and continue execution of sketch.
heap | Print contents of heap, i.e. dynamic allocated data.
help | Print list of commands.
memory | Print memory status.
quit | Stop sketch.
stack | Print contents of stack, i.e. call frames, arguments, return addresses.
variables | Print list of registered variables.
where | Print source code file and line where debug shell was called.
