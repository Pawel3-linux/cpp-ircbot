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
    int s; //the socket descriptor
 
    char *nick;
    char *usr;
 
    bool isConnected(char *buf);
    char * timeNow();
    bool sendData(char *msg);
    void sendPong(char *buf);
    void msgHandel(char *buf);
    void msgprint(char * buf);
};
bool msg5 = true;
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
 
using namespace std;
 
#define MAXDATASIZE 100

char * getNick(char * buf)
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
 
    //Setup run with no errors
    setup = true;
 
    port = "6667";
 
    //Ensure that servinfo is clear
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
 
    //setup hints
    hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
 
    //Setup the structs if error print why
    int res;
    if ((res = getaddrinfo("irc.spotchat.org",port,&hints,&servinfo)) != 0)
    {
        setup = false;
        fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(res));
    }
 
    //setup the socket
    if ((s = socket(servinfo->ai_family,servinfo->ai_socktype,servinfo->ai_protocol)) == -1)
    {
        perror("client: socket");
    }
 
    //Connect
    if (connect(s,servinfo->ai_addr, servinfo->ai_addrlen) == -1)
    {
        close (s);
        perror("Client Connect");
    }
 
    //We dont need this anymore
    freeaddrinfo(servinfo);
 
    //Recv some data
    int numbytes;
    char buf[MAXDATASIZE];
 
    int count = 0;
    while (1)
    {
        //declars
        count++;
 
        switch (count) {
            case 3:
                sendData(nick);
                sendData(usr);
                sendData("PRIVMSG NickServ :IDENTIFY jezykC\r\n");
                break;
            case 4:
                char command[300];
                char chan2[250];
                cout << "Give name of a channel, where I will join (without a #):\n";
                cin >> chan2;
                sprintf(gchan, "#%s", chan2);
                sprintf(command, "JOIN %s\r\n", gchan);
                cout << command << '\n';
                sendData(command);
                break;

            default:
                break;
        }
 
        //Recv & print Data
        numbytes = recv(s,buf,MAXDATASIZE-1,0);
        buf[numbytes]='\0';
        cout << buf;
        //buf is the data that is recived
        if (charSearch(buf,"PING"))
        {
            sendPong(buf);
        }
        //Pass buf to the message handeler
        msgHandel(buf);
 
        //If Ping Recived
        /*
         * must reply to ping overwise connection will be closed
         * see http://www.irchelp.org/irchelp/rfc/chapter4.html
         */
        if (charSearch(buf,"PING"))
        {
            sendPong(buf);
        }
 
        //break if connection closed
        if (numbytes==0)
        {
            cout << "----------------------CONNECTION CLOSED---------------------------"<< endl;
            cout << timeNow() << endl;
 
            break;
        }
    }
}
 
bool IrcBot::charSearch(char *toSearch, char *searchFor)
{
    int len = strlen(toSearch);
    int forLen = strlen(searchFor); // The length of the searchfor field
 
    //Search through each char in toSearch
    for (int i = 0; i < len;i++)
    {
        //If the active char is equil to the first search item then search toSearch
        if (searchFor[0] == toSearch[i])
        {
            bool found = true;
            //search the char array for search field
            int x;
            for (x = 1; x < forLen; x++)
            {
                if (toSearch[i+x]!=searchFor[x])
                {
                    found = false;
                }
            }
 
            //if found return true;
            if (found == true)
                return x;
        }
    }
 
    return 0;
}
 
bool IrcBot::isConnected(char *buf)
{//returns true if "/MOTD" is found in the input strin
    //If we find /MOTD then its ok join a channel
    if (charSearch(buf,"/MOTD") == true)
        return true;
    else
        return false;
}
 
char * IrcBot::timeNow()
{//returns the current date and time
    time_t rawtime;
    struct tm * timeinfo;
 
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
 
    return asctime (timeinfo);
}
 
bool IrcBot::sendData(char *msg)
{//Send some data
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
    char sendto[400];
    sprintf(sendto, "PRIVMSG %s :%s\r\n", gchan, buf);
    sendData(sendto);
}
void IrcBot::sendPong(char *buf)
{
    //Get the reply address
    //loop through bug and find the location of PING
    //Search through each char in toSearch
 
    char * toSearch = "PING ";
 
    for (int i = 0; i < strlen(buf);i++)
        {
            //If the active char is equil to the first search item then search toSearch
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
 
                //if found return true;
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
 
                    buf[1] = 'O';
                    sendData(buf);
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
    FILE * file = fopen("log.html", "a+");
    fprintf(file, "%s||| %s <br> \n", timeNow(), buf);
    if (!charSearch(buf, "`"))
        toLowerString(buf);
    char sending[1000];
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
    if (charSearch(buf, "cppbot:reg"))
    {
        /*There is password for CppBot. I hide it for you.*/
    }
    if (charSearch(buf, "*") || charSearch(buf, "you are stupid"))
    {
        char * nick2 = (char *) malloc(900);
        nick2 = getNick(buf);
        sprintf(sending, "PRIVMSG ChanServ: KICKBAN #%s %s ", gchan, getNick(buf));
    }
    if (charSearch(buf, "fuck") || charSearch(buf, "whore") || charSearch(buf, "cock") || charSearch(buf, "sex"))
    {
        if (write2)
        {
            sprintf(sending, "PRIVMSG ChanServ :kickban %s %s swearing\r\n", gchan, getNick(buf));
            sendData(sending);
            fprintf(file, "<b> %s </b> <br> <br>\n", buf);
        }
        else write2 = true;
    }
    if (charSearch(buf, "cppbot:mynick"))
    {
        sprintf(sending, "PRIVMSG %s :%s\r\n", gchan, getNick(buf));
        sendData(sending);
    }
    if (charSearch(buf, "cppbot:from"))
    {
        sprintf(sending, "PRIVMSG %s :Pawel: Poland\r\n", gchan);
        sendData(sending);
        sprintf(sending, "PRIVMSG %s :aq2: Finland\r\n", gchan);
        sendData(sending);
        //strcpy(sending, "");
        sprintf(sending, "PRIVMSG %s :el3: Denmark\r\n", gchan);
        sendData(sending);
        //strcpy(sending, "");
        sprintf(sending, "PRIVMSG %s :kadiro: Algieria\r\n", gchan);
        sendData(sending);
        //strcpy(sending, "");
        sprintf(sending, "PRIVMSG %s :hakiro: Greece\r\n", gchan);
        sendData(sending);
        //strcpy(sending, "");
        sprintf(sending, "PRIVMSG %s :Brutus: Netherland\r\n", gchan);
        sendData(sending);
        //strcpy(sending, "");
        sprintf(sending, "PRIVMSG %s :lathifar_: Indonesia\r\n", gchan);
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
        //strcpy(sending, "");
        sprintf(sending, "PRIVMSG %s :cppbot:maxval - Usage:'cppbot:maxval:[maxval]' - Sets maximal value in cppbot:lotto. This value can be equal to 49/100.\r\n", gchan);
        sendData(sending);
        //strcpy(sending, "");
        sprintf(sending, "PRIVMSG %s :cppbot:balls - Usage:'cppbot:balls:[numberofballs]' - Sets number of balls in cppbot:lotto. This value can be equal to 6/10/25.\r\n", gchan);
        sendData(sending);
        //strcpy(sending, "");
        sprintf(sending, "PRIVMSG %s :cppbot:from - Displays nationalities of standard channel users.\r\n", gchan);
        sendData(sending);
        //strcpy(sending, "");
        sprintf(sending, "PRIVMSG %s :cppbot:owner - Displays owner of this bot.\r\n", gchan);
        sendData(sending);
        //strcpy(sending, "");
        sprintf(sending, "PRIVMSG %s :cppbot:mynick - Displays your username.\r\n", gchan);
        sendData(sending);
        //strcpy(sending, "");
        sprintf(sending, "PRIVMSG %s :cppbot:hack - Displays fake hacking informations.\r\n", gchan);
        sendData(sending);
        //strcpy(sending, "");
        sprintf(sending, "PRIVMSG %s :cppbot:test - If bot display 'I live.', it not dead.\r\n", gchan);
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
            //strcpy(sending, "");
            sprintf(sending, "PRIVMSG %s :Clearing YouTube page and deleting all videos there...\r\n", gchan);
            sendData(sending);
        }
        else if (randed == 1)
        {
            //strcpy(sending, "");
            sprintf(sending, "PRIVMSG %s :I have hacked all users passwords!\r\n", gchan);
            sendData(sending);
        }
        else if (randed == 2)
        {
            //strcpy(sending, "");
            sprintf(sending, "PRIVMSG %s :I started hacking Linux Mint page...\r\n", gchan);
            sendData(sending);
        }
        else if (randed == 3)
        {
            //strcpy(sending, "");
            sprintf(sending, "PRIVMSG %s :Clearing %s's hard disc...\r\n", gchan, getNick(buf));
            sendData(sending);
        }
        else if (randed == 4)
        {
            //strcpy(sending, "");
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
    IrcBot bot = IrcBot("NICK CppBot\r\n","USER CppBot a a :CppBot\r\n");
    bot.start();
    return 0;
}
