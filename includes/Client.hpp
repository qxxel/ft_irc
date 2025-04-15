/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 16:03:22 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/15 14:03:39 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <map>
# include <vector>
# include "Channel.hpp"
#include "Request.hpp"

class Channel;
class Request;

class Client
{
private:
	int						_fd;
	bool					_auth;
	bool					_pwd;
	std::string				_user;
	std::string				_nick;
	Channel					*_currentChannel;
	Request					*_req;
	std::vector<Channel*>	_joinableChannels;

public:
	Client(int fd);
	~Client();

	void	setUnjoinableChannel(Channel *channel);
	bool	isJoinableChannel(Channel *channel);

	//SETTERS AND GETTERS
	void	setFd(int fd);
	void	setUser(std::string user);
	void	setNick(std::string nick);
	void	setAuth(bool auth);
	void	setPwd(bool pwd);
	void	setCurrentChannel(Channel *channel);


	bool					getAuth();
	bool					getPwd();
	int						getFd();
	std::string				getUser();
	std::string				getNick();
	Channel					*getCurrentChannel();
	std::vector<Channel*>	&getJoinableChannels();
	Request					*getRequest();
};

std::ostream &	operator<<(std::ostream &o, Client &client);

#endif
