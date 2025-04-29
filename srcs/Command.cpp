/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:30:49 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/29 16:17:54 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "Bot.hpp"

Command::Command(std::string raw) : _raw(raw), _name("")
{
	parse();
}

Command::~Command()
{
}

bool	Command::parse_arg(std::string arg)
{
	for (std::string::iterator it = arg.begin(); it != arg.end(); it++)
	{
		if (Server::isValidChar(*it))
			return (true);
	}
	return (false);
}

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
			i = _raw.size();
		else
		{
			while (i < _raw.size() && isascii(_raw[i]) && !isspace(_raw[i]))
				i++;
		}
		str = _raw.substr(j, i - j);
		if (_name.empty())
			_name = str;
		else
			args.push_back(str);
		i++;
	}
	setArgs(args);
}

bool	Command::is_available(Server &serv, std::string name)
{
	std::vector<Client*>::const_iterator	it;
	for (it = serv.getClients().begin(); it != serv.getClients().end(); it++)
	{
		if (!(*it)->getUser().compare(name))
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

void Command::executeCommand(Server &serv, Client *client, Command *cmd)
{
	std::vector<std::string>	args = cmd->getArgs();
	try
	{
		if (!cmd->getName().compare("!GAME"))
			Bot::handleBot(serv, client, &args);
		else if (!cmd->getName().compare("PASS"))
			cmd->handlePass(serv, client, &args);
		else if (!cmd->getName().compare("NICK"))
			cmd->handleNick(client, &args);
		else if (!cmd->getName().compare("USER"))
			cmd->handleUser(serv, client, &args);
		else if (!cmd->getName().compare("JOIN"))
			cmd->handleJoin(serv, client, &args);
		else if (!cmd->getName().compare("PART"))
			cmd->handlePart(serv, client, &args);
		else if (!cmd->getName().compare("KICK"))
			cmd->handleKick(serv, client, &args);
		else if (!cmd->getName().compare("INVITE"))
			cmd->handleInvite(serv, client, &args);
		else if (!cmd->getName().compare("TOPIC"))
			cmd->handleTopic(serv, client, &args);
		else if (!cmd->getName().compare("MODE"))
			cmd->handleMode(serv, client, &args);
		else
			Server::sendClient(client->getFd(), UKWN_CMD + cmd->getName() + "\n");
	}
	catch(const std::exception& e)
	{
		throw ;
	}
}

void	Command::handlePass(Server &serv, Client *client, std::vector<std::string> *args)
{
	try
	{
		if (!client->getPwd())
		{
			if (args->size() != 1)
			{
				return (Server::sendClient(client->getFd(), INV_FORMAT ENTER_PWD));
			}
			else if (!(args->at(0).compare(serv.getPwd())))
			{
				client->setPwd(true);
				Server::sendClient(client->getFd(), PWD_GOOD);
				if (!client->getAuth() && client->getNick().compare("") && client->getUser().compare(""))
					return (client->setAuth(true));
				else
					return (Server::sendClient(client->getFd(), ENTER_NCK_USR));
			}
			else
				return (Server::sendClient(client->getFd(), INV_PWD));
		}
		else
			return (Server::sendClient(client->getFd(), PWD_SET));
	}
	catch(const std::exception& e)
	{
		throw ;
	}
}

void	Command::handleNick(Client *client, std::vector<std::string> *args)
{
	try
	{
		if (!client->getPwd())
			return (Server::sendClient(client->getFd(), ENTER_PWD));
		else if (client->getNick().compare(""))
		{
			Server::sendClient(client->getFd(), CANT_NICK);
			Server::sendClient(client->getFd(), NICK_NAME + client->getNick() + "\n");
			return ;
		}
		else if (args->size() != 1)
			return (Server::sendClient(client->getFd(), ENTER_NICK));
		else
		{
			if (parse_arg(args->at(0)))
			{
				client->setNick(args->at(0));
				Server::sendClient(client->getFd(), NICK_NAME + client->getNick() + "\n");
			}
			else
				return (Server::sendClient(client->getFd(), HAS_INVALID_CHARS));
		}
		if (!client->getNick().empty() && !client->getUser().empty() && client->getPwd())
			client->setAuth(true);
		else if (!client->getUser().compare(""))
			return (Server::sendClient(client->getFd(), ENTER_USER));
	}
	catch(const std::exception& e)
	{
		throw ;
	}
}
void	Command::handleUser(Server &serv, Client *client, std::vector<std::string> *args)
{
try
	{
		if (!client->getPwd())
			return (Server::sendClient(client->getFd(), ENTER_PWD));
		else if (args->size() != 3 || args->at(1).compare("0") || args->at(2).compare("*"))
			return (Server::sendClient(client->getFd(), ENTER_USER));
		else if (!client->getUser().empty())
		{
			Server::sendClient(client->getFd(), CANT_USER);
			Server::sendClient(client->getFd(), USR_NAME + client->getUser() + "\n");
			return ;
		}
		else
		{
			if (parse_arg(args->at(0)))
			{
				if (is_available(serv, args->at(0)))
				{
					client->setUser(args->at(0));
					Server::sendClient(client->getFd(), USR_NAME + client->getUser() + "\n");
				}
				else
					return (Server::sendClient(client->getFd(), IS_TAKEN ENTER_USER));
			}
			else
				return (Server::sendClient)(client->getFd(), HAS_INVALID_CHARS);
		}
		if (!client->getNick().empty() && !client->getUser().empty() && client->getPwd())
			client->setAuth(true);
		else if (!client->getNick().compare(""))
			return (Server::sendClient(client->getFd(), ENTER_NICK));
	}
	catch(const std::exception& e)
	{
		throw ;
	}
}

// JOIN CHANNEL
void	Command::handleJoin(Server &serv, Client *client, std::vector<std::string> *args) // have to handle on invonly and pwd modes
{
	if (!args || args->size() < 1 || 2 < args->size())
	{
		Server::sendClient(client->getFd(), JOIN_USG);
		std::cout << "handle JOIN failed => join one by one" << std::endl;
		return ;
	}

	// CHECK IF CLIENT ALREADY IN THE CHANNEL
	if (client->getCurrentChannel() && !(client->getCurrentChannel()->getName().compare(args->at(0))))
	{
		Server::sendClient(client->getFd(), ALRDY_IN_CHNL);
		std::cout << "handle JOIN failed => client already in this channel" << std::endl;
		return ;
	}

	try
	{
		Channel *newChannel = new Channel(args->at(0), client);

		// DELETE OLD CHANNEL IF EMPTY
		if (client->getCurrentChannel() && client->getCurrentChannel()->getClientsList().size() == 1)
			this->deleteChannel(serv, client->getCurrentChannel());

		// CREATE CHANNEL
		serv.addChannel(newChannel);
		client->setCurrentChannel(newChannel);
		Server::sendClient(client->getFd(), client->getUser() + " JOIN " + newChannel->getName());
		Server::sendClient(client->getFd(), "MODE " + newChannel->getName() + " +l");
	}
	catch (Channel::NameIsntValid &e)
	{
		std::cout << "handle JOIN exception: " << e.what() << std::endl;
		Server::sendClient(client->getFd(), INV_CHNL_NAME);
		return ;
	}
	catch (Server::ChannelAlreadyExists &)
	{
		Channel *channel = serv.searchChannel(args->at(0));
		if (!channel) // maybe adapt
			return ;

		// CHECK IF CLIENT CAN ACCESS
		if (channel && channel->getInvOnly() && !(client->isJoinableChannel(channel) || channel->isOpName(client->getUser())))
		{
			Server::sendClient(client->getFd(), NOT_ALW);
			std::cout << "handle JOIN failed => client not allowed in this channel" << std::endl;
			return ;
		}

		// CHECK PASSWORD
		if (!channel->getPwd().empty() && !client->isJoinableChannel(channel))
		{
			if (args->size() == 1)
			{
				Server::sendClient(client->getFd(), ND_PASS);
				std::cout << "handle JOIN failed => need password to join" << std::endl;
				return ;
			}
			if (channel->getPwd() != args->at(1))
			{
				Server::sendClient(client->getFd(), WRNG_PASS);
				std::cout << "handle JOIN failed => wrong password" << std::endl;
				return ;
			}
		}

		// CHECK IF CHANNEL IS FULL
		if (!(channel->getClientsList().size() < (size_t)channel->getMaxUsers()))
		{
			Server::sendClient(client->getFd(), CHNL_FULL);
			std::cout << "handle JOIN failed => channel full" << std::endl;
			return ;
		}

		client->setCurrentChannel(channel);
		channel->sendClients(client->getUser() + " JOIN " + channel->getName());
	}

	std::cout << "handle JOIN successfully called" << std::endl;
}

// LEAVE CHANNEL
void	Command::handlePart(Server &serv, Client *client, std::vector<std::string> *args)
{
	if (!args || args->size() < 1)
	{
		Server::sendClient(client->getFd(), INV_FORMAT);
		std::cout << "handle PART failed => wrong format" << std::endl;
		return ;
	}

	// FIND CHANNEL IN SERVER
	Channel	*channel = serv.searchChannel(args->at(0));
	if (!channel)
	{
		Server::sendClient(client->getFd(), NO_CHNL);
		std::cout << "handle PART failed => inexistant channel" << std::endl;
		return ;
	}


	// CHECK IF CLIENT IS IN THIS CHANNEL
	if (channel != client->getCurrentChannel())
	{
		Server::sendClient(client->getFd(), NO_CHNL_ASK);
		std::cout << "handle PART failed => client isn't in the channel asked" << std::endl;
		return ;
	}

	// DELETE OLD CHANNEL IF EMPTY
	if (channel->getClientsList().size() == 1)
		this->deleteChannel(serv, channel);

	if (channel->getOpList().size() == 1 && channel->isOpName(client->getUser()))
		channel->getOpList().push_back(channel->getOldestClient()); // Handle oldest client

	channel->sendClients(client->getUser() + " PART " + Command::joinStrings(*args));
	channel->delClientName(client->getUser());
	client->setCurrentChannel(NULL);
	std::cout << "handle PART successfuly called" << std::endl;
}

// KICK USER ON THE CURRENT CHANNEL
void	Command::handleKick(Server &serv, Client *client, std::vector<std::string> *args)
{
	if (!args || args->size() < 2)
	{
		Server::sendClient(client->getFd(), KICK_USG);
		std::cout << "handle KICK failed => kick one by one" << std::endl;
		return ;
	}

	// CHECK IF CHANNEL EXIST
	Channel	*channel = serv.searchChannel(args->at(0));
	if (!channel)
	{
		Server::sendClient(client->getFd(), INV_CHNL_NAME);
		std::cout << "handle KICK failed => channel don't exist" << std::endl;
		return ;
	}

	// FIND CLIENT IN THE CHANNEL
	if (channel != client->getCurrentChannel())
	{
		Server::sendClient(client->getFd(), NO_CHNL_ASK);
		std::cout << "handle KICK failed => client not in the channel asked" << std::endl;
		return ;
	}

	// CHECK IF CLIENT IS OP
	if (!channel->isOpName(client->getUser()))
	{
		Server::sendClient(client->getFd(), NO_PERM);
		std::cout << "handle KICK failed => client isn't moderator" << std::endl;
		return ;
	}

	// FIND TARGET IN THE CHANNEL
	Client	*target = channel->findClientName(args->at(1));
	if (target)
	{
		channel->delClientName(target->getUser());

		// HANDLE FOR INVITATION ONLY
		if (channel->getInvOnly())
			target->setUnjoinableChannel(channel);

		target->setCurrentChannel(NULL);
		channel->sendClients(client->getUser() + " KICK " + Command::joinStrings(*args));
		Server::sendClient(target->getFd(), client->getUser() + " KICK " + Command::joinStrings(*args));
	}
	else
	{
		Server::sendClient(client->getFd(), BAD_TRGT);
		std::cout << "handle KICK failed => target isn't in the channel" << std::endl;
		return ;
	}

	std::cout << "handle KICK successfully called\n";
}

// INVITE USER IN CURRENT CHANNEL
void	Command::handleInvite(Server &serv, Client *client, std::vector<std::string> *args)
{
	if (!args || args->empty() || args->size() != 1)
	{
		Server::sendClient(client->getFd(), INV_FORMAT);
		std::cout << "handle INVITE failed => wrong input" << std::endl;
		return ;
	}

	// FIND TARGET IN SERVER
	Client	*target = serv.findClientName(args->at(0));
	if (!target)
	{
		Server::sendClient(client->getFd(), BAD_TRGT);
		std::cout << "handle INVITE failed => target don't exist" << std::endl;
		return ;
	}

	// FIND CHANNEL IN SERVER
	Channel	*channel = serv.searchChannel(args->at(1));
	if (!channel)
	{
		Server::sendClient(client->getFd(), NO_CHNL);
		std::cout << "handle INVITE failed => inexistant channel" << std::endl;
		return ;
	}

	// FIND CLIENT IN THE CHANNEL
	if (client->getCurrentChannel() != channel)
	{
		Server::sendClient(client->getFd(), NO_CHNL_ASK);
		std::cout << "handle INVITE failed => client isn't in the channel" << std::endl;
		return ;
	}

	// FIND TARGET IN THE CHANNEL
	if (target->getCurrentChannel() == channel)
	{
		Server::sendClient(client->getFd(), ALRDY_IN_CHNL);
		std::cout << "handle INVITE failed => target already in the channel" << std::endl;
		return ;
	}

	// CHECK INVITE ONLY CHANNEL
	if (channel->getInvOnly())
	{
		// CHECK IF CLIENT IS OP
		if (!channel->isOpName(target->getUser()))
		{
			Server::sendClient(client->getFd(), NO_PERM);
			std::cout << "handle INVITE failed => client isn't moderator" << std::endl;
			return ;
		}

		// ADD TARGET TO ACCESS LIST
		target->getJoinableChannels().push_back(channel);
	}

	Server::sendClient(target->getFd(), client->getUser() + " INVITE " + target->getUser() + " " + channel->getName());
	Server::sendClient(client->getFd(), "INVITE " + client->getUser() + " " + target->getUser() + " " + channel->getName());
	std::cout << "handle INVITE successfuly called \n";
}

// VIEW OR SET TOPIC OF THE CURRENT CHANNEL
void	Command::handleTopic(Server &serv, Client *client, std::vector<std::string> *args)
{
	if (!args || args->empty())
	{
		Server::sendClient(client->getFd(), INV_FORMAT);
		std::cout << "handle TOPIC failed => no args" << std::endl;
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

	// VIEW TOPIC
	if (args->size() == 1)
	{
		if (channel->getTopic().empty())
			Server::sendClient(client->getFd(), client->getUser() + " " + channel->getName() + " :" + NO_TPC);
		else
			Server::sendClient(client->getFd(), client->getUser() + " " + channel->getName() + " :" + channel->getTopic());
	}
	// SET TOPIC
	else
	{
		// FIND CLIENT IN THE CHANNEL
		if (client->getCurrentChannel() != channel)
		{
			Server::sendClient(client->getFd(), NO_CHNL_ASK);
			std::cout << "handle TOPIC failed => client isn't in the channel asked" << std::endl;
			return ;
		}

		// CHECK IF CLIENT IS OP
		if (channel->getLockTopic() && !channel->isOpName(client->getUser()))
		{
			Server::sendClient(client->getFd(), NO_PERM);
			std::cout << "handle TOPIC failed => client isn't moderator" << std::endl;
			return ;
		}

		// CHECK IF THE TOPIC IS ALREADY SET
		if (channel->getTopic() == args->at(1)) // fix parsing for args->at(1) == new topic
		{
			Server::sendClient(client->getFd(), ACTL_TPC);
			std::cout << "handle TOPIC failed => client isn't moderator" << std::endl;
			return ;
		}

		channel->setTopic(args->at(1)); // fix parsing for args->at(1) == new topic
		channel->sendClients(client->getUser() + " TOPIC " + channel->getName() + " :" + channel->getTopic());
	}

	std::cout << "handle TOPIC successfully called\n";
}

template <typename T>
std::string	toString(T value)
{
	std::stringstream	ss;

	ss << value;
	return ss.str();
}

int	stringToInt(std::string str)
{
    std::stringstream ss(str);
    int num;

    ss >> num;
    if (ss.fail())
		throw std::exception();

    return (num);
}

// SET OR UNSET MODES ON CURRENT CHANNEL
void	Command::handleMode(Server &serv, Client *client, std::vector<std::string> *args)
{
	if (!args || args->size() < 1)
	{
		Server::sendClient(client->getFd(), INV_FORMAT);
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
	if (client->getCurrentChannel() != channel)
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
			modesArgs += " " + toString(channel->getMaxUsers());
		}
		else
			modes = "";

		Server::sendClient(client->getFd(), client->getNick() + " " + channel->getName() + modes + modesArgs);
		Server::sendClient(client->getFd(), client->getNick() + " " + channel->getName() + " " + toString(channel->getModeSetTimestamp()));
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

		// ADD CHECK SIZE ARGS
	// ADD INVITE ONLY MODE
	if (args->at(1) == "+i")
	{
		channel->setInvOnly(true);
		channel->setModeSetTimestamp(time(NULL));
		// ADD CHANNEL IN CURRENT CLIENTS' JOINABLE CHANNELS
		for (std::vector<Client*>::iterator it = channel->getClientsList().begin(); it != channel->getClientsList().end(); it++)
			(*it)->getJoinableChannels().push_back(channel);
	}
	// SUPP INVITE ONLY MODE
	if (args->at(1) == "-i")
	{
		channel->setInvOnly(false);
		channel->setModeSetTimestamp(time(NULL));
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

		// ADD CHECK SIZE ARGS
	// ADD LOCKED TOPIC MODE
	if (args->at(1) == "+t")
	{
		channel->setLockTopic(true);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients(client->getUser() + " MODE " + channel->getName() + " +t");
	}
	// SUPP LOCKED TOPIC MODE
	else if (args->at(1) == "-t")
	{
		channel->setLockTopic(false);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients(client->getUser() + " MODE " + channel->getName() + " -t");
	}

		// ADD CHECK SIZE ARGS
	// ADD PASSWORD MODE
	else if (args->at(1) == "+k")
	{
		channel->setPwd(args->at(2));
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients(client->getUser() + " MODE " + channel->getName() + " +k");
	}
	// SUPP PASSWORD MODE
	else if (args->at(1) == "-k")
	{
		if (args->at(2) != channel->getPwd())
		{
			Server::sendClient(client->getFd(), WRNG_PASS);
			std::cout << "handle modify MODE failed => wrong password" << std::endl;
			return ;
		}

		channel->setPwd("");
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients(client->getUser() + " MODE " + channel->getName() + " -k");
	}

		// ADD CHECK SIZE ARGS
	// SET OP TARGET
	else if (args->at(1) == "+o")
	{
		Client	*target = channel->findClientName(args->at(2));

		if (channel->isOpName(target->getUser()))
		{
			Server::sendClient(client->getFd(), ALRD_OP);
			std::cout << "handle modify MODE failed => target already op" << std::endl;
			return ;
		}

		channel->getOpList().push_back(target);
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients(client->getUser() + " MODE " + channel->getName() + " +o " + target->getUser());
	}
	// UNSET OP TARGET
	else if (args->at(1) == "-o")
	{
		Client	*target = channel->findClientName(args->at(2));

		if (!channel->isOpName(target->getUser()))
		{
			Server::sendClient(client->getFd(), ISNT_OP);
			std::cout << "handle modify MODE failed => target isn't op" << std::endl;
			return ;
		}

		channel->delOpName(target->getUser());
		channel->setModeSetTimestamp(time(NULL));
		channel->sendClients(client->getUser() + " MODE " + channel->getName() + " -o " + target->getUser());
	}

		// ADD CHECK SIZE ARGS
	// ADD USER LIMIT MODE
	else if (args->at(1) == "+l")
	{
		if (args->size() != 3)
		{
			Server::sendClient(client->getFd(), INV_FORMAT);
			std::cout << "handle modify MODE failed => invalid format" << std::endl;
			return ;
		}

		channel->setMaxUsers(stringToInt(args->at(2)));
	}
	// SUPP USER LIMIT MODE
	else if (args->at(1) == "-l"){
		// a finir
		std::cerr << "SUPP USER LIMIT MODE\n";
	}
	std::cout << "handle modify MODE successfuly called\n";
}

void	Command::deleteChannel(Server &serv, Channel *channel) const
{
	if (!channel)
		return ;
	serv.deleteChannel(channel);
	delete channel;
}
