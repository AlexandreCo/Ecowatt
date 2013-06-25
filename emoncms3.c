
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <mysql.h>
#define MAX_REQ_SQL 255
typedef struct
{
	float kWatt;
	time_t time;
}t_data;

char acPass[255];
static void vSendDataMysql(void)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int i;
	t_data atPowerMois[255];
	t_data atPowerJour[255];
	int iNbMois=0;
	int iNbJour=0;
	int iTime=0;
	char acBuf[MAX_REQ_SQL];
	conn = mysql_init(NULL);
	/* Connect to database */
	if (!mysql_real_connect(conn, "192.168.0.14",
			"ecowatt", acPass, "emoncms", 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return;
	}
	/* GET last entry */
	sprintf(acBuf,"SELECT *  FROM  `%s`","feed_37");
	if (mysql_query(conn, acBuf)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		return;
	}
	res = mysql_use_result(conn);
	float fCumulMois,fCumulJour=0;
	time_t timestamp;
	struct tm *t;
	char cMois=-1;
	char cJour=-1;
	time_t timestampMois=0,timestampJour=0;


	while ((row = mysql_fetch_row(res)) != NULL)
	{


		timestamp=atoi(row[0]);
		t = localtime(&timestamp);
		if(cMois!=t->tm_mon)
		{
			if(timestampMois)
			{
				atPowerMois[iNbMois].kWatt=fCumulMois/1000;
				atPowerMois[iNbMois++].time=timestampMois;
			}
			timestampMois=timestamp;
			cMois=t->tm_mon;
			fCumulMois=0;
		}
		if(cJour!=t->tm_mday)
		{
			//envoyer la sauce
			if(timestampJour)
			{
				atPowerJour[iNbJour].kWatt=fCumulJour/1000;
				atPowerJour[iNbJour++].time=timestampJour;
			}

			timestampJour=timestamp;
			cJour=t->tm_mday;
			fCumulJour=0;
		}

		fCumulMois+=atof(row[1]);
		fCumulJour+=atof(row[1]);

		printf("%02d/%02d\t%s\t%f\t%f\n",t->tm_mday,t->tm_mon+1,row[1],fCumulMois,fCumulJour);


	}


	/* close connection */
	mysql_free_result(res);

	for(i =0 ;i< iNbMois;i++)
	{
		sprintf(acBuf,"INSERT INTO feed_39 (time,data) VALUES(%ld,%f)" ,atPowerMois[i].time ,atPowerMois[i].kWatt);
		printf("%s\n",acBuf);
		if (mysql_query(conn, acBuf)) {
			fprintf(stderr, "%s\n", mysql_error(conn));
			return;
		}
	}

	for(i =0 ;i< iNbJour;i++)
	{
		//envoyer la sauce
		sprintf(acBuf,"INSERT INTO feed_38 (time,data) VALUES(%ld,%f)" ,atPowerJour[i].time ,atPowerJour[i].kWatt);
		printf("%s\n",acBuf);
		if (mysql_query(conn, acBuf)) {
			fprintf(stderr, "%s\n", mysql_error(conn));
			return;
		}
	}

	mysql_close(conn);


}


int main (int argc, char *argv[])
{
	if(argc==2)
	{
		strncpy(acPass,argv[1],255);
	}
	vSendDataMysql();
	return 0;
}
