# kangaroo-docs

![Kangaroo Logo](/kangaroo-logo.png)

KangarooDocs is a collaborative text editor in C++ based on Qt framework. The application uses the rich text editor by Qt (TextEdit), modifying it and extending it to support collaborative editing using a CRDT (common replicate data type) instead of characters.
The applicaton connects to a server which stores accessible files and allows for creating new ones. After authenticating with the server, applications can work either on the same file or on different files.
The SharedEditor class substitutes QTextEdit, integrating it with a symbol vector on which every action is replicated. While every interaction with the editor stays the same (insertion of text, style change, etc), each is applied to the symbol vector first, enforcing coherence between the two.
Style elements are also represented as symbols in the symbol vector, using a system similar to HTML tags. Content is enclosed in symbol tags of complementary (twin) type.
Whenever the text is modified in some way, the editor sends a message to the server commmunicating what change happened. The server applies those changes to its internal copy of the file and propagates the message to all the other editors working on the same file.
Commutativity and idempotency are guaranteed by the CRDT, implying the correct functioning of the application.
This is a project for a university course (Programmazione di Sistema, Politecnico di Torino) and as such it's not meant to be secure or to work under any circumstance. It is meant to be working reasonably well, reasonably fast.

## **Typical use case**
#### *Logging in*
Opening the application, a dialog box pops up, asking the informations required to access the server and allowing login and registration operations. Specifically, the dialog box contains a textbox for the server IP address plus port, another textbox for username and another textbox for password; two buttons for login and register operations are also present.
For a registration, the server saves the username-password pair in a text file (users.txt) and gives a confirmation message to the user, that is displayed in a pop up message box.
For logging in, the server opens up the file where username-password pairs are stored (users.txt) and looks if the inserted data match with some entry in the file. If not, an error message is displayed on the user side. If the inserted username-password pair is correct, the server assigns a siteId (associated to the user) to the editor both internally (memorizing information in an internal structure) and externally (sending the siteId to the editor).
After the login operation, the editor is disabled. Only some operations are allowed: create a new file, open an existing file, see statistics for the user, quit.
#### *Client-server communication protocol*
Client and server communicate through wrappers of the QTcpSocket class. The payload always is a serialization of the Message class. A Message can be used both to send information about an insertion/erasure of a symbol or to send control commands such as file openings or creations. Different message types (Insert, Erase, Register, Login, Create, Open, Stats, Error, List, ...) tag different message contents. Each type has its own enriching attributes (symbol for insert/erase, string for others).
For this reason, a precise protocol for the string contents in the exchanged messages needs to be defined.
* INSERT - ERASE : Symbol payload, no string.
* REGISTER - LOGIN : Two strings, username and password.
* CREATE - OPEN : One string with filename.
* STATS : No extra information.
* ERROR : One string describing the error.
#### *Opening a file / Creating a new file*
When creating a new file, the real file (consisting in a file-saved serialized symbol vector) is created server side. Then, the local editor clears itself and sends the insertions of the basic starting simbols (such as default Arial font and black color). Any request to open a file that is being created needs to be refused (easier) or delayed; a file is correctly initialized only when it has all the default symbols. When creating a file, the access to that is exclusive to the creation (it cannot be opened by another editor).
Opening a file consists in the editor receiving insert messages for each symbol in the server symbol vector associated to that file. During this data transmission the server can still apply modifications to the file, since the file processing (adding insert messages to the output queue) is atomic and fully completed before reading any other message from the input queue.
#### *Modifying the content of the file*
The content of the file can be modified in several ways. For the application to work properly, each of them must be intercepted and propagated to the symbol vector.
Interactions modifying the symbol vector are:
* key pressed on keyboard 
* mouse right key + action selected (to be disabled)
* keyboard shortcuts for styling
* buttons on the interface (as alignment, bold, etc)
* copy&paste (disabled)
Whenever some keys are pressed, the event is intercepted and analyzed. If the event is inserting a printable key, the content insertion is replicated on the symbol vector. If it is a style shortcut (as CTRL+B for bold), the style is applied to the selected text or to the word under the cursor if there is no active selection. If it is a copy-paste action or something else, it is ignored.
Propagating an action on the symbol vector means replicating its effects on it and preparing a Message for the server describing the modification that has just been performed. It is notable that any of this action can happen with or without an active selection of text, and that correct behaviour is implemented and replicated on the symbol vector for any case.
#### *Multithreading*
The application operates on multiple threads both for client and server.
The client runs 4 threads:
* the **input** thread, that is woken up by the main thread when some data is available on the socket, reads the data from the socket and puts the message into the input queue
* the **message** thread, that is woken up by the input message queue when some Message is present in it, processes input messages applying their actions on the symbol vector
* the **output** thread, that is woken up by the output message queue when some Message is present in it, sends the messages in the queue to the server via socket
* the **application** thread, responsible for running the application, intercepting keypresses, pushing messsages into the output queue, and everything else;
Every action on symbol vector needs to be atomic; all messages are processed one at a time since there is only one message process thread. This could conflict with the main thread; for this reason, the functions "localInsert/localErase" must be atomic. Any of these functions should be locked to be used exclusively. The symbol vector can, in this way, not be thread-safe since every function that operates on is mutually exclusive with the others.
The server runs 3 similar threads (input, output, message).

## **Classes**
For classes description, it is advised to read the respective headers.

## **TODO list**
- [ ] Create dialog box for login on client
- [ ] Implement login process on server, also adding informations in the ConnectedEditor structure
- [ ] Implement login process on client, also receiving and setting proper siteId
- [ ] Impose login and file selection/creation before enabling the editor
- [ ] Fix the "SharedEditor::process" function on client
- [ ] MessageQueue, FIFO, to be shared by input-message threads and output-application threads
- [ ] Implement serialization mechanism using Qt
- [ ] Copy constructor, assignment and destructor where appropriate
- [ ] Check what functions should be tagged as "const"
- [ ] SharedEditor: set the proper siteCounter when opening/creating a file
- [ ] After correcting serialization, be SURE that no memory leaks happen
- [ ] Multithreading, locking localInsert/localErase functions
- [ ] Multithreading in general, creating threads etc
