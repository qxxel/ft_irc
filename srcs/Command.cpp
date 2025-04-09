/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibjean-b <ibjean-b@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-04-02 16:30:49 by ibjean-b          #+#    #+#             */
/*   Updated: 2025-04-02 16:30:49 by ibjean-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Command.hpp"
#include "Server.hpp"

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
	o << "args: ";
	std::vector<std::string>	vec = cmd.getArgs();
	for (size_t i = 0; i < vec.size() - 1; i++)
	{
		if (i)
			o << "\t";
		o << i << ": " << vec[i] << std::endl;
	}
	return (o);
}


void Command::executeCommand(Client *client, Command *cmd)
{
	std::vector<std::string>	args = cmd->getArgs();
	try
	{
		if (cmd->getName().compare("PASS"))
			handlePass(client, &args);
		else
			Server::sendClient(client->getFd(), "unknown command: " + cmd->getName());
	}
	catch(const std::exception& e)
	{
		throw ;
	}
}

void	Command::handlePass(Client *client, std::vector<std::string> *args)
{
	(void)client;
	(void)args;
	std::cout << "handlePASS called \n";
}