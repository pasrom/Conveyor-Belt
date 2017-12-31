#include "PtpClient.h"

PTPClient::PTPClient() {
	myInPTPClient = this;
	myInUdpClient = new UdpClient;
	recTimeStamp.tv_sec = 0;
	recTimeStamp.tv_nsec = 0;
	return;
}

PTPClient::~PTPClient() {
	printf("PTPClient Dekonstruktor\n");
	return;
}

void PTPClient::startPTPClient() {
	taskID = taskSpawn(getTaskName(), 110, VX_FP_TASK, 0x10000, (FUNCPTR) PTPClient::wrapperTask, (int) myInPTPClient, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	printf("taskID Client %i\n\r", taskID);
}

void PTPClient::stopPTPClient() {
	taskDelete(taskID);
	myInUdpClient->deleteSfd();
	delete myInUdpClient;
	printf("PTPClient stopPTPClient\n");
}

struct timespec PTPClient::getTime() {
	recTimeStamp.tv_sec = time(NULL) - timeOffset;
	recTimeStamp.tv_nsec = 0;
	return recTimeStamp;
}
void PTPClient::setTaskName(char * in) {
	strcpy(taskName, in);
}
char * PTPClient::getTaskName() {
	return taskName;
}

void PTPClient::wrapperTask(PTPClient* obj) {
	PTPClient* myself = (PTPClient*) obj;
	myself->syncClk(0);
	printf("jou\n");
}

void PTPClient::syncClk(int debug) {

	long t0, t1, t2, t3;
	int i = 0;

	time_val.tv_nsec = 0;
	time_val.tv_sec = 0;
	// Initialize myReceiveRequest
	myRequest.PTP_version = PTP_CURRENT_VERSION; /* PTP version used */
	myRequest.message_type = 0; /* PTP mesage type is here */
	myRequest.epoch_number = 0; /* part of timestamp */
	myRequest.seconds = 0; /* part of timestamp */
	myRequest.nanoseconds = 0; /* part of timestamp */
	//strcpy(myRequest.inetAddr, "91.0.0.91");
	strcpy(myRequest.inetAddr, "91.0.0.91");

	FOREVER {
		// Wait for sync
		do {
			myInUdpClient->receiveUDP(&myRequest);
		} while (myRequest.message_type != SYNC_MESSAGE);
		clock_gettime(CLOCK_REALTIME, &time_val);
		/* if the received timestamp is realy too bad, take it emediatly */
		if (abs(myRequest.seconds - time_val.tv_sec) > 1000) {
			recTimeStamp.tv_sec = myRequest.seconds;
			recTimeStamp.tv_nsec = 0;
			clock_settime(CLOCK_REALTIME, &recTimeStamp);
		} else {
			t1 = (long) time(NULL);
			if (debug)
				printf("Sync recieved at t1: %d\n", t1);

			// Wait for follow-up
			myInUdpClient->receiveUDP(&myRequest);
			t0 = (long) myRequest.seconds;
			if (debug)
				printf("Sync was sent at t0: %d\n", t0);

			if (i == 0) {
				i++;

				// Send delay request
				myRequest.message_type = DELAY_REQ_MESSAGE;
				t2 = (long) time(NULL);
				myInUdpClient->sendUDP(&myRequest);
				if (debug)
					printf("Delay req was sent at t2: %d\n", t2);

				// Get delay response
				myInUdpClient->receiveUDP(&myRequest);
				t3 = (long) myRequest.seconds;
				if (debug)
					printf("Delay resp received at t3: %d\n", t3);

			} else {
				// wenn eine Random-Zahl modulo 13 ist, dann i=0
				if ((rand() % 13) == 0)
					i = 0;
			}

			// Hier kommt die Offset/Delay-Berechnung hinein
			timeOffset = (t1 - t0 - t3 + t2) / 2;
			propDelay = (t1 - t0 + t3 - t2) / 2;
			/* set the realtime clock to the new value */
			recTimeStamp.tv_sec = time_val.tv_sec - timeOffset;
			recTimeStamp.tv_nsec = time_val.tv_nsec;
			printf("clocksettime %i\n", clock_settime(CLOCK_REALTIME, &recTimeStamp));

			if (debug)
				printf("Time offset is: %d\n", timeOffset);
			if (debug)
				printf("Prop delay is: %d\n", propDelay);

		}
	}
}
