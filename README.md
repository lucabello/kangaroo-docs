# kangaroo-docs

![Kangaroo Logo](/kangaroo-logo.png)

KangarooDocs is a collaborative text editor in C++ based on Qt framework. The application uses the rich text editor by Qt (TextEdit), modifying it and extending it to support collaborative editing using a CRDT (common replicate data type, see [Wikipedia](https://en.wikipedia.org/wiki/Conflict-free_replicated_data_type) for reference) instead of characters. This enhancement grants the system the properties of commutativity and idempotency of actions, that are very central in a collaborative text editor: 
* commutativity ensures that the state of the system is consistent through all the editors regardless of the order of the operations, so that the order in which the packets arrive doesn't matter (e.g. two letters inserted by two different users may arrive at different times, but the final outcome must be the same for all editors);
* idempotency ensures that operations that are requested multiple times only take place once (e.g. multiple editors simultaneously deleting the same character should produce only one deletion for all editors).

The client application connects to a server which stores files and allows for creating new ones; the files are shared and accessible by all the users that can connect to the server. A file can also be shared through a URI from an authenticated user; this allows unregistered temporary users to work on the file. The functionalities will be explored through the commentary of a typical use case that will be detailed below.
The central core of this project is the implementation of the class SharedEditor to substitute QTextEdit; this allows its integration with a symbol vector on which every action performed in the window is replicated. Any interaction with the editor apparently stays the same (insertion of text, style change, etc); in reality, it gets intercepted, properly augmented to fit in the collaboration context and then applied to the symbol vector first, enforcing coherence between the two. This coherence between what's visibile on screen and what is stored in the symbol vector lays the foundations for the program functionalities.
An easily noticeable complexity is brought by the "rich text" functionality, which allows to add different styles to the text; in fact, reporting those styles into the symbol vector proved to be quite a challenge. Several solutions have been explored and proven faulty: for example, simply adding a "property" to the symbols to say if they are bold or not doesn't respect the properties of commutativity and idempotency that are the core of a collaborative text editor. Our solution has style elements also represented as symbols in the symbol vector, using a system similar to HTML tags. Content is thus enclosed in symbol tags of complementary type (referred as "twin" in the code).
#### **Disclaimer**
This is a project for a university course (Programmazione di Sistema, Politecnico di Torino) and as such it's not meant to be secure or to work under any circumstance or edge-case, because many things just take time to implement and their knowledge is tested by other courses. Thus, the goal of this project is to produce a collaborative editor that works reasonably well, reasonably fast.

## **General structure**
Before venturing into the use case story, it's useful to give some information about the project structure, going through all the developed classes.
The repository articulates in three main folders with self-explanatory names: *common*, *client* and *server*. The *common* folder has classes needed by both the client and the server, specifically: `Symbol`, which extends the  plain character to a CRDT; `Message`, that models the high-level form of communication between clients and the server; `MessageQueue`, that offers a thread-safe structure to queue incoming and outgoing messages; `SharedEditor`, which is the core class and models the actual editor by replacing the Qt component QTextEdit. The *client* folder 
has a modification of the QT Rich Text Editor through the modified class `TextEdit`, that includes the new editor and intercepts the user actions; the classes `LoginWindow` and `FileListWindow` allow the authentication and file list objects to appear in the program; `LSEQAllocator` correctly assigns the needed CRDT properties to a newly inserted character; finally, `ClientSocket` allows communication between the client and the server by wrapping a multiplatform socket interface. The `main_client.cpp` file correctly connects Qt signals and slots and allows the program startup. The *server* folder's main class is `KangarooServer`, which models the actual server; `ConnectedEditor` represents an abstraction of a connected client with the goal of grouping its information; the `ServerSocket` class serves the same purpose as the Client one, which is wrapping a socket interface; `main_server.cpp` performs initializations and runs the server startup.
More information about each individual class is present in its documentation in the relative header file.
## **Client-server communication protocol**
Client and server communicate through wrappers of the QTcpSocket class. The payload of each of these communications is a serialization of the Message class. A Message can be used both to send information about an insertion/erasure of a symbol or to send control commands such as file openings or creations. Different message types (Insert, Erase, Register, Login, Create, Open, Stats, Error, List, ...) tag different message contents. Each type has its own enriching attributes (symbol for insert/erase, string for others).
For this reason, a precise protocol for the string contents in the exchanged messages needs to be defined. For example, these are few types of Message with their relative structure:
* INSERT - ERASE : Symbol payload, no string.
* REGISTER - LOGIN : Two strings, username and password.
* CREATE - OPEN : One string with filename.
* STATS : No extra information.
* ERROR : One string describing the error.
More information is available in the class documentation.

## **Typical use case**
#### *Logging in*
Opening the application, a dialog box pops up, asking the informations required to access the server and allowing login and registration operations. Specifically, the dialog box contains several textbox to the server IP address plus port, the username and the password; two buttons for login and register operations are also present.
For a registration, the server saves the username-password pair in a text file (users.txt) and gives a confirmation message to the user, that is displayed in a pop up message box.
For logging in, the server connects to a database to check the user information and see if the user is already registered. If not, an error message is displayed on the user side. If the inserted username-password pair is correct, the server assigns a siteId (a unique numerical id associated to the user) to the editor both internally (memorizing information in an internal structure) and externally (sending the siteId to the editor).
After the login operation, the following screen only allows some operations to be executed: create a new file, open an existing file, see statistics for the user, quit.
All the information about the users are kept in a database.

#### *Creating a new file / Opening a file*
When creating a new file, the real file (consisting in a file-saved serialized symbol vector) is created on the server side. Then, the local editor clears itself and sends the insertions of the basic starting symbols (style symbols that represent the empty file default styles, such as default Arial font and black color). Any request to open a file that is being created needs to be refused (easier) or delayed; a file is correctly initialized only when it has all the default symbols. When creating a file, our choice is that the access to that file is exclusive to the creation, meaning that it cannot be opened by another editor.
Opening a file consists in the editor receiving insert messages for each symbol in the server symbol vector associated to that file. During this data transmission the server can still apply modifications to the file, since the file processing (adding insert messages to the output queue) is atomic and fully completed before reading any other message from the input queue.

#### *Modifying the content of the file*
The content of the file can be modified in several ways. As mentioned before, each possible interaction with the editor must be captured, intercepted, properly replicated on symbol vector and then applied to the editor. This propagation of the actions is the key mechanism this program is based on.
Some typical interactions that modify the symbol vector are:
* key pressed on keyboard 
* mouse right key + action selected (to be disabled)
* keyboard shortcuts for copy&paste
* buttons on the interface (as alignment, bold, italic, underlined, color, font, dimension and color)
* copy&paste with shortcuts
 
Whenever some keys are pressed, the event is intercepted and analyzed. If the event is inserting a printable key, the content insertion is replicated on the symbol vector. If it is a style shortcut (as CTRL+B for bold), the style is applied to the selected text or to the word under the cursor if there is no active selection. If it is a copy-paste action, the text is copied and pasted as raw text, without formatting. If something happens that is not replicable or expected, it's simply ignored.
Propagating an action on the symbol vector means not only replicating its effects on it, but also preparing a Message for the server describing the modification that has just been performed. It is worth noting that any of this action can happen with or without an active selection of text, and that correct behaviour is implemented and replicated on the symbol vector for any case.

It is possible in the interface to see the users connected to the same file, see a colored cursor where they have their cursor and it is also possible to modify the user profile changing for example the nickname

#### *Implemented Extensions*
We implemented some extensions to the original project that were not strictly required by the assignment, but suggested:
* Export to PDF
* Rich Text
* Copy & Paste
* Invite to collaborate (URI)

<!-- #### *Multithreading*
The application operates on multiple threads both for client and server.
The client runs 4 threads:
* the **input** thread, that is woken up by the main thread when some data is available on the socket, reads the data from the socket and puts the message into the input queue
* the **message** thread, that is woken up by the input message queue when some Message is present in it, processes input messages applying their actions on the symbol vector
* the **output** thread, that is woken up by the output message queue when some Message is present in it, sends the messages in the queue to the server via socket
* the **application** thread, responsible for running the application, intercepting keypresses, pushing messsages into the output queue, and everything else;
Every action on symbol vector needs to be atomic; all messages are processed one at a time since there is only one message process thread. This could conflict with the main thread; for this reason, the functions "localInsert/localErase" must be atomic. Any of these functions should be locked to be used exclusively. The symbol vector can, in this way, not be thread-safe since every function that operates on is mutually exclusive with the others.
The server runs 3 similar threads (input, output, message). -->

## **Team members**
* Luca Bello ( https://github.com/lucabello)
* Francesco Galati ( https://github.com/manigalati )
* Alessandro Iucci ( https://github.com/alessandro-iucci )
* Angelo Russi ( https://github.com/Roger1254 )
