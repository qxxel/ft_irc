/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 15:18:46 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/05/07 19:34:47 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <cerrno>
#include <unistd.h>
#include "Bot.hpp"

// ---------------------------------------------SERVER SETUP---------------------------------------------

bool	Server::_running = true;

Server::~Server()
{
	// DELETE BOT
	if (this->_bot)
		delete this->_bot;

	// DELETE CLIENTS
	std::vector<Client*>::iterator	it1;
	for (it1 = this->_clients.begin(); it1 != this->_clients.end(); ++it1)
		delete *it1;

	// DELETE CHANNELS
	std::vector<Channel*>::iterator	it2;
	for (it2 = this->_channels.begin(); it2 != this->_channels.end(); ++it2){ std::cerr << "aa" << std::endl;
		delete *it2;}
}

//Simple creation of a server object.
Server::Server(std::string port, std::string password)
{
	try
	{
		this->_pwd = simpleHash(password) * 2 % 333 / 4 + 5 * 6;
		setPort(parsePort(port));
		this->_bot = new Bot();
		std::cout << *this;
	}
	catch(const std::exception& e)
	{
		throw ;
	}
}

//Makes sure the server's port is valid
int	Server::parsePort(std::string port)
{
	for (size_t i = 0; i < port.size(); i++)
	{
		if (!std::isdigit(port[i]))
			throw (std::runtime_error("Error: invalid port"));
	}
	return (atoi(port.c_str()));
}

//Hashes the password so it's only used once as the raw format (for security reasons)
long	Server::simpleHash(std::string const &clear_text)
{
	long	hash = 0;
	for (size_t i = 0; i < 	clear_text.length(); i++)
		hash = 818 * hash + clear_text[i];
	return (hash);
}

// Creates a socket, setup the server communications, binds and listens on the socket on non-blockable mode
void	Server::start(void)
{
	int	fd_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_socket == -1)
		throw (std::runtime_error("Error: socket failed: " + std::string(strerror(errno))));

	int	opt = 1;
	if (setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
	{
		close(fd_socket);
		throw (std::runtime_error("Error: setsockopt failed: " + std::string(strerror(errno))));
	}

	if (fcntl(fd_socket, F_SETFL, O_NONBLOCK) == -1)
	{
		close(fd_socket);
		throw (std::runtime_error("Error: fcntl failed: " + std::string(strerror(errno))));
	}

	sockaddr_in	network;
	network.sin_family = AF_INET;
	network.sin_port = htons(_port);
	network.sin_addr.s_addr = INADDR_ANY;

	if (bind(fd_socket, (struct sockaddr*)&network, sizeof(network)) == -1)
	{
		close(fd_socket);
		throw (std::runtime_error("Error: bind failed: " + std::string(strerror(errno))));
	}

	if (listen(fd_socket, SOMAXCONN) == -1)
	{
		close(fd_socket);
		throw (std::runtime_error("Error: listen failed: " + std::string(strerror(errno))));
	}

	try
	{
		run(fd_socket);
	}
	catch(const std::exception& e)
	{
		throw ;
	}
}

// ---------------------------------------------SERVER RUNNING---------------------------------------------

//Runs the server until it dies (_running should never be false) and checks for events triggered by the clients when connecting/ disconnecting/ requesting
void	Server::run(int sock)
{
	int				epfd = epoll_create1(0);
	epoll_event		ev, events[MAX_EVENTS];

	if (epfd == -1)
		throw (std::runtime_error("Error: epoll_create failed: " + std::string(strerror(errno))));

	ev.data.fd = sock;
	ev.events = EPOLLIN | EPOLLRDHUP | EPOLLOUT;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev) == -1)
		throw (std::runtime_error("Error: epoll_ctl failed: " + std::string(strerror(errno))));

	int nfds;
	while (_running)
	{
		nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
		if (nfds == -1)
		{
			deleteServer(sock, epfd);
			throw (std::runtime_error("Error: epoll_wait failed: " + std::string(strerror(errno))));
		}

		try
		{
			for (int i = 0; i < nfds; i++)
			{
				if (events[i].data.fd == sock)
					acceptClient(sock, epfd);
				else if (events[i].events & EPOLLIN)
					clientRequest(events[i].data.fd, epfd);
				else if (events[i].events & EPOLLRDHUP)
					disconnectClient(events[i].data.fd, epfd);
			}
		}
		catch(const std::exception&)
		{
			deleteServer(sock, epfd);
			throw ;
		}
	}
	deleteServer(sock, epfd);
}

// ---------------------------------------------SERVER ACTIONS---------------------------------------------

//Adds a client to the server and defines the events that will be monitored by the server
int	Server::acceptClient(int sock, int epfd)
{
	epoll_event	ev;
	sockaddr_in	client;
	socklen_t	client_size = sizeof(client);
	int			fd = accept(sock, (struct sockaddr*)&client, &client_size);

	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLRDHUP | EPOLLOUT;
	if (fd == -1)
		throw (std::runtime_error("Error: accept failed: " + std::string(strerror(errno))));

	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error("Error: fcntl failed: " + std::string(strerror(errno))));

	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
		throw (std::runtime_error("Error: epoll_ctl failed: " + std::string(strerror(errno))));

	try
	{
		Client	*tp = new Client(fd);
		_clients.push_back(tp);
		sendClient(tp->getFd(), SERVER_WELCOME);
		sendClient(tp->getFd(), ENTER_PWD);
		std::cout << "\nNew client connected !\n";
	}
	catch(const std::exception& e)
	{
		throw ;
	}

	return (fd);
}

//Disconnects a client from the server and removes it's data
void	Server::disconnectClient(int fd, int epfd)
{
	Client	*client = findClientFd(fd);

	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
		throw (std::runtime_error("Error: deleting client from epoll failed: " + std::string(strerror(errno))));

	if (close(fd) == -1)
		throw (std::runtime_error("Error: closing client socket failed: " + std::string(strerror(errno))));

	this->deleteClient(fd);
	delete client;

	std::cout << "Client " << fd << " disconnected" << std::endl;
}

//Answers the client request by sending it a message proprely.
void	Server::sendClient(int client, std::string msg)
{
	ssize_t		n;
	ssize_t		total_sent = 0;
	ssize_t		size = msg.size();
	const char	*ptr = msg.data();

	if (client == -2 || size == 0)
		return ;
	while (total_sent < size)
	{
		n = send(client, ptr + total_sent, size - total_sent, 0);
		if (n == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return ;
			throw (std::runtime_error("Error: sending data to clients failed: " + std::string(strerror(errno))));
		}
		else
			total_sent += n;
	}
}

//Valid characters in all the <names> used (channels or user/nick names).
bool	Server::isValidChar(char c)
{
	return (!(!isprint(c) || c == ' ' || c == ',' || c == ':' || c == '#'));
}

//Speaks for itself
std::string	Server::str_toupper(std::string str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); it++)
		*it = toupper(*it);
	return str;
}

//Reads data from a triggered file descriptor of a client that has been triggered by a EPOLLIN event and creates a request that is usable by the server
void	Server::clientRequest(int client, int epfd)
{
	ssize_t		n;
	ssize_t		total_read = 0;
	char		buffer[MAX_BODY_SIZE + 2];

	memset(buffer, 0, sizeof(buffer));
	while (true)
	{
		n = recv(client, buffer + total_read, MAX_BODY_SIZE + 1, 0);
		if (n == 0)
			return (disconnectClient(client, epfd));
		else if (n == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				break ;
			else
				return (void)(std::cerr << ("Error: recv failed: " + std::string(strerror(errno))));
		}
		else
		{
			if (total_read + n > MAX_BODY_SIZE)
			{
				while (n > 0)
					n = recv(client, buffer + total_read, MAX_BODY_SIZE, 0);
				return (void)sendClient(client, std::string("Error: message size too big: max 5000 characters\n"));
			}
			else
				total_read += n;
		}
	}
	// std::cout << "\n------------SERVER RECEIVED-------------\n\n" << buffer << std::endl; //debug
	Client	*tp = findClientFd(client);
	tp->getRequest()->append(buffer, total_read);
	if (total_read > 0 && buffer[total_read - 1] == '\n')
	{
		tp->getRequest()->split_Request();
		for (std::vector<Command>::iterator	it = tp->getRequest()->getArr().begin(); it != tp->getRequest()->getArr().end() ; it++)
		{
			Command::executeCommand(*this, tp, &(*it), epfd);
			if (!this->findClientFd(client))
				return ;
		}
		tp->getRequest()->clear();
	}
}

//Finds a client form its file descriptor in the server's vector of clients and returns it, returns NULL otherwise
Client	*Server::findClientFd(int fd)
{
	std::vector<Client*>::iterator	it;

	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if ((*it)->getFd() == fd)
			return (*it);
	}
	return (NULL);
}

//Finds a client form its name in the server's vector of clients and returns it, returns NULL otherwise
Client	*Server::findClientName(std::string name)
{
	std::vector<Client*>::iterator	it;

	for (it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if ((*it)->getUser() == name)
			return ((*it));
	}
	if (name == _bot->getUser())
		return (_bot);
	return (NULL);
}

void	Server::exit(void)
{
	Server::_running = false;
}

void	Server::addChannel(Channel *channel)
{
	if (!channel)
		return ;
	if (this->searchChannel(channel->getName()))
		throw ChannelAlreadyExists();
	this->_channels.push_back(channel);
}

void	Server::deleteClient(int client)
{
	if (client < 3)
		return ;

	// DELETE CLIENT IN THE SERVER
	for (std::vector<Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if ((*it)->getFd() == client)
		{
			this->_clients.erase(it);
			break ;
		}
	}

	// DELETE CLIENT IN THE CHANNEL
	for (std::vector<Channel*>::iterator it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		// DELETE THE CLIENT IN THE CLIENT LIST
		for (std::vector<Client*>::iterator it_clients = (*it)->getClientsList().begin(); it_clients != (*it)->getClientsList().end(); it_clients++)
		{
			if ((*it_clients)->getFd() == client)
			{
				(*it)->getClientsList().erase(it_clients);
				(*it)->sendClients("", (*it_clients)->getNick() + ":" + (*it_clients)->getUser() + " PART " + (*it)->getName() + "\n");
				break ;
			}
		}
		// DELETE THE CLIENT IN THE OP LIST
		for (std::vector<Client*>::iterator it_ops = (*it)->getOpList().begin(); it_ops != (*it)->getOpList().end(); it_ops++) // PROBLEM
		{
			if ((*it_ops)->getFd() == client)
			{
				(*it)->getOpList().erase(it_ops);

				if ((*it)->getOpList().empty() && !(*it)->getClientsList().empty())
				{

					Client	*oldestClient = (*it)->getOldestClient();
					if (oldestClient)
						(*it)->getOpList().push_back(oldestClient);
				}

				break ;
			}
		}
		// DELETE CHANNEL IF EMPTY
		if ((*it)->getClientsList().empty())
		{
			this->deleteChannel((*it));
			it--;
		}
	}
}

void	Server::deleteChannel(Channel *channel)
{
	if (!channel)
		return ;

	// CHECK ALL CLIENTS JOINABLE CHANNELS TO ERASE
	for (std::vector<Client*>::iterator it1 = this->_clients.begin(); it1 != this->_clients.end(); it1++)
	{
		for (std::vector<Channel*>::iterator it2 = (*it1)->getJoinableChannels().begin(); it2 != (*it1)->getJoinableChannels().end(); it2++)
		{
			if ((*it2) == channel)
			{
				(*it1)->getJoinableChannels().erase(it2);
				break ;
			}
		}
	}

	// CHECK ALL CHANNELS TO ERASE
	for (std::vector<Channel*>::iterator it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		if ((*it) == channel)
		{
			delete (*it);
			this->_channels.erase(it);
			return ;
		}
	}
}

void	Server::deleteServer(int sock, int epfd)
{
	// DELETE ALL CLIENTS IN EPOLL AND CLOSE THEM
	std::vector<Client*>::iterator	it_clnt;
	for (it_clnt = this->_clients.begin(); it_clnt != this->_clients.end(); it_clnt++)
	{
		epoll_ctl(epfd, EPOLL_CTL_DEL, (*it_clnt)->getFd(), NULL);
		close((*it_clnt)->getFd());
	}

	// CLOSE SOCKET
	epoll_ctl(epfd, EPOLL_CTL_DEL, sock, NULL);
	close(sock);
	// (void)sock;

	// CLOSE EPOLL
	close(epfd);
}

Channel	*Server::searchChannel(std::string name)
{
	for (std::vector<Channel*>::iterator it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		if ((*it)->getName() == name)
			return (*it);
	}
	return (NULL);
}

// ---------------------------------------------SERVER SETTERS AND GETTERS---------------------------------------------

void	Server::setPwd(long pwd)
{
	_pwd = pwd;
}

void	Server::setRunning(bool running)
{
	_running = running;
}

void	Server::setPort(int port)
{
	_port = port;
}

int	Server::getPort(void)
{
	return (_port);
}

bool	Server::getRunning(void)
{
	return (_running);
}

long	Server::getPwd(void)
{
	return (_pwd);
}

std::vector<Client*>	&Server::getClients()
{
	return (this->_clients);
}

std::vector<Channel*>	&Server::getChannels()
{
	return (this->_channels);
}

Bot	*Server::getBot()
{
	return (this->_bot);
}

std::ostream &	operator<<(std::ostream &o, Server &serv)
{
	o << "-----------------SERVER-----------------"<< std::endl;
	o << "\tport: " << serv.getPort() << "\n\tpwd: " << serv.getPwd() << std::endl;
	o << "----------------------------------------\n"<< std::endl;
	return (o);
}
