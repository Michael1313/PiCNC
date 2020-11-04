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
