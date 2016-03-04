#ifndef ___HTTP_SOCKET_H___
#define ___HTTP_SOCKET_H___

#include "peel/Socket.h"
#include "peel/chartype.h"

namespace peel
{


//! A socket object for Http communications

class HttpSocket  : public Socket
{
public:
	HttpSocket() : Socket() {};
	HttpSocket(SOCKET s) : Socket(s) {};

	/*
		Send
	*/

	//! Send a HTTP GET with userAgent as "PeelLib"
        void     SendGet(const string &url, const string &host);

	//!< Send a HTTP GET command with specified userAgent
        void     SendGet(const string &url, const string &userAgent, const string &host);

	//! Static SendGet with http data split by head/body
	/*!  Creates a Socket object, connects to host/port sends the get comand
          and uses Receive() to get any data back, and parses the data for the
		  header and body data */
        static void SendGet(const string &url, const string &host, const int &port, string *data = NULL);

	//static void   GetFileInet(const string &url, const string &host, const sint port, string filename, string md5);


#ifdef _WIN32
	// Win Inet Gets
        static void  SendGetInet(const STRING &url, const STRING &host, const int &port, std::vector<char> *data = NULL);

	//! Gets a url and saves it as a file, using wininet
	/*! \param url the remote file to get, e.g. "\index.html" */
	static void    GetFileInet(const STRING url, STRING host, int port, STRING filename, string md5);

#endif

	//! Use a HTTP POST to send a file, simple POST command - returns number of bytes sent
	/*! Just sends the headers only! */
        long     SendFile(const PL_CHAR *filename, const char *url, const string &host);

	//! Use a HTTP POST to send a file as a multipart form style post - returns number of bytes sent
	/*! Sends file using a multipart form */
        long     SendFileForm(const PL_CHAR *filename, const char *url, const string &host);

	//! Static POST
	/*! Creates a socket object and uses SendFile() to transfer the file */
        static long   SendFile(const string host, const int port, const string url, const STRING filename, string *data = NULL);

#ifdef _WIN32
	//! Sends the file using wininet
        static long   SendFileFormInet(STRING host, int port, STRING url, STRING filename, string *data = NULL);
	//! Throws an error for SendFileFormInet
	static void   SendFileFormInetError(const PL_CHAR *msg, HANDLE hFile, HINTERNET hNet, HINTERNET hCon, HINTERNET hReq);
#endif

        static long SendFileForm(string host, int port, STRING url, string filename, string *data = NULL);



	/*
		Receive
	*/ 
	//! Read an incoming http stream from a server and split it up by header and data
	/*! This is useful to do after sending an http get command */
        bool     ReceiveHttp(string *header, string *data);

	//! Read the headers
	/*! Reads only the headers from an incoming http stream, the rest of the data is left
	    on the wire - this can be used after asking for a http GET */
	bool     ReceiveHeader(string &header);  // reads an incomming request


	/*
  		Reply
  	*/
	//! Reply to an incoming HTTP GET request (for a server)
	/* @param code should be 200 or 404 only   
 	   @param htmlBody should be true if the data contained in body is html formatted
 	*/
	void     ReplyGet(const std::string body, bool htmlBody = true, int code = 200);
};

}// namespace

#endif
