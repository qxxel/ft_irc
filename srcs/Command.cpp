/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:30:49 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/05/08 15:12:22 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"
#include "Command.hpp"
#include "Server.hpp"

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
		if (!(*it)->getNick().compare(name))
			return (false);
	}
	return (true);
}

bool	Command::isValidString(std::string str, bool skip)
{
	for (std::string::iterator it = str.begin(); it != str.end(); it++)
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

// Speaks for itself :)
void Command::executeCommand(Server &serv, Client *client, Command *cmd, int epfd)
{
	std::vector<std::string>	args = cmd->getArgs();
	if (!cmd->getName().compare("PASS"))
		cmd->handlePass(serv, client, &args);

	else if (!cmd->getName().compare("NICK"))
		cmd->handleNick(serv, client, &args);

	else if (!cmd->getName().compare("USER"))
		cmd->handleUser(client, &args);

	else if (!cmd->getName().compare("PRIVMSG"))
		cmd->handlePrivMsg(serv, client, &args);

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

	else if (!cmd->getName().compare("NAMES") || !cmd->getName().compare("WHO"))
		cmd->handleNames(serv, client, &args);

	else if (!cmd->getName().compare("MODE"))
		cmd->handleMode(serv, client, &args);

	else if (!cmd->getName().compare("HELP"))
		cmd->handleHelp(client, &args);

	else if (!cmd->getName().compare("QUIT"))
	{
		cmd->handleQuit(serv, client, &args, epfd);
		return ;
	}

	else
	{
		if (client->getNick().empty())
			Server::sendClient(client->getFd(), ":localhost 906 * :" UKWN_CMD + cmd->getName() + "\n");
		else
			Server::sendClient(client->getFd(), ":localhost 906 " + client->getNick() + " :" UKWN_CMD + cmd->getName() + "\n");
		std::cout << "Unknown command" << std::endl;
	}
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
	if (ss.fail() || !ss.eof() || num < 0 || num > INT_MAX)
		throw notUnsignedIntNumber();

	return (static_cast<int>(num));
}

void	Command::deleteChannel(Server &serv, Channel *channel) const
{
	if (!channel)
		return ;
	serv.deleteChannel(channel);
	// /!\ delete channel;
}
