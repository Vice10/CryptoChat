#include <WinSock2.h>
#include <iostream>
#include <string>
#include "stdio.h"
#include <nlohmann/json.hpp>
#include <algorithm>
#include "ChatRoom.h"
#include <conio.h>

using nlohmann::json;
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

SOCKET connections[26];
HANDLE handles[26];
bool nameSet[26];
int counter = 1;

ChatRoom cr{};
void from_json(const json& j, Message& strct) {
	j.at("sender").get_to(strct.sender);
	j.at("str").get_to(strct.str);
	j.at("tag").get_to(strct.tag);
	j.at("date").get_to(strct.date);
	j.at("sign").get_to(strct.sign);
	j.at("iv").get_to(strct.iv);
}

void to_json(json& j, const Message& strct) {
	j = json{ {"sender", strct.sender}, {"str", strct.str}, {"tag", strct.tag},
		{"date", strct.date}, {"sign", strct.sign}, {"iv", strct.iv} };
}

void clientHandle(int index) {
	char message[10000];
	while (true) {
		recv(connections[index], message, sizeof(message), NULL); /// receive an encrypted 
		if (message[0] == '1') {								  /// add it to the message list
			std::string newMes = message;
			newMes.erase(newMes.begin()); /// remove '1'
			json jnewMes = newMes;
			bool addRes = cr.addMessage(jnewMes);
			if (addRes)
				std::cout << "Added message from " << index << "\n";
			else
				std::cout << "Failed to add message from " << index << "\n";

			for (int i = 1; i < counter; i++) {
				if (i == index) continue;
				if (send(connections[i], message, sizeof(message), NULL) > 0)
					std::cout << "Message sent to socket " << i << std::endl;
				else continue;
			}
		}
		else if (message[0] == '2') { /// send all messages
			std::string allMessages = cr.getAllMessages(0);
			allMessages = "0" + allMessages;
			send(connections[index], allMessages.c_str(), allMessages.size() + 1, NULL);
		}
		else if (message[0] == '9') {
			DWORD code;
			GetExitCodeThread(handles[index], &code);
			//CloseHandle(handles[index]);
			ExitThread(code);
			closesocket(connections[index]);
			bool remRes = cr.removeMember(index);
			if (remRes)
				std::cout << "User " << index << " is removed from the chat member list\n";
			else
				std::cout << "Failed to remove user " << index << " from the chat member list\n";
		}
		else if (message[0] == '#') { /// Post member name
			if (nameSet[index]) {
				std::cout << "Socket " << index << " tried to post a new name. Operation ignored.\n";
					continue;
			}
			std::string userName = message;
			// remove spaces
			userName.erase(remove_if(userName.begin(), userName.end(), isspace), userName.end());
			// remove # in the beginning
			userName.erase(userName.begin());
			userName += "_" + std::to_string(index); // never change this
			Member newMemb{ userName };
			bool addSuccess = cr.addMember(newMemb);
			if (addSuccess) {
				nameSet[index] = true;
				std::cout << "User " << userName << " has entered the chat\n";
				std::map<uint, string> newKey = cr.getMemberByName(newMemb.getUniqueName()).getSecretKeys();
				Message msg{};
				msg.tag = newKey.begin().first;
				msg.str = newKey.begin().second;
				json jKey;
				to_json(jKey, msg);
				string msg_to_all = "3" + jKey.dump();
				for(int i = 1; i < counter; i++){
					if (send(connections[i], msg_to_all.c_str(), msg_to_all.size()+1, NULL) > 0)
						std::cout << "New key sent to socket " << i << std::endl;
					else continue;
				}
			}
			else{
				std::cout << "User " << userName << " failed to enter the chat\n";
				DWORD code;
				GetExitCodeThread(handles[index], &code);
				//CloseHandle(handles[index]);
				ExitThread(code);
				closesocket(connections[index]);
			}
		}
		else
			std::cout << "Operation " << message[0] << " from " << index << " is not recognised\n";
	}
}

int main(int argc, char* argv[]) {
	Member dummy{ "Dummy_0" };
	cr.addMember(dummy);
	WSAData wsadata;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsadata) != 0) {
		std::cout << "Error! Library download failed!" << std::endl;
		return 1;
	}
	SOCKADDR_IN address;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(1111);
	address.sin_family = AF_INET;

	int sizeOfAddress = sizeof(address);
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&address, sizeof(address));
	listen(sListen, SOMAXCONN);

	SOCKET newConnect;
	for (int i = 1; i < 26; i++) {  //NEEDED not for but while
		newConnect = accept(sListen, (SOCKADDR*)&address, &sizeOfAddress);
		if (newConnect == 0) {
			std::cout << "Error! The client failed to connect to the server!" << std::endl;
		}
		else {
			std::cout << "New client successfully connected!" << std::endl;

			connections[i] = newConnect;
			counter++;
			handles[i] = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandle, (LPVOID)i, NULL, NULL);

		}
	}

	return 0;
}