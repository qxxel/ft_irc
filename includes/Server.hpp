/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 15:18:07 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/15 14:09:13 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <sstream>
# include <vector>
# include "Channel.hpp"
# include "Client.hpp"
# include "Request.hpp"

class Channel;
class Client;

class Channel;
class Client;

class Client;
class Channel;

class Server
{
	private:
		static bool				_running;
		int						_port;
		long					_pwd;
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
		void	addChannel(Channel *channel);
		void	deleteClient(int client);
		void	deleteChannel(Channel *channel);
		Client	*findClientFd(int fd);
		Client	*findClientName(std::string name);
		Channel	*searchChannel(std::string name);

		static void			exit(void);
		static bool			isValidChar(char c);
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


		class	ChannelAlreadyExists: public std::exception
		{
			public:
				const char*	what() const throw() { return ("the channel already exists"); };
		};
};

std::ostream &	operator<<(std::ostream &o, Server &serv);

#endif
