//
// Created by dupuy on 02/04/24.
//

#include "channel.hpp"
#include "client.hpp"

std::vector<std::string> Server::split_cmd(const std::string& str)
{
	std::vector<std::string> vector;
	std::istringstream iss(str);
	std::string token;
	while ( iss >> token )
	{
		vector.push_back(token);
		token.clear();
	}
	return vector;
}

void Server::parseCommand(std::string& cmd, int client_fd)
{
	Client* client = getClientByFd(client_fd);

	if ( cmd.empty() )
		return;

	std::vector<std::string> splited_cmd = split_cmd(cmd);

	size_t found = cmd.find_first_not_of(" \t\v");
		// std::cout << "\n\n"
		// 		  << "Commande: " << cmd << "\n";

	if ( found != std::string::npos )
		cmd = cmd.substr(found);
	if ( splited_cmd.empty() )
		return;

	std::string command = splited_cmd[0];
	std::transform(command.begin(), command.end(), command.begin(), ::toupper);

	if ( command == "PASS" )
		PASS(cmd, client_fd);
	else if ( command == "NICK" )
		NICK(cmd, client_fd);
	else if ( command == "USER" )
		USER(cmd, client_fd);
	else if (command == "QUIT")
		QUIT(splited_cmd, cmd, client_fd);
	else if ( command == "JOIN" )
		JOIN(cmd, client_fd);
	else if ( command == "PRIVMSG" )
		PRIVMSG(cmd, client_fd);
	else if ( command == "KICK" )
		KICK(cmd, client_fd);
	else if ( command == "PART" )
		PART(cmd, client_fd);
	else if ( command == "TOPIC" )
		TOPIC(cmd, client_fd);
	else if ( command == "MODE" )
		MODE(splited_cmd, client, client_fd);
	else if ( command == "INVITE" )
		INVITE(splited_cmd, client_fd);
}