#include <brain.h>
#include <curl/curl.h>





Cbrain::Cbrain(){
	
}



Cbrain::~Cbrain(){
}



struct MemoryStruct {
  char *memory;
  size_t size;
};



static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) {
		/* out of memory! */ 
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}



// Remarks	: get valid data from RESPONSE
void getValidData(char* dest, char* src){
	int nLength;
	char *pStart, *pEnd;
	
	// Initialization and check dest and src validation
	if(src == NULL || dest == NULL){
		return;
	}
	dest[0] = '\0';
	
	// Find valid data pointer
	pStart = strchr(src, '{');
	pEnd = strchr(src, '}');
	
	nLength = pEnd - pStart;
	if(nLength > 0){
		strncpy(dest, pStart, nLength + 1);
		dest[nLength+1] = '\0';
	}
}

// Remarks	: seperate key, { :
int pharseKrw(char* dest, char* data){
	char *pValidData, *pLast, *pStartKrw, *pLastKrw;
	int nLength, nKrw;
	char chKrw[256];
	
	nLength = strlen(data);
	if(nLength > 0){
		pValidData = new char[nLength+1];		// NEW

		// 1> Get valid data from RESPONSE
		getValidData(pValidData, data);
		chKrw[0] = '\0';

		// 2> Pharse time, krw
		pLast = strstr(pValidData,"last");
		if(pLast == NULL){
			return -1;
		}
		pStartKrw = strchr(pLast, ':');
		if(pStartKrw == NULL){
			return -1;
		}
		pLastKrw = strchr(pStartKrw+2, '\"');
		if(pLast == NULL){
			return -1;
		}
		nLength = pLastKrw - pStartKrw -2;
		if(nLength > 0){
			strncpy(chKrw, pStartKrw+2, nLength);
			chKrw[nLength] = '\0';
		}

		strcpy(dest, chKrw);

		delete[] pValidData;					// DELETE
	}

	return nKrw;
}



int test(char *value){
	CURL *curl;
	CURLcode res;
	struct MemoryStruct chunk;
	static const char *postthis = "currency_pair=btc_krw";
	
	chunk.memory = (char*)malloc(1);  /* will be grown as needed by realloc above */ 
	chunk.size = 0;    /* no data at this point */ 
	
	curl_global_init(CURL_GLOBAL_ALL);
	//curl_global_init(CURL_GLOBAL_DEFAULT);
	
	curl = curl_easy_init();
	if(curl) {
		//curl_easy_setopt(curl, CURLOPT_URL, "https://api.korbit.co.kr/v1/ticker");
		curl_easy_setopt(curl, CURLOPT_URL, "https://api.korbit.co.kr/v1/ticker?currency_pair=xrp_krw");
		//curl_easy_setopt(curl, CURLOPT_URL, "http://www.example.org/");
		
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		//curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		//curl_easy_setopt(curl, CURLOPT_COOKIE, "currency_pair=eth_krw&");

		/* send all data to this function  */ 
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		
		/* we pass our 'chunk' struct to the callback function */ 
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
		
		/* some servers don't like requests that are made without a user-agent
		field, so we provide one */ 
		//curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
	
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		//curl_easy_setopt(curl, CURLINFO_HEADER_OUT, true);
		
		// For CURLE_SSL_CACERT error
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

		/* if we don't provide POSTFIELDSIZE, libcurl will strlen() by
		itself */ 
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));
		//curl_easy_setopt(curl, CURLOPT_POST, 1L);
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
		//curl_easy_setopt(curl, CURLOPT_READDATA, (void *)&chunk2);
		//curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_data);
		
		//curl_easy_setopt( curl, CURLOPT_HEADERFUNCTION, CurlHandleResponse );

		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK){
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
			sprintf(value, "curl_easy_perform res = %d(Error no)", res);
			//strcpy(value, "CURLE_N");
		}else {
			char chKrw[256];
			pharseKrw(chKrw, chunk.memory);
			
			strcpy(value, chKrw);
			#if 0
			char *ct;
			res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
			if((CURLE_OK == res) && ct){
				strcpy(value, ct);
			}else{
				printf("%s\n",chunk.memory);
				strcpy(value, chunk.memory);
			}
			#endif
			/*
			* Now, our chunk.memory points to a memory block that is chunk.size
			* bytes big and contains the remote file.
			*
			* Do something nice with it!
			*/ 
		}

		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}

	free(chunk.memory);

	curl_global_cleanup();
	
	return 0;
}
