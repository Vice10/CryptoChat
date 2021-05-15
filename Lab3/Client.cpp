#include <WinSock2.h>
#include <iostream>
#include <string>
#include "stdio.h"
#include <nlohmann/json.hpp>
#include <sstream>
#include <vector>
#include "Member.h"
#include<conio.h>
using nlohmann::json;
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

SOCKET connection;

//[1 Marina Hello 13 etc] -> 1 -> [1 Marina Hello 13 etc] -> Marina Hello 13 etc
//{1 }Marina Hello 13 etc

//struct Message {
//	std::string sender;
//	std::string str; // BigInt{ 3445 }.ToString();
//	int tag; // uint 
//	std::string date;
//	std::string sign; // base64
//	std::string iv; // numerical
//};

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

std::string getPlainMessage(std::string inp, int& idx);

Member m {
	"Alice"
};

void handle() {
	char recvMessage[10000];
	while (true) {
		recv(connection, recvMessage, sizeof(recvMessage), NULL);
		int i = 1;
		if (recvMessage[0] == '1') { // Send to all
			std::string mes;
			while (recvMessage[i] != '\0') {
				mes += recvMessage[i];
				i++;
			}
			Message js;
			try {
				js = json::parse(mes);
			}
			catch (std::exception ex) {
				std::cout << "Receive message parsing error!" << std::endl;
			}
			/// decrypt
			bool verified = false;
			std::string recovered = m.decryptSecretMessage(js, verified);
			if (!recovered.empty()) {
				std::cout << js.sender << ": " << recovered << "\n";
				std::cout << js.date << "  ";
					if (verified)
						std::cout << "Verification success\n\n";
					else
						std::cout << "Verification failure\n\n";
			}
			else {
				std::cout << "Could not decrypt message\n" << mes << "\n";
			}

		}
		else if (recvMessage[0] == '0') { // get all messages
			std::vector<Message> messages = m.transformMes(recvMessage)
			for (const auto& mes : messages) {
				bool verified = false;
				std::string recovered = m.decryptSecretMessage(mes, verified);
				if (!recovered.empty()) {
					std::cout << mes.sender << ": " << recovered << "\n";
					std::cout << mes.date << "   Verified: ";
					if (verified) std::cout << "true\n\n";
					else std::cout << "false\n\n";
				}
			}
		}
		else if(recvMessage[0] == '3'){ //// get a new key
			std::string newKey = recvMessage;
			newKey.erase(newKey.begin());
			json jKey = newKey;
			bool addRes = m.addSecretKey(jKey);
			if(addRes)
				std::cout << "Added new secret key\n\n";
			else 
				std::cout << "Failed to add secret key\n\n";
		}
		else{
			std::cout << "Command " << recvMessage[0] << " not recognised\n";
		}
	}
}

int main(int argc, char* argv[]) {
	std::string userName;
	std::cout << "Enter your name\n";
	if (!(std::cin >> userName))exit(618);
	m.setUniqueName(userName);
	userName = "#" + userName;
	WSAData wsadata;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsadata) != 0) {
		std::cout << "Error! Library download failed!" << std::endl;
		return -1;
	}
	SOCKADDR_IN address;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(1111);
	address.sin_family = AF_INET;

	int sizeOfAddress = sizeof(address);
	connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(connection, (SOCKADDR*)&address, sizeOfAddress) != 0) {
		std::cout << "Error! Couldn't connect to the server!" << std::endl;
		return 1;
	}
	else
		std::cout << "Connection established" << std::endl;
	//char message[256];
	//recv(connection, message, sizeof(message), NULL);
	//std::cout << message << std::endl;

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)handle, NULL, NULL, NULL);
	/// Send username to server
	send(connection, userName.c_str(), userName.length() + 1, NULL); /// send   # Bob
	
	//1 Marina Hello 13 etc etc etc
	char sendMessage[10000];
	std::string temporar;
	std::cin.get();
	while (true) {
		getline(std::cin, temporar);
		if (m.getSecretKeysCount() == 0) {
			std::cout << "No secret keys available. Fetch your secret keys via command 0\n";
			continue;
		}
		std::string mes;
		int index;
		try {
			mes = getPlainMessage(temporar, index); /// index = 1 to send one message to all
		}
		catch (int e) {
			std::cout << "Error getPlainMessage(): invalid message\n";
		}
		catch (std::exception ex) {
			std::cout << "Unknown error!" << std::endl;
		}
		json sendMes;
		try {
			sendMes = m.compoundSecretMessage(mes, m.getLatestTag());
		}
		catch (std::exception ex) {
			std::cout << "Failed to compound secret message\n";
			std::cout << ex.what() << "\n";
		}

		std::string mes_to_send = "1" + sendMes.dump();
		mes_to_send += '\0';
		send(connection, mes_to_send.c_str(), mes_to_send.length(), NULL);
		if (temporar[0] == '9') { //NEEDED cut the thread as well
			break;
		}
		Sleep(10);
	}
	return 0;
}

std::string getPlainMessage(std::string inp, int& idx)  //1 Marina Hello -> [1][Marina][Hello][tag][date][sign][iv]
{
	std::vector<std::string> strvec;

	std::stringstream strbuf{ inp };

	std::string temp;

	while (strbuf >> temp)
	{
		strvec.push_back(temp);
	}
	std::string plainText;
	for (const auto& elem : strvec)
		plainText += elem;

	//// 1 Hello to everyone
	idx = atoi(strvec[0].c_str());
	return plainText;
}
