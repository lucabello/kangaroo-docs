#include "ConnectedEditor.h"
#include "ServerSocket.h"

ConnectedEditor::ConnectedEditor(){};

ConnectedEditor::ConnectedEditor(ServerSocket *s) : mySocket(s),
    descriptor(s->getDescriptor()), siteId(-1) {}

ServerSocket* ConnectedEditor::getSocket(){
    return mySocket;
}

int ConnectedEditor::getDescriptor(){
    return descriptor;
}

QString ConnectedEditor::getUsername(){
    return username;
}

int ConnectedEditor::getSiteId(){
    return siteId;
}

QString ConnectedEditor::getWorkingFile(){
    return workingFile;
}

void ConnectedEditor::setDescriptor(int descr){
    descriptor = descr;
}

void ConnectedEditor::setUsername(QString user){
    username = user;
}

void ConnectedEditor::setSiteId(int id){
    siteId = id;
}

void ConnectedEditor::setWorkingFile(QString filename){
    workingFile = filename;
}
