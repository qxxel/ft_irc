/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 16:03:22 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/05/02 14:26:43 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Channel.hpp"

class Channel;
class Request;

class Client
{
protected:
	int						_fd;
	bool					_auth;
	bool					_pwd;
	std::string				_user;
	std::string				_nick;
	Request					*_req;
	std::vector<Channel*>	_currentsChannels;
	std::vector<Channel*>	_joinableChannels;

public:
	Client(int fd);
	~Client();

	void	delJoinableChannel(Channel *channel);
	void	delCurrentChannel(Channel *channel);
	bool	isJoinableChannel(Channel *channel);
	bool	isCurrentChannel(std::string name);

	//SETTERS AND GETTERS
	void	setFd(int fd);
	void	setUser(std::string user);
	void	setNick(std::string nick);
	void	setAuth(bool auth);
	void	setPwd(bool pwd);


	bool					getAuth();
	bool					getPwd();
	int						getFd();
	std::string				getUser();
	std::string				getNick();
	std::vector<Channel*>	&getCurrentsChannels();
	std::vector<Channel*>	&getJoinableChannels();
	Request					*getRequest();
};

std::ostream &	operator<<(std::ostream &o, Client &client);

#endif
