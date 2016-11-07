# BF_CDThread

This is a cooperative scheduler with messages and events capability that is compiled with Analog devices visualDSP 5.x compiler
 for the Blackfin DSP, but is usable with few modifiations in any 32bit processor.

I had created it to solve a simple problem, have a system that allow me to manage complex code in task as the VDK (O.S.)
 without add heavy and great resource use. Typically I use it inside one thread of VDK to perform runtime analysis and debugging.
One of the key of this scheduler is that is statically configurable (no dynamic memory allocation) for the CDthreads, CDEvents,
 CDmessages, cdStream and Roundstream, so directly at compile time is possible to evaluate the memory footprint.
Also the scheduler is very simple and could be re-coded to solve different application constrain.

At today I have used it in many project but there is a great problem with documentation that is not updated and present only
at the starting of the header, and some source, file. Please se the cdthread.h to understand how the code work

Good work
