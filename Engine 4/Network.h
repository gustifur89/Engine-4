#pragma once
#include "Header.h"
#include "GameObject.h"
#include "EventManager.h"

class ClientData
{
private:
	int id_;
	std::string username_;

public:
	std::string lastUpdate;
	bool didUpdate;

	ClientData(int id) : id_(id) { didUpdate = false; }

	std::shared_ptr<GameObject> clientObject;
	std::queue<glm::vec3> posQue;
	std::queue<glm::vec3> velQue;
	bool updateMutex;

	void setUsername(std::string username) { username_ = username; }

	int getID() { return id_; }
	std::string getUsername() { return username_; }
};

class Network
{
public:
	enum TYPE_FLAG
	{
		UPDATE,
		EVENT,
		NEW_USER,
		SET_USER_ID,
		DISCONNECT		
	};
	enum NETWORK_TYPE
	{
		SERVER,
		CLIENT
	};

	static const int FLOAT_SIZE;
	static const int RECIEVE_DELAY;
	static const int SEND_DELAY;
	static const int HEARTBEAT_TIMESTEP;

	static void ReadAddressFromFile(std::string fileName, std::string* address, int* port, bool* valid);
	static std::string packData(int id, glm::vec3 position, glm::vec3 velocity);
	static std::shared_ptr<Network> makeNetwork(NETWORK_TYPE type, std::string addressFile);
	static void readPacketData(std::string data, glm::vec3* position, glm::vec3* velocity);

	//regular function
	Network() { connectionValid = false; }

	virtual void start() = 0;

//	std::map<int, std::shared_ptr<GameObject>> networkObjects;
	std::queue<ClientData*> clientsToAdd;
	
	std::map<int, ClientData*> clientMap;

	std::queue<std::string> msgBuffer;

	bool running;
	NETWORK_TYPE type;
	std::string ipAddress;
	int port;
	std::thread mainThread;
	std::thread subThread;
	bool connectionValid;
	//std::shared_ptr<EventManager> eventManager;
};

class Server : public Network
{
public:
	//functions
	void* server();
	void ParsePacket(std::string packet, int* type_flag, int* id, std::string* data);
	void ParseData(ENetHost* server, char* data);
	void BroadcastPacket(ENetHost* server, std::string data);
	void SendPacket(ENetPeer* peer, std::string data);
	void* serverHeartbeat(ENetHost* server);
	
	bool heartbeatMutex;

	Server() : Network() {}
	void start();
	//parameters

};

class Client : public Network
{
private:
	glm::vec3 posInt, velInt, oldPos, oldVel;
	std::queue<glm::vec3> posQue;
	std::queue<glm::vec3> velQue;
	bool updateMutex;

public:
	//functions
	void* client();
	void* msgLoop(ENetHost* client, ENetPeer* peer);
	void ParsePacket(std::string packet, int* type_flag, int* id, std::string* data);
	void ParseData(char* data);
	void SendPacket(ENetPeer* peer, std::string data);
	void updateNetworkPosVel(glm::vec3 pos, glm::vec3 vel);

	Client() : Network() {}
	void start();
	
	//parameters
	int clientID;
};






