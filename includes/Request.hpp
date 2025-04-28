/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mreynaud <mreynaud@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 14:50:56 by ibjean-b          #+#    #+#             */
/*   Updated: 2025/04/11 19:50:06 by mreynaud         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <vector>
#include "Command.hpp"

class Command;

class Request
{
private:
	std::string				_line;
	std::vector<Command>	_arr;
public:
	~Request();
	Request();
	Request(char *raw);

	std::string				&append(const char* s, size_t n);
	void					clear();
	std::string				getLine();
	void 					split_Request();

	void					setArr(std::vector<Command> arr);
	std::vector<Command>	&getArr();
};

#endif
