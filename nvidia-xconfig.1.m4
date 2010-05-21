dnl This file is to be preprocessed by m4.
changequote([[[, ]]])dnl
define(__OPTIONS__, [[[include([[[options.1.inc]]])dnl]]])dnl
.\" Copyright (C) 2005-2010 NVIDIA Corporation.
.\"
__HEADER__
.TH nvidia\-xconfig 1 "__DATE__" "nvidia\-xconfig __VERSION__"
.SH NAME
nvidia-xconfig \- manipulate X configuration files for the NVIDIA driver
.SH SYNOPSIS
.B nvidia-xconfig
[
.I options
]
.SH DESCRIPTION
.PP
.B nvidia-xconfig
is a tool intended to provide basic control over configuration options available in the NVIDIA X driver.
.PP
.B nvidia-xconfig
performs its operations in several steps:
.TP
1)
The system X configuration file is found and read into memory.
If no configuration file can be found,
.B nvidia-xconfig
generates one from scratch using default settings; in this case, 
.B nvidia-xconfig 
will automatically determine the name of the X 
configuration file to create:
.I /etc/X11/xorg.conf
if the X server
in use is X.org or 
.I /etc/X11/XF86Config
if the X server in use is XFree86.
.TP
2)
The configuration in memory is modified to support the NVIDIA driver.
This consists of changing the display driver to "nvidia", removing the commands to load the "GLcore" and "dri" modules, and adding the command to load the "glx" module.
.TP
3)
The configuration in memory is modified according to the options specified on the command line.
Please see the NVIDIA README for a description of the NVIDIA X configuration file options.
Note that
.B nvidia-xconfig 
does not perform any validation of the X configuration file options requested on the command line;
X configuration file option validation is left for the NVIDIA X driver.
.TP
4)
The configuration is written back to the file from which it was read.
A backup of the original configuration is created with "\.backup" appended.
For example, if your X configuration is
.I /etc/X11/xorg.conf
then
.B nvidia-xconfig
will copy it to
.I /etc/X11/xorg.conf.backup
before writing the new configuration.
The
.B \-\-post\-tree (\-T)
option can be used to print the new configuration to standard out in tree form instead.  This option is useful to see what
.B nvidia-xconfig
will do while leaving the original configuration intact.
dnl Call gen-manpage-opts to generate this section.
__OPTIONS__
.SH EXAMPLES
.TP
.B nvidia-xconfig
Reads an existing X config file and adapts it to use the NVIDIA driver.
If no X config file can be found, a new one is created at /etc/X11/XF86Config with default settings.
.TP
.B nvidia-xconfig \-\-post\-tree \-\-twinview
Reads the existing X configuration file, adds the TwinView option, and then prints the resulting config file to standard out in tree form.
The configuration file is not modified.
.TP
.B nvidia-xconfig \-\-enable\-all\-gpus
Examines the system and configures an X screen for each display device it finds.
.TP
.BI "nvidia-xconfig \-\-mode=" 1600x1200
Adds a 1600x1200 mode to an existing X configuration.
.TP
.BI "nvidia-xconfig \-\-mode-list=" "1600x1200 1280x1024"
Removes any existing modes from the X configuration file, replacing them with "1600x1200" and "1280x1024".
.TP
.BI "nvidia-xconfig \-\-metamodes=" "1024x768 +0+0, 1024x768 +1024+0"
Adds the MetaMode "1024x768 +0+0, 1024x768 +1024+0" to the existing X configuration file, replacing any existing MetaModes X configuration option.
.TP
.B nvidia-xconfig \-\-only\-one\-x\-screen \-\-sli=Auto
Configures the X server to have just one X screen that will use SLI when available.
.\" .SH FILES
.\" .I /etc/X11/XF86Config
.\" .I /etc/X11/xorg.conf
.SH AUTHOR
Aaron Plattner
.br
NVIDIA Corporation
.SH "SEE ALSO"
.BR nvidia-settings (1),
.I /usr/share/doc/NVIDIA_GLX-1.0/README.txt
.SH COPYRIGHT
Copyright \(co 2005-2010 NVIDIA Corporation.
