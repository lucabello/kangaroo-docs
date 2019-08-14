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
            std::string filename = descriptorToEditor.at(descriptor).getWorkingFile();
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
    int l=0, r=_symbols.size()-1, middle=-1;
    if(m.getType() == MessageType::Insert){
        /*
         * After this binary search, the variable middle can be:
         * - -1 if the vector is empty
         * - index of the first element bigger than s
         * - index of the first element smaller than s
         * If the symbol is found, the function will return.
         */
        while(l<=r){
            middle = (l+r)/2;
            if(_symbols.at(middle) < m.getSymbol())
                l = middle+1;
            else if(m.getSymbol() < _symbols.at(middle))
                r = middle-1;
            else
                break;
        }
        if(l<=r) //Symbol found (already inserted, idempotency)
            return;
        if(middle == -1){ //empty vector, never entered while cycle
            _symbols.insert(_symbols.begin(), m.getSymbol());
        }
        else if (m.getSymbol() < _symbols.at(middle)){
            _symbols.insert(_symbols.begin()+middle, m.getSymbol());
        } else {
            _symbols.insert(_symbols.begin()+middle+1, m.getSymbol());
        }
    } else if (m.getType() == MessageType::Erase) {
        //middle variable is modified or the function returns
        while(l<=r){
            middle = (l+r)/2;
            if(_symbols.at(middle) < m.getSymbol())
                l = middle+1;
            else if(m.getSymbol() < _symbols.at(middle))
                r = middle-1;
            else
                break;
        }
        if(l > r) //Symbol not found (already deleted or empty vector)
            return;
        _symbols.erase(_symbols.begin()+middle);
    }
}

void KangarooServer::propagate(int descriptor, Message message){
    qDebug() << "A: " << QString::fromStdString(message.toString());
    std::string filename = descriptorToEditor.at(descriptor).getWorkingFile();
    for(int d : filenameToDescriptors.at(filename)){
        if(d != descriptor){
            descriptorToEditor.at(d).getSocket()->writeMessage(message);
        }
    }
}

void KangarooServer::doLogin(int descriptor, Message message){
    QString loginString = QString::fromStdString(message.getCommand());
    QFile inputFile("users.txt");
    bool result = false;
    std::string siteId = "-1";
    if(inputFile.open(QIODevice::ReadOnly)){
        QTextStream in(&inputFile);
        while(!in.atEnd() && !result){
            QString line = in.readLine();
            if(line.startsWith(loginString)){
                result = true;
                siteId = line.split(",").at(2).toStdString();
            }
        }
        inputFile.close();
    }
    Message m;
    if(result == true){
        descriptorToEditor.at(descriptor).setSiteId(std::stoi(siteId));
        m = Message{MessageType::Login, siteId};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
        sendFileList(descriptor);
    }
    else {
        std::string content = "Error. Username and password are not correct. Try again.";
        m = Message{MessageType::Error, content};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    }
}

void KangarooServer::doRegister(int descriptor, Message message){
    QString registerString = QString::fromStdString(message.getCommand());
    QFile userFile("users.txt");
    bool result = true;
    int newSiteId = 1;
    if(userFile.open(QIODevice::ReadOnly)){
        QTextStream in(&userFile);
        while(!in.atEnd() && result){
            QString line = in.readLine();
            if(line.startsWith(registerString)){
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
        m = Message{MessageType::Register, ""};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
        sendFileList(descriptor);
    }
    else {
        std::string content = "Error. User already exists.";
        m = Message{MessageType::Error, content};
        descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
    }
}

void KangarooServer::doCreate(int descriptor, Message message){
    Message m;
    std::string pathname = std::string(FILES_DIRNAME) + "/" + message.getCommand() + ".kangaroo";
    std::string filename = message.getCommand();
    if(QDir().exists(QString::fromStdString(pathname))){
        m = Message{MessageType::Error, "Error while creating file. A file with name "+filename+" already exists."};
    }
    else {
        QFile(QString::fromStdString(pathname)).open(QIODevice::WriteOnly);
        descriptorToEditor.at(descriptor).setWorkingFile(filename);
        if(filenameToDescriptors.count(filename) == 0)
            filenameToDescriptors.insert({filename, std::vector<int>()});
        filenameToDescriptors.at(filename).push_back(descriptor);
        //should always be true, since the file does not exist yet
        if(filenameToSymbols.count(filename) == 0)
            filenameToSymbols.insert({filename, std::vector<Symbol>()});
        m = Message{MessageType::Create, ""};
    }
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
}

void KangarooServer::doOpen(int descriptor, Message message){
    Message m;
    std::string pathname = std::string(FILES_DIRNAME) + "/" + message.getCommand() + ".kangaroo";
    std::string filename = message.getCommand();
    if(!QDir().exists(QString::fromStdString(pathname))){
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
        sendFile(descriptor, filename, alreadyInMemory);
        m = Message{MessageType::Open, ""};
    }
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
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

    Message m{MessageType::FileList,fileList};
    descriptorToEditor.at(descriptor).getSocket()->writeMessage(m);
}

void KangarooServer::saveFile(std::string filename){
    std::string pathname = std::string(FILES_DIRNAME) + "/" + filename + "_temp";
    std::string pathnamePlain = std::string(FILES_DIRNAME) + "/" + filename + "_tempPlain";
    QFile file(QString::fromStdString(pathname));
    QFile filePlain(QString::fromStdString(pathnamePlain));
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
    std::string newPath = std::string(FILES_DIRNAME) + "/" + filename + ".kangaroo";
    std::string newPathPlain = std::string(FILES_DIRNAME) + "/" + filename + ".txt";
    QFile::remove(QString::fromStdString(newPath));
    QFile::remove(QString::fromStdString(newPathPlain));
    QFile::rename(QString::fromStdString(pathname), QString::fromStdString(newPath));
    QFile::rename(QString::fromStdString(pathnamePlain), QString::fromStdString(newPathPlain));
}

void KangarooServer::sendFile(int descriptor, std::string filename, bool alreadyInMemory){
    /*
    ServerSocket* socket = descriptorToEditor.at(descriptor).getSocket();
    std::string pathname = std::string(FILES_DIRNAME) + "/" + filename + ".kangaroo";
    std::vector<Symbol>& symbols = filenameToSymbols.at(filename);
    QFile f(QString::fromStdString(pathname));
    if(f.open(QIODevice::ReadOnly)){
        char serS[100];
        char payloadLen[4];
        int len = -1;
        while(f.read(payloadLen, 4) == 4){
            qDebug() << "Inside while...";
            len = Symbol::peekIntFromByteArray(payloadLen);
            qDebug() << "1";
            f.read(serS, len);
            qDebug() << "2";
            Symbol s = Symbol::unserialize(serS);
            qDebug() << "3";
            Message m {MessageType::Insert, s};
            qDebug() << "4";
            if(!alreadyInMemory)
                modifyFileVector(m, symbols);
            qDebug() << "5";
            socket->writeMessage(m);
            qDebug() << "6";
        }
        f.close();
    }
    */
}

void KangarooServer::hostDisconnected(int descriptor){
    std::string filename = descriptorToEditor.at(descriptor).getWorkingFile();
    std::vector<int>& v = filenameToDescriptors.at(filename);
    //remove descriptor from map
    v.erase(std::remove(v.begin(), v.end(), descriptor), v.end());
    if(v.size() == 0)
        saveFile(filename);
    filenameToDescriptors.erase(filename);
    filenameToSymbols.erase(filename);
    descriptorToEditor.erase(descriptor);
}
