/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agerbaud <agerbaud@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/26 15:18:46 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/14 14:16:15 by agerbaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "defines.hpp"
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <cerrno>
#include <unistd.h>
#include "Request.hpp"

// ---------------------------------------------SERVER SETUP---------------------------------------------

bool	Server::_running = true;

Server::~Server()
{
	// DELETE CLIENTS
	std::vector<Client*>::iterator	it1;
	for (it1 = _clients.begin(); it1 != _clients.end(); it1++)
		delete *it1;

	// DELETE CHANNELS
	std::vector<Channel*>::iterator	it2;
	for (it2 = _channels.begin(); it2 != _channels.end(); it2++)
		delete *it2;

	// CLOSE PORT
	if (close(_port) == -1)
		throw (std::runtime_error("Error: close failed: " + std::string(strerror(errno))));
}

Server::Server(std::string port, std::string password)
{
	try
	{
		_pwd = password;
		setPort(parsePort(port));
		std::cout << *this;
		start();
	}
	catch(const std::exception& e)
	{
		throw ;
	}
}

int	Server::parsePort(std::string port)
{
	for (size_t i = 0; i < port.size(); i++)
	{
		if (!std::isdigit(port[i]))
			throw (std::runtime_error("Error: invalid port"));
	}
	return (atoi(port.c_str()));
}

void	Server::start(void)
{
	int	fd_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_socket == -1)
		throw (std::runtime_error("Error: socket failed: " + std::string(strerror(errno))));

	int	opt = 1;
	if (setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		throw (std::runtime_error("Error: setsockopt failed: " + std::string(strerror(errno))));
	
	if (fcntl(fd_socket, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error("Error: fcntl failed: " + std::string(strerror(errno))));

	sockaddr_in	network;
	network.sin_family = AF_INET;
	network.sin_port = htons(_port);
	network.sin_addr.s_addr = INADDR_ANY;

	if (bind(fd_socket, (struct sockaddr*)&network, sizeof(network)) == -1)
		throw (std::runtime_error("Error: bind failed: " + std::string(strerror(errno))));

	if (listen(fd_socket, SOMAXCONN) == -1)
		throw (std::runtime_error("Error: listen failed: " + std::string(strerror(errno))));

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
			throw (std::runtime_error("Error: epoll_wait failed: " + std::string(strerror(errno))));
		
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
		catch(const std::exception& e)
		{
			throw ;
		}
	}
}

// ---------------------------------------------SERVER ACTIONS---------------------------------------------

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

void	Server::disconnectClient(int client, int epfd)
{
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, client, NULL) == -1)
		throw (std::runtime_error("Error: deleting client from epoll failed: " + std::string(strerror(errno))));

	if (close(client) == -1)
		throw (std::runtime_error("Error: closing client socket failed: " + std::string(strerror(errno))));

	this->deleteClient(client);
	delete findClientFd(client);

	std::cout << "Client " << client << " disconnected" << std::endl;	
}

void	Server::sendClient(int client, std::string msg)
{
	if (send(client, msg.c_str(), msg.size(), 0) == -1)
		throw (std::runtime_error("Error: sending data to clients failed: " + std::string(strerror(errno))));
}		

bool	Server::isValidChar(char c)
{
	return (!(c == 0 || c == 7 || c == '\r' || c == '\n' || c == ' ' || c == ',' || c == ':'));
}

void	Server::clientRequest(int client, int epfd)
{
	char		buffer[MAX_BODY_SIZE + 2];
	ssize_t		n;
	
	memset(buffer, 0, MAX_BODY_SIZE + 2);
	n = recv(client, &buffer, MAX_BODY_SIZE + 1, 0);
	if (n == -1)
		return (void)(std::cerr << ("Error: recv failed: " + std::string(strerror(errno))));
	
	// std::cout << "\n------------SERVER RECEIVED-------------\n\n" << buffer << std::endl; //debug	
	try
	{
		if (n == 0)
			return (disconnectClient(client, epfd));
		if (strlen(buffer) > MAX_BODY_SIZE)
		{
			sendClient(client, "Error: input too big (max_body_size = 5000)\n");
			return (void)(std::cerr << "Error: client request too big: max " << MAX_BODY_SIZE << " characters" << std::endl);
		}
		Request	req = Request(buffer);
		Client	*tp = findClientFd(client);
		std::vector<Command>::iterator	it;

		for (it = req.getArr().begin(); it != req.getArr().end() ; it++)
			Command::executeCommand(*this, tp, &(*it));
	}			
	catch(const std::exception& e)
	{
		throw ;
	}
}	

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

Client	*Server::findClientName(std::string name)
{
	std::vector<Client*>::iterator	it;

	for (it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if ((*it)->getUser() == name)
			return ((*it));
	}
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
	if (!client)
		return ;

	// CHECK ALL CLIENTS TO ERASE
	for (std::vector<Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if ((*it)->getFd() == client)
		{
			this->_clients.erase(it);
			return ;
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
			this->_channels.erase(it);
			return ;
		}
	}
}

Channel	*Server::searchChannel(std::string name)
{
	for (std::vector<Channel*>::iterator it = this->_channels.begin(); it < this->_channels.end(); it++)
	{
		if ((*it)->getName() == name)
			return (*it);
	}
	return (NULL);
}

// ---------------------------------------------SERVER SETTERS AND GETTERS---------------------------------------------

void	Server::setPwd(std::string pwd)
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

std::string	Server::getPwd(void)
{
	return (_pwd);
}

std::vector<Client*>	Server::getClients() const
{
	return (this->_clients);
}

std::vector<Channel*>	Server::getChannels() const
{
	return (this->_channels);
}

std::ostream &	operator<<(std::ostream &o, Server &serv)
{
	o << "-----------------SERVER-----------------"<< std::endl;
	o << "\tport: " << serv.getPort() << "\n\tpwd: " << serv.getPwd() << std::endl;
	o << "----------------------------------------\n"<< std::endl;
	return (o);
}
