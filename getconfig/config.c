#include <stdlib.h>
#include "config.h"

#define LOG //printf
#define MAX_LINE_LENGTH 256
#define MAX_VALUE_LENGTH 64

static int isLongChar(char c)
{
	return (c == '*' || c == '-' || (c >= '0'&& c <= '9') || c== 'x');	
}

static int isFloatChar(char c)
{
	return (c == '*' || c == '.' || c == '-' || (c >= '0' && c<= '9'));	
}

static int isStringChar(char c)
{
	return ((c >= '0' && c <= '9') || (c >= 'a' && c<= 'z') || (c >= 'A' && c <= 'Z') || c == '-' || c == '_' || c == ' ');
}

static int isDelimiter(char c)
{
	return (c == ' ' || c == '\t' || c == '\v');
}

typedef int (*judgement_func)(char);
typedef int (*getValue_func)(const char*, const char*, void*);

static judgement_func JUDGE_FUNC[VALUE_TYPE_MAX] = {
	(judgement_func)isLongChar,
	(judgement_func)isFloatChar,
	(judgement_func)isStringChar
};

static char* getValueString(const char* p, char* value, VALUE_TYPE type)
{
	int index = 0;
	judgement_func jFunc = JUDGE_FUNC[type];

	while (*p != '\0'){
		if (isDelimiter(*p)) {
			p++;
		}else{
			break;
		}
	}	
	
	while (*p != '\0'){
		if (jFunc(*p)){
			value[index]=*p;
			index++;
			p++;
		}else{
			break;
		}
	}
	value[index] = '\0';
	return value;	
}



static char* getLine(const char* p, char* lineStr)
{
	int index = 0;
	while (*p!='\0' && *p!='\r' && *p!='\n' && index<MAX_LINE_LENGTH){
		lineStr[index] = *p;
		p++;
		index++;
	}
	return lineStr;	 		
}

static int getLongValue(const char* cfgData, const char* key, void* pValue)
{
	if (cfgData == NULL || key == NULL || pValue ==NULL) {
		LOG("getLong error, cfgData || key || pValue is NULL\n");
		return ERROR_TYPE_INVALID_PARAM;
	}


	char* p = strstr(cfgData, key);
		
	if (p == NULL) {
		LOG("getLong error, key not find in cfgData\n");
		return ERROR_TYPE_VALUE_NOT_FIND;
	}	
	
	char pLineStr[MAX_LINE_LENGTH] = {0};
 	getLine(p, pLineStr);
	
	LOG("pLineStr = %s\n", pLineStr);
	
	p = strchr(pLineStr, '=');
	
	if (p == NULL) {
		LOG("getLong error , = not found\n");
		return ERROR_TYPE_VALUE_NOT_FIND;
	}
	
	p++;
	
	char pValueStr[MAX_VALUE_LENGTH] = {0};
	
	getValueString(p, pValueStr, VALUE_TYPE_LONG); 
	
	LOG("pValueStr = %s\n", pValueStr);

	long value;
	
	p = strtok(pValueStr, "*");
	
	LOG("p=%s\n", p);
	
	if (p==NULL){
		LOG("getLong error, strtok == NULL");
		return ERROR_TYPE_VALUE_NOT_FIND;
	}
	
	if (p[0]=='0' && p[1]=='x'){
		value = strtol(p, NULL, 16);
	}else{
		value = atoi(p);
	}
	p = strtok(NULL, "*");
	LOG("p= %s\n", p);
	while (p != NULL){
		if (p[0]=='0' && p[1]=='x'){
			value *= strtol(p, NULL, 16);
		}else {
			value *= atoi(p);
		}
		p = strtok(NULL, "*");
	}
	
	*((long*)pValue) = value;
	
	return ERROR_TYPE_NONE;	
}

static int getFloatValue(const char* cfgData, const char* key, void* pValue)
{
	int res = ERROR_TYPE_NONE;
	if (cfgData == NULL || key == NULL || pValue == NULL){
		LOG("getFloatValue is error , invalid param");
		return ERROR_TYPE_INVALID_PARAM;
	}

	char* p = strstr(cfgData, key);
	 
	char pLineStr[MAX_LINE_LENGTH] = {0};
	getLine(p, pLineStr);
	LOG("pLineStr = %s\n", pLineStr);
	p = strchr(pLineStr, '=');

	if (p == NULL) {
		LOG("getFloatValue error , get pLineStr is NULL\n");
		return ERROR_TYPE_VALUE_NOT_FIND;
	}
	p++;
	
	LOG("p=%s\n", p);
	char pValueStr[MAX_VALUE_LENGTH] = {0};
	getValueString(p, pValueStr, VALUE_TYPE_FLOAT);
	LOG("pValueStr=%s\n", pValueStr);
	p = strtok(pValueStr, "*");
	
	if (p == NULL) {
		LOG("get FloatValue error, get VauleStr is NULL\n");
		return ERROR_TYPE_VALUE_NOT_FIND;
	}
	LOG("p = %s\n", p);
	float value = atof(p);
	LOG("value = %f\n", value);
	p = strtok(NULL, "*");
	while (p != NULL) {
		value *= atof(p);
		p = strtok(NULL, "*");
	}
	LOG("value = %f\n", value);
	*((float*)pValue) = value;
	
	return res;
} 

static int getStringValue(const char* cfgData, const char* key, void* pValue)
{
	int res = ERROR_TYPE_NONE;
	if (cfgData == NULL || key == NULL || pValue == NULL) {
		LOG("get StringValue error, invalid param\n");		
	}
	
	char* p = strstr(cfgData, key);

	char pLineStr[MAX_LINE_LENGTH] = {0};
		
	getLine(p, pLineStr);

	p = strchr(pLineStr, '=');
	
	if (p == NULL) {
		LOG("strchr error p = NULL\n");
		return ERROR_TYPE_VALUE_NOT_FIND;
	}
	p++;
	
	char pValueStr[MAX_VALUE_LENGTH] = {0};
	getValueString(p, pValueStr, VALUE_TYPE_STRING);
	
	strcpy((char*)pValue, pValueStr);	
	
	
	return res;
}


static getValue_func GET_VALUE_FUNC[VALUE_TYPE_MAX] = {
        (getValue_func)getLongValue,
        (getValue_func)getFloatValue,
        (getValue_func)getStringValue
};

int getConfigValue(const char* path, const char* key, void* value, VALUE_TYPE type)
{
	int res = ERROR_TYPE_NONE;
	if (path == NULL || key == NULL || value == NULL) {
		LOG("invalid param error\n");
		return ERROR_TYPE_INVALID_PARAM;
	}
	
	int fs = 0;
        char* buf = NULL;
        FILE* fp = fopen(path, "r");
        if (fp!=NULL){
                fseek(fp, 0, SEEK_END);
                fs = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                if (fs>0)
                        buf = (char*)malloc(fs);
                if (buf!=NULL){
                        fread(buf, 1, fs, fp);
                }
	} else {
		LOG("file not find \n");
		return ERROR_TYPE_INVALID_PARAM;
	}
        fclose(fp);

	getValue_func getValueFunc = GET_VALUE_FUNC[type];

	res = getValueFunc(buf, key, value);

	return res;
}
