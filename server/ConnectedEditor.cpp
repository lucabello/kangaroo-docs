#include "ConnectedEditor.h"
#include "ServerSocket.h"

ConnectedEditor::ConnectedEditor(){};

ConnectedEditor::ConnectedEditor(ServerSocket *s) : mySocket(s),
    descriptor(s->getDescriptor()), siteId(-1) {}

int ConnectedEditor::getDescriptor(){
    return descriptor;
}

ServerSocket* ConnectedEditor::getSocket(){
    return mySocket;
}

QString ConnectedEditor::getWorkingFile(){
    return workingFile;
}

void ConnectedEditor::setSiteId(int id){
    siteId = id;
}

void ConnectedEditor::setWorkingFile(QString filename){
    workingFile = filename;
}
