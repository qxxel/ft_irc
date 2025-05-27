/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 18:18:05 by agerbaud          #+#    #+#             */
/*   Updated: 2025/05/27 21:32:28 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

Channel::Channel(std::string name, Client *creator): _oldestClient(NULL), _modeSetTimestamp(time(NULL)), _maxUsers(100), _invOnly(false), _lockTopic(false)
{
	this->_name = name;
	this->_clientsList.push_back(creator);
	this->_opList.push_back(creator);
}

Channel::~Channel() { }


Client	*Channel::findClientName(std::string name)
{
	std::vector<Client*>::iterator	it;

	for (it = this->_clientsList.begin(); it != this->_clientsList.end(); it++)
	{
		if ((*it)->getNick() == name)
			return ((*it));
	}
	return (NULL);
}

void	Channel::delClientName(std::string name)
{
	for (std::vector<Client*>::iterator it = this->_clientsList.begin(); it != this->_clientsList.end(); it++)
	{
		if ((*it)->getNick() == name)
		{
			this->_clientsList.erase(it);
			return ;
		}
	}
}

void	Channel::delOpName(std::string name)
{
	for (std::vector<Client*>::iterator it = this->_opList.begin(); it != this->_opList.end(); it++)
	{
		if ((*it)->getNick() == name)
		{
			this->_opList.erase(it);
			return ;
		}
	}
}

bool	Channel::isOpName(std::string name)
{
	std::vector<Client*>::iterator	it;

	for (it = this->_opList.begin(); it != this->_opList.end(); it++)
	{
		if ((*it)->getNick() == name)
			return (true);
	}
	return (false);
}

void	Channel::sendClients(std::string exceptionName, std::string message)
{
	for (std::vector<Client*>::iterator it = this->_clientsList.begin(); it != this->_clientsList.end(); it++)
	{
		if (exceptionName.compare((*it)->getNick()))
			Server::sendClient((*it)->getFd(), message);
	}
}

std::string	Channel::listClients()
{
	std::string	list;

	for (std::vector<Client*>::iterator it = this->_clientsList.begin(); it != this->_clientsList.end(); it++)
	{
		if (!list.empty())
			list += " ";
		if (this->isOpName((*it)->getNick()))
			list += "@";
		list += (*it)->getNick();
	}

	return (list);
}

// ---------------------------------------------CHANNEL SETTERS AND GETTERS---------------------------------------------

std::vector<Client*>	&Channel::getClientsList()
{
	return (this->_clientsList);
}

std::vector<Client*>	&Channel::getOpList()
{
	return (this->_opList);
}


Client	*Channel::getOldestClient()
{
	std::vector<Client*>::iterator	it;

	for (it = this->_clientsList.begin(); it != this->_clientsList.end(); it++)
	{
		if (!this->isOpName((*it)->getNick()))
			return (*it);
	}

	return (NULL);
}

const std::string	&Channel::getName() const
{
	return (this->_name);
}

const std::string	&Channel::getTopic() const
{
	return (this->_topic);
}

const std::string	&Channel::getPwd() const
{
	return (this->_pwd);
}

const std::string	&Channel::getTopicSetter() const
{
	return (this->_topicSetter);
}

time_t	Channel::getTopicSetTimestamp() const
{
	return (this->_topicSetTimestamp);
}

time_t	Channel::getModeSetTimestamp() const
{
	return (this->_modeSetTimestamp);
}

int	Channel::getMaxUsers() const
{
	return (this->_maxUsers);
}

bool	Channel::getInvOnly() const
{
	return (this->_invOnly);
}

bool	Channel::getLockTopic() const
{
	return (this->_lockTopic);
}

void	Channel::setOldestClient(Client *oldestClient)
{
	this->_oldestClient = oldestClient;
}

void	Channel::setName(const std::string &name)
{
	this->_name = name;
}

void	Channel::setTopic(const std::string &topic)
{
	this->_topic = topic;
}

void	Channel::setPwd(const std::string &pwd)
{
	this->_pwd = pwd;
}

void	Channel::setTopicSetter(const std::string &topicSetter)
{
	this->_topicSetter = topicSetter;
}

void	Channel::setTopicSetTimestamp(time_t topicSetTimestamp)
{
	this->_topicSetTimestamp = topicSetTimestamp;
}

void	Channel::setModeSetTimestamp(time_t modeSetTimestamp)
{
	this->_modeSetTimestamp = modeSetTimestamp;
}

void	Channel::setMaxUsers(int maxUsers)
{
	this->_maxUsers = maxUsers;
}

void	Channel::setInvOnly(bool invOnly)
{
	this->_invOnly = invOnly;
}

void	Channel::setLockTopic(bool lockTopic)
{
	this->_lockTopic = lockTopic;
}
