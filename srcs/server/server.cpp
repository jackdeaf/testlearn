/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: steh <steh@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/11 17:43:00 by steh              #+#    #+#             */
/*   Updated: 2023/03/11 19:57:01 by steh             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

server::server()
{
    cout << "server default constructor" << endl;
}

server::server(ifstream& config_file) : _config(config_file)
{
    cout << "server config constructor" << endl;
}

server::server(const server& other)
{
    if (this != &other)
        this->_config = other._config;
}

server::~server()
{
    return ;
}

server& server::operator=(const server& other)
{
    if (this != &other)
        this->_config = other._config;
    return (*this);
}
