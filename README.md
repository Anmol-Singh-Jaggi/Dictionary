# Dictionary

- **What is it exactly ?**
 - It is an application created to help increase your vocabulary.
 - You can enter new word-meaning pairs as you learn new words.
 - It can then take your test by showing you any random word and asking for its meaning.
  
- **Which platforms are supported ?**
 - Linux [ Tested On Ubuntu 13.10 ]
 - Windows [ Tested on Windows 7 ]
  
- **How to compile it ?**
 - This program was has been tested only on GCC 4.9.2. But it should work on other C++ compilers too.
 - This program requires the [Boost][1] [Filesystem][2] library.
 - Check you compiler and the Boost documentation for the exact compilation instructions.
  
- **How to enter a word ?**  
 - A *word* can be a multi-word string, unlike what its name suggests.
 - When the program asks to enter a query, type a string and press **`Enter`**.

- **How to enter a meaning ?**
 - A *meaning* can be a multi-line string.
 - When you are done writing the meaning, just press **`Enter`** twice.
 - If the entered meaning is empty, the word-meaning pair will be discarded.

- **What functionality/commands are supported ?**
 - Type *`help`* to show a list of commands when it asks for a query.
  
- **How to exit ?**
 - Just type *`exit`* when it asks to enter a query.
 - Do not click on the close button of the console window, or else all the data in the dictionary file will be lost.

**A sample dictionary is included with the program.** 

[1]:http://www.boost.org
[2]:http://www.boost.org/doc/libs/1_57_0/libs/filesystem/doc/index.htm
