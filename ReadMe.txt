connection tools:
1.mygrep
2.mywc
3.mycat (pass)
4.myls
5.myfind
6.mystat
7.myjobs
//
environment variable:
PATH = /home/bo-yang/bin:/usr/local/bin
search "/home/bo-yang/bin" first then search "/usr/local/bin"
05/29/2026
I add both history and mycat("cat") to my project, since I add Command Line utility I have to add
this executable file to my own path (mkdir ~/bin), (~ = /home/bo-yang), and add my own path to the 
environment variable $PATH I use : [export PATH="$HOME/bin:$PATH"]
($HOME = /home/bo-yang, $PATH = recent path)to temporary change my $PATH , 
[echo 'export PATH="$HOME/bin:$PATH"' >> ~/.bashrc] for change $PATH forever.

.bashrc is a Bash startup configuration script.
when a new Bash shell starts, it automatically reads and executes the command inside .bashrc.
The command inside bashrc are regular Bash commands or built-in shell commands.
The export command is a bash built-in command used to create or modify environment variables.

