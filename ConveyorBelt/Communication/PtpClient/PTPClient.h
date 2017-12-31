#ifndef PTP_CLIENT_H
#define PTP_CLIENT_H

#include "FHV.h"
#include "UdpClient.h"
//#include "time.h"

class UdpClient;

class PTPClient {
private:

public:
	PTPClient* myInPTPClient;
	UdpClient * myInUdpClient;
	PTPClient();
	~PTPClient();
	static void wrapperTask(PTPClient* obj);
	void startPTPClient();
	void stopPTPClient();
	struct timespec getTime();
	void setTaskName(char * in);
	char * getTaskName();

private:
	void syncClk(int debug);
	long propDelay, timeOffset;
	int taskID;
	struct timespec recTimeStamp;
	struct timespec time_val;
	char taskName[20];
	bool deleteTask;
	struct requestPTP myRequest;
};
#endif
