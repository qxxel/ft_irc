/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 16:04:15 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/15 14:10:42 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"

Client::~Client()
{
	delete _req;
}

Client::Client(int fd) : _fd(fd), _auth(false),  _pwd(false), _user(""), _nick(""), _currentChannel(NULL) , _req(new Request())
{
	if (this->_currentChannel)
	{
		this->_currentChannel->delClientName(this->_user);
		if (this->_currentChannel->isOpName(this->_user))
			this->_currentChannel->delOpName(this->_user);
	}
}

void	Client::setUnjoinableChannel(Channel *channel)
{
	if (!channel)
		return ;

	for (std::vector<Channel*>::iterator	it = this->_joinableChannels.begin(); it != this->_joinableChannels.end(); it++)
	{
		if ((*it) == channel)
		{
			this->_joinableChannels.erase(it);
			return ;
		}
	}
}

bool	Client::isJoinableChannel(Channel *channel)
{
	if (!channel)
		return (false);

	for (std::vector<Channel*>::iterator	it = this->_joinableChannels.begin(); it != this->_joinableChannels.end(); it++)
	{
		if ((*it) == channel)
			return (true);
	}
	return (false);
}


// ---------------------------------------------CLIENT SETTERS AND GETTERS---------------------------------------------

void	Client::setFd(int fd)
{
	_fd = fd;
}

void	Client::setNick(std::string nick)
{
	_nick = nick;
}

void	Client::setUser(std::string user)
{
	_user = user;
}

void	Client::setAuth(bool auth)
{
	try
	{
		if (auth)
		{
			Server::sendClient(_fd, AUTHY_GOOD);
			Server::sendClient(_fd, USR_NAME + _user + "\n" + NICK_NAME + _nick + "\n");
		}
		_auth = auth;
	}
	catch(const std::exception& e)
	{
		throw ;
	}
}

void	Client::setPwd(bool pwd)
{
	_pwd = pwd;
}

void	Client::setCurrentChannel(Channel *channel)
{
	if (!channel)
		return ;
	this->_currentChannel = channel;
}

int		Client::getFd()
{
	return (_fd);
}

bool	Client::getAuth()
{
	return (_auth);
}

bool	Client::getPwd()
{
	return (_pwd);
}

std::string	Client::getNick()
{
	return (_nick);
}

std::string	Client::getUser()
{
	return (_user);
}

Channel		*Client::getCurrentChannel()
{
	return (this->_currentChannel);
}

Request		*Client::getRequest()
{
	return _req;
}

std::vector<Channel*>	&Client::getJoinableChannels()
{
	return (this->_joinableChannels);
}

std::ostream &	operator<<(std::ostream &o, Client &client)
{
	o << "-----------Client-----------\n";
	o << "user: " << client.getUser() << std::endl;
	o << "nick: " << client.getNick() << std::endl;
	o << "clfd: " << client.getFd() << std::endl;
	o << "auth: " << client.getAuth() << std::endl;
	return (o);
}
