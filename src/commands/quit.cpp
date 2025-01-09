#include "include.hpp"
#include "server.hpp"

 void Server::QUIT(std::vector<std::string> splited_cmd, std::string& cmd, int client_fd)
{
	if ( splited_cmd.size() > 1 )
	{
		std::string quitMsg = cmd.substr(cmd.find_first_of(' ', +1));
		clientDisconnect(client_fd);
	}
	else if ( splited_cmd.size() == 1 )
	{
		clientDisconnect(client_fd);
	}
	else if ( splited_cmd.size() > 2 )
	{
		std::cout << "Command not found" << std::endl;
		std::cout << "usage for the command 'QUIT' : QUIT [<reason>]" << std::endl;
	}
 }

void Server::clientDisconnect(int client_fd)
{
	Client* client = _clients[client_fd];
	if ( client )
	{
//		sendQuitToAll(client);
		if ( !client->getNickname().empty() )
		{
			_nickUsed.erase(client->getNickname());
			client->setLogged(false);
		}
		_partialCommand.erase(client_fd);
		std::cout << "Client <" << GRE << client_fd << WHI << "> removed from clients list" << std::endl;
	}
	close(client_fd);
}