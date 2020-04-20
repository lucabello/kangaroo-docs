#include "KangarooServer.h"
#include <fstream>
#include <QDir>
#define FILES_DIRNAME "textfiles"

KangarooServer::KangarooServer(QObject *parent) :
    QObject(parent)
{
    server = new QTcpServer(this);
    // whenever a user connects, it will emit signal
    connect(server, SIGNAL(newConnection()),
            this, SLOT(newConnection()));

    if(!server->listen(QHostAddress::Any, 9999))
    {
        qDebug() << "Server could not start";
    }
    else
    {
        qDebug() << "Server started!";
    }
    openDBConnection();
}

KangarooServer::KangarooServer(std::string a, unsigned short p) :
    QObject(nullptr)
{
    server = new QTcpServer(this);

    // whenever a user connects, it will emit signal
    connect(server, SIGNAL(newConnection()),
            this, SLOT(newConnection()));

    QHostAddress addr;
    addr.setAddress(QString::fromStdString(a));
    if(!server->listen(addr, p))
    {
        qDebug() << "[ERR] Server could not start";
    }
    else
    {
        qDebug() << "[SERVER] Server started - " << QString::fromStdString(a) << ":" << p;
    }
    openDBConnection();
}

KangarooServer::~KangarooServer(){
    usersDB.close();
    delete server;
}

bool KangarooServer::openDBConnection(){
    usersDB = QSqlDatabase::addDatabase("QSQLITE");
    usersDB.setDatabaseName("users.sqlite");
    //    usersDB.setUserName("kangarooserver");
    //    usersDB.setPassword("kangarooPSW");
    //    usersDB.setHostName("kangaroo");
    if (!usersDB.isValid()) {
        qDebug() << "[KangarooServer] - DB not valid "+usersDB.lastError().driverText();
        return false;
    }
//    if (!usersDB.isOpen() ){
//        qDebug() << "[KangarooServer] - Failed to connect to driver";
//        return false;
//    }
    if( !usersDB.open() ){
        qDebug() << "[KangarooServer] - Failed to connect to open the users database " + usersDB.lastError().driverText();
        return false;
    }
    createUsersDB();

    qDebug() << "[KangarooServer] - Connected to the users DB ";
    return true;
}

void KangarooServer::createUsersDB(){
    qDebug() << usersDB.lastError().text();
    QString query = "CREATE TABLE kangaroo_users (Username VARCHAR(30) PRIMARY KEY NOT NULL, Password VARCHAR(30) NOT NULL, Nickname VARCHAR(30))";
    QSqlQuery q = usersDB.exec(query);
    if(!q.isValid()){
        qDebug() << "[KangarooServer] - DB exists";
//        qDebug() << usersDB.lastError().text();
    }
    else
        qDebug() << "[KangarooServer] - Users table created";
}

void KangarooServer::changeNickname(Message message){
    QString username = message.getCommand().split(',')[0];
    QString nickname = message.getCommand().split(',')[1];
    QSqlQuery q = usersDB.exec("UPDATE kangaroo_users SET Nickname = '"+nickname+"' WHERE Username = '"+username+"'");
    qDebug() << "[KangarooServer] - last query : " << q.lastQuery();
}

void KangarooServer::newConnection()
{
    qDebug() << "[SERVER] Connection received!";
    ServerSocket *mySocket = new ServerSocket(server->nextPendingConnection());
    connect(mySocket, &ServerSocket::signalMessage,
            this, &KangarooServer::incomingMessage);
    connect(mySocket, &ServerSocket::hostDisconnected,
            this, &KangarooServer::hostDisconnected);

    emit mySocket->connected();
    qintptr descriptor = mySocket->getDescriptor();
    ConnectedEditor ce(mySocket);
    descriptorToEditor.insert({descriptor, ce});
    qDebug() << "[SERVER] Editor info created for " << descriptor << "!";
    qDebug() << "[SERVER] socket state: " << mySocket->isConnected();
}

void KangarooServer::incomingMessage(qintptr descriptor,Message message){
    switch(message.getType()){
        case MessageType::Erase:
        case MessageType::Insert:
        {
            QString filename = descriptorToEditor.at(descriptor).getWorkingFile();
            modifyFileVector(message, filenameToSymbols.at(filename));
            propagate(descriptor,message);
            break;
        }
        case MessageType::Login:
            doLogin(descriptor,message);
            break;
        case MessageType::Register:
            doRegister(descriptor,message);
            break;
        case MessageType::Create:
            doCreate(descriptor, message);
            break;
        case MessageType::Open:
            doOpen(descriptor, message);
            break;
        case MessageType::FileList:
            sendFileList(descriptor);
            break;
        case MessageType::URI:
            doOpenURI(descriptor, message);
            break;
        case MessageType::FileSent:
            break;
        case MessageType::EditNick:
            changeNickname(message);
            break;
        default:
            qDebug() << "[ERR] - " << QString::fromStdString(message.toString());
        break;
    }
}

//NOT WORKING PROPERLY --> fixed changing C&P editor's behavior
//COPY&PASTE OF SharedEditor::process
//TODO: FIX THIS ASAP AND TEST IT WITH MULTIPLE EDITORS
void KangarooServer::modifyFileVector(const Message &m, std::vector<Symbol>& _symbols) {
    if(m.getType() == MessageType::Insert){
        Symbol sym = m.getSymbol();
        if(_symbols.size() == 0){
            _symbols.insert(_symbols.begin(), m.getSymbol());
            return;
        }
        if(std::find(_symbols.begin(), _symbols.end(), m.getSymbol()) != _symbols.end())
            return;
        auto it = _symbols.begin();
        for(; it != _symbols.end(); ++it){
            if(sym < *it)
                break;
        }
        _symbols.insert(it, m.getSymbol());
    } else if (m.getType() == MessageType::Erase) {
        auto it = std::find(_symbols.begin(), _symbols.end(), m.getSymbol());
        if(it == _symbols.end()) //Symbol not found (already deleted or empty vector)
            return;
        _symbols.erase(it);
        return;
    }
}

void KangarooServer::propagate(qintptr descriptor, Message message){
    qDebug() << "A: " << QString::fromStdString(message.toString());
    QString filename = descriptorToEditor.at(descriptor).getWorkingFile();
    for(qintptr d : filenameToDescriptors.at(filename)){
        if(d != descriptor){
            descriptorToEditor.at(d).getSocket()->writeMessage(message);
        }
    }
}

void KangarooServer::doLogin(qintptr descriptor, Message message){
    QString loginString = message.getCommand();
    QString username = loginString.split(",").at(0);
    QString password = loginString.split(",").at(1);
    QString rowid = "";
    QString nickname = "";
    bool result = false;
    QSqlQuery q = usersDB.exec("SELECT ROWID, Nickname FROM kangaroo_users WHERE Username = '"+username+"' AND Password = '"+ password +"'");
    qDebug() << "[KangarooServer] - last query : " << q.lastQuery() << ":" << q.value(0).toInt() << ":" << q.lastInsertId().toString() ;
    int size = 0;
    while (q.next()) {
        size++;
        rowid = q.value("rowid").toString();
        nickname = q.value("nickname").toString();
        qDebug() << "[KangarooServer] - rowid " << rowid << "nickname" << nickname;
    }
//    qDebug() << "size : " << size;
    if(size == 1)
        result = true;
    else
        result = false;
    Message m;
    if(result == true){
        qDebug() << "Preparing Message";
        QString siteId = QString::number(rowid.toInt()+50);
        descriptorToEditor.at(descriptor).setDescriptor(descriptor);
        descriptorToEditor.at(descriptor).setSiteId(siteId.toInt());
        descriptorToEditor.at(descriptor).setUsername(username);
        QString info = username+','+siteId+','+nickname;
        m = Message{MessageType::Login, info};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
        sendFileList(descriptor);
    }
    else {
        QString content = "Error. Username and password are not correct. Try again.";
        m = Message{MessageType::Error, content};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    }
}

void KangarooServer::doRegister(qintptr descriptor, Message message){
    QString registerString = message.getCommand();
    QString username = registerString.split(",")[0];
    QString password = registerString.split(",")[1];
//    QString nickname = registerString.split(",")[2];
    bool result = true;
    //new fashion
    QSqlQuery q = usersDB.exec("SELECT COUNT(*) FROM kangaroo_users WHERE Username = '"+username+"'");
    qDebug() << "[KangarooServer] - last query : " << q.lastQuery() << ":" << q.executedQuery() << ":" << q.lastError().text();
    q.next();
    int size = q.value(0).toInt();
    qDebug() << "[KangarooServer] - query result size : " << size;
    if(size == 1)
        result = false;
    else
        q.exec("INSERT INTO kangaroo_users (Username, Password, Nickname) VALUES ('"+username+"','"+password+"','ChangeME')");
    qDebug() << "[KangarooServer] - last query : " << q.lastQuery() << ":" << q.last() << ":" << q.lastInsertId().toInt();
    qDebug() << usersDB.lastError().text();
    Message m;
    if(result == true){
        QString rowid = q.lastInsertId().toString();
        if(rowid.isEmpty()){
            qDebug() << "EMPTY siteID";
        }

        QString siteId = QString::number(rowid.toInt()+50);
        qDebug() << "[KangarooServer] - siteID: " << siteId;
        descriptorToEditor.at(descriptor).setDescriptor(descriptor);
        descriptorToEditor.at(descriptor).setSiteId(siteId.toInt());
        descriptorToEditor.at(descriptor).setUsername(username);
        m = Message{MessageType::Register, username+','+siteId+",ChangeME"};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
        sendFileList(descriptor);
    }
    else {
        QString content = "Error. User already exists.";
        m = Message{MessageType::Error, content};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    }
}

void KangarooServer::doCreate(qintptr descriptor, Message message){
    Message m;
    QDir().mkpath(FILES_DIRNAME);
    QString pathname = QString(FILES_DIRNAME) + "/" + message.getCommand() + ".kangaroo";
    QString filename = message.getCommand();
    if(QDir().exists(pathname)){
        m = Message{MessageType::Error, "Error while creating file. A file with name "+filename+" already exists."};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    }
    else {
        QFile(pathname).open(QIODevice::WriteOnly);
        descriptorToEditor.at(descriptor).setWorkingFile(filename);
        if(filenameToDescriptors.count(filename) == 0)
            filenameToDescriptors.insert({filename, std::vector<qintptr>()});
        filenameToDescriptors.at(filename).push_back(descriptor);
        //should always be true, since the file does not exist yet
        if(filenameToSymbols.count(filename) == 0)
            filenameToSymbols.insert({filename, std::vector<Symbol>()});
        for(qintptr d : filenameToDescriptors.at(filename)){
            sendEditorList(d, filename);
        }
        m = Message{MessageType::Create, ""};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
        insertControlSymbols(descriptor, filename);
    }

}

void KangarooServer::doOpen(qintptr descriptor, Message message){
    Message m;
    QString pathname;
    QString filename;
        pathname = QString(FILES_DIRNAME) + "/" + message.getCommand() + ".kangaroo";
        filename = message.getCommand();
    if(!QDir().exists(pathname)){
        m = Message{MessageType::Error, "Error while opening file. A file with name "+filename+" does not exist."};
    }
    else {
        bool alreadyInMemory = true;
        descriptorToEditor.at(descriptor).setWorkingFile(filename);
        if(filenameToDescriptors.count(filename) == 0)
            filenameToDescriptors.insert({filename, std::vector<qintptr>()});
        filenameToDescriptors.at(filename).push_back(descriptor);
        if(filenameToSymbols.count(filename) == 0){
            filenameToSymbols.insert({filename, std::vector<Symbol>()});
            alreadyInMemory = false;
        }
        for(qintptr d : filenameToDescriptors.at(filename)){
            sendEditorList(d, filename);
        }
        m = Message{MessageType::Open, ""};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
        sendFile(descriptor, filename, alreadyInMemory);
    }
}


void KangarooServer::doOpenURI(qintptr descriptor, Message message){
    Message m;
    QString pathname;
    QString filename;
    if(guestId > 50) {
        m = Message{MessageType::Error, "Too many guests connected. Try later"};
        return;
    }
    QString gId = QString::number(guestId);
    m = Message{MessageType::URI, "Guest"+gId+","+gId+",Guest"};
    descriptorToEditor.at(descriptor).setDescriptor(descriptor);
    descriptorToEditor.at(descriptor).setSiteId(guestId);
    descriptorToEditor.at(descriptor).setUsername("Guest"+gId);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    guestId++;
    QString serverAddr = this->server->serverAddress().toString();
    QStringList uri = message.getCommand().split("/");
    qDebug() << "URI: " << uri << "Message content: " << message.getCommand();
    if(uri.first() != serverAddr){
        m = Message{MessageType::Error, "Wrong URI request. This server does not exist."};
        return;
    }
    pathname = QString(FILES_DIRNAME) + "/" + uri.last() + ".kangaroo";
    filename = uri.last();
    if(!QDir().exists(pathname)){
        m = Message{MessageType::Error, "Error while opening file. A file with name "+filename+" does not exist."};
    }
    else {
        bool alreadyInMemory = true;
        descriptorToEditor.at(descriptor).setWorkingFile(filename);
        if(filenameToDescriptors.count(filename) == 0)
            filenameToDescriptors.insert({filename, std::vector<qintptr>()});
        filenameToDescriptors.at(filename).push_back(descriptor);
        if(filenameToSymbols.count(filename) == 0){
            filenameToSymbols.insert({filename, std::vector<Symbol>()});
            alreadyInMemory = false;
        }
        for(qintptr d : filenameToDescriptors.at(filename)){
            sendEditorList(d, filename);
        }
        m = Message{MessageType::Open, ""};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
        sendFile(descriptor, filename, alreadyInMemory);
    }
}

void KangarooServer::sendFileList(qintptr descriptor){
    std::string fileList;
    QDir currentDirectory{FILES_DIRNAME};
    //set name filters to avoid plaintext versions of files
    QStringList filters;
    filters << "*.kangaroo";
    currentDirectory.setNameFilters(filters);
    currentDirectory.setFilter(QDir::Files);
    QStringList entries = currentDirectory.entryList();
    for(QString filename : entries){
        //remove .kangaroo extension so the user sees only file names
        fileList += filename.toStdString().substr(0, filename.size()-9) + ",";
    }
    fileList=fileList.substr(0, fileList.size()-1);

    QString result = QString::fromStdString(fileList);
    Message m{MessageType::FileList,result};
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
}

void KangarooServer::sendEditorList(qintptr descriptor, QString filename){
    std::string editorList;
    if(filenameToDescriptors.count(filename) != 0){
        for(qintptr d : filenameToDescriptors.at(filename)){
            ConnectedEditor& ce = descriptorToEditor.at(d);
            QString username = ce.getUsername();
            int site = ce.getSiteId();
            editorList.append(username.toStdString());
            editorList.append(":");
            editorList.append(std::to_string(site));
            editorList.append(",");
        }
        editorList = editorList.substr(0, editorList.size()-1);
    }
    Message m{MessageType::EditorList, QString::fromStdString(editorList)};
    //send list (outside if should be empty
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
}

void KangarooServer::saveFile(QString filename){
    QString pathname = QString(FILES_DIRNAME) + "/" + filename + "_temp";
    QString pathnamePlain = QString(FILES_DIRNAME) + "/" + filename + "_tempPlain";
    QFile file(pathname);
    QFile filePlain(pathnamePlain);
    if(file.open(QIODevice::WriteOnly) && filePlain.open(QIODevice::WriteOnly)){
        std::vector<Symbol>& symbols = filenameToSymbols.at(filename);
        QDataStream stream(&file);
        QDataStream streamPlain(&filePlain);
        for(Symbol sym : symbols){
            stream << sym;
            if(sym.isContent() && !sym.isFake())
                streamPlain << sym.getContent();
        }
        file.close();
        filePlain.close();
    }
    QString newPath = QString(FILES_DIRNAME) + "/" + filename + ".kangaroo";
    QString newPathPlain = QString(FILES_DIRNAME) + "/" + filename + ".txt";
    QFile::remove(newPath);
    QFile::remove(newPathPlain);
    QFile::rename(pathname, newPath);
    QFile::rename(pathnamePlain, newPathPlain);
    qDebug() << "[SERVER] File saved.";
}

void KangarooServer::sendFile(qintptr descriptor, QString filename, bool alreadyInMemory){
    ServerSocket* socket = descriptorToEditor.at(descriptor).getSocket();
    std::vector<Symbol>& symbols = filenameToSymbols.at(filename);
    if(!alreadyInMemory){
        QString pathname = QString(FILES_DIRNAME) + "/" + filename + ".kangaroo";
        QFile f(pathname);
        if(f.open(QIODevice::ReadOnly)){
            QDataStream in(&f);
            Symbol sym;
            Message msg;
            while(!f.atEnd()&&socket->isConnected()){
                in >> sym;
                msg = Message{MessageType::Insert, sym};
                modifyFileVector(msg, symbols);
                socket->writeMessage(msg);
            }
            f.close();
        }
    }
    else {
        Message m;
        for(auto it = symbols.begin(); it != symbols.end(); ++it){
            if(socket->isConnected()){
                m = Message{MessageType::Insert, *it};
                socket->writeMessage(m);
            }
        }
    }
    if(socket->isConnected())
        socket->writeMessage(Message{MessageType::FileSent, ""});
}

void KangarooServer::hostDisconnected(qintptr descriptor){
    QString filename = descriptorToEditor.at(descriptor).getWorkingFile();
    if(filename.isNull()) //editor did not open anything yet
        return;
    std::vector<qintptr>& v = filenameToDescriptors.at(filename);
    //remove descriptor from map
    v.erase(std::remove(v.begin(), v.end(), descriptor), v.end());
    for(qintptr d : v){
        sendEditorList(d, filename);
    }
    if(v.size() == 0){
        saveFile(filename);
        filenameToDescriptors.erase(filename);
        filenameToSymbols.erase(filename);
    }
    descriptorToEditor.erase(descriptor);
}

void KangarooServer::insertControlSymbols(qintptr descriptor, QString filename){
    Symbol s;
    Message m;
    std::vector<Symbol>& symbols = filenameToSymbols.at(filename);
    std::vector<int> position;
    //paragraph
    position = {1, 0};
    s = Symbol{0, 1, position,StyleType::Paragraph, AlignmentType::AlignLeft};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //font
    position = {2, 0};
    s = Symbol{0, 1, position,StyleType::Font, "Arial"};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //fontsize
    position = {3, 0};
    s = Symbol{0, 1, position,StyleType::FontSize, 8};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //color
    position = {4, 0};
    s = Symbol{0, 1, position,StyleType::Color, "#000000"};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //empty symbol
    position = {5, 0};
    s = Symbol{0, 0, position,"\u0000"};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //fontEnd
    position = {6, 0};
    s = Symbol{0, 1, position,StyleType::FontEnd, "Arial"};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //fontsizeEnd
    position = {7, 0};
    s = Symbol{0, 1, position,StyleType::FontSizeEnd, 8};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //colorEnd
    position = {8, 0};
    s = Symbol{0, 1, position,StyleType::ColorEnd, "#000000"};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
}
