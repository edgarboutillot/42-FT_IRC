#include "include.hpp"
#include "server.hpp"

void Server::USER(std::string& cmd, int client_fd)
{
	std::vector<std::string> splited_cmd = split_cmd(cmd);
	if ( splited_cmd.size() < 5 )
	{
		queueDataForSend(ERR_NEEDMOREPARAMS(std::string("*"), std::string(" USER")), client_fd);
		return;
	}
	clientSetUsername(client_fd, splited_cmd[1], cmd);
}

void Server::clientSetUsername(int client_fd, std::string& username, std::string& cmd)
{
	std::string realname = cmd.substr(cmd.find_first_of(':', 0) + 1);
	Client* client		 = getClientByFd(client_fd);

	if ( client->isRegistered() )
	{
		if ( client->getUsername().empty() && client->getRealname().empty() )
		{
			client->setUsername(username);
			client->setRealname(realname);
			if ( !client->getNickname().empty() )
			{
				queueDataForSend(RPL_WELCOME(std::string(client->getNickname())), client_fd);
				client->setLogged(true);
			}
		}
		else
		{
			queueDataForSend(ERR_ALREADYREGISTRED(std::string(client->getNickname())), client_fd);
		}
	}
	else
	{
		queueDataForSend(ERR_NOTREGISTERED(std::string("*")), client_fd);
	}
}
