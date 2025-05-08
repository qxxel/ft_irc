/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 15:18:07 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/05/08 18:27:32 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <cstdlib>
# include <csignal>
# include <cstring>
# include <sys/socket.h>
# include <fcntl.h>
# include <netinet/in.h>
# include <sys/epoll.h>
# include <cerrno>
# include <unistd.h>
# include <exception>
# include <iostream>
# include <limits.h>
# include <map>
# include <sstream>
# include <vector>
# include <sys/wait.h>
# include "defines.hpp"

class Bot;
class Channel;
class Client;
class Command;
class Request;

class Server
{
	private:
		static bool				_running;
		int						_port;
		long					_pwd;
		std::vector<Client*>	_clients;
		std::vector<Channel*>	_channels;
		Bot						*_bot;

	public:
		Server(std::string port, std::string password);
		~Server();


		int		parsePort(std::string port);
		void	start(void);
		void	run(int sock);
		int		acceptClient(int sock, int epfd);
		void	disconnectClient(int client, int epfd);
		void	clientRequest(int client, int epfd);
		void	addChannel(Channel *channel);
		void	deleteChannel(Channel *channel);
		void	deleteClient(int client);
		void	deleteServer(int sock, int epfd);
		Client	*findClientFd(int fd);
		Client	*findClientName(std::string name);
		Channel	*searchChannel(std::string name);

		static void			exit(void);
		static bool			isValidChar(char c);
		static std::string	str_toupper(std::string str);
		static void			sendClient(int client, std::string msg);
		static long			simpleHash(std::string const &clear_text);

		void					setRunning(bool running);
		void					setPwd(long pwd);
		void					setPort(int port);
		int						getPort(void);
		bool					getRunning(void);
		long					getPwd(void);
		std::vector<Client*>	&getClients();
		std::vector<Channel*>	&getChannels();
		Bot						*getBot();


		class	ChannelAlreadyExists: public std::exception
		{
			public:
				const char*	what() const throw() { return ("the channel already exists"); };
		};
};

std::ostream &	operator<<(std::ostream &o, Server &serv);
