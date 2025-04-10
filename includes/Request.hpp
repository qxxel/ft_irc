/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibjean-b <ibjean-b@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-04-01 14:50:56 by ibjean-b          #+#    #+#             */
/*   Updated: 2025-04-01 14:50:56 by ibjean-b         ###   ########.fr       */
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
	std::vector<Command>	_arr;
public:
	~Request();
	Request(char *raw);

	void					setArr(std::vector<Command> arr);
	std::vector<Command>	&getArr();
};

#endif