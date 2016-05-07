char gchan[300];
int ilosc = 6;
int maxval = 49;
class IrcBot
{
public:
    IrcBot(char * _nick, char * _usr);
    virtual ~IrcBot();
 
    bool setup;
 
    void start();
    bool charSearch(char *toSearch, char *searchFor);
 
private:
    char *port;
    int s;
    char *nick;
    char *usr;
 
    bool isConnected(char *buf);
    char * timeNow();
    bool sendData(char *msg);
    void sendPong(char *buf);
    void msgHandel(char *buf);
    void msgprint(char * buf);
    void poc(char *);
};
bool msg5 = true;
bool reged;
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <fstream>

bool logging;

using namespace std;
 
#define MAXDATASIZE 250

char * getNick(char * buf)
{
    char * newnick = (char *) malloc(900);
    buf++;
    while (*buf++ != '!');
    for (int licz = 0;*buf != '@'; buf++, licz++)
    {
        newnick[licz] = *buf;
    }
    return newnick;
}
char * getNick2(char * buf)
{
    char * newnick = (char *) malloc(900);
    buf++;
    for (int licz = 0;*buf != '!'; buf++, licz++)
    {
        newnick[licz] = *buf;
    }
    return newnick;
}
char * toLowerString(char * str)
{
    for (int licz = 0; str[licz] != '\0'; licz++)
    {
        str[licz] = tolower(str[licz]);
    }
}

IrcBot::IrcBot(char * _nick, char * _usr)
{
    nick = _nick;
    usr = _usr;
}
 
IrcBot::~IrcBot()
{
    close (s);
}
 
void IrcBot::start()
{
    srand(time(0));
    struct addrinfo hints, *servinfo;
    setup = true;
    port = "6667";
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int res;
    if ((res = getaddrinfo("irc.spotchat.org",port,&hints,&servinfo)) != 0)
    {
        setup = false;
        fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(res));
    }
    if ((s = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol)) == -1)
    {
        perror("client: socket");
    }
    if (connect(s,servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        close (s);
        perror("Client Connect");
    }
    freeaddrinfo(servinfo);
    int numbytes;
    char buf[MAXDATASIZE];
 
    int count = 0;
    while (1)
    {
        count++;
 
        switch (count) {
            case 3:
                sendData(nick);
                sendData(usr);
                break;
            case 4:
                char command[300];
                char chan2[250];
                cin >> chan2;
                sprintf(gchan, "#%s", chan2);
                sprintf(command, "JOIN %s\r\n", gchan);
                cout << command << '\n';
                sendData(command);
                break;

            default:
                break;
        }
        numbytes = recv(s,buf,MAXDATASIZE-1,0);
        buf[numbytes]='\0';
        cout << buf;
        if (charSearch(buf,"PING"))
        {
            sendPong(buf);
        }
        msgHandel(buf);

        if (charSearch(buf,"PING"))
        {
            sendPong(buf);
        }

        if (numbytes==0)
        {
            cout << "Connection closed! Time:"<< endl;
            cout << timeNow() << endl;
            break;
        }
    }
}

void IrcBot::poc(char * toput)
{
    char * costam2 = (char *) malloc(1000);
    sprintf(costam2, "PRIVMSG %s :%s", gchan, toput);
    sendData(costam2);
    free(costam2);
}

bool IrcBot::charSearch(char *toSearch, char *searchFor)
{
    int len = strlen(toSearch);
    int forLen = strlen(searchFor);

    for (int i = 0; i < len;i++)
    {
        if (searchFor[0] == toSearch[i])
        {
            bool found = true;
            int x;
            for (x = 1; x < forLen; x++)
            {
                if (toSearch[i+x]!=searchFor[x])
                {
                    found = false;
                }
            }

            if (found == true)
                return x;
        }
    }
 
    return 0;
}
 
bool IrcBot::isConnected(char *buf)
{
    if (charSearch(buf,"/MOTD") == true)
        return true;
    else
        return false;
}
 
char * IrcBot::timeNow()
{
    time_t rawtime;
    struct tm * timeinfo;
 
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
 
    return asctime (timeinfo);
}
 
bool IrcBot::sendData(char *msg)
{
    //Send some data
    int len = strlen(msg);
    int bytes_sent = send(s,msg,len,0);
 
    if (bytes_sent == 0)
        return false;
    else
        return true;
}
void IrcBot::msgprint(char * buf)
{
    while (*buf++ != ':');
    while (*buf++ != ':');
    buf++;
    if (buf[0] == '`')
        buf++;
    char * newstr = (char *) malloc(100);
    if (!strncmp(buf, "!ban CppBot", 11))
    {
        while (*buf-- != ':');
        while (*buf-- != ':');
        sprintf(newstr, "PRIVMSG %s :!kick %s\r\n", gchan, getNick2(buf));
        sendData(newstr);
    }
    free(newstr);
    char sendto[400];
    sprintf(sendto, "PRIVMSG %s :%s\r\n", gchan, buf);
    sendData(sendto);
}
void IrcBot::sendPong(char *buf)
{
    char * toSearch = "PING ";
 
    for (int i = 0; i < strlen(buf);i++)
        {
            if (buf[i] == toSearch[0])
            {
                bool found = true;
                //search the char array for search field
                for (int x = 1; x < 4; x++)
                {
                    if (buf[i+x]!=toSearch[x])
                    {
                        found = false;
                    }
                }
                if (found == true)
                {
                    int count = 0;
                    //Count the chars
                    for (int x = (i+strlen(toSearch)); x < strlen(buf);x++)
                    {
                        count++;
                    }
 
                    //Create the new char array
                    char returnHost[count + 5];
                    returnHost[0]='P';
                    returnHost[1]='O';
                    returnHost[2]='N';
                    returnHost[3]='G';
                    returnHost[4]=' ';
 
                    count = 0;
                    //set the hostname data
                    for (int x = (i+strlen(toSearch)); x < strlen(buf);x++)
                    {
                        returnHost[count+5]=buf[x];
                        count++;
                    }
                    sendData("PONG :abc\r\n");
                    cout << timeNow() <<"  Ping Pong" << endl;
                    return;
                }
            }
        }
}
bool write2;
void IrcBot::msgHandel(char * buf)
{
    char * orgbuf = (char *) malloc(900);
    strcpy(orgbuf, buf);
    FILE * file = fopen("log.html", "a+");
    if (charSearch(buf, "cppbot:reg"))
    {
        sendData("PRIVMSG NickServ :IDENTIFY YourBotNick [password]\r\n");
        reged = true;
    }
    if (!reged)
        return;
    if (logging)
    {
        fprintf(file, "%s||| %s <br> \n", timeNow(), buf);
    }
    if (charSearch(buf, "cppbot:enablelog:27072003:jezykC2003"))
    {
        logging = true;
    }
    if (charSearch(buf, "cppbot:disablelog:27072003:jezykC2003"))
    {
        logging = false;
    }
    if (!charSearch(buf, "`") && !charSearch(buf, "JOIN :"))
        toLowerString(buf);
    char sending[1000];
    char sending2[1000];
    if (charSearch(buf, "cppbot:owner"))
    {
        sprintf(sending, "PRIVMSG %s :Pawel.\r\n", gchan);
        sendData(sending);
    }
    if (charSearch(buf, "cppbot:test"))
    {
        sprintf(sending, "PRIVMSG %s :I live.\r\n", gchan);
        sendData(sending);
    }
    if (charSearch(buf, "fuck") || charSearch(buf, "whore") || charSearch(buf, "cock") || charSearch(buf, "sex"))
    {
        sprintf(sending, "PRIVMSG %s :!kickban %s swearing\r\n", gchan, getNick2(buf));
        sendData(sending);
        fprintf(file, "<b> %s </b> <br> <br>\n", buf);
    }
    if (charSearch(buf, "cppbot:mynick"))
    {
        sprintf(sending, "PRIVMSG %s :%s\r\n", gchan, getNick(buf));
        sendData(sending);
    }

    if (charSearch(buf, "cppbot:balls:6"))
    {
        ilosc = 6;
    }
    if (charSearch(buf, "cppbot:balls:10"))
    {
        ilosc = 10;
    }
    if (charSearch(buf, "cppbot:balls:25"))
    {
        ilosc = 25;
    }
    if (charSearch(buf, "cppbot:maxval:49"))
    {
        maxval = 49;
    }
    if (charSearch(buf, "cppbot:maxval:100"))
    {
        maxval = 100;
    }
    if (charSearch(buf, "c++:syntax:cout:display_your_object;"))
    {
        cout << "Debug.";
        poc("Syntax:\r\n");
        poc("ostream & operator<<(ostream & screen, [your-class-name] & name)\r\n");
        poc("{\r\n");
        poc("/*Code, which displays your object. Please remember, that you must use screen, not cout.*/\r\n");
        poc("/*For example, you must write 'screen << \"Object\";, no cout << \"Object\";*/\r\n");
        poc("}\r\n");
    }
    if (charSearch(buf, "c++:syntax:cout;"))
    {
        poc("cout syntax:\r\n");
        poc("cout << [object1] << [object2] << ... << [objectN];\r\n");
        poc("If you created your own object, you must define function for cout,\r\n");
        poc("because cout don't know, how to print your object on console screen.\r\n");
        poc("To see, how to define that function, write: \"c++:syntax:cout:display_your_object;\".\r\n");
    }
    if (charSearch(buf, "cppbot:lotto"))
    {
        srand(time(0));
        int number = 0;
        for (int licz = 0; licz < ilosc; licz++)
        {
            number = ((rand() % maxval) + 1);
            sprintf(sending, "PRIVMSG %s :number%d: %d\r\n", gchan, licz + 1, number);
            sendData(sending);
            while (number < 1 || number > maxval)
                number = rand();
        }
    }
    if (charSearch(buf, "cppbot:help"))
    {
        sprintf(sending, "PRIVMSG %s :cppbot:register - Register a bot to account.\r\n", gchan);
        sendData(sending);
        sprintf(sending, "PRIVMSG %s :cppbot:lotto - Display random values. See also cppbot:balls, and cppbot:maxval.\r\n", gchan);
        sendData(sending);
        sprintf(sending, "PRIVMSG %s :cppbot:maxval - Usage:'cppbot:maxval:[maxval]' - Sets maximal value in cppbot:lotto. This value can be equal to 49/100.\r\n", gchan);
        sendData(sending);
        sprintf(sending, "PRIVMSG %s :cppbot:balls - Usage:'cppbot:balls:[numberofballs]' - Sets number of balls in cppbot:lotto. This value can be equal to 6/10/25.\r\n", gchan);
        sendData(sending);
        sprintf(sending, "PRIVMSG %s :cppbot:owner - Displays owner of this bot.\r\n", gchan);
        sendData(sending);
        sprintf(sending, "PRIVMSG %s :cppbot:mynick - Displays your username.\r\n", gchan);
        sendData(sending);
        sprintf(sending, "PRIVMSG %s :cppbot:kickme - Kicks you from the channel.\r\n", gchan);
        sendData(sending);
        sprintf(sending, "PRIVMSG %s :cppbot:hack - Displays fake hacking informations.\r\n", gchan);
        sendData(sending);
        sprintf(sending, "PRIVMSG %s :cppbot:test - If bot display 'I live.', it not dead.\r\n", gchan);
        sendData(sending);
    }
    if (charSearch(buf, "cppbot:kickme"))
    {
        sprintf(sending, "PRIVMSG %s :!kick %s bye\r\n", gchan, getNick2(buf));
        sendData(sending);
    }
    if (charSearch(buf, "c++:quiz"))
    {
        int randomval = ((rand() % 25) + 1);
        
    }
    if (charSearch(buf, "JOIN :"))
    {
        sprintf(sending, "PRIVMSG %s :Welcome to %s channel, %s! Please read channel topic and channel rules. Good day, %s!\r\n", gchan, gchan, getNick2(buf), getNick2(buf));
        sendData(sending);
        sprintf(sending, "PRIVMSG %s :Please also remember, that there are logging features in this channel. Please don't swear. If you would like to display bot commands, type cppbot:help .\r\n", getNick2(buf));
        sendData(sending);
    }
    if (charSearch(buf, "`"))
    {
        msgprint(buf);
    }
    if (charSearch(buf, "cppbot:hack"))
    {
        int randed = (rand() % 6);
        if (randed == 0)
        {
            sprintf(sending, "PRIVMSG %s :Clearing YouTube page and deleting all videos there...\r\n", gchan);
            sendData(sending);
        }
        else if (randed == 1)
        {
            sprintf(sending, "PRIVMSG %s :I have hacked all users passwords!\r\n", gchan);
            sendData(sending);
        }
        else if (randed == 2)
        {
            sprintf(sending, "PRIVMSG %s :I started hacking Linux Mint page...\r\n", gchan);
            sendData(sending);
        }
        else if (randed == 3)
        {
            sprintf(sending, "PRIVMSG %s :Clearing %s's hard disc...\r\n", gchan, getNick(buf));
            sendData(sending);
        }
        else if (randed == 4)
        {
            sprintf(sending, "PRIVMSG %s :Hacking %s's files...\r\n", gchan, getNick(buf));
            sendData(sending);
        }
        else if (randed == 5)
        {
            sprintf(sending, "PRIVMSG %s :%s's hard disc is now broken!\r\n", gchan, getNick(buf));
            sendData(sending);
        }
    }
    fclose(file);
}
int main()
{
    logging = false;
    reged = false;
    IrcBot bot = IrcBot("NICK YourBotNick\r\n","USER YourBotNick a a :YourBotNick\r\n");
    bot.start();
    return 0;
}
