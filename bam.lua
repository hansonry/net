settings = NewSettings()
source = Collect("*.c");
if family == "windows" then
   sep = "\\"
   settings.debug = 0
   settings.cc.flags:Add("/MD");
   settings.link.flags:Add("/SUBSYSTEM:CONSOLE");   
   settings.link.libs:Add("ws2_32");
else
   sep = "/"
   settings.cc.flags:Add("-Wall");
end


objects = Compile(settings, source)
exe = Link(settings, "net_test", objects)
