/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:30:13 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/14 13:54:19 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	COMMAND_HPP
#define	COMMAND_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"
#include "Server.hpp"

class Server;
class Client;

class Command
{
private:
	std::string					_raw;
	std::string					_name;
	std::vector<std::string>	_args;
public:
	Command(std::string raw);
	~Command();

	static std::string joinStrings(const std::vector<std::string>& vec);

	void	parse();

	//Setters and getters
	std::string					getName();
	std::vector<std::string>	getArgs();
	void						setName(std::string name);
	void						setArgs(std::vector<std::string> args);

	static void	executeCommand(Server &serv, Client *client, Command *cmd);
	void	handlePass(Server &serv, Client *client, std::vector<std::string> *args);
	void	handleNick(Client *client, std::vector<std::string> *args);
	void	handleUser(Client *client, std::vector<std::string> *args);
	void	handleJoin(Server &serv, Client *client, std::vector<std::string> *args);
	void	handlePart(Server &serv, Client *client, std::vector<std::string> *args);
	void	handleKick(Server &serv, Client *client, std::vector<std::string> *args);
	void	handleInvite(Server &serv, Client *client, std::vector<std::string> *args);
	void	handleTopic(Server &serv, Client *client, std::vector<std::string> *args);
	void	handleMode(Server &serv, Client *client, std::vector<std::string> *args);
	void	deleteChannel(Server &serv, Channel *channel) const;
};

std::ostream &	operator<<(std::ostream &o, Command &cmd);
#endif