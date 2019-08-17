#include "KangarooServer.h"
#include <fstream>
#include <QDir>

#define FILES_DIRNAME "textfiles"

KangarooServer::KangarooServer(QObject *parent) :
    QObject(parent)
{
    server = new QTcpServer(this);
    QDir().mkpath(FILES_DIRNAME); //needs rights to work
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
}

void KangarooServer::newConnection()
{
    qDebug() << "[SERVER] Connection received!";
    ServerSocket *mySocket = new ServerSocket(server->nextPendingConnection());

    connect(mySocket, &ServerSocket::signalMessage,
            this, &KangarooServer::incomingMessage);
    connect(mySocket, &ServerSocket::hostDisconnected,
            this, &KangarooServer::hostDisconnected);

    int descriptor = mySocket->getDescriptor();
    ConnectedEditor ce(mySocket);
    descriptorToEditor.insert({descriptor, ce});
    qDebug() << "[SERVER] Editor info created for " << descriptor << "!";
    //mySocket->writeData(QString::fromStdString("Welcome, beautiful client " + std::to_string(descriptor) + ".\r\n"));
//    socket->flush();
//    socket->waitForBytesWritten(3000);
//    socket->close();
}

void KangarooServer::incomingMessage(int descriptor,Message message){
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
        default:
            break;
    }
}

//NOT WORKING PROPERLY
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

void KangarooServer::propagate(int descriptor, Message message){
    qDebug() << "A: " << QString::fromStdString(message.toString());
    QString filename = descriptorToEditor.at(descriptor).getWorkingFile();
    for(int d : filenameToDescriptors.at(filename)){
        if(d != descriptor){
            descriptorToEditor.at(d).getSocket()->writeMessage(message);
        }
    }
}

void KangarooServer::doLogin(int descriptor, Message message){
    QString loginString = message.getCommand();
    QString username = loginString.split(",").at(0);
    QFile inputFile("users.txt");
    bool result = false;
    QString siteId = "-1";
    if(inputFile.open(QIODevice::ReadOnly)){
        QTextStream in(&inputFile);
        while(!in.atEnd() && !result){
            QString line = in.readLine();
            if(line.startsWith(loginString)){
                result = true;
                siteId = line.split(",").at(2);
            }
        }
        inputFile.close();
    }
    Message m;
    if(result == true){
        descriptorToEditor.at(descriptor).setDescriptor(descriptor);
        descriptorToEditor.at(descriptor).setSiteId(siteId.toUInt());
        descriptorToEditor.at(descriptor).setUsername(username);
        m = Message{MessageType::Login, siteId};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
        sendFileList(descriptor);
    }
    else {
        QString content = "Error. Username and password are not correct. Try again.";
        m = Message{MessageType::Error, content};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    }
}

void KangarooServer::doRegister(int descriptor, Message message){
    QString registerString = message.getCommand();
    QString username=registerString.split(",")[0];
    QFile userFile("users.txt");
    bool result = true;
    int newSiteId = 1;
    if(userFile.open(QIODevice::ReadOnly)){
        QTextStream in(&userFile);
        while(!in.atEnd() && result){
            QString line = in.readLine();
            if(line.startsWith(username)){
                result = false;
            }
            newSiteId++;
        }
        userFile.close();
    }
    Message m;
    if(result == true){
        if(userFile.open(QIODevice::WriteOnly | QIODevice::Append)){
            QString siteId = QString().setNum(newSiteId);
            QString line = registerString + "," + siteId + "\n";
            userFile.write(line.toUtf8());
            userFile.close();
        }
        descriptorToEditor.at(descriptor).setDescriptor(descriptor);
        descriptorToEditor.at(descriptor).setSiteId(newSiteId);
        descriptorToEditor.at(descriptor).setUsername(username);
        m = Message{MessageType::Register, ""};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
        sendFileList(descriptor);
    }
    else {
        QString content = "Error. User already exists.";
        m = Message{MessageType::Error, content};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    }
}

void KangarooServer::doCreate(int descriptor, Message message){
    Message m;
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
            filenameToDescriptors.insert({filename, std::vector<int>()});
        filenameToDescriptors.at(filename).push_back(descriptor);
        //should always be true, since the file does not exist yet
        if(filenameToSymbols.count(filename) == 0)
            filenameToSymbols.insert({filename, std::vector<Symbol>()});
        for(int d : filenameToDescriptors.at(filename)){
            sendEditorList(d, filename);
        }
        m = Message{MessageType::Create, ""};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
        insertControlSymbols(descriptor, filename);
    }

}

void KangarooServer::doOpen(int descriptor, Message message){
    Message m;
    QString pathname = QString(FILES_DIRNAME) + "/" + message.getCommand() + ".kangaroo";
    QString filename = message.getCommand();
    if(!QDir().exists(pathname)){
        m = Message{MessageType::Error, "Error while opening file. A file with name "+filename+" does not exist."};
    }
    else {
        bool alreadyInMemory = true;
        descriptorToEditor.at(descriptor).setWorkingFile(filename);
        if(filenameToDescriptors.count(filename) == 0)
            filenameToDescriptors.insert({filename, std::vector<int>()});
        filenameToDescriptors.at(filename).push_back(descriptor);
        if(filenameToSymbols.count(filename) == 0){
            filenameToSymbols.insert({filename, std::vector<Symbol>()});
            alreadyInMemory = false;
        }
        for(int d : filenameToDescriptors.at(filename)){
            sendEditorList(d, filename);
        }
        m = Message{MessageType::Open, ""};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
        sendFile(descriptor, filename, alreadyInMemory);
    }
}

void KangarooServer::sendFileList(int descriptor){
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

void KangarooServer::sendEditorList(int descriptor, QString filename){
    std::string editorList;
    if(filenameToDescriptors.count(filename) != 0){
        for(int d : filenameToDescriptors.at(filename)){
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
                streamPlain << sym.getPlaintext().at(0);
        }
        file.close();
        filePlain.close();
    }
    /*
    if(file.open(QIODevice::WriteOnly) && filePlain.open(QIODevice::WriteOnly)){
        //Always present since this function is called when the last editor on that file disconnects
        std::vector<Symbol>& symbols = filenameToSymbols.at(filename);
        for(Symbol s : symbols){
            char *serS = s.serialize(s);
            int len = Symbol::peekIntFromByteArray(serS+4);
            file.write(serS, len);
            if(s.isContent()){
                char content[1];
                content[0] = s.getContent();//loses wchar_t
                filePlain.write(content, 1);
            }
        }
        char terminator[1];
        terminator[0] = '\0';
        filePlain.write(terminator, 1);
        file.close();
        filePlain.close();

    }
    */
    QString newPath = QString(FILES_DIRNAME) + "/" + filename + ".kangaroo";
    QString newPathPlain = QString(FILES_DIRNAME) + "/" + filename + ".txt";
    QFile::remove(newPath);
    QFile::remove(newPathPlain);
    QFile::rename(pathname, newPath);
    QFile::rename(pathnamePlain, newPathPlain);
    qDebug() << "[SERVER] File saved.";
}

void KangarooServer::sendFile(int descriptor, QString filename, bool alreadyInMemory){
    ServerSocket* socket = descriptorToEditor.at(descriptor).getSocket();
    std::vector<Symbol>& symbols = filenameToSymbols.at(filename);
    if(!alreadyInMemory){
        QString pathname = QString(FILES_DIRNAME) + "/" + filename + ".kangaroo";
        QFile f(pathname);
        if(f.open(QIODevice::ReadOnly)){
            QDataStream in(&f);
            Symbol sym;
            Message msg;
            while(!f.atEnd()){
                in >> sym;
                msg = Message{MessageType::Insert, sym};
                //qDebug() << "[sendFile] " << QString::fromStdString(msg.toString());
                modifyFileVector(msg, symbols);
                socket->writeMessage(msg);
            }
            f.close();
        }
    }
    else {
        Message m;
        for(auto it = symbols.begin(); it != symbols.end(); ++it){
            m = Message{MessageType::Insert, *it};
            socket->writeMessage(m);
        }
    }
}

void KangarooServer::hostDisconnected(int descriptor){
    QString filename = descriptorToEditor.at(descriptor).getWorkingFile();
    if(filename.isNull()) //editor did not open anything yet
        return;
    std::vector<int>& v = filenameToDescriptors.at(filename);
    //remove descriptor from map
    v.erase(std::remove(v.begin(), v.end(), descriptor), v.end());
    for(int d : v){
        sendEditorList(d, filename);
    }
    if(v.size() == 0){
        saveFile(filename);
        filenameToDescriptors.erase(filename);
        filenameToSymbols.erase(filename);
    }
    //descriptorToEditor.erase(descriptor);
}

void KangarooServer::insertControlSymbols(int descriptor, QString filename){
    Symbol s;
    Message m;
    std::vector<Symbol>& symbols = filenameToSymbols.at(filename);
    std::vector<int> position;
    //paragraph
    position = {1, 0};
    s = Symbol{StyleType::Paragraph, AlignmentType::AlignLeft, 0, 1, position};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //font
    position = {2, 0};
    s = Symbol{StyleType::Font, "Arial", 0, 1, position};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //fontsize
    position = {3, 0};
    s = Symbol{StyleType::FontSize, 8, 0, 1, position};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //color
    position = {4, 0};
    s = Symbol{StyleType::Color, "#000000", 0, 1, position};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //empty symbol
    position = {5, 0};
    s = Symbol{'\u0000', 0, 0, position};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //fontEnd
    position = {6, 0};
    s = Symbol{StyleType::FontEnd, "Arial", 0, 1, position};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //fontsizeEnd
    position = {7, 0};
    s = Symbol{StyleType::FontSizeEnd, 8, 0, 1, position};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    //colorEnd
    position = {8, 0};
    s = Symbol{StyleType::ColorEnd, "#000000", 0, 1, position};
    m = Message{MessageType::Insert, s};
    modifyFileVector(m, symbols);
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
}
