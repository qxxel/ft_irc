/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:30:49 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/05/07 16:59:36 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Bot.hpp"

Command::Command(std::string raw) : _raw(raw), _name("")
{
	parse();
}

Command::~Command() { }


std::string Command::joinStrings(const std::vector<std::string>& vec)
{
	std::string	result;

	for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
	    if (it != vec.begin())
	        result += " ";
	    result += *it;
	}

	return (result);
}

std::vector<std::string>	Command::splitChannels(std::string str, char del)
{
	std::vector<std::string>	vec;

	std::stringstream	ss(str);
	std::string			tmp;

	while (getline(ss, tmp, del))
	{
		if (tmp.empty())
			throw splitFailed();
		vec.push_back(tmp);
	}

	return (vec);
}

std::map<std::string, std::string>	Command::splitChannelsPasswords(std::string str1, std::string str2, char del)
{
	std::map<std::string, std::string> map;

	std::stringstream	ss1(str1);
	std::string			tmp1;
	std::stringstream	ss2(str2);
	std::string			tmp2;

	while (getline(ss2, tmp2, del) && getline(ss1, tmp1, del))
	{
		if (tmp1.empty())
			throw splitFailed();
		map.insert(std::pair<std::string, std::string>(tmp1, tmp2));
	}
	while (getline(ss1, tmp1, del))
	{
		if (tmp1.empty())
			throw splitFailed();
		map.insert(std::pair<std::string, std::string>(tmp1, ""));
	}

	return (map);
}

void	Command::parse()
{
	std::size_t	i = 0;
	std::size_t	j;
	std::string	str;
	std::vector<std::string>	args;

	while (i <= _raw.size())
	{
		while (i < _raw.size() && isspace(_raw[i]))
			i++;
		if (i == _raw.size())
			break;
		j = i;
		if (_raw[i] == ':')
			i = _raw.size() - 1;
		else
		{
			while (i < _raw.size() && isascii(_raw[i]) && !isspace(_raw[i]))
				i++;
		}
		str = _raw.substr(j, i - j);
		if (_name.empty())
			_name = Server::str_toupper(str);
		else
			args.push_back(str);
		i++;
	}
	setArgs(args);
}

bool	Command::is_available(Server &serv, std::string name)
{
	// DO NOT HAVE THE SAME USERNAME THAN THE BOT
	if (name == "GameBot")
		return (false);

	std::vector<Client*>::const_iterator	it;
	for (it = serv.getClients().begin(); it != serv.getClients().end(); it++)
	{
		if (!(*it)->getUser().compare(name))
			return (false);
	}
	return (true);
}

bool	Command::isValidString(std::string str, bool skip)
{
	for (std::string::iterator it = str.begin(); it < str.end(); it++)
	{
		if (skip && it == str.begin())
			continue ;
		if (!Server::isValidChar(*it))
			return (false);
	}
	return (true);
}


std::vector<std::string>	Command::getArgs()
{
	return (_args);
}

std::string	Command::getName()
{
	return (_name);
}

void	Command::setName(std::string name)
{
	_name = name;
}

void	Command::setArgs(std::vector<std::string> args)
{
	_args = args;
}

std::ostream &	operator<<(std::ostream &o, Command &cmd)
{
	o << "--------------COMMAND--------------\n";
	o << "name: " << cmd.getName() << std::endl;
	if (!cmd.getArgs().empty())
		o << "args: ";
	std::vector<std::string>	vec = cmd.getArgs();
	for (size_t i = 0; i < vec.size(); i++)
	{
		if (i)
			o << "\t";
		o << i << ": " << vec[i] << std::endl;
	}
	if (0 == vec.size())
		o << std::endl;
	return (o);
}

//Speaks for itself :)
void Command::executeCommand(Server &serv, Client *client, Command *cmd, int epfd)
{
	std::vector<std::string>	args = cmd->getArgs();
	if (!cmd->getName().compare("PASS"))
		cmd->handlePass(serv, client, &args);
	else if (!cmd->getName().compare("NICK"))
		cmd->handleNick(client, &args);
	else if (!cmd->getName().compare("USER"))
		cmd->handleUser(serv, client, &args);
	else if (!cmd->getName().compare("PRIVMSG") || !cmd->getName().compare("MSG"))
		cmd->handlePrivMsg(serv, client, &args);
	else if (!cmd->getName().compare("JOIN"))
		cmd->handleJoin(serv, client, &args);
	else if (!cmd->getName().compare("PART"))
		cmd->handlePart(serv, client, &args);
	else if (!cmd->getName().compare("QUIT"))
	{
		cmd->handleQuit(serv, client, &args, epfd);
		return ;
	}
	else if (!cmd->getName().compare("KICK"))
		cmd->handleKick(serv, client, &args);
	else if (!cmd->getName().compare("INVITE"))
		cmd->handleInvite(serv, client, &args);
	else if (!cmd->getName().compare("TOPIC"))
		cmd->handleTopic(serv, client, &args);
	else if (!cmd->getName().compare("NAMES"))
		cmd->handleNames(serv, client, &args);
	else if (!cmd->getName().compare("MODE"))
		cmd->handleMode(serv, client, &args);
	else if (!cmd->getName().compare("HELP"))
		cmd->handleHelp(client, &args);
	else
		Server::sendClient(client->getFd(), ":localhost ??? " + client->getUser() + " :" UKWN_CMD + cmd->getName() + "\n");
	Server::sendClient(client->getFd(), "------------------------------------------\n");
}

//Checks for the first connection step, giving the server's PASSWORD
void	Command::handlePass(Server &serv, Client *client, std::vector<std::string> *args)
{
	// IF CLIENT ALREADY ENTERED PWD
	if (!client->getPwd())
	{
		// IF TOO MANY ARGS OR NOT ENOUGH
		if (args->size() != 1)
			return (void)Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " :" INV_FORMAT ENTER_PWD);
		// IF PWD IS GOOD
		else if (Server::simpleHash(args->at(0)) * 2 % 333 / 4 + 5 * 6 == serv.getPwd())
		{
			client->setPwd(true);
			Server::sendClient(client->getFd(), PWD_GOOD);
			if (!client->getAuth() && client->getNick().compare("") && client->getUser().compare(""))
				return (client->setAuth(true));
			else
				return (Server::sendClient(client->getFd(), ENTER_NCK_USR));
		}
		// IF PWD IS WRONG
		else
			return (Server::sendClient(client->getFd(), INV_PWD));
	}
	else
		return (void)Server::sendClient(client->getFd(), ":localhost 462 " + client->getUser() + " :" PWD_SET);
}

//Checks for the 2nd connection step, setting up a Nickname
void	Command::handleNick(Client *client, std::vector<std::string> *args)
{
	// IF CLIENT HAS NOT GIVEN THE PASSWORD
	if (!client->getPwd())
		return (void)Server::sendClient(client->getFd(), ":localhost 666 " + client->getUser() + " :" ENTER_PWD);
	// IF CLIENT HAS ALREADY SET A NICKNAME
	else if (client->getNick().compare(""))
	{
		if (client->getUser().empty())
		Server::sendClient(client->getFd(), ":localhost 667 * :" CANT_NICK);
		return ;
	}
	// IF BAD ARGUMENTS NUMBER
	else if (args->size() != 1)
		return (void)Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " :" ENTER_NICK);
	else
	{
		// IF GOOD NICKNAME
		if (isValidString(args->at(0), false))
		{
			client->setNick(args->at(0));
			Server::sendClient(client->getFd(), ":localhost ??? " + client->getUser() + " :" NICK_NAME + client->getNick() + "\n");
		}
		// IF BAD NICKNAME
		else
			Server::sendClient(client->getFd(), ":localhost ??? " + client->getUser() + " :" HAS_INVALID_CHARS);
	}
	// SETS AUTH TO TRUE IF AUTH FINISHED
	if (!client->getNick().empty() && !client->getUser().empty() && client->getPwd())
		client->setAuth(true);
	// TELLS THE CLIENTS TO SET A USERNAME
	else if (!client->getUser().compare(""))
		return (Server::sendClient(client->getFd(), ":localhost ??? " + client->getUser() + " :" ENTER_USER));
}

//Checks for the 3rd connection step, setting up a username
void	Command::handleUser(Server &serv, Client *client, std::vector<std::string> *args)
{
	// IF CLIENT HAS NOT GIVEN THE PASSWORD
	if (!client->getPwd())
		return (Server::sendClient(client->getFd(), ":localhost 699 " + client->getUser() + " :" ENTER_PWD));
	// IF BAD ARGUMENTS
	else if (args->size() < 3 || args->size() > 4 || args->at(1).compare("0") || args->at(2).compare("*"))
		return (Server::sendClient(client->getFd(), ":localhost 699 " + client->getUser() + " :" ENTER_USER));
	// IF CLIENT HAS ALREADY SET A USERNAME
	else if (!client->getUser().empty())
		return (Server::sendClient(client->getFd(), ":localhost ??? " + client->getUser() + " :" CANT_USER USR_NAME + client->getUser() + "\n"));
	else
	{
		// IF GOOD USERNAME
		if (isValidString(args->at(0), false))
		{
			// IF AVAILABLE USERNAME
			if (is_available(serv, args->at(0)))
			{
				client->setUser(args->at(0));
				return (Server::sendClient(client->getFd(), ":localhost ??? " + client->getUser() + " :" USR_NAME + client->getUser() + "\n"));
			}
			// IF USERNAME IS TAKEN
			else
				return (Server::sendClient(client->getFd(), ":localhost 811 " + client->getUser() + " :" IS_TAKEN ENTER_USER));
		}
		// IF BAD USERNAME
		else
			return (Server::sendClient(client->getFd(), ":localhost 007 " + client->getUser() + " :" HAS_INVALID_CHARS));
	}
	// IF DONE AUTHENTIFIED
	if (!client->getNick().empty() && !client->getUser().empty() && client->getPwd())
		client->setAuth(true);
	// IF NOT DONE AUTHENTIFIED
	else if (!client->getNick().compare(""))
		return (Server::sendClient(client->getFd(), ":localhost ??? " + client->getUser() + " :" ENTER_NICK));
}

// SEND MESSAGE
void	Command::handlePrivMsg(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle PRIVMSG failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: PRIVMSG <target> :<message>
	if (!args || args->size() != 2)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " PRIVMSG :Not enough parameters");
		std::cout << "handle PRIVMSG failed => invalid format" << std::endl;
		return ;
	}

	// SEARCH FOR ':' IN FRONT OF THE MESSAGE // ONLY FOR NC
	if (args->at(1)[0] != ':')
	{
		Server::sendClient(client->getFd(), ":localhost 902 " + client->getNick() + " :Need a ':' in front of the message\n");
		std::cout << "handle PRIVMSG failed => no ':' at the start of the message" << std::endl;
		return ;
	}

	// CHECK IF THE TARGET IS A CHANNEL
	if (args->at(0)[0] == '#')
	{
		// TRY TO FIND CHANNEL IN THE SERVER
		Channel	*channel = serv.searchChannel(args->at(0));
		if (!channel)
		{
			Server::sendClient(client->getFd(), ":localhost 401 " + client->getNick() + " :No such nick/channel");
			std::cout << "handle PRIVMSG failed => inexistant channel" << std::endl;
			return ;
		}

		// CHECK IF CLIENT IS IN THIS CHANNEL
		if (!client->isCurrentChannel(channel->getName()))
		{
			Server::sendClient(client->getFd(), NO_CHNL_ASK);
			std::cout << "handle PRIVMSG failed => client isn't in the channel asked" << std::endl;
			return ;
		}

		channel->sendClients(client->getUser(), ":" + client->getNick() + "!" + client->getUser() + " PRIVMSG " + channel->getName() + " " + args->at(1) + "\n");
		if (serv.getBot()->isCurrentChannel(channel->getName()))
			Bot::handleBot(channel, client, args->at(1));
	}
	// CHECK IF THE TARGET IS A CLIENT
	else
	{
		// TRY TO FIND THE TARGET IN THE SERVER
		Client	*target = serv.findClientName(args->at(0));
		if (!target)
		{
			Server::sendClient(client->getFd(), ":localhost 401 " + client->getNick() + " :No such nick/channel");
			std::cout << "handle PRIVMSG failed => inexistant target" << std::endl;
			return ;
		}

		Server::sendClient(target->getFd(), ":" + client->getNick() + "!" + client->getUser() + " PRIVMSG " + target->getNick() + " " + args->at(1) + "\n");
		if (target->getNick() == "GameBot")
			Bot::handleBot(NULL, client, args->at(1));
	}

	std::cout << "handle PRIVMSG successfuly called" << std::endl;
}

// JOIN CHANNEL
void	Command::handleJoin(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle JOIN failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: JOIN <channel>{,<channel>} [<key>{,<key>}]
	if (!args || args->size() < 1 || 2 < args->size())
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " JOIN :Not enough parameters");
		std::cout << "handle JOIN failed => invalid format" << std::endl;
		return ;
	}

	// TRY TO MAKE PAIR WITH EACH CHANNELS AND PASSWORDS
	try
	{
		std::string	passwords;
		if (args->size() == 2)
			passwords = args->at(1);

		std::map<std::string, std::string> 				channelsAsk = splitChannelsPasswords(args->at(0), passwords, ',');
		std::map<std::string, std::string>::iterator	it;
		for (it = channelsAsk.begin(); it != channelsAsk.end(); it++)
		{
			// CHECK IF CLIENT ALREADY IN THE CHANNEL
			if (client->isCurrentChannel(it->first))
			{
				Server::sendClient(client->getFd(), ":localhost 443 " + client->getNick() + " " + it->first + " :is already on channel\n");
				std::cout << "handle JOIN failed => client already in this channel" << std::endl;
				continue ;
			}

			// CHECK IF THE NAME IS VALID
			if (it->first.size() < 2 || 50 < it->first.size() || it->first[0] != '#' || !isValidString(it->first, true))
			{
				Server::sendClient(client->getFd(), ":localhost 476 " + client->getNick() + " " + it->first + " :Bad Channel Mask\n");
				std::cout << "handle JOIN failed => invalid name" << std::endl;
				continue ;
			}

			// FIND THE CHANNEL IN THE SERVER
			Channel	*channel = serv.searchChannel(it->first);
			if (!channel)
			{
				// CREATE CHANNEL
				Channel *newChannel = new Channel(it->first, client);

				serv.addChannel(newChannel);
				client->getCurrentsChannels().push_back(newChannel);
				Server::sendClient(client->getFd(), ":" + client->getNick() + "!" + client->getUser() + "@localhost JOIN :" + newChannel->getName() + "\n");
				Server::sendClient(client->getFd(), ":localhost 331 " + client->getNick() + " " + newChannel->getName() + " :No topic is set\n");
				Server::sendClient(client->getFd(), ":localhost 353 " + client->getNick() + " " + newChannel->getName() + " :" + newChannel->listClients() + "\n");
				Server::sendClient(client->getFd(), ":localhost 366 " + client->getNick() + " " + newChannel->getName() + " :End of NAMES list\n");
				Server::sendClient(client->getFd(), "MODE " + newChannel->getName() + " +l\n"); // a mettre en forme
				continue ;
			}

			// CHECK IF CLIENT CAN ACCESS
			if (channel->getInvOnly() && !(client->isJoinableChannel(channel) || channel->isOpName(client->getUser())))
			{
				Server::sendClient(client->getFd(), ":localhost 473 " + client->getNick() + " " + channel->getName() + " :Cannot join channel (+i)");
				std::cout << "handle JOIN failed => client not allowed in this channel" << std::endl;
				continue ;
			}

			// CHECK PASSWORD
			if (!channel->getPwd().empty() && !client->isJoinableChannel(channel))
			{
				// CHECK IF PASSWORD IS SENDED AND TRY IT
				if (it->second.empty() || channel->getPwd() != it->second)
				{
					Server::sendClient(client->getFd(), ":localhost 475 " + client->getNick() + " " + channel->getName() + " :Cannot join channel (+k)");
					std::cout << "handle JOIN failed => wrong password" << std::endl;
					continue ;
				}
			}

			// CHECK IF CHANNEL IS FULL
			if (channel->getMaxUsers() != 0 && !(channel->getClientsList().size() < (size_t)channel->getMaxUsers()))
			{
				Server::sendClient(client->getFd(), ":localhost 471 " + client->getNick() + " " + channel->getName() + " :Cannot join channel (+l)");
				std::cout << "handle JOIN failed => channel full" << std::endl;
				continue ;
			}

			// ADD CLIENT IN CHANNEL AND SEND MESSAGES
			client->getCurrentsChannels().push_back(channel);
			channel->getClientsList().push_back(client);
			channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost JOIN :" + channel->getName() + "\n");
			if (channel->getTopic().empty())
				Server::sendClient(client->getFd(), ":localhost 331 " + client->getNick() + " " + channel->getName() + " :No topic is set\n");
			else
				Server::sendClient(client->getFd(), ":localhost 332 " + client->getNick() + " " + channel->getName() + " " + channel->getTopic() + "\n");
			Server::sendClient(client->getFd(), ":localhost 353 " + client->getNick() + " " + channel->getName() + " :" + channel->listClients() + "\n");
			Server::sendClient(client->getFd(), ":localhost 366 " + client->getNick() + " " + channel->getName() + " :End of NAMES list\n");
		}
	}
	// CATCH IF WRONG INPUT IN CHANNELS
	catch (splitFailed &)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " JOIN :Not enough parameters");
		std::cout << "handle JOIN failed => invalid format" << std::endl;
	}

	std::cout << "handle JOIN successfully called" << std::endl;
}

// LEAVE CHANNEL
void	Command::handlePart(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle PART failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: PART <channel>{,<channel>} [:<message>]
	if (!args || args->size() < 1)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " PART :Not enough parameters");
		std::cout << "handle PART failed => wrong format" << std::endl;
		return ;
	}

	// TRY TO SPLIT CHANNELS IN ARGS
	try
	{
		std::vector<std::string>	channels = splitChannels(args->at(0), ',');
		std::vector<std::string>::iterator	it;
		for (it = channels.begin(); it != channels.end(); it++)
		{
			// FIND CHANNEL IN SERVER
			Channel	*channel = serv.searchChannel((*it));
			if (!channel)
			{
				Server::sendClient(client->getFd(), ":localhost 403 " + client->getNick() + " " + *it + " :No such channel\n");
				std::cout << "handle PART failed => inexistant channel" << std::endl;
				continue ;
			}

			// CHECK IF CLIENT IS IN THIS CHANNEL
			if (!client->isCurrentChannel(channel->getName()))
			{
				Server::sendClient(client->getFd(), "localhost 442 " + client->getNick() + " " + channel->getName() + " :You're not on that channel\n");
				std::cout << "handle PART failed => client isn't in the channel asked" << std::endl;
				continue ;
			}

			// CHECK IF THERE IS NO OP AFTER CLIENT QUIT
			if (channel->getOpList().size() == 1 && channel->isOpName(client->getUser()))
			{
				// SEARCH FOR THE OLDEST CLIENT NOT OP TO OP HIM
				Client	*oldestClient = channel->getOldestClient();
				if (oldestClient)
					channel->getOpList().push_back(oldestClient);
			}

			// IF CHANNEL IS EMPTY
			if (channel->getClientsList().size() == 1)
			{
				Server::sendClient(client->getFd(), ":" + client->getNick() + "!" + client->getUser() + "@localhost PART " + Command::joinStrings(*args) + "\n");
				this->deleteChannel(serv, channel);
				continue ;
			}

			// HANDLE FOR INVITATION ONLY
			if (channel->getInvOnly())
				client->delJoinableChannel(channel);

			// HANDLE IF TARGET IS OP
			if (channel->isOpName(client->getUser()))
				channel->delOpName(client->getUser());

			channel->delClientName(client->getUser());
			client->delCurrentChannel(channel);
			channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost PART " + Command::joinStrings(*args) + "\n");
		}
	}
	// CATCH IF WRONG INPUT IN CHANNELS
	catch (splitFailed &)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " PART :Not enough parameters");
		std::cout << "handle PART failed => wrong format" << std::endl;
		return ;
	}

	std::cout << "handle PART successfuly called" << std::endl;
}

// LEAVE SERVER
void	Command::handleQuit(Server &serv, Client *client, std::vector<std::string> *args, int epfd)
{
	// USAGE: QUIT
	if (!args || args->size() != 0)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " QUIT :Not enough parameters");
		std::cout << "handle PART failed => wrong format" << std::endl;
		return ;
	}

	serv.disconnectClient(client->getFd(), epfd);

	std::cout << "handle QUIT successfuly called" << std::endl;
}

// KICK USER ON THE CURRENT CHANNEL
void	Command::handleKick(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle KICK failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: KICK <channel> <user> [:<comment>]
	if (!args || args->size() < 2 || 3 < args->size())
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " KICK :Not enough parameters");
		std::cout << "handle KICK failed => invalid format" << std::endl;
		return ;
	}

	// CHECK IF CHANNEL EXIST
	Channel	*channel = serv.searchChannel(args->at(0));
	if (!channel)
	{
		Server::sendClient(client->getFd(), ":localhost 403 " + client->getNick() + " " + args->at(0) + " :No such channel\n");
		std::cout << "handle KICK failed => channel don't exist" << std::endl;
		return ;
	}

	// FIND CLIENT IN THE CHANNEL
	if (!client->isCurrentChannel(channel->getName()))
	{
		Server::sendClient(client->getFd(), ":localhost 442 " + client->getNick() + " " + channel->getName() + " :You're not on that channel\n");
		std::cout << "handle KICK failed => client not in the channel asked" << std::endl;
		return ;
	}

	// FIND TARGET IN THE CHANNEL
	Client	*target = channel->findClientName(args->at(1));
	if (!target)
	{
		Server::sendClient(client->getFd(), ":localhost 441 " + client->getNick() + " " + args->at(1) + " " + channel->getName() + " :They aren't on that channel\n");
		std::cout << "handle KICK failed => target isn't in the channel" << std::endl;
		return ;
	}

	// CHECK IF CLIENT IS OP
	if (!channel->isOpName(client->getUser()))
	{
		Server::sendClient(client->getFd(), ":localhost 482 " + client->getNick() + " " + channel->getName() + " :You're not channel operator\n");
		std::cout << "handle KICK failed => client isn't moderator" << std::endl;
		return ;
	}

	// CHECK IF CLIENT IS TRING TO KICK HIMSELF
	if (client == target)
	{
		Server::sendClient(client->getFd(), ":localhost 903 " + client->getUser() + " " + channel->getName() + " :You cannot kick yourself\n");
		std::cout << "handle KICK failed => the client cannot kick himself" << std::endl;
		return ;
	}

	// CHECK IF THERE IS A ':' IN FRONT OF THE COMMENT // ONLY FOR NC
	if (args->size() == 3 && args->at(2)[0] != ':')
	{
		Server::sendClient(client->getFd(), ":localhost 902 " + client->getUser() + " " + channel->getName() + " :Need a ':' in front of the comment\n");
		std::cout << "handle KICK failed => there is no ':' in front of the comment" << std::endl;
		return ;
	}

	// HANDLE FOR INVITATION ONLY
	if (channel->getInvOnly())
		target->delJoinableChannel(channel);

	// HANDLE IF TARGET IS OP
	if (channel->isOpName(target->getUser()))
		channel->delOpName(target->getUser());

	channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost KICK " + Command::joinStrings(*args) + "\n");
	target->delCurrentChannel(channel);
	channel->delClientName(target->getUser());

	std::cout << "handle KICK successfully called" << std::endl;
}

// INVITE USER IN CURRENT CHANNEL
void	Command::handleInvite(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle INVITE failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: INVITE <user> <channel>
	if (!args || args->empty() || args->size() != 2)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " INVITE :Not enough parameters");
		std::cout << "handle INVITE failed => invalid format" << std::endl;
		return ;
	}

	// FIND TARGET IN SERVER
	Client	*target = serv.findClientName(args->at(0));
	if (!target)
	{
		Server::sendClient(client->getFd(), ":localhost 401 " + client->getNick() + " " + target->getNick() + " :No such nick/channel\n");
		std::cout << "handle INVITE failed => target don't exist" << std::endl;
		return ;
	}

	// FIND CHANNEL IN SERVER
	Channel	*channel = serv.searchChannel(args->at(1));
	if (!channel)
	{
		Server::sendClient(client->getFd(), ":localhost 403 " + client->getNick() + " " + args->at(1) + " :No such channel\n");
		std::cout << "handle INVITE failed => inexistant channel" << std::endl;
		return ;
	}

	// FIND CLIENT IN THE CHANNEL
	if (!client->isCurrentChannel(channel->getName()))
	{
		Server::sendClient(client->getFd(), ":localhost 442 " + client->getNick() + " " + channel->getName() + " :You're not on that channel\n");
		std::cout << "handle INVITE failed => client isn't in the channel" << std::endl;
		return ;
	}

	// FIND TARGET IN THE CHANNEL
	if (target->isCurrentChannel(channel->getName()))
	{
		Server::sendClient(client->getFd(), ":localhost 443 " + client->getNick() + " " + target->getNick() + " " + channel->getName() + " :is already on channel\n");
		std::cout << "handle INVITE failed => target already in the channel" << std::endl;
		return ;
	}

	// CHECK INVITE ONLY CHANNEL
	if (channel->getInvOnly())
	{
		// CHECK IF CLIENT IS OP
		if (!channel->isOpName(target->getUser()))
		{
			Server::sendClient(client->getFd(), ":localhost 482 " + client->getNick() + " " + channel->getName() + " :You're not channel operator\n");
			std::cout << "handle INVITE failed => client isn't moderator" << std::endl;
			return ;
		}

		// ADD TARGET TO ACCESS LIST
		target->getJoinableChannels().push_back(channel);
	}

	if (target->getUser() == "GameBot")
		serv.getBot()->joinChannel(channel);
	else
	{
		Server::sendClient(target->getFd(), ":" + client->getNick() + "!" + client->getUser() + "@localhost INVITE " + target->getNick() + " :" + channel->getName() + "\n");
		Server::sendClient(client->getFd(), ":localhost 341 " + client->getNick() + " " + target->getNick() + " " + channel->getName() + "\n");
	}
	std::cout << "handle INVITE successfuly called" << std::endl;
}

// VIEW OR SET TOPIC OF THE CURRENT CHANNEL
void	Command::handleTopic(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle TOPIC failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: TOPIC <channel> [:<new_topic>]
	if (!args || args->empty() || args->size() > 2)
	{
		// 461 <nick> TOPIC :Not enough parameters
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getNick() + " TOPIC :Not enough parameters");
		std::cout << "handle TOPIC failed => no args" << std::endl;
		return ;
	}

	// FIND CHANNEL IN SERVER
	Channel	*channel = serv.searchChannel(args->at(0));
	if (!channel)
	{
		Server::sendClient(client->getFd(), ":localhost 403 " + client->getNick() + " " + channel->getName() + " :No such channel\n");
		std::cout << "handle TOPIC failed => channel don't exist" << std::endl;
		return ;
	}

	// VIEW TOPIC
	if (args->size() == 1)
	{
		if (channel->getTopic().empty())
			Server::sendClient(client->getFd(), ":localhost 331 " + client->getNick() + " " + channel->getName() + " :" + NO_TPC);
		else
		{
			Server::sendClient(client->getFd(), ":localhost 332 " + client->getNick() + " " + channel->getName() + " " + channel->getTopic() + "\n");
			Server::sendClient(client->getFd(), ":localhost 333 " + client->getNick() + " " + channel->getName() + " " + channel->getTopicSetter() + timeToString(channel->getTopicSetTimestamp()) + "\n");
		}
	}
	// SET TOPIC
	else
	{
		// FIND CLIENT IN THE CHANNEL
		if (!client->isCurrentChannel(channel->getName()))
		{
			Server::sendClient(client->getFd(), ":localhost 442 " + client->getNick() + " " + channel->getName() + " :You're not on that channel\n");
			std::cout << "handle set TOPIC failed => client isn't in the channel asked" << std::endl;
			return ;
		}

		// CHECK IF CLIENT IS OP
		if (channel->getLockTopic() && !channel->isOpName(client->getUser()))
		{
			Server::sendClient(client->getFd(), ":localhost 482 " + client->getNick() + " " + channel->getName() + " :You're not channel operator\n");
			std::cout << "handle set TOPIC failed => client isn't moderator" << std::endl;
			return ;
		}

		// CHECK IF THERE IS A ':' IN FRONT OF THE NEW TOPIC // ONLY FOR NC
		if (args->at(1)[0] != ':')
		{
			Server::sendClient(client->getFd(), ":localhost 902 " + client->getNick() + " " + channel->getName() + " :Need a ':' in front of the topic\n");
			std::cout << "handle set TOPIC failed => there is no ':' in front of the topic" << std::endl;
			return ;
		}

		// CHECK IF ALL CHARACTERS ARE VALID
		if (isValidString(args->at(1), true))
		{
			Server::sendClient(client->getFd(), ":localhost 900 " + client->getNick() + " " + channel->getName() + " :invalid characters in topic\n");
			std::cout << "handle set TOPIC failed => there is invalid character" << std::endl;
			return ;
		}

		// CHECK IF THE TOPIC IS ALREADY SET
		if (channel->getTopic() == args->at(1))
		{
			Server::sendClient(client->getFd(), ":localhost 901 " + client->getNick() + " " + channel->getName() + " :Topic already set\n");
			std::cout << "handle set TOPIC failed => topic already set" << std::endl;
			return ;
		}

		channel->setTopic(args->at(1));
		channel->setTopicSetter(client->getNick());
		channel->setTopicSetTimestamp(time(NULL));
		channel->sendClients("", ":" + client->getNick() + "!" + client->getUser() + "@localhost TOPIC " + channel->getName() + " " + channel->getTopic() + "\n");
	}

	std::cout << "handle TOPIC successfully called\n";
}

// VIEW CLIENTS LIST OF A CHANNEL
void	Command::handleNames(Server &serv, Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle NAMES failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: NAMES [<channel>{,<channel>}]
	if (!args || args->size() > 1)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " NAMES :Not enough parameters");
		std::cout << "handle NAMES failed => no args" << std::endl;
		return ;
	}

	// IF CLIENT WANT TO SEE THE CLIENT LIST OF ALL CHANNELS
	if (args->size() == 0)
	{
		std::vector<Channel*>::iterator	it_chnl;
		for (it_chnl = serv.getChannels().begin(); it_chnl != serv.getChannels().end(); it_chnl++)
		{
			Server::sendClient(client->getFd(), client->getNick() + " " + (*it_chnl)->getName() + " :" + (*it_chnl)->listClients() + "\n");
			Server::sendClient(client->getFd(), client->getNick() + " " + (*it_chnl)->getName() + " :End of NAMES list\n");
		}

		std::cout << "handle NAMES all successfully called" << std::endl;
		return ;
	}

	// IF CLIENT WANT TO SEE THE CLIENT LIST OF SPECIFIC CHANNEL
	try
	{
		std::vector<std::string>			channels = splitChannels(args->at(0), ',');
		std::vector<std::string>::iterator	it;
		for (it = channels.begin(); it != channels.end(); it++)
		{
			// FIND CHANNEL IN SERVER
			Channel	*channel = serv.searchChannel(args->at(0));
			if (!channel)
			{
				Server::sendClient(client->getFd(), NO_CHNL);
				std::cout << "handle NAMES failed => channel don't exist" << std::endl;
				return ;
			}

			Server::sendClient(client->getFd(), client->getNick() + " " + channel->getName() + " :" + channel->listClients() + "\n");
			Server::sendClient(client->getFd(), client->getNick() + " " + channel->getName() + " :End of NAMES list\n");
		}
	}
	catch (splitFailed &)
	{
		Server::sendClient(client->getFd(), NAMES_USG);
		std::cout << "handle NAMES failed => wrong input" << std::endl;
		return ;
	}

	std::cout << "handle NAMES successfully called\n";
}

// VIEW OR SET A MODE IN A CHANNEL
void	Command::handleMode(Server &serv, Client *client, std::vector<std::string> *args)
// SET OR UNSET MODES ON CURRENT CHANNEL
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle MODE failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: MODE <channel> [<+/-modes>] [<arguments>]
	if (!args || args->size() < 1)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " MODE :Not enough parameters");
		std::cout << "handle MODE failed => invalid format" << std::endl;
		return ;
	}

	// FIND CHANNEL IN SERVER
	Channel	*channel = serv.searchChannel(args->at(0));
	if (!channel)
	{
		Server::sendClient(client->getFd(), NO_CHNL);
		std::cout << "handle TOPIC failed => channel don't exist" << std::endl;
		return ;
	}

	// FIND CLIENT IN THE CHANNEL
	if (!client->isCurrentChannel(channel->getName()))
	{
		Server::sendClient(client->getFd(), NO_CHNL_ASK);
		std::cout << "handle TOPIC failed => client isn't in the channel asked" << std::endl;
		return ;
	}

	// SEND ACTIVE MODES
	if (args->size() == 1)
	{
		std::string	modes = " +";
		std::string	modesArgs;

		if (channel->getInvOnly())
			modes += "i";
		else if (!channel->getTopic().empty())
		{
			modes += "t";
			modesArgs += " " + channel->getTopic();
		}
		else if (!channel->getPwd().empty())
			modes += "k";
		else if (channel->getMaxUsers())
		{
			modes += "l";
			modesArgs += " " + intToString(channel->getMaxUsers());
		}
		else
			modes = "";

		Server::sendClient(client->getFd(), client->getNick() + " " + channel->getName() + modes + modesArgs + "\n");
		Server::sendClient(client->getFd(), client->getNick() + " " + channel->getName() + " " + timeToString(channel->getModeSetTimestamp()) + "\n");
		std::cout << "handle ask MODE successfuly called\n";
		return ;
	}

	// CHECK IF CLIENT IS OP
	if (!channel->isOpName(client->getUser()))
	{
		Server::sendClient(client->getFd(), NO_PERM);
		std::cout << "handle modify MODE failed => client isn't moderator" << std::endl;
		return ;
	}

	// ADD INVITE ONLY MODE
	if (args->at(1) == "+i")
	{
		// USAGE: MODE <channel> +i
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> +i\n");
			std::cout << "handle modify MODE +i failed => invalid format" << std::endl;
			return ;
		}

		channel->setInvOnly(true);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " +i\n");

		// ADD CHANNEL IN CURRENT CLIENTS' JOINABLE CHANNELS
		for (std::vector<Client*>::iterator it = channel->getClientsList().begin(); it != channel->getClientsList().end(); it++)
			(*it)->getJoinableChannels().push_back(channel);
	}
	// SUPP INVITE ONLY MODE
	if (args->at(1) == "-i")
	{
		// USAGE: MODE <channel> -i
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> -i\n");
			std::cout << "handle modify MODE -i failed => invalid format" << std::endl;
			return ;
		}

		channel->setInvOnly(false);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " -i\n");

		// ERASE CHANNEL IN ALL CLIENTS' JOINABLE CHANNELS
		for (std::vector<Client*>::iterator it1 = serv.getClients().begin(); it1 != serv.getClients().end(); it1++)
		{
			for (std::vector<Channel*>::iterator it2 = (*it1)->getJoinableChannels().begin(); it2 != (*it1)->getJoinableChannels().end(); it2++)
			{
				if ((*it2) == channel)
				{
					(*it1)->getJoinableChannels().erase(it2);
					break ;
				}
			}
		}
	}

	// ADD LOCKED TOPIC MODE
	if (args->at(1) == "+t")
	{
		// USAGE: MODE <channel> +t
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> +t\n");
			std::cout << "handle modify MODE +t failed => invalid format" << std::endl;
			return ;
		}

		channel->setLockTopic(true);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " +t\n");
	}
	// SUPP LOCKED TOPIC MODE
	else if (args->at(1) == "-t")
	{
		// USAGE: MODE <channel> -t
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> -t\n");
			std::cout << "handle modify MODE -t failed => invalid format" << std::endl;
			return ;
		}

		channel->setLockTopic(false);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " -t\n");
	}

	// ADD PASSWORD MODE
	else if (args->at(1) == "+k")
	{
		// USAGE: MODE <channel> +k <new_password>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> +k <new_password>\n");
			std::cout << "handle modify MODE +k failed => invalid format" << std::endl;
			return ;
		}

		// CHECK IF ALL CHARACTERS ARE VALID
		if (this->isValidString(args->at(2), true))
		{
				Server::sendClient(client->getFd(), INV_PASS_FRMT);
				std::cout << "handle modify MODE +k failed => invalid password format" << std::endl;
				return ;
		}

		channel->setPwd(args->at(2));
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " +k\n");
	}
	// SUPP PASSWORD MODE
	else if (args->at(1) == "-k")
	{
		// USAGE: MODE <channel> -k <actual_password>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> -k <actual_password>\n");
			std::cout << "handle modify MODE -k failed => invalid format" << std::endl;
			return ;
		}

		// CHECK IF THE PASSWORD IS GOOD
		if (args->at(2) != channel->getPwd())
		{
			Server::sendClient(client->getFd(), WRNG_PASS);
			std::cout << "handle modify MODE -k failed => wrong password" << std::endl;
			return ;
		}

		channel->setPwd("");
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " -k\n");
	}

	// SET OP TARGET
	else if (args->at(1) == "+o")
	{
		// USAGE: MODE <channel> +o <target_username>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> +o <target_username>\n");
			std::cout << "handle modify MODE +o failed => invalid format" << std::endl;
			return ;
		}

		// TRY TO FIND TARGET IN CHANNEL
		Client	*target = channel->findClientName(args->at(2));
		if (!target)
		{
			Server::sendClient(client->getFd(), TRGT_NOT_FOUND);
			std::cout << "handle modify MODE +o failed => inexistant target" << std::endl;
			return ;
		}

		// CHECK IF TARGET IS OP
		if (channel->isOpName(target->getUser()))
		{
			Server::sendClient(client->getFd(), ALRD_OP);
			std::cout << "handle modify MODE +o failed => target already op" << std::endl;
			return ;
		}

		channel->getOpList().push_back(target);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " +o " + target->getUser() + "\n");
	}
	// UNSET OP TARGET
	else if (args->at(1) == "-o")
	{
		// USAGE: MODE <channel> -o <target_username>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> -o <target_username>\n");
			std::cout << "handle modify MODE -o failed => invalid format" << std::endl;
			return ;
		}

		// TRY TO FIND TARGET IN CHANNEL
		Client	*target = channel->findClientName(args->at(2));
		if (!target)
		{
			Server::sendClient(client->getFd(), TRGT_NOT_FOUND);
			std::cout << "handle modify MODE -o failed => inexistant target" << std::endl;
			return ;
		}

		// CHECK IF TARGET ISNT OP
		if (!channel->isOpName(target->getUser()))
		{
			Server::sendClient(client->getFd(), ISNT_OP);
			std::cout << "handle modify MODE -o failed => target isn't op" << std::endl;
			return ;
		}

		channel->delOpName(target->getUser());
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " -o " + target->getUser() + "\n");
	}

	// ADD USER LIMIT MODE
	else if (args->at(1) == "+l")
	{
		// USAGE: MODE <channel> +l <new_user_limit>
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> +t <new_user_limit>\n");
			std::cout << "handle modify MODE +l failed => invalid format" << std::endl;
			return ;
		}

		// TRY TO SET THE NEW LIMIT
		try
		{
			channel->setMaxUsers(stringToInt(args->at(2)));
			channel->setModeSetTimestamp(time(NULL));
			channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " +l " + args->at(2) + "\n");
		}
		// IF THE INPUT ISNT AN INT
		catch (notIntNumber &e)
		{
			Server::sendClient(client->getFd(), std::string(e.what()) + "\n");
			std::cout << "handle modify MODE +l failed => " << e.what() << std::endl;
			return ;
		}
	}
	// SUPP USER LIMIT MODE
	else if (args->at(1) == "-l")
	{
		// USAGE: MODE <channel> -l
		if (args->size() != 2)
		{
			Server::sendClient(client->getFd(), "Usage: MODE <channel> -l\n");
			std::cout << "handle modify MODE -l failed => invalid format" << std::endl;
			return ;
		}

		channel->setMaxUsers(0);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients("", client->getNick() + ":" + client->getUser() + " MODE " + channel->getName() + " -l\n");
	}

	// OTHERS USAGES
	else
	{
		Server::sendClient(client->getFd(), LST_MODES);
		std::cout << "handle modify MODE failed => not an existant flag" << std::endl;
		return ;
	}

	std::cout << "handle modify MODE " << args->at(1) << " successfuly called\n";
}

// DISPLAY ALL COMMANDS USAGES
void	Command::handleHelp(Client *client, std::vector<std::string> *args)
{
	// CHECK IF CLIENT IS AUTH
	if (!client->getAuth())
	{
		Server::sendClient(client->getFd(), ":localhost 451 * :You have not registered\n");
		std::cout << "handle HELP failed => client isn't auth" << std::endl;
		return ;
	}

	// USAGE: HELP
	if (!args || args->size() >= 2)
	{
		Server::sendClient(client->getFd(), ":localhost 461 " + client->getUser() + " HELP :Not enough parameters");
		std::cout << "handle HELP failed => wrong format" << std::endl;
		return ;
	}

	// SHOW ALL COMMANDS AND DESCRIPTIONS
	if (args->size() == 0)
	{
		Server::sendClient(client->getFd(), "Commands list:\n");
		Server::sendClient(client->getFd(), "PRIVMSG\t\t" PRIVMSG_DESC);
		Server::sendClient(client->getFd(), "JOIN\t\t" JOIN_DESC);
		Server::sendClient(client->getFd(), "PART\t\t" PART_DESC);
		Server::sendClient(client->getFd(), "QUIT\t\t" QUIT_DESC);
		Server::sendClient(client->getFd(), "KICK\t\t" KICK_DESC);
		Server::sendClient(client->getFd(), "INVITE\t\t" INV_DESC);
		Server::sendClient(client->getFd(), "TOPIC\t\t" TPC_DESC);
		Server::sendClient(client->getFd(), "NAMES\t\t" NAMES_DESC);
		Server::sendClient(client->getFd(), "MODE\t\t" MODE_DESC);
	}
	// SHOW USAGE OF THE ASKED COMMAND
	else
	{
		// IF THE CLIENT WANT HELP WITH PRIVMSG
		if (args->at(0) == "PRIVMSG")
		{
			Server::sendClient(client->getFd(), "PRIVMSG:\n");
			Server::sendClient(client->getFd(), PRIVMSG_USG);
			Server::sendClient(client->getFd(), PRIVMSG_DESC);
		}

		// IF THE CLIENT WANT HELP WITH JOIN
		else if (args->at(0) == "JOIN")
		{
			Server::sendClient(client->getFd(), "JOIN:\n");
			Server::sendClient(client->getFd(), JOIN_USG);
			Server::sendClient(client->getFd(), JOIN_DESC);
		}

		// IF THE CLIENT WANT HELP WITH PART
		else if (args->at(0) == "PART")
		{
			Server::sendClient(client->getFd(), "PART:\n");
			Server::sendClient(client->getFd(), PART_USG);
			Server::sendClient(client->getFd(), PART_DESC);
		}

		// IF THE CLIENT WANT HELP WITH QUIT
		else if (args->at(0) == "QUIT")
		{
			Server::sendClient(client->getFd(), "QUIT:\n");
			Server::sendClient(client->getFd(), QUIT_USG);
			Server::sendClient(client->getFd(), QUIT_DESC);
		}

		// IF THE CLIENT WANT HELP WITH KICK
		else if (args->at(0) == "KICK")
		{
			Server::sendClient(client->getFd(), "KICK:\n");
			Server::sendClient(client->getFd(), KICK_USG);
			Server::sendClient(client->getFd(), KICK_DESC);
		}

		// IF THE CLIENT WANT HELP WITH INVITE
		else if (args->at(0) == "INVITE")
		{
			Server::sendClient(client->getFd(), "INVITE:\n");
			Server::sendClient(client->getFd(), INV_USG);
			Server::sendClient(client->getFd(), INV_DESC);
		}

		// IF THE CLIENT WANT HELP WITH TOPIC
		else if (args->at(0) == "TOPIC")
		{
			Server::sendClient(client->getFd(), "TOPIC:\n");
			Server::sendClient(client->getFd(), TPC_USG);
			Server::sendClient(client->getFd(), TPC_DESC);
		}

		// IF THE CLIENT WANT HELP WITH NAMES
		else if (args->at(0) == "NAMES")
		{
			Server::sendClient(client->getFd(), "NAMES:\n");
			Server::sendClient(client->getFd(), NAMES_USG);
			Server::sendClient(client->getFd(), NAMES_DESC);
		}

		// IF THE CLIENT WANT HELP WITH MODE
		else if (args->at(0) == "MODE")
		{
			Server::sendClient(client->getFd(), "MODE:\n");
			Server::sendClient(client->getFd(), MODE_USG);
			Server::sendClient(client->getFd(), MODE_DESC);
		}

		// OTHERS INPUT
		else
		{
			Server::sendClient(client->getFd(), NO_CMD);
			std::cout << "handle HELP failed => command not found" << std::endl;
			return ;
		}
	}

	std::cout << "handle HELP successfuly called" << std::endl;
}

std::string	Command::intToString(int value)
{
	std::stringstream	ss;

	ss << value;
	return ss.str();
}

std::string	Command::timeToString(time_t value)
{
	std::stringstream	ss;

	ss << value;
	return ss.str();
}

int	Command::stringToInt(std::string str)
{
	std::stringstream ss(str);
	long long	num;

	ss >> num;
	if (ss.fail() || !ss.eof() || num < INT_MIN || num > INT_MAX)
		throw notIntNumber();

	return (static_cast<int>(num));
}

void	Command::deleteChannel(Server &serv, Channel *channel) const
{
	if (!channel)
		return ;
	serv.deleteChannel(channel);
	delete channel;
}
