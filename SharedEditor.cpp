//
// Created by lucab on 06/05/2019.
//

#include <string>
#include "SharedEditor.h"

SharedEditor::SharedEditor(NetworkServer &s) : _server(s){};

void SharedEditor::localInsert(int index, char value) {
    //TODO: implement
}

void SharedEditor::localErase(int index) {
    //TODO: implement
}

void SharedEditor::process(const Message &m) {
    //TODO: implement
}

std::string SharedEditor::to_string() {
    //TODO: implement
    return "";
}