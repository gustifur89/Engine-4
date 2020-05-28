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
	std::string nameTexData;
	bool updateName;

	ClientData(int id) : id_(id) { didUpdate = false; updateName = false;  }

	std::shared_ptr<GameObject> clientObject;
	std::shared_ptr<GameObject> clientDisplayObject;
	std::queue<glm::vec3> posQue;
	std::queue<glm::vec3> velQue;
	std::queue<float> rotQue;
	std::queue<std::string> stateQue;
	float currentRot;
	float goalRot;
	glm::vec3 currentPos;
	glm::vec3 goalPos;
	bool updateMutex;
	std::string name;
	std::string texture;
	std::string currentState;

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
		NAME_UPDATE,
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
	static std::string packData(int id, glm::vec3 position, glm::vec3 velocity, float rot, std::string state);
	static std::shared_ptr<Network> makeNetwork(NETWORK_TYPE type, std::string addressFile);
	static void readPacketData(std::string data, glm::vec3* position, glm::vec3* velocity, float* rot, std::string* state);
	static void readNameData(std::string msg, std::string* name, std::string* texture);

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
	float rotInt, oldRot;
	std::string stateInt, oldState;
	std::queue<glm::vec3> posQue;
	std::queue<glm::vec3> velQue;
	std::queue<float> rotQue;
	std::queue<std::string> stateQue;
	std::string name, tex;
	bool newNameTex;
	bool updateMutex;

public:
	//functions
	void* client();
	void* msgLoop(ENetHost* client, ENetPeer* peer);
	void ParsePacket(std::string packet, int* type_flag, int* id, std::string* data);
	void ParseData(char* data);
	void SendPacket(ENetPeer* peer, std::string data);
	void updateNetworkPosVel(glm::vec3 pos, glm::vec3 vel, float rot, std::string state);
	void updateNametex(std::string name, std::string tex);

	Client() : Network() {}
	void start();
	
	//parameters
	int clientID;
};






