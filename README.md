# xs8Keylogger

Stealth Low-Level Hook Keylogger (Also with screenshots!)

## Features
●Compresses everything in a zip archive (you can also set a password) and send the archive to you by e-mail using a trusted Windows process (mshta.exe)<br /><br />
●Screenshot of the screen when the left mouse button is pressed<br />(you can set the dimension and how often the program can take a ss)<br /><br />
●Dead keys support (Will not work with IME)<br /><br />
●Unicode characters support<br />
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;e.g: if the victim uses a US keyboard layout and press SHIFT+2, the keylogger will read the correct
		 character (@).
		 <br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;if the victim uses a IT keyboard layout, the keylogger will catch (")<br /><br />
●Write to the log also the focused window title<br /><br />
●Reads the clipboard when CTRL+C and CTRL+V are pressed (No "Copy" mouse support)
 <br /><br />
### Prerequisites
You need to use this small command to can launch the VBS payload who will launch the "sendEmail.vbs" file<br />
[MSHTA VBScript Download & Execute](src/sendEmail.vbs)
 <br /><br />
```
zip.h  | Lucian Wischik
```
http://www.wischik.com/lu/programmer/zip_utils.html
```
base64 | René Nyffenegger
```
https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp
## License
see the [LICENSE](LICENSE) file for details

## This program was written only as a personal challenge. I don't assume any responsibility about how you will use it.
