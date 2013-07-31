Overview
========

Dreams is a program to display a cycle-level representation of
activity in a system, typically a processor. The dreams program
accepts a trace of activity, such as would be generated by a
simulator, that includes the time of creation/destruction of tasks or
jobs in the system, the nodes/edges in the system and the time when
tasks flow over the edges from one node to another or occupy a buffer
slot in a node. Many of these activies can also be tagged with
additional textual information such as the program counter or
instruction. The dreams program creates a graphical representation of
the activity in the system by combining that trace information and
instructions for the layout of the display. Those layout instructions
are is specified by an XML-based file, which is usually embedded in
the trace itself.

System activity can be visualized in multiple styles:

1. Resource-based display - Where each resource is represented as a
   row in the display and each column corresponds to a cycle, so that
   activity in a resource in a cycle is illustrated with a symbol in
   the resource's row and the column for the cycle where the activity
   occured.

2. Task-based display - Where each row corresponds to a task in the
   system, and activity by that task in a particular resource is
   illustrated by sequence of symbols in each of the columns
   corresponding to the cycles in which activities associated with
   that task occured.


3. Activity-based display - Where the entire 2-D canvas is used to
   represent the various resources in the system and a video player
   like control interface allows one to move step-by-step through the
   design's activity with active resources being highlighted, e.g.,
   via a color change, in the cycle they were active.

The layout langauage gives the programmer considerable power to
control presentation, such as the shape and color of the symbols. For
example a _hit_ can be colored differently than a _miss_.

Building
========

Dependenecies:
--------------

* Qt3 - version 3.0.4 or later

* Libraries from asim-core. If not installed or available in
  ../asim-core, then check out the ./confgure switches on
  how to find them.

Build procedure:
----------------

    ./configure
    make
    make install

Resources
=========

See the [Redmine project](http://asim.csail.mit.edu/redmine/projects/dreams) at asim.csail.mit.edu.
