# Virtual File System

A virtual file system where a user can create/delete/move folders and files,
among other operations described below. The filesystem is organized as a Tree. 
Each inode contains metadata about the node (e.g., file or folder, size, date). A
folder can have zero or many folders and files, while a file is considered as a leaf node only.
Deleting a file puts the inode’s element in a limited size queue (the Bin/Trash) which can be
emptied manually or automatically. The interface allows a user to navigate and perform
the described tasks below. At the beginning and end of the session, the file system is loaded,
and dumped to the disk, respectively.

1. help
○ Prints the following menu of commands.
2. pwd
○ Prints the path of current inode
3. realpath filename
○ Prints the full path of a given file
4. ls
○ Prints the children of the current inode (if folder, error otherwise)
○ Each line shows: filetype (dir/file), filename, size, date
○ ls sort: order by descending file size
5. mkdir foldername
○ Creates a folder under the current folder
6. touch filename size
○ Creates a file under the current inode location with the specified filename, size,
and current datetime
7. cd
○ cd foldername: change current inode to the specified folder
○ cd filename: return an error
○ cd .. changes current inode to its parent folder
○ cd - changes current inode to the previous folder
○ cd changes current inode to root.
○ cd /my/path/name changes the current inode to the specified path if it exists
8. (find foldername) or (find filename)
○ Returns the path of the file (or the folder) if it exists
○ You should print all matching paths. The same name may exist in different
locations
○ Starts the search from the root folder
9. mv filename foldername
○ Moves a file located under the current inode location, to the specified folder path
○ The specified file and folder have to be one of the current inode’s children (an
error is returned otherwise)
10. (rm foldername) or (rm filename)
○ Removes the specified folder or file and puts it in a Queue of MAXBIN=10
○ The specified file or folder has to be one of the current inode’s children (an error
is returned otherwise)
11. Implement mv and rm on arbitrary inode locations.
○ The inode and destination folder are specified using a path
12. (size foldername) or (size filename)
○ Returns the total size of the folder (including all its subfiles), or the size of the file
13. emptybin
○ Empties the bin
14. showbin
○ Shows the oldest inode of the bin (including its path)
15. recover
○ Reinstates the oldest inode back from the bin to its original position in the tree (if
the path doesn’t exist anymore, an error is returned)
16. Exit
○ The program stops and the filesystem is saved in the described format
