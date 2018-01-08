# PEVAT

The Process Event Vulnerability Assessment Tool (PEVAT) can but used to analyze IPC and synchronization between userland programs and privileged components like drivers and the kernel.

PEVAT was developed testing Windows Antivirus software. The tests involved manipulating shared Windows events, like an event that could be set by the userland task bar widget (unprivileged) to signal the AV scanner to shutdown (privileged component).

PEVAT suscessfully attacked 22 or 23 Windows antivirus suites. A paper is available called "Old Dogs and New Tricks - Do You Know Where Your Handles Are?"
