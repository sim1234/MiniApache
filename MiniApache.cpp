/*
Copyright (c) 2015, Szymon Zmilczak
*/


#include "MiniApache.h"



MiniApache::MiniApache(int port){
	_server = new EthernetServer(port);
	//_server(port);
	
	_storage_path = "";
	ClearStr(_HTTP_request, REQUEST_BUFFER_SIZE);
	//ClearStr(_request_path, REQUEST_BUFFER_SIZE);
	_error_status = 0;
	_serve_status = 0;
}

bool MiniApache::begin(char *storage_path, int cspin){
	_storage_path = storage_path;
	_serve_status = 1;
	
	if (!SD.begin(cspin)) {
		#if SERIAL_DEBUG == 1
			Serial.println("ERROR - SD card initialization failed!");
		#endif
		_error_status = 1;
		return false;    // init failed
	}
	bool r = TestIndexFile();
	#if SERIAL_DEBUG == 1
		if (r){
			Serial.println("SUCCESS - MiniApache initialized.");
		}
	#endif
	return r;
}

bool MiniApache::TestIndexFile(){
	char* index = JoinStr(_storage_path, DEFAULT_INDEX_FILE);
	if (!SD.exists(index)){
		#if SERIAL_DEBUG == 1
			Serial.print("ERROR - Can't find index file!");
		#endif
		_error_status = 2;
		delete [] index;
		return false;
	}
	delete [] index;	
	return true;
}

void MiniApache::SetError(int code){
	_error_status = code;
}

char* MiniApache::GetErrorMessage(){
	switch(_error_status){
		case 0:
			return "No error decetded.";
		break;
		case 1:
			return "SD card initialization failed!";
		break;
		case 2:
			return "Can't find index file! Check SD card and reset device.";
		break;
		case 3:
			return "Ethernet module initialization failed!";
		break;
		default:
			return "Unknown error!";
		break;
	}
}

char* MiniApache::ExtractRequestPath(){
    int rlen = strlen(_HTTP_request);
    int start = FindChar(_HTTP_request, ' ', 0, rlen) + 1;
    int end = FindChar(_HTTP_request, '?', start, rlen);
    if (end == -1){
    	end = FindChar(_HTTP_request, ' ', start, rlen);
    }
	char * retVal = SubStr(_HTTP_request, start, end);
	return retVal;  // Remember to delete [] retVal to not leak memory
}

char* MiniApache::ExtractRequestData(){
	int rlen = strlen(_HTTP_request);
	int start = FindChar(_HTTP_request, '?', 0, rlen) + 1;
	int end = FindChar(_HTTP_request, ' ', start, rlen);
	if (start == 0){
		start = end;
	}
	char * retVal = SubStr(_HTTP_request, start, end); // extract GET data
    return retVal;  // Remember to delete [] retVal to not leak memory
	
}

char* MiniApache::GetMIMEType(char *path){
    char ext[8] = {0,0,0,0,0,0,0,0};
    int pos = -1;
    for (int i = 0; i < strlen(path); i++) { // Find file extension after '.'
        if (pos > -1 && pos < 8) {
            ext[pos] = tolower(path[i]);
            pos++;
        }
        if (path[i] == '.') {
            pos = 0;
        }
    }
    if (strcmp(ext, "htm") == 0 || strcmp(ext, "html") == 0) return "text/html";
    if (strcmp(ext, "js") == 0) return "application/javascript";
    if (strcmp(ext, "css") == 0) return "text/css";
    if (strcmp(ext, "jpg") == 0) return "image/jpeg";
    if (strcmp(ext, "gif") == 0) return "image/gif";
    if (strcmp(ext, "png") == 0) return "image/png";
    if (strcmp(ext, "ico") == 0) return "image/vnd.microsoft.icon";
    if (strcmp(ext, "jsn") == 0) return "application/json";
    if (strcmp(ext, "txt") == 0 || strcmp(ext, "log") == 0) return "text/plain";
    return "application/octet-stream";
    
}

void MiniApache::PrintHeader(char* content_type){
	PrintHeader(200, "OK", content_type, false);
}
void MiniApache::PrintHeader(int status, char *status2, char* content_type, bool cached){
	client.print("HTTP/1.1 ");
	client.print(status);
	client.print(" ");
	client.println(status2);
	client.print("Content-Type: ");
	client.println(content_type);
	if (cached){
		client.println("Cache-Control: max-age=604800, public");// 290304000
	} else {
		client.println("Cache-Control: no-store, no-cache");
	}
	client.println("Connection: close");
	client.println();
}

void MiniApache::QuickResponse(char* data){
	PrintHeader(200, "OK", "text/html", false);
	client.print(data);
	ReportClientServed(200);
}

int MiniApache::ServeFile(char *path){ // Not used
    _storageFile = SD.open(path);
    if (_storageFile){ // Serve file
    	PrintHeader(200, "OK", GetMIMEType(path), true);
        while(_storageFile.available()){
            client.write(_storageFile.read());
        }
        _storageFile.close();
        return 200;
    } else { // Return 404 error
    	PrintHeader(404, "Not Found", "text/html", true);
        client.println("File Not Found");
        return 404;
    }
}

void MiniApache::ReportClientServed(int status){
	#if SERIAL_DEBUG == 1
		Serial.print(_HTTP_request); // Debug
		Serial.print(" -> ");
		Serial.println(status);
	#endif
		
	ClearStr(_HTTP_request, REQUEST_BUFFER_SIZE);
	_request_index = 0;
	_serve_status = 1;
	
	//delay(10);      // give the web browser time to receive the data
	client.stop(); // close the connection
}

bool MiniApache::RequestIs(char* path){
	return (strcmp(request_path, path) == 0);
}

bool MiniApache::RequestStartsWith(char* path){
	return (strncmp(request_path, path, strlen(path)) == 0);
}

char* MiniApache::GetGETVariable(char * name){
	char * varp = JoinStr(name, "=");
    int dlen = strlen(GET_data);
	int vlen = strlen(varp);
    int start = -1;
    char * sub;
    for (int i = 0; i < dlen; i++) { // Find variable
	    sub = SubStr(GET_data, i, i + vlen);
	    if (strcmp(varp, sub) == 0 && (i == 0 || GET_data[i-1] == '&')) {
 		    start = i + vlen;
 		    delete [] sub;
		    break;
	    }
	    delete [] sub;
    }
    if (start == -1) {
        start = dlen;
    }
    int end = FindChar(GET_data, '&', start, dlen);
    if (end == -1) {
        end = dlen;
    } 
    delete [] varp;
    char * retVal = SubStr(GET_data, start, end);
    return retVal; // Remember to delete [] retVal to not leak memory
}

bool MiniApache::GETVariableIs(char * name, char * value){
	char * v = GetGETVariable(name);
	bool r = (strcmp(v, value) == 0);
	delete [] v;
	return r;
}

bool MiniApache::PendingRequest(){
	if (_serve_status == 1){ // Waiting for client
		client = _server->available();  // try to get client
		if (client) {  // got client?
			ClearStr(_HTTP_request, REQUEST_BUFFER_SIZE);
			_serve_status = 2;
		}
	}
	if (_serve_status == 2){ // Reading request
        if (client.connected()){
            while (client.available()){ // client data available to read
				char c = client.read();
				if (_request_index < (REQUEST_BUFFER_SIZE - 1)){ // save HTTP request character
					_HTTP_request[_request_index] = c; 
					_request_index++;
				} else {
					PrintHeader(400, "Bad request", "text/html", false);
					ReportClientServed(400);
					break;
				}
				if (c == '\r' || c == '\n'){
					_serve_status = 3;
					delete [] GET_data;
					delete [] request_path;
					request_path = ExtractRequestPath(); // Get path of the HTTP request, ignore type (assume GET)
					GET_data = ExtractRequestData();     // Get GET data of the HTTP request
					
					if (_error_status){ // Error was detected (earlier)
						PrintHeader(500, "Internal Server Error", "text/html", false);
						client.println("<thml><hrad><title>Internal Server Error</title></head><body>500: Internal Server Error<br>Error code:");
						client.println(_error_status);
						client.println("<br>Error message:");
						client.println(GetErrorMessage());
						client.println("<hr>MiniApache</body></html>");
						ReportClientServed(500);
					}
					
                    #if SERIAL_DEBUG == 1
                        Serial.print("Free memory: ");
                        Serial.println(freeRam());
	                #endif
					
					break; // Ignore rest of the HTTP request
				} 
            }
        }
	}
	return (_serve_status == 3);
}

void MiniApache::ProcessRequest(){
	if (_serve_status == 3){ // Process request
		if (strcmp(request_path, "/") == 0 || strcmp(request_path, "") == 0) { // Redirect from '/' to DEFAULT_INDEX_FILE
			delete [] request_path;
			request_path = JoinStr(DEFAULT_INDEX_FILE, "");
		}
		char* file_path = JoinStr(_storage_path, request_path);
		_storageFile = SD.open(file_path);
		if (_storageFile){
			if (_storageFile.isDirectory()){ // List directory
				PrintHeader(200, "OK", "text/html", false);
				client.print("<html><head><title>");
				client.print(file_path);
				client.print("</title></head><body>List of ");
				client.print(file_path);
				client.println("<table><thead><tr><th>Name</th><th>Size</th></tr></thead><tbody><tr><td><a href=\"../\">../</a></td><td></td></tr>");
				while(true) {
					 File entry =  _storageFile.openNextFile();
					 if (!entry) break;
					 client.print("<tr><td><a href=\"");
					 client.print(entry.name());
					 if (entry.isDirectory()) client.print("/");
					 client.print("\">");
					 client.print(entry.name());
					 if (entry.isDirectory()) client.print("/");
					 client.print("</a></td><td>");
					 if (!entry.isDirectory()) {
					   client.print(entry.size(), DEC);
					   //client.print(" b");
					 }
					 client.println("</td></tr>");
					 entry.close();
				   }
				client.println("</tbody></table><hr>MiniApache</body></html>");
				_storageFile.close();
				ReportClientServed(200);
				
			} else { // Start serving file
				PrintHeader(200, "OK", GetMIMEType(file_path), true);
				_serve_status = 4;
			}
		} else { //File not found, Return 404 error
			TestIndexFile(); // Check if CD cardi is working
			PrintHeader(404, "Not Found", "text/html", true);
			client.println("<thml><hrad><title>File Not Found</title></head><body>404: File Not Found<br>");
			client.println(file_path);
			client.println("<hr>MiniApache</body></html>");
			ReportClientServed(404);
		}
		delete [] file_path;
    }
	if (_serve_status == 4){ // Read and write chunk of file
		int t;
		char T[CLIENT_CHUNK_SIZE];
		t = _storageFile.readBytes(T, CLIENT_CHUNK_SIZE);
		client.write(T, t);
		if (t < CLIENT_CHUNK_SIZE){
			ReportClientServed(200);
			_storageFile.close();
		}
	}
}


