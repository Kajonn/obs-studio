#include <QTcpSocket>
#include <QByteArray>
#include <QTcpServer>

#include "command-socket.hpp"

#include "window-basic-main.hpp"

CommandSocket::CommandSocket(OBSBasic *parent, int port) :
	_parent(parent)
{
	blog(LOG_INFO, "========  Starting server on port %d =========", port);

	_qtcpServer = new QTcpServer();

	_qtcpServer->setMaxPendingConnections(1);

	connect(_qtcpServer, &QTcpServer::newConnection, 
			this, &CommandSocket::newConnection);

	if (!_qtcpServer->listen(QHostAddress::Any, port)) 
	{
		blog( LOG_ERROR, "Cannot start server, %s", _qtcpServer->errorString() );
	}

	connect(this, &CommandSocket::startRecording,
			parent, &OBSBasic::StartRecording);
	
	connect(this, &CommandSocket::stopRecording,
			parent, &OBSBasic::StopRecording);
}


CommandSocket::~CommandSocket()
{
}


void CommandSocket::newConnection()
{
	if (_connectedSocket == NULL)
	{
		blog(LOG_INFO, "========  Client connected to server =========");

		_connectedSocket = _qtcpServer->nextPendingConnection();

		connect(_connectedSocket, &QAbstractSocket::disconnected,
				_connectedSocket, &QObject::deleteLater);
		connect(_connectedSocket, &QAbstractSocket::disconnected,
				this, &CommandSocket::socketDisconnect);

		connect(_connectedSocket, &QAbstractSocket::readyRead,
				this, &CommandSocket::incommingCommand);
		
		_connectedSocket->write("Connected to OBS, write help to see avaliable commands\n\r");
		_connectedSocket->write("======================================================\n\r");
	}
	else {
		QTcpSocket *socket = _qtcpServer->nextPendingConnection();

		connect(socket, &QAbstractSocket::disconnected,
				socket, &QObject::deleteLater);
		_connectedSocket->close();
	}
	
}

void CommandSocket::incommingCommand()
{
	if (_connectedSocket != NULL)
	{
		QByteArray data = _connectedSocket->readAll();
		
		_dataString.append(data);

		if (_dataString.endsWith('\n'))
		{
			blog(LOG_INFO, "Client sent command: %s", _dataString.toStdString().c_str());

			if (_dataString.toLower().startsWith("record"))
			{
				QStringList splitStr = _dataString.split(" ", QString::SkipEmptyParts);
				QString fileName;
				if (splitStr.size() > 1) 
				{
					fileName = splitStr.at(1);
				}
				blog(LOG_INFO, "Recived record command to file %s", fileName.toStdString().c_str());
				emit startRecording(fileName);
			}
			else if (_dataString.toLower().startsWith("stop"))
			{
				blog(LOG_INFO, "Recived stop command");
				emit stopRecording();
			}
			else if (_dataString.toLower().startsWith("bye")) {
				_connectedSocket->close();
			}
			else if (_dataString.toLower().startsWith("help")) {
				_connectedSocket->write("record filename - Starts recording, filename is optional\n\rstop - Stops recording\n\r");
			}
			else {
				_connectedSocket->write("Unknown command\n\r");
			}

			_dataString.clear();
		}
	}
}

void CommandSocket::socketDisconnect()
{
	blog(LOG_INFO, "========  Client disconnected from server =========");
	_connectedSocket = NULL;
	_dataString.clear();

	if (_qtcpServer->hasPendingConnections())
	{
		newConnection();
	}
}

void CommandSocket::recordingStarted()
{
	if (_connectedSocket)
	{
		_connectedSocket->write("STARTED\n\r");
	}
}

void CommandSocket::recordingStopped()
{
	if (_connectedSocket)
	{
		_connectedSocket->write("STOPPED\n\r");
	}
}


void CommandSocket::sendInfoMsg(QString infoMsg)
{
	if (_connectedSocket)
	{
		_connectedSocket->write(infoMsg.toLocal8Bit());
		_connectedSocket->write("\n\r");
	}
}

void CommandSocket::sendErrorMsg(QString errorMsg)
{
	if (_connectedSocket)
	{
		_connectedSocket->write("ERROR\n\r");
		_connectedSocket->write(errorMsg.toLocal8Bit());
		_connectedSocket->write("\n\r");
	}
}