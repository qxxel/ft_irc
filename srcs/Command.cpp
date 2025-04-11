/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:30:49 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/11 14:52:05 by mreynaud         ###   ########.fr       */
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
		if (i >= _raw.size())
			break;
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
	if (0 == vec.size())
		o << std::endl;
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
			cmd->handleInvite(client, &args);
		else if (!cmd->getName().compare("TOPIC"))
			cmd->handleTopic(client, &args);
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
void	Command::handleJoin(Server &serv, Client *client, std::vector<std::string> *args)
{
	if (args->size() != 1)
	{
		std::cout << "handle join failed => join one by one" << std::endl;
		return ;
	}

	if (client->getCurrentChannel() && !(client->getCurrentChannel()->getName().compare(args->at(0))))
	{
		std::cerr << "debug3" << std::endl;
		Server::sendClient(client->getFd(), ALRDY_IN_CHNL);
		std::cout << "handle join failed => client already in this channel" << std::endl;
		return ;
	}

	try
	{
		Channel	channel(args->at(0), client);
		serv.addChannel(channel);
	}
	catch (Channel::NameIsntValid &e)
	{
		std::cout << "handle join exception: " << e.what() << std::endl;
		return ;
	}
	catch (Server::ChannelAlreadyExists &)
	{
		client->setCurrentChannel(serv.searchChannel(args->at(0)));
	}

	std::cout << "handle join successfully called" << std::endl;
}

void	Command::handleKick(Client *client, std::vector<std::string> *args)
{
	(void)client;
	(void)args;
	std::cout << "handle Kick called \n";
}
void	Command::handleInvite(Client *client, std::vector<std::string> *args)
{
	(void)client;
	(void)args;
	std::cout << "handle Invite called \n";
}
void	Command::handleTopic(Client *client, std::vector<std::string> *args)
{
	(void)client;
	(void)args;
	std::cout << "handle Topic called \n";
}

void	Command::handleMode(Client *client, std::vector<std::string> *args)
{
	(void)client;
	(void)args;
	std::cout << "handle Mode called \n";
}
