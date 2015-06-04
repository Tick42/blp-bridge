Tick42 BLP Bridge for OpenMama - binary release

To run the binary release you will need install on a machine with a logged in bloomberg terminal or have access to a bloomberg sapi or managed-bpipe service.

In either case you will need to configure a mama.properties file. The binary release contains a minimal mama.properties that is sufficient to run the mamalistenc.exe sample in this release.

1. Set an environment variable "WOMBAT_PATH" to the directory containing your mama.properties file

2. Edit mama.properties to set the hostname and port for your bloombger connection. The example version of the file is correct for a normal local bloombger terminal

mama.tick42blp.transport.blp_tport.url=localhost:8194

you should only have to change this setting if you are connecting to a sapi or managed bpipe service.


mamalistenc.cmd provides an example command line to request a default set of real-time data fields for VOD LN Equity


Please refer to the document "Tick42 BLP Bridge for Open Mama.pdf" for details of how to request reference data and to specify the set of fields

