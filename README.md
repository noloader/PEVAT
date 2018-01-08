# PEVAT

The Process Event Vulnerability Assessment Tool (PEVAT) can but used to analyze and disrupt IPC and synchronization between userland programs and privileged components like drivers and the kernel.

PEVAT was developed for testing Windows Antivirus software. The tests involved manipulating shared Windows events, like an event that could be set by the UI task bar widget (unprivileged) to signal the AV scanner to shutdown (privileged component).

PEVAT suscessfully attacked 22 or 23 Windows antivirus suites. It can attack named and unnamed events. The tool also has an automated testing mode to test all events in a process.

A paper is available called "Old Dogs and New Tricks - Do You Know Where Your Handles Are?" It is included with the source code.

Another file is also available. It is called ProtectProcess.rar. The sample code in ProtectProcess shows you how to secure your process's DACL to help avoid the manipulations. The code mimics a Windows [Protected Process](http://download.microsoft.com/download/a/f/7/af7777e5-7dcd-4800-8a0a-b18336565f5b/process_vista.doc), which was introduced for Digital Rights Management (DRM) and protecting rich content.