/*REQUIREMENTS BEFORE EXECUTION :
MySQL server has to be established
create a new database and table 'urlhash' with fields ID BIGINT,URL VARCHAR(30) and TinyURL VARCHAR(7)
URL field should be set as primary key in database to avoid duplicate insertion
We have limited the size of TinyURL field to 7 to ensure that size of shortened link remains fairly small*/
#include<iostream>
#include<string>
#include<time.h>
#include<windows.h>
#include<mysql.h>
#include<sstream>
#include<math.h>
#include<algorithm>
using namespace std;
struct entry   //value inserted into database
{
    long long id;
    string url;
    string turl;
};
unsigned long long Randomize();
void insertdb(entry,MYSQL*);
void extractdb(MYSQL*,string);
bool check_id_db(MYSQL*,long long);
long long hashfn(MYSQL*);
string encode(long long);

int main()
{
    entry e;
    string t_url;
    MYSQL *conn;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn,"192.168.56.1","admin","admin","hash2",0,NULL,0);
    //parameters in the above function: connection pointer, IPv4 address, user name, password, name of database, 0, NULL, 0
    if(conn)
        cout<<"Connected to server database"<<endl;
    else
    {
        cout<<"ERROR: No connection established\n";
        return 0;
    }
    cout<<"ShortURL Domain : ERB"<<endl;
    while(1)
    {
        int ch;
        cout<<"1.Shorten URL\n2.TinyURL Redirect\n3.Exit\nInput Choice : ";
        cin>>ch;
        switch(ch)
        {
            case 1 :cout<<"Enter URL : ";
                    cin>>e.url;
                    e.id = hashfn(conn);
                    e.turl = encode(e.id);
                    insertdb(e,conn);
                    cout<<endl;
                    break;
            case 2 :cout<<"Enter Tiny URL : https://erb.ly/";
                    cin>>t_url;
                    extractdb(conn,t_url);
                    cout<<endl;
                    break;
            default: return 0;
        }
    }
}

unsigned long long Randomize()
{
    //Long range Random function generator
    srand(time(0));
    unsigned long long randnumber = 0;
    int digits[20];
    int nd =rand()%(9+1-4)+4;

    for (int i = nd; i >= 1; i--)
        digits[i]=rand()%10;
    for(int i=nd; i>=1; i--)
    {
       unsigned long long power = pow(10, i-1);

        if (power%2 != 0 && power != 1)     //eliminates "bug" (which comes from long long power is not a float))
            power++;

        randnumber += power * digits[i];
    }
    return randnumber;
}
string encode(long long n)
{
    //conversion to base 62
    char map[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string shorturl = "";
    while (n)
    {
        shorturl.push_back(map[n%62]);
        n = n/62;
    }
    reverse(shorturl.begin(), shorturl.end());
    return shorturl;
}
long long hashfn(MYSQL *conn)
{
    //Ensures no duplicate random numbers are used
    long long id = Randomize();
    while(!check_id_db(conn,id))
        id = Randomize();
    return id;
}
bool check_id_db(MYSQL* conn,long long n) //returns 1 if no entry found else returns 0
{
    MYSQL_ROW row;
    MYSQL_RES *res;
    if(conn)
    {
        stringstream ss;
        ss<<"SELECT URL FROM urlhash WHERE ID = "<<n<<"";
        string temp = ss.str();
        int qstate = mysql_query(conn,temp.c_str());
        if(!qstate)
        {
            res=mysql_store_result(conn);
            row=mysql_fetch_row(res);
            if(!row)
                return 1;
            return 0;
        }
    }
    cout<<"Connection Lost"<<endl;
    exit(0);
}
void insertdb(entry e,MYSQL *conn)
{
    //Function to insert values into the database
    int qstate=0;
    stringstream ss;
    //MYSQL query
    ss<<"INSERT INTO urlhash(ID,URL,TinyURL) VALUES("<<e.id<<",'"<<e.url<<"','"<<e.turl<<"')";
    string query = ss.str();
    const char* q = query.c_str();
    qstate = mysql_query(conn,q);
    if(qstate == 0)
        cout<<"TinyURL for given link is https://erb.ly/"<<e.turl<<endl;
    else
    {
        //MYSQL query
        string temp = "SELECT TinyURL FROM urlhash WHERE URL = '" + e.url + "'";
        MYSQL_ROW row;
        MYSQL_RES *res;
        const char *c = temp.c_str();
        int qstate1 = mysql_query(conn,c);
        if(!qstate1)
        {
            res=mysql_store_result(conn);
            row=mysql_fetch_row(res);
            if(row)
                cout<<"Shortened Link already exists : https://erb.ly/"<<row[0]<<endl;
        }

    }
}
void extractdb(MYSQL* conn,string turl)
{
    //Function to extract values from the database
    MYSQL_ROW row;
    MYSQL_RES *res;
    if(conn)
    {
        //MYSQL query
        string temp = "SELECT URL FROM urlhash WHERE TinyURL = '" + turl + "'";
        const char *c = temp.c_str();
        int qstate = mysql_query(conn,c);
        if(!qstate)
        {
            res=mysql_store_result(conn);
            row=mysql_fetch_row(res);
            if(row)
                cout<<"Redirected to URL : "<<row[0]<<endl;
            else
                cout<<"Invalid Path Input\n";
        }
    }
}


