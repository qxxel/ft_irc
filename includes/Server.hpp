/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 15:18:07 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/10 20:04:58 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include "Channel.hpp"
#include "Client.hpp"
#include "Request.hpp"

class Server
{
private:
	static bool				_running;
	int						_port;
	std::string				_pwd;
	std::vector<Client*>	_clients;
	std::vector<Channel*>	_channels;
public:
	~Server();
	Server(std::string port, std::string password);


	int		parsePort(std::string port);
	void	start(void);
	void	run(int sock);
	int		acceptClient(int sock, int epfd);
	void	disconnectClient(int client, int epfd);
	void	clientRequest(int client, int epfd);
	Client	*findClient(int fd);

	static void	exit(void);
	static void	sendClient(int client, std::string msg);

	void		setRunning(bool running);
	void		setPwd(std::string pwd);
	void		setPort(int port);
	int			getPort(void);
	bool		getRunning(void);
	std::string	getPwd(void);
};

std::ostream &	operator<<(std::ostream &o, Server &serv);

#endif