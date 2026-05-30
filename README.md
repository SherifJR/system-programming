this is a File Manager based on system calls in C, I built 2 versions, the second is more optimized and have edit feature<br>
at the moment, it can create/delete/open/edit a file ,BUT edit need to be enhanced <br>
I will work to enhance edit and make it more user-friendly and also add GUI. <br>

<br> compilation and run of v1 and v2
<br>to compile: gcc file_manipulator.c -o v 
<br>to run: ./v <br>

<br><br>
Now at v3 you can edit by determining the line number then edit it and it will be saved
compile and run v3:
<br> compile: gcc FMv2.c -lreadline -o fm       we use -lreadline due to the usage of <readline/readline.h>, so we need to link and compile it  
<br> run: ./fm
