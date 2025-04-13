/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:30:49 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/13 20:49:26 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"
#include "defines.hpp"

Command::Command(std::string raw) : _raw(raw), _name("")
{
	parse();
}

Command::~Command()
{
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
			break ;
		j = i;
		while (i < _raw.size() && isascii(_raw[i]) && !isspace(_raw[i]))
			i++;
		str = _raw.substr(j, i - j);
		if (_name.empty())
			_name = str;
		else
			args.push_back(str);
		i++;
	}
	setArgs(args);
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
	return (o);
}

void Command::executeCommand(Server &serv, Client *client, Command *cmd)
{
	std::vector<std::string>	args = cmd->getArgs();
	try
	{
		std::cout << "serv_pwd: " << serv.getPwd() << "    pwd_size: " << serv.getPwd().size() << std::endl;
		std::cout << "cmd_arg : " << cmd->getArgs().at(0) << "      cmd_size: " << cmd->getArgs().at(0).size() << std::endl;
		if (!cmd->getName().compare("PASS"))
			cmd->handlePass(serv, client, &args);
		else if (!cmd->getName().compare("NICK"))
			cmd->handleNick(client, &args);
		else if (!cmd->getName().compare("USER"))
			cmd->handleUser(client, &args);
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
			cmd->handleMode(client, &args);
		else
			Server::sendClient(client->getFd(), "Unknown command: " + cmd->getName() + "\n");
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
			std::cout << "serv_pwd: " << serv.getPwd() << "    pwd_size: " << serv.getPwd().size() << std::endl;
			std::cout << "cmd_arg : " << args->at(0) << "      cmd_size: " << args->at(0).size() << std::endl; 
			if (args->empty() || args->size() != 1)
				return (Server::sendClient(client->getFd(), std::string(INV_FORMAT, ENTER_PWD)));
			else if (!(args->at(0).compare(serv.getPwd())))
			{
				client->setPwd(true);
				if (!client->getAuth() && !client->getNick().empty() && !client->getUser().empty())
				{
					client->setAuth(true);
					return (Server::sendClient(client->getFd(), std::string(PWD_GOOD, AUTHY_GOOD)));
				}
				else
					return (Server::sendClient(client->getFd(), std::string(PWD_GOOD, ENTER_NCK_USR)));
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
	(void)client;
	(void)args;
	std::cout << "handle nick called \n";
}

void	Command::handleUser(Client *client, std::vector<std::string> *args)
{
	(void)client;
	(void)args;
	std::cout << "handle user called \n";
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
		if (!(channel->getClientsList().size() < channel->getMaxUsers()))
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

// SET OR UNSET MODES ON CURRENT CHANNEL
void	Command::handleMode(Client *client, std::vector<std::string> *args)
{
	(void)client;
	(void)args;
	std::cout << "handle Mode called \n";
}

void	Command::deleteChannel(Server &serv, Channel *channel) const
{
	if (!channel)
		return ;
	serv.deleteChannel(channel);
	delete channel;
}
