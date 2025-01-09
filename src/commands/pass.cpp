#include "include.hpp"
#include "server.hpp"

void Server::PASS(std::string& cmd, int client_fd)
{
	std::vector<std::string> splited_cmd = split_cmd(cmd);
	if ( splited_cmd.size() < 2 )
	{
		queueDataForSend(ERR_NEEDMOREPARAMS(std::string("*"), std::string(" PASS")), client_fd);
		return;
	}
	clientAuthentification(client_fd, splited_cmd[1]);
}

void Server::clientAuthentification(int client_fd, std::string& cmd)
{
	Client* client = getClientByFd(client_fd);
	if ( cmd == getPassword() || _password.empty())
	{
		if ( client->isRegistered() && client->getNickname().empty() )
		{
			std::cout << "Client <" << GRE << client_fd << WHI << "> is already registered" << std::endl;
			queueDataForSend(ERR_ALREADYREGISTRED(std::string("*")), client_fd);
			return;
		}
		else if ( client->isRegistered() && !client->getNickname().empty() )
		{
			queueDataForSend(ERR_ALREADYREGISTRED(std::string(client->getNickname())), client_fd);
			return;
		}
		else
		{
			std::cout << "Client <" << GRE << client_fd << WHI << "> is now registered" << std::endl;
			client->setRegistered(true);
		}
	}
	else if ( !client->getNickname().empty() )
	{
		queueDataForSend(ERR_PASSWDMISMATCH(client->getNickname()), client_fd);
		return;
	}
	else
		queueDataForSend(ERR_PASSWDMISMATCH(std::string("*")), client_fd);
}
