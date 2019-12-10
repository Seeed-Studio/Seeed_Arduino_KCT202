# Seeed Arduino KCT202

## Introduction
The KCT202 is a integrated fingerprint,which based on BYD's fingerprint sensor BF5325 and a cortex M4 MCU GD32FFPRTG


## Notice
* Since atmega328(like arduino UNO) has only one serial port,The library use hardware serial port to output log info,and control the fingerprint sensor via softserial(2,3)，So the grove cable should attach to (D2,D3) port. The log serial's baudrate default 115200,The transport serial's baudrate fixed 57600.   


* When using the samd board(like arduino zero,seeeduino cortex M0+),the SerialUSB for printing log info.The Serial(defined in library)for send AT command.  



***
This software is written by downey  for seeed studio<br>
Email:baozhu.zuo@gmail.com
and is licensed under [The MIT License](http://opensource.org/licenses/mit-license.php). Check License.txt for more information.<br>

Contributing to this software is warmly welcomed. You can do this basically by<br>
[forking](https://help.github.com/articles/fork-a-repo), committing modifications and then [pulling requests](https://help.github.com/articles/using-pull-requests) (follow the links above<br>
for operating guide). Adding change log and your contact into file header is encouraged.<br>
Thanks for your contribution.

Seeed Studio is an open hardware facilitation company based in Shenzhen, China. <br>
Benefiting from local manufacture power and convenient global logistic system, <br>
we integrate resources to serve new era of innovation. Seeed also works with <br>
global distributors and partners to push open hardware movement.<br>



