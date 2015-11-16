/*
Copyright (c) 2015, Szymon Zmilczak
*/
#ifndef MiniApache_h
#define MiniApache_h

//#include <inttypes.h>

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Ethernet.h"
#include "SD.h"

#include "MiniApacheUtils.h"


#define REQUEST_BUFFER_SIZE   100
#define DEFAULT_INDEX_FILE   "/index.htm"
#define CLIENT_CHUNK_SIZE   512
#define SERIAL_DEBUG   1



// MiniApache class
class MiniApache {
  private:
	EthernetServer* _server;                       // EthernetServer instance
	char* _storage_path;                           // path to folder containing files which should be served 
	File _storageFile;                             // currently served File object
	char _HTTP_request[REQUEST_BUFFER_SIZE];       // buffered HTTP request stored as null terminated string
	int _request_index;                            // index into HTTP_req buffer
	int _error_status;                             // number of error
	int _serve_status;                             // number of server state
	
	
  public:
	EthernetClient client;                         // EthernetClient object which is served at the moment
	char* request_path;                            // path of current request (without GET data)
	char* GET_data;                                // GET data of current request
	
	MiniApache(int port);                          // init on port
	bool begin(char* storage_path, int cspin);     // test components and start server; cspin is SD card pin
	char* GetMIMEType(char *path);                 // guess MIME type of path
	char* ExtractRequestPath();                    // extract request_path from _HTTP_request
	char* ExtractRequestData();                    // extract GET_data from _HTTP_request
	int ServeFile(char *path);                     // read and send chunk of file to client
	void PrintHeader(char* content_type = "text/html");                                     // send headers to client (status = 200, status2 = "OK", cached = false)
	void PrintHeader(int status, char *status2, char* content_type, bool cached = false);   // send headers to client (now response content can be send)
	void QuickResponse(char* data);                // print headers, data and close connection
	bool RequestIs(char* path);                    // check if path is equal to request_path 
	bool RequestStartsWith(char* path);            // check if request_path starts with path
	char* GetGETVariable(char * name);             // return value of GET variable with name name
	bool GETVariableIs(char * name, char * value); // check if value of GET variable with name is equal to value
	bool PendingRequest();                         // check if there is request waiting to be handled 
	void ProcessRequest();                         // process request internally (not only pending request)
	void ReportClientServed(int status = 200);     // inform server that request was handled with status (flush buffers, close connection)
	char* GetErrorMessage();                       // return error message
	void SetError(int code = 0);                   // set server in error mode with given code
	bool TestIndexFile();                          // test existance of DEFAULT_INDEX_FILE
};

#endif
