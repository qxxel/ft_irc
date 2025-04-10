/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 16:30:13 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/10 20:16:43 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	COMMAND_HPP
#define	COMMAND_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"

class Command
{
private:
	std::string					_raw;
	std::string					_name;
	std::vector<std::string>	_args;
public:
	Command(std::string raw);
	~Command();
	
	void	parse();

	//Setters and getters
	std::string					getName();
	std::vector<std::string>	getArgs();
	void						setName(std::string name);
	void						setArgs(std::vector<std::string> args);

	static void	executeCommand(Client *client, Command *cmd);
	void	handlePass(Client *client, std::vector<std::string> *args);
	void	handleNick(Client *client, std::vector<std::string> *args);
	void	handleUser(Client *client, std::vector<std::string> *args);
	void	handleJoin(Server *serv, Client *client, std::vector<std::string> *args);
	void	handleKick(Client *client, std::vector<std::string> *args);
	void	handleInvite(Client *client, std::vector<std::string> *args);
	void	handleTopic(Client *client, std::vector<std::string> *args);
	void	handleMode(Client *client, std::vector<std::string> *args);
};

std::ostream &	operator<<(std::ostream &o, Command &cmd);
#endif