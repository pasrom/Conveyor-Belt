#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <vxWorks.h>
#include <sockLib.h>
#include <inetLib.h>
#include <hostLib.h>
#include <stdioLib.h>
#include <strLib.h>
#include <ioLib.h>
#include <fioLib.h>
#include <time.h>
#include "ptpUdpDefs.h"

class UdpClient {
private:
	int sFd; /* socket file descriptor */
public:
	/* Default constructor */
	UdpClient();
	/* Default destructor */
	~UdpClient();
	STATUS receiveUDP(struct requestPTP * myRequestP);
	STATUS sendUDP(struct requestPTP * myRequestP);
	/* this is a overloading for testing purpose */
	STATUS sendUDP(struct requestPTP * myRequestP, char * str);
	void deleteSfd();
};

#endif
