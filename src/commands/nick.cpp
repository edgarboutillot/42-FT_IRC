#include "include.hpp"
#include "server.hpp"

void Server::NICK ( std::string& cmd, int client_fd )
{
	std::vector< std::string > splited_cmd = split_cmd ( cmd );
	if ( splited_cmd.size () < 2 )
	{
		queueDataForSend ( ERR_NONICKNAMEGIVEN ( std::string ( "*" ) ), client_fd );
		return;
	}
	clientSetNickname ( client_fd, splited_cmd[1] );
}

bool isValidNickname ( const std::string& nickname )
{
	const std::string validChars	= "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]{}\\|_^`-";
	const std::string reservedChars = "#:";

	if ( isdigit ( nickname[0] ) )
	{
		std::cerr << "Nickname cannot start with a digit." << std::endl;
		return false;
	}

	for ( size_t i = 0; i < nickname.size (); ++i )
	{
		if ( validChars.find ( nickname[i] ) == std::string::npos )
		{
			std::cerr << "Invalid character '" << nickname[i] << "' in nickname." << std::endl;
			return false;
		}
	}

	if ( reservedChars.find ( nickname[0] ) != std::string::npos )
	{
		std::cerr << "Nickname cannot start with '#' or ':'." << std::endl;
		return false;
	}

	if ( nickname.size () > 50 )
	{
		std::cerr << "Nickname is too long." << std::endl;
		return false;
	}

	return true;
}

void Server::sendNickToAll ( Client* client, const std::string& oldNickname )
{
	if ( !client )
		return;

	std::map< std::string, Channel* > channels = client->getChannels ();

	std::string newNick = client->getNickname ();

	for ( std::map< std::string, Channel* >::iterator it = channels.begin (); it != channels.end (); ++it )
	{
		std::map< std::string, Client* > clients = it->second->getMapClient ();

		for ( std::map< std::string, Client* >::iterator ite = clients.begin (); ite != clients.end (); ++ite )
			if ( ite->second )
				reply ( RPL_NICKCHANGE ( oldNickname, newNick ), ite->second->getFd () );
			else
				std::cerr << "Client is NULL" << std::endl;
	}
}

bool Server::compareNickName ( std::string& nickToCheck, int client_fd )
{
	if ( _nickUsed.empty () )
	{
		// std::cout << "No nickname to compare" << std::endl;
		return true;
	}
	for ( nickSet::iterator it = _nickUsed.begin (); it != _nickUsed.end (); ++it )
	{
		if ( isSameString ( it->first, nickToCheck ) )
		{
			if ( it->second != client_fd )
				return false;
		}
	}
	return true;
}

bool Server::isInNicks ( std::string& nickToCheck )
{
	nickSet::iterator it = _nickUsed.find ( nickToCheck );
	if ( it != _nickUsed.end () )
	{
		std::cout << "Nickname already used" << std::endl;
		return true;
	}
	return false;
}

// void Server::changeOperNickInChan(std::string& newOp, std::string& oldOp, int client_fd)
//{
//	Client* client = getClientByFd(client_fd);
//	(void)client_fd;
//	std::map<std::string, Channel*>::iterator it = _MapChannels.begin();
//	for ( ; it != _MapChannels.end(); ++it )
//	{
//		if ( it->second->isOperator(client) )
//		{
//			it->second->removeOperator(client);
//			it->second->setChannelOperator(client);
//		}
//		for ( std::map<std::string, Client*>::iterator ite = it->second->getMapClient().begin();
//			  ite != it->second->getMapClient().end(); ++ite )
//		{
//			if ( ite->first == oldOp )
//			{
//				it->second->getMapClient().erase(ite);
//				it->second->getMapClient().insert(std::pair<std::string, Client*>(newOp, client));
//			}
//		}
//	}
// }

void Server::changeOperNickInChan ( std::string& newOp, std::string& oldOp, int client_fd )
{
	Client* client = getClientByFd ( client_fd );
	(void)client_fd;

	std::map< std::string, Channel* >::iterator it = _MapChannels.begin ();
	for ( ; it != _MapChannels.end (); ++it )
	{
		if ( it->second->isOperator ( client ) )
		{
			it->second->removeOperator ( client );
			it->second->setChannelOperator ( client );
		}

		std::map< std::string, Client* >& clientMap	   = it->second->getMapClient ();
		std::map< std::string, Client* >::iterator ite = clientMap.begin ();

		while ( ite != clientMap.end () )
		{
			if ( ite->first == oldOp )
			{
				std::map< std::string, Client* >::iterator temp = ite;
				++temp;

				clientMap.erase ( ite );
				clientMap.insert ( std::pair< std::string, Client* > ( newOp, client ) );

				ite = temp;
			}
			else
			{
				++ite;
			}
		}
	}
}

void Server::clientSetNickname ( int client_fd, std::string& nick_name )
{
	Client* client = getClientByFd ( client_fd );
	if ( client->isRegistered () )
	{
		if ( isValidNickname ( nick_name ) )
		{
			if ( client->getNickname () == nick_name )
				return;
			if ( isInNicks ( nick_name ) || !compareNickName ( nick_name, client_fd ) )
			{
				queueDataForSend ( ERR_NICKNAMEINUSE ( std::string ( "*" ) ), client_fd );
				return;
			}
			if ( client->getNickname ().empty () )
			{
				client->setNickname ( nick_name );
				_nickUsed[nick_name] = client->getFd ();
				if ( !client->getUsername ().empty () && !client->getRealname ().empty () )
				{
					queueDataForSend ( RPL_WELCOME ( std::string ( client->getNickname () ) ), client_fd );
					client->setLogged ( true );
				}
				queueDataForSend ( RPL_NICK ( nick_name ), client_fd );
			}
			else
			{
				std::string oldNickname = client->getNickname ().substr ();
				_nickUsed.erase ( client->getNickname () );
				_nickUsed[nick_name] = client->getFd ();
				changeOperNickInChan ( nick_name, oldNickname, client_fd );
				client->setNickname ( nick_name );
				queueDataForSend ( RPL_NICK ( nick_name ), client_fd );
				sendNickToAll ( client, oldNickname );
			}
		}
		else
		{
			queueDataForSend ( ERR_ERRONEUSNICKNAME ( std::string ( "*" ) ), client_fd );
		}
	}
	else
	{
		queueDataForSend ( ERR_NOTREGISTERED ( std::string ( "*" ) ), client_fd );
	}
}

bool Server::isNumber ( std::string& basicString )
{
	for ( size_t i = 0; i < basicString.size (); ++i )
		if ( !isdigit ( basicString[i] ) )
			return false;
	return true;
}
