#include "ChatRoom.h"

ChatRoom::ChatRoom()
{
	ifstream fin(currentTagFilename, ios::in);
	uint messagesCount;
	if (!(fin >> currentTag >> messagesCount)) exit(123);
	fin.close();
	if (messagesCount > 0)
		inputMessages();
}

ChatRoom::~ChatRoom()
{
	ofstream fout(currentTagFilename, ios::out);
	if (!(fout << currentTag << " " << messages.size())) exit(643);
	fout.close();
	outputMessages();
}

void ChatRoom::inputMessages()
{
	ifstream fin(messagesFilename);
	json inpMessages;
	fin >> inpMessages;
	if (inpMessages == NULL)
		return;
	Message newmes{};
	for (auto const& mes : inpMessages) {
		jt.from_json(mes, newmes);
		messages.push_back(newmes);
	}
	fin.close();
}

void ChatRoom::outputMessages()
{
	ofstream fout(messagesFilename, ofstream::binary);
	json outMessages;
	json outMes;
	for (auto const& mes : messages) {
		jt.to_json(outMes, mes);
		outMessages.push_back(outMes);
	}
	fout << setw(4) << outMessages;
	fout.close();
}

vector<Message> ChatRoom::getAllMessages()
{
	return messages;
}

std::string ChatRoom::getAllMessages(int num) {
	json allMes;
	json curMes;
	for (const auto& mes : messages) {
		jt.to_json(curMes, mes);
		allMes.push_back(curMes);
	}
	return allMes.dump();
}

vector<Message> ChatRoom::getMessages(uint tag)
{
	vector<Message> tmp;
	for (auto const& mes : messages)
		if (mes.tag == tag)
			tmp.push_back(mes);
	return tmp;
}

vector<Message> ChatRoom::getMessages(vector<uint> tags)
{
	vector<Message> tmp;
	for (uint t : tags) {
		vector<Message> tmp2;
		tmp2 = getMessages(t);
		if (!tmp2.empty())
			for (auto const& mes : tmp2)
				tmp.push_back(mes);
	}
	return tmp;
}

vector<std::string> ChatRoom::getMessagesAndKeys(std::string name)
{
	// keys then messages
	map<uint, std::string> keys;
	for (auto& const memb : members)
		if (memb.getUniqueName() == name)
			keys = memb.getSecretKeys();
	vector<std::string> res;
	Message tmpMes;
	tmpMes.date = "123";
	tmpMes.sender = "123";
	tmpMes.sign = "123";
	tmpMes.iv = "-1"; /// KEY IDENTIFIER
	json curMes;
	for (const auto& key : keys) {
		tmpMes.tag = key.first;
		tmpMes.str = key.second;
		jt.to_json(curMes, tmpMes);
		std::string mesString = curMes.dump(); /// explicit conversion into std::string
		res.push_back(mesString);
	}

	for (const auto& mes : messages) {
		jt.to_json(curMes, mes);
		std::string mesString = curMes.dump();
		res.push_back(mesString);
	}
	return res;
}

std::string ChatRoom::getMessagesAndKeys(std::string name, int dum)
{
	auto keysAndMes = getMessagesAndKeys(name);
	if (keysAndMes.size() == 0) return "[]";
	std::string res = "[";
	for (const auto& mes : keysAndMes) {
		res += mes;
		res += ",";
	}
	res.pop_back(); /// remove the last commas
	res += "]";
	return res;
}

bool ChatRoom::addMessage(json message)
{
	if (message.is_null()) return false;
	Message m{};
	jt.from_json(message, m);
	messages.push_back(m);
	return true;
}

bool ChatRoom::initPrivateKeyGen()
{
	vector<Member> activeMembs;
	for (uint i = 0; i < members.size(); i++)
		if (members[i].membIsActive())
			activeMembs.push_back(members[i]);

	if (activeMembs.size() < 2) {
		cout << "Too few members for key exchange.\n\n";
		return false;
	}
	currentTag++;
	vector<BigInt> pubKeys;
	for (auto& memb : activeMembs)
		pubKeys.push_back(memb.genPublicKey());
	vector<BigInt> checkResponse;

	checkResponse.push_back(activeMembs[0].genSecretKey(pubKeys[pubKeys.size() - 1], currentTag));
	for (uint i = 1; i < pubKeys.size(); i++)
		checkResponse.push_back(activeMembs[i].genSecretKey(pubKeys[i - 1], currentTag));

	if (checkResponse[pubKeys.size() - 1] != checkResponse[0]) return false;
	for (uint i = 1; i < pubKeys.size(); i++)
		if (checkResponse[i] != checkResponse[i - 1])
			return false;
	return true;
}

bool ChatRoom::addMember(Member newMem)
{
	for (auto mem : members)
		if (mem.getUniqueName() == newMem.getUniqueName())
			return false;
	members.push_back(newMem);
	if (members.size() > 1)
		initPrivateKeyGen();
	return true;
}

std::string ChatRoom::getMemberNameByIdx(uint idx) {
	std::string tmp = "";
	for (auto memb : members) {
		std::string membName = memb.getUniqueName();
		for (int i = membName.size() - 1; i > -1; i--) {
			if (membName.at(i) == '_')
				break;
			else
				tmp += membName.at(i);
		}
		reverse(tmp.begin(), tmp.end());
		if (tmp == to_std::string(idx))
			return membName;
	}
	return std::string();
}

bool ChatRoom::removeMember(std::string name)
{
	if (members.size() == 0) return false;
	if (members.size() == 1 && members[0].getUniqueName() == name) {
		members.pop_back();
		return true;
	}
	for (uint i = 0; i < members.size(); i++) {
		if (members[i].getUniqueName() == name) {
			if (i == members.size() - 1) {
				members.pop_back();
				return true;
			}
			else {
				members[i].setSecretKeys(members[members.size() - 1].getSecretKeys());
				members[i].setUniqueName(members[members.size() - 1].getUniqueName());
				//members[i] = members[members.size() - 1];
				members.pop_back();
				return true;
			}
		}
	}
	return false;
}

bool ChatRoom::removeMember(uint idx) {
	std::string membName = getMemberNameByIdx(idx);
	if (membName.empty()) return false;
	for (uint i = 0; i < members.size(); i++) {
		if (members[i].getUniqueName() == membName) {
			members[i].setInactive();
			return true;
		}
	}
	return false;
}

Member ChatRoom::getMemberByName(std::string name)
{
	if (name.empty()) throw invalid_argument("getMemberByName(): empty name error");
	for (uint i = 0; i < members.size(); i++) {
		if (members[i].getUniqueName() == name)
			return members[i];
	}
	throw invalid_argument("getMemberByName(): " + name + " does not exist\n");
}
