/*
*	CommandSocket starts a tcp server which a user can connect to 
*	and use simple commands to start and stop recordings.
*/

#pragma once

#include <QPointer>

class QTcpServer;
class OBSBasic;
class QTcpSocket;

class CommandSocket : public QObject
{
	Q_OBJECT
public:
	CommandSocket(OBSBasic *parent, int port);
	~CommandSocket();

signals:
	void startRecording(QString msg);
	void stopRecording();

public slots:
	void recordingStarted();
	void recordingStopped();
	void sendInfoMsg(QString msg);
	void sendErrorMsg(QString errorMsg);


private slots:
	void newConnection();
	void incommingCommand();
	void socketDisconnect();

private:

	OBSBasic *_parent;
	QPointer<QTcpServer> _qtcpServer;
	QPointer<QTcpSocket> _connectedSocket;
	QString _dataString;

};

