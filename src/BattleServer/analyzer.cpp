#include "../Shared/networkcommands.h"
#include <Utilities/network.h>
#include <PokemonInfo/battlestructs.h>

#include "analyzer.h"

Analyzer::Analyzer(GenericSocket sock, int id) : socket(new Network<GenericSocket>(sock, id)), m_id(id)
{
    socket->setParent(this);

    connect(socket, SIGNAL(disconnected()), SIGNAL(disconnected()));
    connect(socket, SIGNAL(isFull(QByteArray)), this, SLOT(commandReceived(QByteArray)));
    connect(socket, SIGNAL(_error()), this, SLOT(error()));

    connect(this, SIGNAL(sendCommand(QByteArray)), socket, SLOT(send(QByteArray)));
}

void Analyzer::error()
{
    emit connectionError(socket->error(), socket->errorString());
}

void Analyzer::commandReceived(const QByteArray &command)
{
    dealWithCommand(command);
}

void Analyzer::dealWithCommand(const QByteArray &commandline)
{
    DataStream in (commandline, version.version);
    uchar command;

    in >> command;

    //qDebug() << "Dealing with command " << int(command);

    switch (command) {
    case EngageBattle: {
        qint32 battleid;
        BattlePlayer p1, p2;
        ChallengeInfo c;
        TeamBattle t1, t2;

        in >> battleid >> p1 >> p2 >> c >> t1.fullSerial() >> t2.fullSerial();

        emit newBattle(battleid, p1, p2, c, t1, t2);
        break;
    }
    case KeepAlive: {
        notify(KeepAlive);
        break;
    }
    case BattleMessage: {
        qint32 battle, player;
        BattleChoice choice;

        in >> battle >> player >> choice;

        emit playerChoice(battle, player, choice);
        break;
    }
    case SpectatingBattleChat: case BattleChat:  {
        qint32 battle, player;
        QString message;

        in >> battle >> player >> message;

        if (command == BattleChat) {
            emit battleChat(battle, player, message);
        } else {
            emit spectatingChat(battle, player, message);
        }
        break;
    }
    case SpectateBattle: {
        qint32 battle;
        bool spectate;
        qint32 player;
        QString name;

        in >> battle >> spectate >> player >> name;

        emit spectating(battle, spectate, player, name);
        break;
    }
    case BattleFinished: {
        qint32 battle;
        uchar result;
        qint32 forfeiter;

        in >> battle >> result >> forfeiter;

        emit battleFinished(battle, result, forfeiter);
    }
    case DatabaseMod: {
        QString mod;
        in >> mod;

        emit modChanged(mod);
    }
    case LoadPlugin: {
        bool load;
        in >> load;

        if (load) {
            QString path;
            in >> path;

            emit loadPlugin(path);
        } else {
            QString name;
            in >> name;

            emit unloadPlugin(name);
        }
        break;
    }
    default:
        //emit protocolError(UnknownCommand, tr("Protocol error: unknown command received"));
        break;
    }
}
