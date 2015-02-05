#!/usr/bin/perl

use strict;
use Socket;
use FileHandle;

my $port = shift || 12346;
my $proto = getprotobyname('tcp');

socket(SOCK, PF_INET, SOCK_STREAM, $proto) || die "socket: $!";
SOCK->autoflush();

bind(SOCK, sockaddr_in($port, INADDR_ANY)) || die "bind: $!";

listen(SOCK, SOMAXCONN) || die "list: $!";

while(1)
{
    my $paddr = accept(CLIENT, SOCK);
    CLIENT->autoflush();
    my $msg = <CLIENT>;
    print "server: recv from client: $msg \n";
    print CLIENT "Hello there, its now ", scalar localtime, "\n";
    close(CLIENT);
}
