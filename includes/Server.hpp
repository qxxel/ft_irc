/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibjean-b <ibjean-b@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-03-26 15:18:07 by ibjean-b          #+#    #+#             */
/*   Updated: 2025-03-26 15:18:07 by ibjean-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include "Client.hpp"
#include "Request.hpp"

class Server
{
private:
	int						_port;
	bool					_running;
	std::string				_pwd;
	std::vector<Client*>	_clients;
public:
	~Server();
	Server(std::string port, std::string password);


	int		parsePort(std::string port);
	void	start(void);
	void	run(int sock);
	void	sendClient(int client, std::string msg);
	int		acceptClient(int sock, int epfd);
	void	disconnectClient(int client, int epfd);
	void	clientRequest(int client, int epfd);
	Client	*findClient(int fd);
	void	executeCommand(Client *client, Command *cmd);
	void	auth_client(Client *client, Command *cmd);

	void	setPwd(std::string pwd);
	void	setPort(int port);
	void	setRunning(bool running);
	int			getPort(void);
	bool		getRunning(void);
	std::string	getPwd(void);
};

std::ostream &	operator<<(std::ostream &o, Server &serv);

#endif