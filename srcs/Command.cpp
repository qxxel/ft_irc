/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:30:49 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/12 19:24:09 by agerbaud         ###   ########.fr       */
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
		else if (!cmd->getName().compare("KICK"))
			cmd->handleKick(client, &args);
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
void	Command::handleJoin(Server &serv, Client *client, std::vector<std::string> *args) // have to handle on invonly and pwd modes
{
	if (!args || args->size() != 1)
	{
		Server::sendClient(client->getFd(), JOIN_USG);
		std::cout << "handle JOIN failed => join one by one" << std::endl;
		return ;
	}

	if (client->getCurrentChannel() && !(client->getCurrentChannel()->getName().compare(args->at(0))))
	{
		Server::sendClient(client->getFd(), ALRDY_IN_CHNL);
		std::cout << "handle JOIN failed => client already in this channel" << std::endl;
		return ;
	}

	try
	{
		Channel	channel(args->at(0), client);
		serv.addChannel(channel);
		client->setCurrentChannel(&channel);
		Server::sendClient(client->getFd(), CHNL_CREATE);
	}
	catch (Channel::NameIsntValid &e)
	{
		std::cout << "handle JOIN exception: " << e.what() << std::endl;
		Server::sendClient(client->getFd(), INV_CHNL_NAME);
		return ;
	}
	catch (Server::ChannelAlreadyExists &)
	{
		client->setCurrentChannel(serv.searchChannel(args->at(0)));
		Server::sendClient(client->getFd(), std::string(CHNL_JOIN));
	}

	std::cout << "handle JOIN successfully called" << std::endl;
}

void	Command::handleKick(Client *client, std::vector<std::string> *args)
{
	Channel	*channel = client->getCurrentChannel();
	if (!channel)
	{
		Server::sendClient(client->getFd(), NO_CHNL_IN);
		std::cout << "handle KICK failed => client isn't in a channel" << std::endl;
		return ;
	}

	if (!channel->getClientsList().find(client)->second)
	{
		Server::sendClient(client->getFd(), NO_PERM);
		std::cout << "handle KICK failed => client isn't moderator" << std::endl;
		return ;
	}

	if (!args || args->size() != 1)
	{
		Server::sendClient(client->getFd(), KICK_USG);
		std::cout << "handle KICK failed => kick one by one" << std::endl;
		return ;
	}

	Client	*target = channel->findClientName(args->at(0));
	if (target)
	{
		std::map<Client*, bool>::iterator it = channel->getClientsList().find(target);
		if (it != channel->getClientsList().end())
		{
   			channel->getClientsList().erase(it);
			target->setCurrentChannel(NULL);
			Server::sendClient(client->getFd(), TRGT_KICK);
		}
		else
		{
			Server::sendClient(client->getFd(), BAD_TRGT);
			std::cout << "handle KICK failed => target isn't in the channel" << std::endl;
			return ;
		}
	}
	else
	{
		Server::sendClient(client->getFd(), BAD_TRGT);
		std::cout << "handle KICK failed => target isn't in the channel" << std::endl;
		return ;
	}

	std::cout << "handle KICK successfully called\n";
}
void	Command::handleInvite(Server &serv, Client *client, std::vector<std::string> *args)
{
	if (!args || args->empty() || args->size() != 1)
	{
		Server::sendClient(client->getFd(), INV_FORMAT);
		std::cout << "handle INVITE failed => wrong input" << std::endl;
		return ;
	}

	Client	*target = serv.findClientName(args->at(0));
	if (!target)
	{
		Server::sendClient(client->getFd(), BAD_TRGT);
		std::cout << "handle INVITE failed => target don't exist" << std::endl;
		return ;
	}

	Channel	*channel = serv.searchChannel(args->at(1));
	if (!channel)
	{
		Server::sendClient(client->getFd(), NO_CHNL_ASK);
		std::cout << "handle INVITE failed => client isn't in the channel asked" << std::endl;
		return ;
	}

	if (target->getCurrentChannel() == channel)
	{
		Server::sendClient(client->getFd(), ALRDY_IN_CHNL);
		std::cout << "handle INVITE failed => target already in the channel" << std::endl;
		return ;
	}

	if (channel->getInvOnly() && !channel->getClientsList().find(target)->second)
	{
		Server::sendClient(client->getFd(), NO_PERM);
		std::cout << "handle INVITE failed => client isn't moderator" << std::endl;
		return ;
	}

	// ADAPT
	Server::sendClient(client->getFd(), NO_PERM);
	std::cout << "handle INVITE failed => client isn't moderator" << std::endl;
	return ;

	std::cout << "handle INVITE successfuly called \n";
}
void	Command::handleTopic(Server &serv, Client *client, std::vector<std::string> *args)
{
	if (!args || args->empty())
	{
		Server::sendClient(client->getFd(), INV_FORMAT);
		std::cout << "handle TOPIC failed => no args" << std::endl;
		return ;
	}

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
			Server::sendClient(client->getFd(), channel->getName() + ": " + NO_TPC);
		else
			Server::sendClient(client->getFd(), channel->getName() + ": " + channel->getTopic());
	}
	// SET TOPIC
	else
	{
		if (client->getCurrentChannel() != channel)
		{
			Server::sendClient(client->getFd(), NO_CHNL_ASK);
			std::cout << "handle TOPIC failed => client isn't in the channel asked" << std::endl;
			return ;
		}

		if (channel->getLockTopic() && !channel->getClientsList().find(client)->second)
		{
			Server::sendClient(client->getFd(), NO_PERM);
			std::cout << "handle TOPIC failed => client isn't moderator" << std::endl;
			return ;
		}

		channel->setTopic(Command::joinStrings(*args));
		Server::sendClient(client->getFd(), channel->getName() + ": " + channel->getTopic());
	}

	std::cout << "handle TOPIC successfully called\n";
}

void	Command::handleMode(Client *client, std::vector<std::string> *args)
{
	(void)client;
	(void)args;
	std::cout << "handle Mode called \n";
}
