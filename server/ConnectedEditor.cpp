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

std::string ConnectedEditor::getWorkingFile(){
    return workingFile;
}

void ConnectedEditor::setSiteId(int id){
    siteId = id;
}

void ConnectedEditor::setWorkingFile(std::string filename){
    workingFile = filename;
}
