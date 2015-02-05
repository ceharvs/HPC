#!/usr/bin/perl


use strict;
use Socket;
use FileHandle;

my $host = shift || 'localhost';
my $port = shift || 12346;
my $proto = getprotobyname('tcp');

#new
my $iaddr = inet_aton($host) || die "no valid host specified: $host";
my $paddr = sockaddr_in($port, $iaddr);



socket(SOCK, PF_INET, SOCK_STREAM, $proto) || die "socket: $!";
SOCK->autoflush();

connect(SOCK, $paddr) || die "socket failed $!";

#bind(SOCK, sockaddr_in($port, INADDR_ANY)) || die "bind: $!";


print SOCK "Hello from client\n";
my $line = <SOCK>;
print "client : recvd from server  : $line\n";
