/*
 * address.cpp - the source file of Address class
 *
 * communicate with lower-level encrytion library
 *
 * Copyright (C) 2014-2015 Symeon Huang <hzwhuang@gmail.com>
 *
 * This file is part of the libQtShadowsocks.
 *
 * libQtShadowsocks is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * libQtShadowsocks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libQtShadowsocks; see the file LICENSE. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <QHostInfo>
#include "common.h"
#include "address.h"

using namespace QSS;

Address::Address(const QString &a, const quint16 &p, QObject *parent) :
    QObject(parent)
{
    data.second = p;
    setAddress(a);
}

Address::Address(const QHostAddress &ip, const quint16 &p, QObject *parent) :
    QObject(parent)
{
    data.second = p;
    setIPAddress(ip);
}

Address::Address(const Address &o) :
    QObject(o.parent())
{
    *this = o;
}

QString Address::getAddress() const
{
    return data.first;
}

QHostAddress Address::getIPAddress()
{
    if (ipAddrList.isEmpty()) {
        return getRealIPAddress();
    } else if (ipAddrList.count() == 1) {
        return ipAddrList.first();
    }
    return ipAddrList.at(Common::randomNumber(ipAddrList.count()));
}

QHostAddress Address::getRealIPAddress()
{
    if (ipAddrList.isEmpty()) {
        lookUpIP();
    }
    return ipAddrList.first();
}

bool Address::isIPValid()
{
    if (ipAddrList.isEmpty()) {
        lookUpIP();
    }
    if (ipAddrList.count() == 1 && ipAddrList.first().isNull()) {
        return false;
    }
    return true;
}

quint16 Address::getPort() const
{
    return data.second;
}

void Address::setAddress(const QString &a)
{
    data.first = a;
    QHostAddress ipAddress(a);
    if (!ipAddress.isNull()) {
        ipAddrList.clear();
        ipAddrList.append(ipAddress);
    }
}

void Address::setIPAddress(const QHostAddress &ip)
{
    ipAddrList.clear();
    ipAddrList.append(ip);
    data.first = ip.toString();
}

void Address::setPort(const quint16 &p)
{
    data.second = p;
}

int Address::addressType() const
{
    QHostAddress ipAddress(data.first);
    if (ipAddress.isNull()) {//it's a domain if it can't be parsed
        return ADDRTYPE_HOST;
    } else if (ipAddress.protocol() == QAbstractSocket::IPv4Protocol) {
        return ADDRTYPE_IPV4;
    } else {
        return ADDRTYPE_IPV6;
    }
}

Address &Address::operator= (const Address &o)
{
    this->data = o.data;
    this->ipAddrList = o.ipAddrList;
    return *this;
}

void Address::lookUpIP()
{
    ipAddrList = QHostInfo::fromName(data.first).addresses();
    if (ipAddrList.isEmpty()) {
        qWarning() << "Can't look up the IP addresses of " << data.first;
        ipAddrList.append(QHostAddress());
    }
}
