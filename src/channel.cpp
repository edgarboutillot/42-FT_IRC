//
// Created by dupuy on 02/04/24.
//

#include "channel.hpp"

Channel::Channel ( const std::string& name )
	: _isByKey ( false ), _isOnInvitation ( false ), _isTopic ( false ), _isTopicProtected ( false ), _limit ( false ),
	  _name ( name )
{
}

Channel::~Channel () {}

/**
 * @brief Adds a client to the channel.
 *
 * This method is used to add a client to the channel. It first checks if the client
 * is already in the channel. If the client is not in the channel, it adds the client.
 *
 * @param clientName The name of the client to be added.
 * @param client A pointer to the Client object to be added.
 */
void Channel::addClient ( const std::string& clientName, Client* client )
{
	std::map< std::string, Client* >::iterator it = _MapClient.find ( clientName );
	if ( it == _MapClient.end () )
		_MapClient[clientName] = client;
}

/**
 * @brief Removes a client from the channel.
 *
 * This method is used to remove a client from the channel. It first checks if the client
 * is in the channel. If the client is in the channel, it removes the client.
 *
 * @param client A pointer to the Client object to be removed.
 */
void Channel::removeClient ( Client* client )
{
	// Vérification de la validité du pointeur client
	if ( !client )
	{
		// Gérer l'erreur de pointeur nul (vous pouvez lancer une exception, afficher un message d'erreur, etc.)
		return;
	}

	for ( std::map< std::string, Client* >::iterator it = _MapClient.begin (); it != _MapClient.end (); )
	{
		// Vérification de la validité du pointeur it->second
		if ( it->second )
		{
			if ( it->second->getFd () == client->getFd () )
			{
				std::cout << "Client " << it->first << " removed from channel " << _name << std::endl;
				std::map< std::string, Client* >::iterator eraseIt = it;
				++it;
				_MapClient.erase ( eraseIt );
				continue; // Passer à la prochaine itération pour éviter de faire ++it encore une fois
			}
		}
		std::cout << "Client " << it->first << " not removed from channel " << _name << std::endl;
		++it;
	}
}

/**
 * @brief Getter for the channel name.
 *
 * This method is used to get the name of the channel.
 *
 * @return Returns the name of the channel.
 */
const std::string& Channel::getName () const
{
	return _name;
}

/**
 * @brief Getter for the map of clients in the channel.
 *
 * This method is used to get the map of clients in the channel. The map contains
 * the client names as keys and pointers to the Client objects as values.
 *
 * @return Returns the map of clients in the channel.
 */
std::map< std::string, Client* >& Channel::getMapClient ()
{
	return _MapClient;
}

/**
 * @brief Checks if a client is in the channel.
 *
 * This method is used to check if a client is in the channel. It searches for the client's name
 * in the map of clients. If the client's name is found, it returns true, otherwise it returns false.
 *
 * @param name The name of the client to be checked.
 * @return Returns true if the client is in the channel, false otherwise.
 */
bool Channel::isInChannel ( std::string name )
{
	std::map< std::string, Client* >::iterator it = _MapClient.find ( name );
	if ( it != _MapClient.end () )
		return true;
	return false;
}

/**
 * @brief Finds a client in the channel by their file descriptor.
 *
 * This method is used to find a client in the channel by their file descriptor. It iterates over
 * the map of clients and checks if the file descriptor of each client matches the given file descriptor.
 * If a match is found, it returns true, otherwise it returns false.
 *
 * @param client_fd The file descriptor of the client to be found.
 * @return Returns true if a client with the given file descriptor is found, false otherwise.
 */
bool Channel::isInChanByFd ( int client_fd )
{
	// Itération à travers tous les clients dans le canal
	for ( std::map< std::string, Client* >::iterator it = _MapClient.begin (); it != _MapClient.end (); ++it )
	{
		// Vérification de la validité du pointeur client
		if ( it->second )
		{
			// Vérification du descripteur de fichier du client dans le canal
			if ( it->second->getFd () == client_fd )
				return true;
		}
		else
		{
			// Gérer l'erreur de pointeur client invalide (vous pouvez lancer une exception, afficher un message
			// d'erreur, etc.)
		}
	}
	return false;
}

/**
 * @brief Sets the invitation status of the channel.
 *
 * This method is used to set the invitation status of the channel. If the parameter is true,
 * the channel is set to invitation-only. If the parameter is false, the channel is open to all clients.
 *
 * @param isOnInvitation The invitation status to be set for the channel.
 */
void Channel::setOnInvitation ( bool isOnInvitation )
{
	_isOnInvitation = isOnInvitation;
}

/**
 * @brief Sets the password for the channel and its protection status.
 *
 * This method is used to set the password for the channel and whether the channel is protected by a password.
 * If the second parameter is true, the channel is protected by the given password. If it is false, the channel is not
 * password-protected.
 *
 * @param password The password to be set for the channel.
 * @param isByKey The protection status to be set for the channel.
 */
void Channel::setPassword ( const std::string& password, bool isByKey )
{
	_password = password;
	_isByKey  = isByKey;
}

/**
 * @brief Sets a client as an operator of the channel.
 *
 * This method is used to set a client as an operator of the channel. It first checks if the client is already an
 * operator. If the client is not an operator, it adds the client to the list of operators. If the client is already an
 * operator, it outputs a message.
 *
 * @param client A pointer to the Client object to be set as an operator.
 */
void Channel::setChannelOperator ( Client* client )
{
	if ( !isOperator ( client ) )
		_ChannelOperator[client] = std::time ( 0 );
	else
		std::cout << "the client " << client->getNickname () << " is already operator" << std::endl;
}

/**
 * @brief Checks if a client is an operator of the channel.
 *
 * This method is used to check if a client is an operator of the channel. It searches for the client's name
 * in the list of operators. If the client's name is found, it returns true, otherwise it returns false.
 *
 * @param name The name of the client to be checked.
 * @return Returns true if the client is an operator, false otherwise.
 */
bool Channel::isOperator ( Client* client )
{
	OperTime::iterator it = _ChannelOperator.find ( client );
	if ( it != _ChannelOperator.end () )
		return true;
	return false;
}

/**
 * @brief Checks if the channel is limited.
 *
 * This method is used to check if the channel is limited. If the channel is limited, it returns true, otherwise it
 * returns false.
 *
 * @return Returns true if the channel is limited, false otherwise.
 */
bool Channel::isLimited () const
{
	return _limit;
}

/**
 * @brief Sets the limit for the channel.
 *
 * This method is used to set the limit for the channel. If the second parameter is true, the channel is limited to the
 * number of clients specified by the first parameter. If the second parameter is false, the channel is not limited.
 *
 * @param value The maximum number of clients allowed in the channel.
 * @param isLimit The limit status to be set for the channel.
 */
void Channel::setLimit ( std::string value, bool isLimit )
{
	if ( isLimit )
	{
		_limit	= isLimit;
		int tmp = std::atoi ( value.c_str () );
		if ( tmp <= 0 )
			_limitValue = 0;
		else
			_limitValue = tmp;
	}
	else
	{
		_limit		= isLimit;
		_limitValue = 0;
	}
}

/**
 * @brief Gets the number of clients in the channel.
 *
 * This method is used to get the number of clients currently in the channel.
 * It returns the size of the map of clients.
 *
 * @return Returns the number of clients in the channel.
 */
int Channel::getNumberOfClients ()
{
	return _MapClient.size ();
}

/**
 * @brief Gets the maximum number of clients allowed in the channel.
 *
 * This method is used to get the maximum number of clients that can be in the channel at the same time.
 * This value is set when the channel is limited.
 *
 * @return Returns the maximum number of clients allowed in the channel.
 */
int Channel::getClientLimit ()
{
	return _limitValue;
}

/**
 * @brief Gets all client names in the channel.
 *
 * This method is used to get all the names of the clients currently in the channel.
 * It iterates over the map of clients and concatenates their names into a string.
 *
 * @return Returns a string containing all client names in the channel.
 */
std::string Channel::getAllNames ()
{
	std::string names;
	for ( std::map< std::string, Client* >::iterator it = _MapClient.begin (); it != _MapClient.end (); ++it )
	{
		if ( this->isOperator ( it->second ) )
			names += "@";
		names += it->first;
		names += " ";
	}
	return names;
}

/**
 * @brief Gets the operators of the channel.
 *
 * This method is used to get the operators of the channel.
 *
 * @return Returns a vector containing pointers to the Client objects who are operators of the channel.
 */
OperTime& Channel::getOperator ()
{
	return _ChannelOperator;
}

/**
 * @brief Removes a client as an operator of the channel.
 *
 * This method is used to remove a client as an operator of the channel. It first checks if the client
 * is an operator. If the client is an operator, it removes the client from the list of operators.
 *
 * @param client A pointer to the Client object to be removed as an operator.
 */
void Channel::removeOperator ( Client* client )
{
	OperTime::iterator it = _ChannelOperator.find ( client );
	if ( it != _ChannelOperator.end () )
	{
		_ChannelOperator.erase ( it );
		std::cout << "The client " << client->getNickname () << " has been removed as an operator" << std::endl;
	}
	else
	{
		std::cout << "The client " << client->getNickname () << " is not an operator" << std::endl;
	}
}

/**
 * @brief Checks if the channel is invitation-only.
 *
 * This method is used to check if the channel is invitation-only. If the channel is invitation-only, it returns true,
 * otherwise it returns false.
 *
 * @return Returns true if the channel is invitation-only, false otherwise.
 */
bool Channel::isOnInvitation () const
{
	return ( _isOnInvitation );
}

/**
 * @brief Checks if a client is in the invitation list of the channel.
 *
 * This method is used to check if a client is in the invitation list of the channel. It searches for the client's name
 * in the invitation list. If the client's name is found, it returns true, otherwise it returns false.
 *
 * @param name The name of the client to be checked.
 * @return Returns true if the client is in the invitation list, false otherwise.
 */
bool Channel::isInInviteList ( Client* client )
{
	if ( client )
	{
		for ( std::vector< Client* >::iterator it = _inviteList.begin (); it != _inviteList.end (); ++it )
			if ( *it == client )
				return true;
	}
	return false;
}

/**
 * @brief Adds a client to the invitation list of the channel.
 *
 * This method is used to add a client to the invitation list of the channel.
 *
 * @param name The name of the client to be added to the invitation list.
 */
void Channel::addClientToInviteList ( Client* client )
{
	if ( client )
		_inviteList.push_back ( client );
}

/**
 * @brief Checks if the channel is password-protected.
 *
 * This method is used to check if the channel is password-protected. If the channel is password-protected, it returns
 * true, otherwise it returns false.
 *
 * @return Returns true if the channel is password-protected, false otherwise.
 */
bool Channel::isProtected ()
{
	return _isByKey;
}

/**
 * @brief Checks if a given key matches the channel's password.
 *
 * This method is used to check if a given key matches the channel's password. If the key matches the password, it
 * returns true, otherwise it returns false.
 *
 * @param key The key to be checked.
 * @return Returns true if the key matches the channel's password, false otherwise.
 */
bool Channel::isGoodKey ( std::string& key )
{
	if ( _password == key )
		return true;
	return false;
}

/**
 * @brief Adds an argument to the mode vector.
 *
 * This method is used to add an argument to the mode vector. It first adds the argument to the vector,
 * then concatenates all the arguments in the vector into a string.
 *
 * @param argToAdd The argument to be added to the mode vector.
 * @return Returns a string containing all the arguments in the mode vector.
 */
/*std::string Channel::addArgToModeVector ( std::string argToAdd )
{
	std::string modeArgs;
	_modeArgs.push_back ( argToAdd );
	for ( size_t i = 0; i < _modeArgs.size (); ++i )
		modeArgs += _modeArgs[i];
	return modeArgs;
}*/

std::string Channel::addArgToModeVector ( std::string argToAdd )
{
	std::string modeArgs;
	const char nonDuplicateModes[] = { 'i', 't', 'o', 'k', 'l' };
	bool isNonDuplicateMode		   = false;

	for ( int i = 0; i < 5; ++i )
	{
		if ( argToAdd[0] == nonDuplicateModes[i] )
		{
			isNonDuplicateMode = true;
			break;
		}
	}

	if ( isNonDuplicateMode )
	{
		bool alreadyExists = false;
		for ( size_t i = 0; i < _modeArgs.size (); ++i )
		{
			if ( _modeArgs[i] == argToAdd )
			{
				alreadyExists = true;
				break;
			}
		}
		if ( !alreadyExists )
			_modeArgs.push_back ( argToAdd );
	}
	else
		_modeArgs.push_back ( argToAdd );

	for ( size_t i = 0; i < _modeArgs.size (); ++i )
		modeArgs += _modeArgs[i];
	return modeArgs;
}

/**
 * @brief Removes an argument from the mode vector.
 *
 * This method is used to remove an argument from the mode vector. It first checks if the argument is in the vector,
 * if it is, it removes the argument. Then it concatenates all the remaining arguments in the vector into a string.
 *
 * @param argToRemove The argument to be removed from the mode vector.
 * @return Returns a string containing all the remaining arguments in the mode vector.
 */
std::string Channel::removeArgToModeVector ( std::string argToRemove )
{
	std::string modeArgs;
	for ( size_t i = 0; i < _modeArgs.size (); ++i )
		if ( _modeArgs[i] == argToRemove )
			_modeArgs.erase ( std::remove ( _modeArgs.begin (), _modeArgs.end (), argToRemove ), _modeArgs.end () );
	for ( size_t i = 0; i < _modeArgs.size (); ++i )
		modeArgs += _modeArgs[i];
	return modeArgs;
}

/**
 * @brief Gets all the arguments in the mode vector.
 *
 * This method is used to get all the arguments in the mode vector. It concatenates all the arguments in the vector into
 * a string.
 *
 * @return Returns a string containing all the arguments in the mode vector.
 */
std::string Channel::getModeArgs ()
{
	std::string args;
	for ( size_t i = 0; i < _modeArgs.size (); ++i )
		args += _modeArgs[i];
	return args;
}

/**
 * @brief Sets the topic protection status of the channel.
 *
 * This method is used to set the topic protection status of the channel. If the parameter is true,
 * the channel's topic is protected. If the parameter is false, the channel's topic is not protected.
 *
 * @param b The topic protection status to be set for the channel.
 */
void Channel::setTopicProtected ( bool b )
{
	_isTopicProtected = b;
}

/**
 * @brief Checks if the channel's topic is protected.
 *
 * This method is used to check if the channel's topic is protected. If the topic is protected, it returns true,
 * otherwise it returns false.
 *
 * @return Returns true if the channel's topic is protected, false otherwise.
 */
bool Channel::isTopicProtected ()
{
	return _isTopicProtected;
}

/**
 * @brief Sets the topic of the channel and its status.
 *
 * This method is used to set the topic of the channel and whether the channel has a topic.
 * If the second parameter is true, the channel has the given topic. If it is false, the channel does not have a topic.
 *
 * @param topic The topic to be set for the channel.
 * @param isTopic The topic status to be set for the channel.
 */
void Channel::setTopic ( const std::string& topic, bool isTopic )
{
	_isTopic = isTopic;
	_topic	 = topic;
}

/**
 * @brief Checks if the channel has a topic.
 *
 * This method is used to check if the channel has a topic. If the channel has a topic, it returns true,
 * otherwise it returns false.
 *
 * @return Returns true if the channel has a topic, false otherwise.
 */
bool Channel::isTopic ()
{
	return _isTopic;
}

/**
 * @brief Gets the topic of the channel.
 *
 * This method is used to get the topic of the channel.
 *
 * @return Returns the topic of the channel.
 */
std::string Channel::getTopic ()
{
	return _topic;
}

/**
 * @brief Checks if a mode is already set for the channel.
 *
 * This method is used to check if a mode is already set for the channel. It searches for the mode
 * in the mode vector. If the mode is found, it returns true, otherwise it returns false.
 *
 * @param mode The mode to be checked.
 * @return Returns true if the mode is already set, false otherwise.
 */
bool Channel::alreadySet ( std::string mode )
{
	for ( size_t i = 0; i < _modeArgs.size (); ++i )
		if ( mode == _modeArgs[i] )
			return true;
	return false;
}

/**
 * @brief Removes a client from the invitation list of the channel.
 *
 * This method is used to remove a client from the invitation list of the channel. It searches for the client's name
 * in the invitation list. If the client's name is found, it removes the client from the list.
 *
 * @param nickname The name of the client to be removed from the invitation list.
 */
void Channel::removeInviteList ( std::string nickname )
{
	Client* client = _MapClient[nickname];
	if ( client )
		_inviteList.erase ( std::remove ( _inviteList.begin (), _inviteList.end (), client ), _inviteList.end () );
}

void Channel::setClientTime ( Client* client )
{
	_ClientTime[client] = std::time ( 0 );
}

void Channel::removeClientTime ( Client* pClient )
{
	_ClientTime.erase ( pClient );
}

void Channel::setOlderClientAsOperator ()
{
	Client* olderClient;
	time_t olderTime = std::time ( 0 );

	if ( _ClientTime.empty () )
		return;
	for ( std::map< Client*, time_t >::iterator it = _ClientTime.begin (); it != _ClientTime.end (); ++it )
	{
		if ( it->second < olderTime )
		{
			olderTime	= it->second;
			olderClient = it->first;
		}
	}
	if ( olderClient != NULL )
		setChannelOperator ( olderClient );
}

std::vector< std::pair< std::string, std::string > >& Channel::getMultiModeContainer2 ()
{
	return _multiModeContainer2;
}