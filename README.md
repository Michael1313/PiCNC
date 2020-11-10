# PiCNC
<p>Raspberry pi CNC machine.</p>
<p>This project consist of three parts: PiCNC main program, PiCNCEngraver for engrave bitmap images and PiCNCServer for controlling machine from mobile phone over wifi.</p>
<p>Each this part interacts whith kernel module that directly manage stepper motors over his drivers.</p>
<p>All this programs written and tested only on raspberry pi 3 model B.</p>
<p><b>PiCNC interface</b><br>
<img src="https://github.com/Michael1313/PiCNC/blob/main/screenshots/PiCNC.png" alt="raspberry pi CNC machine"></p>
<p><b>PiCNCEngraver interface</b><br>
<img src="https://github.com/Michael1313/PiCNC/blob/main/screenshots/PiCNCEngraver.png" alt="raspberry pi CNC machine"></p>
<p><b>PiCNCServer (mobile) web interface</b><br>
<img src="https://github.com/Michael1313/PiCNC/blob/main/screenshots/PiCNCServer.jpg" alt="raspberry pi CNC machine"></p>
<p><b>PiCNCServer (mobile) web interface interface</b><br>
<img src="https://github.com/Michael1313/PiCNC/blob/main/screenshots/PiCNCManualControl.jpg" alt="raspberry pi CNC machine"></p>
<p><b>Raspberry pi pin out</b></p>
<pre>

XStep 19
XDir  13
XMin  6
XMax  5

YStep 16
YDir  12
YMin  7
YMax  8

ZStep 11
ZDir  9
ZMin  10
ZMax  22

Spdl  18
</pre>
<h2>Installation</h2>
<h3>User space</h3>
  1. Download installation package.
  2. Open a terminal and go to package folder.
  <code>cd /package folder</code>
  3. Make
  <code>dpkg -i PiCNC.deb</code>
  By default the PiCNCServer installing in "/usr/bin" directory. And You can run the server with the icon in main manu. Or start the server automaticly at the boot time, please folove <a href="https://www.dexterindustries.com/howto/run-a-program-on-your-raspberry-pi-at-startup/">this guide.</a>
<h3>Kernel space</h3>
