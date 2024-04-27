These tools are designed specifically to work with the web_for_pentester_i386.iso. You must have this VM hosted locally and must be networked to your attacking machine.

The usage for both lab3sh and lab3sqli is ./<program_name> <IPaddress>

Lab3sh allows you to execute Linux commnads on the target machine and return the output.

Lab3sqli has the below functionality and syntax:
dbs → list databases
tables <database> → list tables for given DB
columns <database> <table> → list columns for given DB and table
dump <database> <table> → dump table content
