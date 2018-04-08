#include "peel/HttpSocket.h"
#include "peel/SocketException.h"
#include "peel/Util.h"
#include "peel/chartype.h"

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Wininet.lib")
#include <stdio.h>
#include <tchar.h>
#include "peel/WinUtil.h"
#include "windows.h"
#endif

#ifdef _WIN32
#define STRLEN _tcslen
#else
#define STRLEN strlen
#endif

using namespace peel;

#define UPLOADFILE "upload_file"

//http://www.experts-exchange.com/Programming/Programming_Languages/Cplusplus/Q_20435257.html?query=shutdown+socket+order&topics=84

const char *boundary    = "---------------------------888888888888a";



/*
	Send
*/

void HttpSocket::SendGet(const string &url, const string &host)
{
    SendGet(url, "PeelLib", host);
}

void HttpSocket::SendGet(const string &url, const string &userAgent, const string &host)
{
    std::ostringstream ss;
	ss << "GET " << url << " HTTP/1.1\r\n";
    ss << "Host: " << host << " \r\n";
	ss << "User-Agent: " << userAgent << "\r\n";
	ss << "Accept: text/plain\r\n";
	ss << "Connection: close\r\n";
	ss << "\r\n";
    std::string s(ss.str());
    std::vector<char> vs( s.begin(), s.end() );
    Send(vs);
}

void HttpSocket::SendGet(const string &url, const string &host, const int &port, string *data)
{
	HttpSocket s;
    s.Connect( MBSTRING( host ).c_str(), port);
	s.SendGet(url, host);
	s.DisconnectSend();
	if(data != NULL)
	{
		string head;
		s.ReceiveHttp(&head, data);
	}
	//s.DisconnectReceive();
	s.DestroySocket();
}

long HttpSocket::SendFile(const PL_CHAR *filename, const char *url, const string &host)
{
	ostringstream ss;

	long fileSize = peel::File::fileSize(filename);

	peel::File pf(filename, STRLEN(filename));
	pf.open("rb");
	if(!pf.okay())
		return 0;

	ss << "POST " << url << " HTTP/1.1\r\n";
    ss << "Host: " << host << "\r\n";
	ss << "User-agent: PeelLib\r\n";
	ss << "Content-type: application/x-renderdata\r\n";
	ss << "FileName: " << filename << "\r\n";
	ss << "Content-Length: " << fileSize << "\r\n\r\n";

	Send((void *)ss.str().c_str(), (int)ss.str().length());

	return fileSize;
}



// Static
long HttpSocket::SendFile(const string host, const int port, const string url, const STRING filename, string *data)
{
	long ret = 0;
	HttpSocket s;
    s.Connect(MBSTRING(host).c_str(), port);
	ret = s.SendFileForm(filename.c_str(), url.c_str(), host);

	if(data != NULL)
	{
		string head;
		try
		{
			s.ReceiveHttp(&head, data);
		}
		catch(SocketException e)
		{
			//*data = e.str();
			return 0;
		}
	}
	s.Disconnect();
	return ret;
}

	

long HttpSocket::SendFileForm(const PL_CHAR *filename, const char *url, const string &host)
{
	stringstream  ss;
	pl_data       buffer[2048];
	//char buffer_enc[2048];
	size_t        bytesRead;
	long          count=0;
	stringstream  message;
	stringstream  message_end;
	string        id="";

	// Entry accepted, now upload the file.

	size_t fileSize = peel::File::fileSize(filename);
	peel::File pf(filename, STRLEN(filename));
	if(!pf.open("rb")) return 0;

	ss.str("");
	message << "--" << boundary << "\r\n";
	message << "Content-Disposition: form-data; name=" << UPLOADFILE << ";";
	message << " filename=\"" << filename << "\"\r\n";
	message << "Content-Type:application/octet-stream\r\n";
	message << "\r\n";	

	message_end << "\r\n--" << boundary << "--";

	ss << "POST " << url << " HTTP/1.1\r\n";
	ss << "Host: " << host.c_str() << "\r\n";
	//ss << "Accept: text/xml, text/html, text/plain\r\n";
	//ss << "Keep Alive: 300\r\n";
	ss << "Content-Type: multipart/form-data; boundary=" << boundary << "\r\n";
	ss << "User-Agent: PeelLib 0.1\r\n";
	//ss << "Connection: Keep-Alive\r\n";
	ss << "Connection: close\r\n";

	ss << "Content-Length: " << message.str().length() + fileSize + message_end.str().length() << "\r\n\r\n";

	Send((void*)ss.str().c_str(), (int)ss.str().length());
	Send((void*)message.str().c_str(), (int)message.str().length());

	pf.read(buffer, 2048, &bytesRead);

	while(bytesRead > 0)
	{
		//PU::b64_encode( buffer, buffer_enc, bytesRead );
		Send(buffer, (int)bytesRead);
		count += (long)bytesRead;
		pf.read(buffer, 2048, &bytesRead);
	}

	Send((void*)message_end.str().c_str(), (int)message_end.str().length());

	pf.close();

	//size_t check =  message.str().length() + fileSize + message_end.str().length();

	return count;

}

#ifdef _WIN32

void HttpSocket::SendFileFormInetError(const PL_CHAR *msg, HANDLE hFile, HINTERNET hNet, HINTERNET hCon, HINTERNET hReq)
{
	if(hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
	if(hNet != NULL) InternetCloseHandle(hNet);
	if(hCon != NULL) InternetCloseHandle(hCon);
	if(hReq != NULL) InternetCloseHandle(hReq);
	throw SocketException(msg, GetError());
}

long HttpSocket::SendFileFormInet(STRING host, int port, STRING url, STRING filename, string *data)
{
    std::ostringstream ss;
	DWORD size_low;
	DWORD size_high;

	INTERNET_BUFFERS BufferIn = {0};
	BufferIn.dwStructSize = sizeof( INTERNET_BUFFERS );

	HANDLE hFile = INVALID_HANDLE_VALUE;
	HINTERNET hNet = NULL;
	HINTERNET hCon = NULL;
	HINTERNET hReq = NULL;
		
	hFile = CreateFile(PU::PCSlash(filename).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile==INVALID_HANDLE_VALUE)
		throw SocketException(_T("Invalid File"), GetError());

	// Get File Size
	size_low = GetFileSize(hFile, &size_high);
	
	if(size_low ==0 && size_high==0) SendFileFormInetError(_T("Zero sized file"), hFile, hNet, hCon, hReq); 
	if(size_low==INVALID_FILE_SIZE)  SendFileFormInetError(_T("Invalid File Size"), hFile, hNet, hCon, hReq);
	if(size_high >0)                 SendFileFormInetError(_T("File is too large"), hFile, hNet, hCon, hReq);
	
	hNet = InternetOpen(_T("peelClient"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	if(!hNet) SendFileFormInetError(_T("Could not open internet interface"), hFile, hNet, hCon, hReq);

	//INTERNET_DEFAULT_HTTP_PORT
	hCon = InternetConnect(hNet, host.c_str(), port , 0, 0, INTERNET_SERVICE_HTTP, 0, 0);
	if(!hCon) SendFileFormInetError(_T("Could not connect to server"), hFile, hNet, hCon, hReq);

	hReq = HttpOpenRequest(hCon, _T("POST"), url.c_str(), 0, 0, 0, 0, 0);
	if(!hReq) SendFileFormInetError(_T("Could not open request to server"), hFile, hNet, hCon, hReq);

	// Prepare Header

	STRING boundary(27, '-');

	// We now need 12 hex digits.
	int r0 = rand() & 0x00ff;
	int r1 = rand() & 0x00ff;
	int r2 = rand() & 0x00ff;

	PL_CHAR temp[13];
#ifdef WIN32
    _tprintf_s(temp, 13, "%04X%04X%04X", r0, r1, r2);
#else
    sprintf_s(temp, 13, "%04X%04X%04X", r0, r1, r2);
#endif

	boundary += temp;

	STRING contentTypeHeader = _T("Content-Type: multipart/form-data; boundary=") + boundary;
	if(!HttpAddRequestHeaders(hReq, contentTypeHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD))
		SendFileFormInetError(_T("Could not add headers to request"), hFile, hNet, hCon, hReq);

	OSTREAM requestHeader;
	OSTREAM requestFooter;

	requestHeader << "--" << boundary << "\r\n";
	requestHeader << "Content-Disposition: form-data; name=" << UPLOADFILE;
	requestHeader << "; filename=\"" << filename.c_str() << "\"\r\n";
	requestHeader << "Content-Type: application/octet-stream\r\n";
	requestHeader << "\r\n";

	requestFooter << "\r\n--" << boundary << "--\r\n";

	// Send Headers

	BufferIn.dwBufferTotal  = (DWORD)(size_low + requestHeader.str().length() + requestFooter.str().length());

	if(!HttpSendRequestEx( hReq, &BufferIn, NULL, HSR_INITIATE, 0))
		SendFileFormInetError(_T("Could not send request to server"), hFile, hNet, hCon, hReq);
/*
	BYTE headers[4096];
	DWORD ll = 4096;
	HttpQueryInfo(hReq, HTTP_QUERY_RAW_HEADERS,  headers, &ll, 0);
*/
	DWORD sum = 0;
	DWORD dwBytesRead;
	DWORD dwBytesWritten;
	BYTE pBuffer[1024]; 
	BOOL bRead, bRet;

	// Send Header

	if (!(bRet=InternetWriteFile( hReq, 
					(void*)requestHeader.str().c_str(), 
					(DWORD)requestHeader.str().length(), 
					&dwBytesWritten)))
		SendFileFormInetError(_T("Could not write data to server"), hFile, hNet, hCon, hReq);

	sum+=dwBytesWritten;

	// Send file

	long ret = 0;
	do
    {
		if  (!(bRead = ReadFile (hFile, pBuffer, sizeof(pBuffer), &dwBytesRead, NULL)))
			SendFileFormInetError(_T("Could not read data from file"), hFile, hNet, hCon, hReq);


		if (!(bRet=InternetWriteFile( hReq, pBuffer, dwBytesRead, &dwBytesWritten)))
			SendFileFormInetError(_T("Could not write data to network"), hFile, hNet, hCon, hReq);


		ret += dwBytesWritten;
		sum += dwBytesWritten;
	}
	while (dwBytesRead == sizeof(pBuffer)) ;

	// Write footer
	if (!(bRet=InternetWriteFile( hReq, (void*)requestFooter.str().c_str(), requestFooter.str().length(), &dwBytesWritten)))
		SendFileFormInetError(_T("Could not write data to network"), hFile, hNet, hCon, hReq);

	sum+=dwBytesWritten;

	// Check to see if all the data was sent
	if(sum != BufferIn.dwBufferTotal)
		SendFileFormInetError(_T("Not all data was written"), hFile, hNet, hCon, hReq);


	DWORD avail;

	HttpEndRequest(hReq, 0, 0, 0);
	PL_CHAR d[4096];
	DWORD len;

	if(data != NULL)
	{
		InternetQueryDataAvailable(hReq, &avail, 0, 0);

		InternetReadFile(hReq, d, 4095, &len);

		int total=0;
		while(len > 0)
		{
			total+=len;
			d[len]=0;
			ss << d;
			InternetReadFile(hReq, d, 1023, &len);
		}
		*data = ss.str();
	}

	InternetCloseHandle(hNet);
	InternetCloseHandle(hCon);
	InternetCloseHandle(hReq);
	CloseHandle(hFile);

	return ret;
}


void HttpSocket::SendGetInet(const STRING &url, const STRING &host, const int &port, std::vector<char> *data)
{
	char d[4096];
	DWORD len;
	stringstream lss;
	INTERNET_BUFFERS BufferIn = {0};

	BufferIn.dwStructSize = sizeof( INTERNET_BUFFERS );

	HINTERNET hNet = InternetOpen(_T("peelClient"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	if(!hNet)
		throw SocketException(_T("Could not open internet"), GetError());
	
	HINTERNET hCon = InternetConnect(hNet, host.c_str(), port , 0, 0, INTERNET_SERVICE_HTTP, 0, 0);
	if(!hCon)
	{
		InternetCloseHandle(hNet);
		throw SocketException(_T("Could not connect to server"), GetError());
	}

	HINTERNET hReq = HttpOpenRequest(hCon, _T("GET"), url.c_str(), 0, 0, 0, INTERNET_FLAG_RELOAD, 0);
	if(!hReq)
	{
		InternetCloseHandle(hNet);
		InternetCloseHandle(hCon);
		throw SocketException(_T("Could not open request to server"), GetError());
	}

	if(!HttpSendRequest(hReq, NULL, NULL, NULL, NULL))
	{
		InternetCloseHandle(hNet);
		InternetCloseHandle(hCon);
		throw SocketException(_T("Could not send request to server"), GetError());
	}



	if(data != NULL)
	{
		lss.str("");

		while(1)
		{
			if(!InternetReadFile(hReq, d, 4095, &len))
				break;
			if(len==0) break;
			d[len]='\0';
            std::copy( d, d+len, (*data).begin());
		}
	}

	InternetCloseHandle(hNet);
	InternetCloseHandle(hCon);

}


void HttpSocket::GetFileInet(const STRING url, STRING host, int port, STRING filename, string md5)
{
	char d[4096];
	DWORD len;
	INTERNET_BUFFERS BufferIn = {0};
	int     bytesRecv = 0;
	time_t  startTime = time(NULL);
	DWORD   written;
	int     total=0;


	if(filename.length()==0)
		throw SocketException(_T("Invalid File Name"), GetError());

	//OFSTRUCT info;
	//if(OpenFile(filename.c_str(), &info, OF_EXIST)!=HFILE_ERROR)
	//	throw SocketException(_T("Could not open file"), GetError());


	// Create the file

	// CREATE_NEW | CREATE_ALWAYS
	HANDLE h =	CreateFile(filename.c_str(), FILE_ALL_ACCESS,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	if(h== INVALID_HANDLE_VALUE)
	{
		OSTREAM ss;
		ss << "Could not create file: " << filename << ":" <<  PU::windowsError(GetLastError());
		throw SocketException(ss.str(), GetError());
	}

	BufferIn.dwStructSize = sizeof( INTERNET_BUFFERS );

	HINTERNET hNet = InternetOpen(_T("peelClient"), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
	if(!hNet)
		throw SocketException(_T("Could not open internet"), GetError());
	
	HINTERNET hCon = InternetConnect(hNet, host.c_str(), port , 0, 0, INTERNET_SERVICE_HTTP, 0, 0);
	if(!hCon)
	{
		InternetCloseHandle(hNet);
		throw SocketException(_T("Could not connect to server"), GetError());
	}

	HINTERNET hReq = HttpOpenRequest(hCon, _T("GET"), url.c_str(), 0, 0, 0, 0, 0);
	if(!hReq)
	{
		InternetCloseHandle(hNet);
		InternetCloseHandle(hCon);
		throw SocketException(_T("Could not open request to server"), GetError());
	}

	if(!HttpSendRequest(hReq, NULL, NULL, NULL, NULL))
	{
		InternetCloseHandle(hNet);
		InternetCloseHandle(hCon);
		throw SocketException(_T("Could not send request to server"), GetError());
	}


	bool valid = true;
	bool running = true;
	while(running)
	{
		if(!InternetReadFile(hReq, d, 4095, &len))
		{
			throw SocketException(_T("Could not read data"), GetError());
		}
		if(len==0) break;
		//d[len]='\0';
		if(!WriteFile(h, d, len, &written, NULL))
		{
			valid = false;
			break;
		}
	}

	InternetCloseHandle(hNet);
	InternetCloseHandle(hCon);
	CloseHandle(h);
}
#endif



/*
	Receive
*/

// Recieves up to \r\n\r\n - no parsing
 
bool HttpSocket::ReceiveHeader(string &data)
{
    data = "";
	stringstream ss;

	char    c[256];
	int     bytesRecv = 0;
	time_t  startTime = time(NULL);

#ifdef _DEBUG
	const char *cstr;
#endif

	while(bytesRecv >= 0 && difftime(time(NULL),startTime) < TIMEOUT )
	{
		bytesRecv = recv( m_socket, c, 1, 0 );
		c[bytesRecv]='\0';
			
		if(bytesRecv < 0 && errno == SOCKET_WOULDBLOCK )
			break;

#ifdef _WIN32
		if ( bytesRecv == 0 || bytesRecv == SOCKET_CONNRESET )  // connection closed
			break;
#else
		if ( bytesRecv == 0 )  // connection closed
			break;
#endif			 
		if( bytesRecv > 0 )
		{
			data += c;
#ifdef _DEBUG
			cstr = data.c_str();
#endif
		}

		if(data.size()>=4)
		{
			if(data.substr(data.size()-4,4)=="\r\n\r\n")
				break;
		}
	}

	if(data.size()>0)
		return true;
	
	return false;
}


// If have sent an http request, e.g. GET to an http server, this will
// parse the response.

bool HttpSocket::ReceiveHttp(std::string *header, std::string *data)
{
	OSTREAM      ss;
	char         temp_data;
	int          bytesRecv = 0;
	size_t       count=0;
	
	if(!header && !data) return false;
	if(header != NULL)  *header  = "";
    if(data   != NULL)  *data    = "";

	string header_data;

	struct timeval timeout;
	timeout.tv_sec = TIMEOUT;
	timeout.tv_usec = 0;

	fd_set readfs;
	FD_ZERO(&readfs);
	FD_SET(m_socket, &readfs);

	std::string s;

	while(bytesRecv >= 0 )
	{		

		int sel = select(m_socket+1, &readfs, NULL, NULL, &timeout);
		if(sel == 0) throw SocketException(_T("Timeout while getting http header"));
		if(sel < 0) throw SocketException(_T("Error while getting http header"));
		
		bytesRecv = recv( m_socket, &temp_data, 1, 0 );
		count+=bytesRecv;

			
		if(bytesRecv < 0 && errno == SOCKET_WOULDBLOCK ) break;
#ifdef _WIN32	
		if ( bytesRecv == 0 || bytesRecv == SOCKET_CONNRESET ) break;
#else	
		if ( bytesRecv == 0 ) break;
#endif
		if ( bytesRecv > 0 )
		{
			header_data += temp_data;

			if(header_data.length() > 4)
			{


				s = header_data.substr(header_data.length()-4,4);
				const char *q = s.c_str();
				if( q[0] == '\r' && q[2] == '\r' && q[1] == '\n' && q[3] == '\n')
				{
					break;
				}
			}
		}
	}

	
	if(header != NULL) *header = header_data;

	if(data == NULL)  return true;

	// Find first space in "HTTP/1.1 200 OK"
	string::size_type t1 = header_data.find_first_of(" \t");
	if(t1==string::npos) throw SocketException(_T("Malformed Header"),0);
	// get all the characters 
	string::size_type t2 = header_data.find_first_not_of(" \t\r\n", t1);
	if(t1==string::npos) throw SocketException(_T("Malformed Header (#2)"),0);
	// Find end of line
	string::size_type t3 = header_data.find_first_of(" \t\r\n", t2);
	if(t2==string::npos) throw SocketException(_T("Malformed Header (#3)"),0);

	// Code (e.g. 200) is between t2 and t3.
	string code = header_data.substr(t2, t3-t2);

	if(code=="404")
		throw SocketException(_T("404 - Content not found Error from server"),0);

	

	string::size_type cl;
	bool hasLen = false;
	int nbytes = 0;
	cl = header_data.find("Content-Length:");

	if(cl!=string::npos)
	{
		cl = header_data.find_first_not_of("\t ", cl+15);
		if(cl!=string::npos)
		{
			string::size_type ws = header_data.find_first_of("\r\n", cl);
			if(ws != string::npos)
			{
				nbytes = atoi(header_data.substr(cl, ws-cl).c_str());
				hasLen = true;
			}
		}
	}

	int content = 0;
	while(bytesRecv >= 0 )
	{

		int sel = select(m_socket+1, &readfs, NULL, NULL, &timeout);
		if(sel == 0) throw SocketException(_T("Timeout while getting http data"));
		if(sel < 0) throw SocketException(_T("Error while getting http data"));

		bytesRecv = recv( m_socket, &temp_data, 1, 0 );
		count += bytesRecv;
		content += bytesRecv;
			
		if(bytesRecv < 0 && errno == SOCKET_WOULDBLOCK )
			break;
#ifdef _WIN32
		if ( bytesRecv == 0 || bytesRecv == SOCKET_CONNRESET ) break;
#else	
		if ( bytesRecv == 0 ) break;
#endif
		 
		if( bytesRecv > 0 )
			if(data != NULL)
				data->push_back(temp_data);	

		if(hasLen && content == nbytes)
		{
			break;
		}
	}

	return true;
}


void HttpSocket::ReplyGet(const std::string body, bool htmlBody, int code)
{
	ostringstream ss;
	char buffer[255];
	time_t t = time(NULL);
	struct tm my_tm;
	gmtime_s(&my_tm, &t);
	strftime(buffer, 255, "%a, %d %b %Y %H:%M:%S GMT", &my_tm);

	if(code == 404) ss << "HTTP/1.1 404 Not Found\r\n";
	else            ss << "HTTP/1.1 200 OK\r\n";

	ss << "Content-Length: " << body.length() << "\r\n";
	ss << "Date: " << buffer << "\r\n"; 
	ss << "Connection: close\r\n";

	if(htmlBody) ss << "Content-Type: text/html; charset=utf8\r\n";
	else         ss << "Content-Type: text/plain; charset=utf8\r\n";
	ss << "\r\n";

	this->Send((void*)ss.str().c_str(), ss.str().length());
	this->Send((void*)body.c_str(), body.length());
}
