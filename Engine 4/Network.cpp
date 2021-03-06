#include "Network.h"

// =============== Network =================

const int Network::FLOAT_SIZE = 10;
const int Network::RECIEVE_DELAY = 10;
const int Network::SEND_DELAY = 10;
const int Network::HEARTBEAT_TIMESTEP = 100;

void Network::ReadAddressFromFile(std::string fileName, std::string* address, int* port, bool* valid)
{
	fileName = std::string("src/data/") + fileName;
	std::ifstream file;
	file = std::ifstream(fileName);

	if (file.fail())
	{
		fprintf(stderr, "failed to load file at : ");
		fprintf(stderr, fileName.c_str());
		fprintf(stderr, "\n");
		return;
	}

	std::string line;
	*valid = true; // assume we want this...

	while (!file.eof())
	{
		//assumes you get 9 in.
		file >> line;

		if (line == "address")
		{
			file >> *address;
		}
		if (line == "port")
		{
			file >> *port;
		}
		if (line == "NO")
		{
			*valid = false;
		}
	}

}

std::string Network::packData(int id, glm::vec3 position, glm::vec3 velocity, float rot, std::string state)
{
	char buffer[FLOAT_SIZE];
	std::string out = "";// std::to_string(id);
	//pos x
	snprintf(buffer, sizeof buffer, "%f", position.x);
	out += buffer;
	out += ",";
	//pos y
	snprintf(buffer, sizeof buffer, "%f", position.y);
	out += buffer;
	out += ",";
	//pos z
	snprintf(buffer, sizeof buffer, "%f", position.z);
	out += buffer;
	out += ",";
	//vel x
	snprintf(buffer, sizeof buffer, "%f", velocity.x);
	out += buffer;
	out += ",";
	//vel y
	snprintf(buffer, sizeof buffer, "%f", velocity.y);
	out += buffer;
	out += ",";
	//vel z
	snprintf(buffer, sizeof buffer, "%f", velocity.z);
	out += buffer;
	out += ",";
	//rot
	snprintf(buffer, sizeof buffer, "%f", rot);
	out += buffer;
	out += ",";
	//state
	snprintf(buffer, sizeof buffer, "%s", state);
	out += buffer;
	return out;
}

std::shared_ptr<Network> Network::makeNetwork(NETWORK_TYPE type, std::string addressFile)
{
	std::shared_ptr<Network> network;
	if (type == NETWORK_TYPE::CLIENT)
	{
		network = std::shared_ptr<Client>(new Client());
	}
	else if (type == NETWORK_TYPE::SERVER)
	{
		network = std::shared_ptr<Server>(new Server());
	}
	else
	{
		std::cout << "Failled to create network.\n";
		return NULL;
	}

	bool valid;
	Network::ReadAddressFromFile(addressFile, &network->ipAddress, &network->port, &valid);
	if (!valid) return NULL;
	return network;
}

void Network::readPacketData(std::string data, glm::vec3* position, glm::vec3* velocity, float* rot, std::string* state)
{
	int firstComma = data.find(",");
	int secondComma = firstComma + 1 + (data.substr(firstComma + 1, data.length() + 1).find(","));
	int thirdComma = secondComma + 1 + (data.substr(secondComma + 1, data.length() + 1).find(","));
	int fourthComma = thirdComma + 1 + (data.substr(thirdComma + 1, data.length() + 1).find(","));
	int fifthComma = fourthComma + 1 + (data.substr(fourthComma + 1, data.length() + 1).find(","));
	int sixthComma = fifthComma + 1 + (data.substr(fifthComma + 1, data.length() + 1).find(","));
	int seventhComma = sixthComma + 1 + (data.substr(sixthComma + 1, data.length() + 1).find(","));
	position->x = std::stof(data.substr(0, firstComma));
	position->y = std::stof(data.substr(firstComma+1, secondComma));
	position->z = std::stof(data.substr(secondComma+1, thirdComma));
	velocity->x = std::stof(data.substr(thirdComma+1, fourthComma));
	velocity->y = std::stof(data.substr(fourthComma+1, fifthComma));
	velocity->z = std::stof(data.substr(fifthComma+1, sixthComma));
	*rot = std::stof(data.substr(sixthComma + 1, seventhComma));
	*state = data.substr(seventhComma +1, data.length()+1);
}

void Network::readNameData(std::string msg, std::string* name, std::string* texture)
{
	int firstComma = msg.find(",");
	*name = msg.substr(0, firstComma);
	*texture = msg.substr(firstComma + 1, msg.length() + 1);
}

// =============== Server ==================

void Server::ParsePacket(std::string packet, int* type_flag, int* id, std::string* data)
{
	int firstBar = packet.find("|");
	int secondBar = firstBar + 1 + (packet.substr(firstBar + 1, packet.length() + 1).find("|"));

	*type_flag = std::stoi(packet.substr(0, firstBar));
	*id = std::stoi(packet.substr(firstBar + 1, secondBar));
	*data = packet.substr(secondBar + 1, packet.length() + 1);
}

void Server::ParseData(ENetHost* server, char* data)
{
	int type;
	int id;
	std::string msg;
	ParsePacket(data, &type, &id, &msg);
	std::cout << data << "\n";
	switch (type)
	{
		case Network::TYPE_FLAG::UPDATE:
		{
			//BroadcastPacket(server, PacketData::MakeServerPacket(TYPE_FLAG::MESSAGE, id, msg));
			//BroadcastPacket(server, data);
			while (heartbeatMutex) { Sleep(1); };
			heartbeatMutex = true;
			//update our own...  this will be broadcast later
			if (clientMap[id])
			{
				clientMap[id]->lastUpdate = data;
				clientMap[id]->didUpdate = true;
			}
			heartbeatMutex = false;

		}
		break;
		case Network::TYPE_FLAG::NAME_UPDATE:
		{
			//BroadcastPacket(server, PacketData::MakeServerPacket(TYPE_FLAG::NEW_USER, id, msg));
			//BroadcastPacket(server, data);
			//BroadcastPacket(server, PacketData::MakeServerPacket(TYPE_FLAG::NEW_USER, id, msg));
			//clientMap[id]->SetUsername(msg);
			while (heartbeatMutex) { Sleep(1); };
			heartbeatMutex = true;
			if (clientMap[id])
			{
				clientMap[id]->nameTexData = data;
				clientMap[id]->updateName = true;
			}			
			heartbeatMutex = false;
		}
		break;
	}
}

void Server::BroadcastPacket(ENetHost* server, std::string data)
{
	ENetPacket* packet = enet_packet_create(data.c_str(), strlen(data.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(server, 0, packet);
}

void Server::SendPacket(ENetPeer* peer, std::string data)
{
	ENetPacket* packet = enet_packet_create(data.c_str(), strlen(data.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void* Server::serverHeartbeat(ENetHost* server)
{
	//the server sends a broadcast of all player position to all the other players every HEARTBEAT_TIMESTEP

	while (running)
	{
		Sleep(HEARTBEAT_TIMESTEP);
		std::cout << "thump\n";
		while (heartbeatMutex) { Sleep(1); };
		heartbeatMutex = true;
		//go through and send all updates
		for (auto const& x : clientMap)
		{
			if (x.second)
			{
				if (x.second->didUpdate)
				{
					BroadcastPacket(server, x.second->lastUpdate);
					x.second->didUpdate = false;
				}
				if (x.second->updateName)
				{
					BroadcastPacket(server, x.second->nameTexData);
					x.second->updateName = false;
				}
			}
		}
		heartbeatMutex = false;
	}

	return nullptr;
}

void Server::start()
{
	mainThread = std::thread(&Server::server, this);
	mainThread.detach();
}

void* Server::server()
{
	if (enet_initialize() != 0)
	{
		std::cout << "error occured while initializing ENet\n";
		return NULL;
	}
	atexit(enet_deinitialize);

	ENetAddress address;
	ENetEvent event;
	ENetPeer* peer;
	ENetHost* server;

	enet_address_set_host(&address, ipAddress.c_str());// "127.0.0.1");
	//address.host = ENET_HOST_ANY;
	//address.host = ENET_HOST_ANY;
	address.port = port;

	server = enet_host_create(&address,
		4, /*max number of players to connect */
		1, /*channels*/
		0, //0 for in and out bandwidth for infinite.
		0);

	if (server == NULL)
	{
		std::cout << "Error occurred while trying to create ENet server\n";
		return NULL;
	}
	
	connectionValid = true;
	heartbeatMutex = false;
//	std::cout << "Server created at " << ipAddress << ":" << port << "\n";
	std::cout << "Server created at " << address.host << ":" << address.port << "\n";

	int new_player_id = 0;
	running = true;

	subThread = std::thread(&Server::serverHeartbeat, this, server);
	subThread.detach();

	while (running)
	{
		while (enet_host_service(server, &event, RECIEVE_DELAY) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
			{
				/*
					printf("a new client connected from %x:%u. \n",
						event.peer->address.host,
						event.peer->address.port);

					for (auto const& x : clientMap)
					{

						BroadcastPacket(server, PacketData::MakeServerPacket(TYPE_FLAG::NEW_USER, x.first, x.second->GetUsername()));
					}

					new_player_id++;
					clientMap[new_player_id] = new ClientData(new_player_id);
					event.peer->data = clientMap[new_player_id];


					SendPacket(event.peer, PacketData::MakeServerPacket(TYPE_FLAG::SET_USERNAME, new_player_id, ""));
				*/
				//send all the current people to the new guy.
				for (auto const& x : clientMap)
				{
					std::string msg = "";
					msg += std::to_string(Network::TYPE_FLAG::NEW_USER);
					msg += "|";
					msg += std::to_string(x.second->getID());
					msg += "|";
					msg += "";
					BroadcastPacket(server, msg);
					//SendPacket(event.peer, msg);
				}
				//send all the people's pos and vels
				for (auto const& x : clientMap)
				{
					if(x.second->lastUpdate != "")
						BroadcastPacket(server, x.second->lastUpdate);
						//SendPacket(event.peer, x.second->lastUpdate);
				}
				//send all the people's name/tex
				for (auto const& x : clientMap)
				{
					if(x.second->nameTexData != "")
						BroadcastPacket(server, x.second->nameTexData);
						//SendPacket(event.peer, x.second->nameTexData);
				}

				//create the new client for the new guy
				new_player_id++;
				clientMap[new_player_id] = new ClientData(new_player_id);
				event.peer->data = clientMap[new_player_id];
				//set the initial update
				std::string initUpdate = "";
				initUpdate += std::to_string(Network::TYPE_FLAG::UPDATE);
				initUpdate += "|";
				initUpdate += std::to_string(new_player_id);
				initUpdate += "|";
				initUpdate += "0.0,0.0,0.0,0.0,0.0,0.0,0.0,stand"; // pos = (0,0,0), vel = (0,0,0)
				clientMap[new_player_id]->lastUpdate = initUpdate;
				clientMap[new_player_id]->didUpdate = true;

				//create the message to tell the guy his id
				std::string userIdMsg = "";
				userIdMsg += std::to_string(Network::TYPE_FLAG::SET_USER_ID);
				userIdMsg += "|";
				userIdMsg += std::to_string(new_player_id);
				userIdMsg += "|";
				userIdMsg += "";
				SendPacket(event.peer, userIdMsg);

				//send the new gusy existance to everyone.
				std::string msg = "";
				msg += std::to_string(Network::TYPE_FLAG::NEW_USER);
				msg += "|";
				msg += std::to_string(clientMap[new_player_id]->getID());
				msg += "|";
				msg += "";
				BroadcastPacket(server, msg);
			}
			break;
			case ENET_EVENT_TYPE_RECEIVE:
			{
				ParseData(server, (char*)event.packet->data);
				enet_packet_destroy(event.packet);	
			}
			break;
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				/*
					printf(" %x:%u disconnected.\n",
						event.peer->address.host,
						event.peer->address.port);

					BroadcastPacket(server, PacketData::MakeServerPacket(TYPE_FLAG::DISCONNECT, static_cast<ClientData*>(event.peer->data)->GetID(), ""));
					event.peer->data = NULL;
				*/
				std::string disconnectMsg = "";
				disconnectMsg += std::to_string(Network::TYPE_FLAG::DISCONNECT);
				disconnectMsg += "|";
				disconnectMsg += std::to_string(static_cast<ClientData*>(event.peer->data)->getID());
				disconnectMsg += "|";
				disconnectMsg += "";
				BroadcastPacket(server, disconnectMsg);
				event.peer->data = NULL;
			}
			break;
			}
		}	
	}
	
	enet_host_destroy(server);
	return NULL;
}

// =============== Client ==================

void Client::ParsePacket(std::string packet, int* type_flag, int* id, std::string* data)
{
	int firstBar = packet.find("|");
	int secondBar = firstBar + 1 + (packet.substr(firstBar + 1, packet.length() + 1).find("|"));

	*type_flag = std::stoi(packet.substr(0, firstBar));
	*id = std::stoi(packet.substr(firstBar + 1, secondBar));
	*data = packet.substr(secondBar + 1, packet.length() + 1);
}

void Client::ParseData(char* data)
{
	int type;
	int id;
	std::string msg;
	ParsePacket(data, &type, &id, &msg);
	//std::cout << data << "\n";
	switch (type)
	{
		case Network::TYPE_FLAG::UPDATE:
		{
			//UPDATE THE REAL PLAYER..
			// Push it to a buffer?
			if (id != clientID)
			{
				
				if (clientMap[id])
				{
					//delays until we can touch the data.
					while (clientMap[id]->updateMutex) { Sleep(1); }

					clientMap[id]->updateMutex = true;
					//std::cout << id << " : " << msg << "\n";
					glm::vec3 pos, vel;
					float rot;
					std::string state;
					Network::readPacketData(msg, &pos, &vel, &rot, &state);
										
					clientMap[id]->posQue.push(pos);
					clientMap[id]->velQue.push(vel);
					clientMap[id]->rotQue.push(rot);
					clientMap[id]->stateQue.push(state);
					clientMap[id]->updateMutex = false;
				}
				
			}
		}
		break;
		case Network::TYPE_FLAG::NEW_USER:
		{
			if (id != clientID)
			{
				clientMap[id] = new ClientData(id);
				clientsToAdd.push(clientMap[id]);
			}
		}
		break;
		case Network::TYPE_FLAG::NAME_UPDATE:
		{
			if (id != clientID)
			{
				if (clientMap[id])
				{
					while (clientMap[id]->updateMutex) { Sleep(1); }
					clientMap[id]->updateMutex = true;
					clientMap[id]->updateName = true;
					Network::readNameData(msg, &clientMap[id]->name, &clientMap[id]->texture);
					clientMap[id]->updateMutex = false;
				}
			}
		}
		break;
		case Network::TYPE_FLAG::SET_USER_ID:
		{	
			if (clientID == -1)
			{
				clientID = id;
				while (posQue.size() > 0)
				{
					posInt = posQue.front();
					posQue.pop();
				}
				while (velQue.size() > 0)
				{
					velInt = velQue.front();
					velQue.pop();
				}

				//we only update if velocity changes. Because the simulations should sinc well enough
				// but velocity changes simulation much more than position...
				//if (posInt != oldPos || velInt != oldVel)
				if (velInt != oldVel)
				{
					//new values so update
					/*
					std::string msg = "";
					msg += std::to_string(Network::TYPE_FLAG::UPDATE);
					msg += "|";
					msg += std::to_string(clientID);
					msg += "|";
					msg += Network::packData(clientID, posInt, velInt);
					SendPacket(peer, msg);
					oldVel = velInt;
					oldPos = posInt;
					*/
				}
			}
		break;
		}
	}

}

void Client::SendPacket(ENetPeer* peer, std::string data)
{
	ENetPacket* packet = enet_packet_create(data.c_str(), strlen(data.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, 0, packet);
}

void Client::updateNetworkPosVel(glm::vec3 pos, glm::vec3 vel, float rot, std::string state)
{
	updateMutex = true;
	posQue.push(pos);
	velQue.push(vel);
	rotQue.push(rot);
	stateQue.push(state);
	updateMutex = false;
}

void Client::updateNametex(std::string name, std::string tex)
{
	this->name = name;
	this->tex = tex;
	newNameTex = true;
}

void Client::start()
{
	mainThread = std::thread(&Client::client, this);
	mainThread.detach();
}

void* Client::client()
{
	clientID = -1;
	updateMutex = false;
	newNameTex = false;
	stateInt = "stand";
	if (enet_initialize() != 0)
	{
		std::cout << "error occured while initializing ENet\n";
		return NULL;
	}
	atexit(enet_deinitialize);

	ENetHost* client;
	client = enet_host_create(NULL, 1, 1, 0, 0);

	if (client == NULL)
	{
		std::cout << "an error occured while trying to create ENet client\n";
		return NULL;
	}

	ENetAddress address;
	ENetEvent event;
	ENetPeer* peer;

	enet_address_set_host_ip(&address, ipAddress.c_str());// "127.0.0.1");
	address.port = port;// 7777;

	peer = enet_host_connect(client, &address, 1, 0);
	if (peer == NULL)
	{
		std::cout << "No peers for ENet connection\n";
		return NULL;
	}

	if (enet_host_service(client, &event, 5000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		//std::cout << "connection to " << ipAddress<<":"<< port <<" succeeded\n";
		std::cout << "connection to " << address.host << ":" << address.port << " succeeded\n";
		connectionValid = true;
	}
	else
	{
		enet_peer_reset(peer);
		std::cout << "connection to " << ipAddress << ":" << port << " failed\n";
		return NULL;
	};

	//create thread to send data
	running = true;

	//subThread = std::thread(&Client::msgLoop, this, &client, &peer);
	subThread = std::thread(&Client::msgLoop, this, client, peer);// , this);
	subThread.detach();

	while (running)
	{
		//ENetEvent event;
		while (enet_host_service(client, &event, RECIEVE_DELAY) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				ParseData((char*)event.packet->data);
				enet_packet_destroy(event.packet);
				break;
			}
		}
	}

	enet_peer_disconnect(peer, 0);

	while (enet_host_service(client, &event, 3000) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
		case ENET_EVENT_TYPE_DISCONNECT:
			std::cout << "disconection suceeded\n";
			break;
		}
	}
	return NULL;
}

void* Client::msgLoop(ENetHost* client, ENetPeer* peer)
{
	while (running)
	{
		//std::string msg;
		//std::getline(std::cin, msg);

		/*char massageData[MSG_SIZE] = "1|";
		strcat_s(massageData, std::to_string(CLIENT_ID).c_str());
		strcat_s(massageData, "|");
		strcat_s(massageData, msg.c_str());
		SendPacket(peer, massageData);*/
		//SendPacket(peer, PacketData::MakeClientPacket(TYPE_FLAG::MESSAGE, msg));
		if(SEND_DELAY > 0)
			Sleep(SEND_DELAY);
		//pauses while we change data...
		while (updateMutex) {};
		while (posQue.size() > 0)
		{
			posInt = posQue.front();
			posQue.pop();
		}
		while (velQue.size() > 0)
		{
			velInt = velQue.front();
			velQue.pop();
		}
		while (rotQue.size() > 0)
		{
			rotInt = rotQue.front();
			rotQue.pop();
		}
		while (stateQue.size() > 0)
		{
			stateInt = stateQue.front();
			stateQue.pop();
		}

		//we only update if velocity changes. Because the simulations should sinc well enough
		// but velocity changes simulation much more than position...
		//if (posInt != oldPos || velInt != oldVel)
		/*if (posInt != oldPos || velInt != oldVel || rotInt != oldRot || stateInt != oldState)
		{
			//new values so update
			std::string msg = "";
			msg += std::to_string(Network::TYPE_FLAG::UPDATE);
			msg += "|";
			msg += std::to_string(clientID);
			msg += "|";
			msg += Network::packData(clientID, posInt, velInt, rotInt, stateInt);
			SendPacket(peer, msg);
			oldVel = velInt;
			oldPos = posInt;
			oldRot = rotInt;
			oldState = stateInt;
		}*/
		//new values so update
		std::string msg = "";
		msg += std::to_string(Network::TYPE_FLAG::UPDATE);
		msg += "|";
		msg += std::to_string(clientID);
		msg += "|";
		msg += Network::packData(clientID, posInt, velInt, rotInt, stateInt);
		SendPacket(peer, msg);
		oldVel = velInt;
		oldPos = posInt;
		oldRot = rotInt;
		oldState = stateInt;


		if (newNameTex)
		{
			//send new name/tex
			std::string msg = "";
			msg += std::to_string(Network::TYPE_FLAG::NAME_UPDATE);
			msg += "|";
			msg += std::to_string(clientID);
			msg += "|";
			msg += name +std::string(",") + tex;
			SendPacket(peer, msg);
			newNameTex = false;
		}

		//if (msg == "exit")
	//	{
	//		running = false;
	//	}
	}
	return NULL;
}
